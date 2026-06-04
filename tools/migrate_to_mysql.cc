/**
 * @file migrate_to_mysql.cc
 * @brief SQLite -> MySQL 数据迁移工具
 *
 * 用法: ./migrate_to_mysql <sqlite_path> <mysql_host> <mysql_port> <mysql_user> <mysql_passwd> <mysql_db>
 *
 * 编译: g++ -std=c++17 -O2 -o migrate_to_mysql tools/migrate_to_mysql.cc \
 *         -lsqlite3 $(mysql_config --cflags --libs)
 */

#include <sqlite3.h>
#include <mysql/mysql.h>

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <string>
#include <vector>

// ---------- utility ----------

static std::string escape(MYSQL *mysql, const char *s, unsigned long len) {
    if (!s) return "NULL";
    std::string out(len * 2 + 2, '\0');
    unsigned long n = mysql_real_escape_string(mysql, &out[0], s, len);
    out.resize(n);
    return "'" + out + "'";
}

static bool is_text_or_blob(const std::string &t) {
    // case-insensitive
    std::string lower;
    for (char c : t) lower += tolower(c);
    return lower == "text" || lower == "blob";
}

static bool is_integer_type(const std::string &t) {
    return t == "INTEGER" || t == "INT" || t == "INT8" || t == "UINT8" ||
           t == "INT16" || t == "UINT16" || t == "INT32" || t == "UINT32" ||
           t == "INT64" || t == "UINT64" || t == "BIGINT" || t == "SMALLINT" ||
           t == "TINYINT" || t == "MEDIUMINT" || t == "BOOL" || t == "BOOLEAN";
}

static std::string sqlite_type_to_mysql(const std::string &t) {
    if (is_integer_type(t)) return "bigint";
    if (t == "REAL" || t == "FLOAT" || t == "DOUBLE" || t == "NUMERIC" || t == "DECIMAL")
        return "double";
    if (t == "TEXT" || t == "CLOB" || t == "CHARACTER" || t == "VARCHAR" ||
        t == "VARYING CHARACTER" || t == "NCHAR" || t == "NATIVE CHARACTER" || t == "NVARCHAR")
        return "text";
    if (t == "BLOB") return "blob";
    if (t == "TIMESTAMP" || t == "DATETIME" || t == "DATE" || t == "TIME")
        return "datetime";
    // fallback
    if (t.find("CHAR") != std::string::npos || t.find("TEXT") != std::string::npos ||
        t.find("CLOB") != std::string::npos)
        return "text";
    if (t.find("INT") != std::string::npos) return "bigint";
    return "text";
}

static std::string convert_default(const std::string &dflt) {
    if (dflt.empty()) return "";
    if (dflt == "current_timestamp") return "CURRENT_TIMESTAMP";
    return dflt;
}

struct Column {
    std::string name;
    std::string type;        // original SQLite type
    std::string mysql_type;  // mapped MySQL type
    std::string dflt;        // converted MySQL default
    bool notnull = false;
    bool pk = false;
    bool autoinc = false;
};

struct Table {
    std::string name;
    std::vector<Column> cols;
    std::vector<std::string> idx_sqls; // CREATE [UNIQUE] INDEX statements
};

// ---------- main ----------

int main(int argc, char *argv[]) {
    if (argc != 7) {
        fprintf(stderr, "Usage: %s <sqlite_path> <mysql_host> <mysql_port> "
                        "<mysql_user> <mysql_passwd> <mysql_db>\n",
                argv[0]);
        return 1;
    }

    const char *sqlite_path  = argv[1];
    const char *mysql_host   = argv[2];
    int         mysql_port   = atoi(argv[3]);
    const char *mysql_user   = argv[4];
    const char *mysql_passwd = argv[5];
    const char *mysql_db     = argv[6];

    // ---- open SQLite ----
    sqlite3 *sq = nullptr;
    if (sqlite3_open_v2(sqlite_path, &sq,
                        SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, nullptr) != SQLITE_OK) {
        fprintf(stderr, "sqlite3_open %s failed: %s\n", sqlite_path, sqlite3_errmsg(sq));
        return 1;
    }
    printf("[OK] opened SQLite: %s\n", sqlite_path);

    // ---- connect MySQL ----
    MYSQL *my = mysql_init(nullptr);
    if (!my) {
        fprintf(stderr, "mysql_init failed\n");
        return 1;
    }
    if (!mysql_real_connect(my, mysql_host, mysql_user, mysql_passwd,
                            mysql_db, mysql_port, nullptr, 0)) {
        fprintf(stderr, "mysql_real_connect %s:%d/%s failed: %s\n",
                mysql_host, mysql_port, mysql_db, mysql_error(my));
        return 1;
    }
    printf("[OK] connected MySQL: %s:%d/%s\n", mysql_host, mysql_port, mysql_db);

    // ------- discover tables -------
    std::vector<std::string> table_names;
    {
        sqlite3_stmt *stmt = nullptr;
        sqlite3_prepare_v2(sq,
            "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' "
            "ORDER BY name",
            -1, &stmt, nullptr);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            table_names.push_back(
                std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0))));
        }
        sqlite3_finalize(stmt);
    }
    printf("\nFound %zu tables\n\n", table_names.size());

    int total_rows = 0;
    int errors     = 0;

    // disable foreign key checks during migration
    mysql_query(my, "SET FOREIGN_KEY_CHECKS = 0");
    mysql_query(my, "SET SESSION sql_mode = 'NO_ENGINE_SUBSTITUTION'");

    for (auto &tname : table_names) {
        Table tbl;
        tbl.name = tname;

        // ---- read column info ----
        {
            sqlite3_stmt *stmt = nullptr;
            std::string sql = "PRAGMA table_info(" + tname + ")";
            sqlite3_prepare_v2(sq, sql.c_str(), -1, &stmt, nullptr);
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                Column c;
                c.name = std::string(
                    reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
                const char *raw_type = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
                c.type = raw_type ? raw_type : "";
                c.mysql_type = sqlite_type_to_mysql(c.type);
                c.notnull    = sqlite3_column_int(stmt, 3) != 0;
                const char *raw_dflt = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
                c.dflt = convert_default(raw_dflt ? raw_dflt : "");
                c.pk   = sqlite3_column_int(stmt, 5) != 0;
                // detect autoincrement: INTEGER + PK
                c.autoinc = c.pk && is_integer_type(c.type);
                tbl.cols.push_back(std::move(c));
            }
            sqlite3_finalize(stmt);
        }

        // ---- read index info ----
        {
            sqlite3_stmt *stmt = nullptr;
            std::string sql = "SELECT sql FROM sqlite_master WHERE type='index' AND tbl_name='" +
                              tname + "' AND sql IS NOT NULL";
            sqlite3_prepare_v2(sq, sql.c_str(), -1, &stmt, nullptr);
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                const char *s =
                    reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
                if (s) tbl.idx_sqls.push_back(s);
            }
            sqlite3_finalize(stmt);
        }

        // ---- build MySQL CREATE TABLE ----
        std::string ddl = "CREATE TABLE IF NOT EXISTS `" + tname + "` (\n";
        std::vector<std::string> pk_cols;

        for (size_t i = 0; i < tbl.cols.size(); ++i) {
            auto &c = tbl.cols[i];
            ddl += "    `" + c.name + "` " + c.mysql_type;

            if (c.autoinc) {
                ddl += " AUTO_INCREMENT";
            }

            // NOT NULL / DEFAULT
            if (c.notnull) {
                ddl += " NOT NULL";
                // MySQL TEXT/BLOB cannot have DEFAULT
                if (!is_text_or_blob(c.mysql_type) && !c.dflt.empty() && !c.autoinc) {
                    ddl += " DEFAULT " + c.dflt;
                }
            } else {
                if (!c.dflt.empty()) {
                    ddl += " DEFAULT " + c.dflt;
                }
            }

            // COLLATE for primary key cols (ignore for now)

            if (c.pk) pk_cols.push_back(c.name);

            if (i + 1 < tbl.cols.size()) ddl += ",";
            ddl += "\n";
        }

        if (!pk_cols.empty()) {
            ddl += "    ,PRIMARY KEY (";
            for (size_t i = 0; i < pk_cols.size(); ++i) {
                if (i > 0) ddl += ", ";
                ddl += "`" + pk_cols[i] + "`";
            }
            ddl += ")\n";
        }

        ddl += ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";

        // ---- execute DDL ----
        printf("[%s] creating table... ", tname.c_str());
        fflush(stdout);
        if (mysql_query(my, ddl.c_str())) {
            printf("ERROR: %s\n  %s\n", mysql_error(my), ddl.c_str());
            errors++;
            continue;
        }
        printf("OK\n");

        // ---- create indexes ----
        for (auto &idx : tbl.idx_sqls) {
            // Extract index name from SQL like "CREATE INDEX foo ON tbl(col)"
            std::string idx_sql = idx;
            size_t on_pos = idx_sql.find(" ON ");
            if (on_pos == std::string::npos) continue;

            // Extract index name
            bool is_unique = (idx_sql.find("CREATE UNIQUE INDEX ") == 0);
            size_t name_start = idx_sql.find("INDEX ") + 6;
            if (is_unique) name_start = idx_sql.find("UNIQUE INDEX ") + 13;
            size_t name_end = on_pos;
            // trim trailing spaces
            while (name_end > name_start && idx_sql[name_end - 1] == ' ') name_end--;
            std::string idx_name = idx_sql.substr(name_start, name_end - name_start);

            // Drop index if exists (ignore errors)
            std::string drop_sql = "ALTER TABLE `" + tname + "` DROP INDEX `" + idx_name + "`";
            mysql_query(my, drop_sql.c_str());

            // Create index
            if (mysql_query(my, idx_sql.c_str())) {
                printf("  [WARN] index failed: %s\n  %s\n", mysql_error(my), idx_sql.c_str());
            }
        }

        // ---- read data from SQLite ----
        std::string sel = "SELECT * FROM `" + tname + "`";
        sqlite3_stmt *rds = nullptr;
        if (sqlite3_prepare_v2(sq, sel.c_str(), -1, &rds, nullptr) != SQLITE_OK) {
            printf("  [ERROR] sqlite select: %s\n", sqlite3_errmsg(sq));
            errors++;
            continue;
        }

        int ncol = sqlite3_column_count(rds);
        std::vector<std::string> col_names;
        for (int i = 0; i < ncol; ++i)
            col_names.push_back(sqlite3_column_name(rds, i));

        int row_count = 0;
        while (sqlite3_step(rds) == SQLITE_ROW) {
            // Build INSERT
            std::string ins = "REPLACE INTO `" + tname + "` (";
            for (int i = 0; i < ncol; ++i) {
                if (i > 0) ins += ", ";
                ins += "`" + col_names[i] + "`";
            }
            ins += ") VALUES (";
            for (int i = 0; i < ncol; ++i) {
                if (i > 0) ins += ", ";
                if (sqlite3_column_type(rds, i) == SQLITE_NULL) {
                    ins += "NULL";
                } else {
                    ins += escape(my,
                                  reinterpret_cast<const char *>(sqlite3_column_text(rds, i)),
                                  sqlite3_column_bytes(rds, i));
                }
            }
            ins += ")";

            if (mysql_query(my, ins.c_str())) {
                printf("  [ERROR] insert row %d: %s\n  %s\n", row_count + 1,
                       mysql_error(my), ins.c_str());
                errors++;
                break;
            }
            row_count++;
        }
        sqlite3_finalize(rds);

        printf("  -> %d rows\n", row_count);
        total_rows += row_count;

        if (row_count == 0 && tbl.cols.empty()) {
            printf("  [SKIP] empty table\n");
        }
    }

    mysql_query(my, "SET FOREIGN_KEY_CHECKS = 1");
    mysql_close(my);
    sqlite3_close(sq);

    printf("\n========== DONE ==========\n");
    printf("Rows migrated: %d\n", total_rows);
    printf("Errors:        %d\n", errors);
    return errors > 0 ? 1 : 0;
}
