# AGENTS.md

This file provides mandatory rules for AI agents working in this repository. Violating these rules may cause segfaults or security issues.

**When writing or modifying any C++ code, invoke the `/cpp-code-style` skill first for complete coding conventions.**

## 1. Null Pointer Safety (MANDATORY)

**Any pointer from an external call must be null-checked before dereference. No exceptions.**

### Never chain pointer access:

```cpp
// ❌ ALWAYS WRONG
int32_t role = UserMgr::GetInstance()->get(uid)->getRole();
auto org_role = getByOrgAndUser(org_id, uid)->getRole();
```

### Always store, check, then use:

```cpp
// ✅ REQUIRED PATTERN
auto user = UserMgr::GetInstance()->get(uid);
if (!user) {
    result->setErrno(errcode::USER_NOT_FOUND);
    break;
}
int32_t role = user->getRole();
```

### Before any Manager::get(), check the key:

```cpp
int64_t uid = getUserId(request);
if (!uid) {                    // check id first
    result->setErrno(errcode::NOT_LOGIN);
    break;
}
auto info = Mgr::GetInstance()->get(id);
if (!info) {                   // check pointer second
    result->setErrno(errcode::SOME_NOT_FOUND);
    break;
}
```

### Common null-able return values:

| Return | Check |
|--------|-------|
| `*Mgr::GetInstance()->get(id)` | `if (!ptr)` |
| `*Mgr::GetInstance()->getByXxx(...)` | `if (!ptr)` |
| `*Dao::Query(...)` | `if (!ptr)` |
| `GetDB()` | `if (!db)` |
| `db->prepare(sql)` | `if (!stmt)` |
| `stmt->query()` | `if (!rt)` |
| `getUserId(request)` | `if (!uid)` |
| `m_cache.get(key)` | check before use |

## 2. Servlet Patterns

### Servlet base classes:
- `BlogServlet` — public endpoint (no login required)
- `BlogLoginedServlet` — requires login (`handlePre` calls `initLogin()`)

### Handler structure:
```cpp
int32_t XxxServlet::handle(request, response, session, result) {
    do {
        // 1. Extract params with DEFINE_AND_CHECK_*
        // 2. Null-check all pointers
        // 3. Business logic
        // 4. Set result data
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}
```

### Error reporting:
```cpp
result->setErrno(errcode::NOT_LOGIN);                    // fixed message from errors.json
result->setErrno(errcode::ACCESS_DENIED, "custom msg");  // custom message
```

### Route registration:
```cpp
// In blog_module.cc → registerServlets()
dp->addServlet("/api/v1/xxx/yyy", XX(XxxServlet));
```

## 3. File Header Comments

New header files (`.h`) require a Doxygen file comment. **Always ask the user which license to use** for the `@copyright` field — do not assume:

```cpp
/**
 * @file xxx.h
 * @brief ...
 * @author Christins
 * @date YYYY-MM-DD
 * @copyright <ask user>
 */
```

Source files (`.cc`) do NOT need this header.

## 4. New Feature Checklist

When adding a new feature:

1. **XML table** (`dbproxy/xml/`) — define table schema, then `make orm`
2. **Manager** (`src/manager/`) — singleton + LRU cache + CRUD methods + null checks
3. **Servlet** (`src/servlets/<domain>/`) — follow handler pattern above
4. **Includes** — update `src/include/tables.h`, `managers.h`, `servlets.h`
5. **Registration** — `blog_module.cc`: table create, migrate, servlet route
6. **Error codes** — `src/error_codes.h` + `errors.json`

## 4. Code Style Quick Reference

| Element | Convention |
|---------|-----------|
| Classes | `PascalCase` |
| Member variables | `m_snake_case` |
| Local variables | `snake_case` |
| Member functions | `camelCase` |
| Static functions | `PascalCase` |
| Macros | `UPPER_SNAKE_CASE` |
| Brace (class/namespace) | same line |
| Brace (function) | new line |
| Brace (if/for/while) | same line, never omit |
| Null pointer | `nullptr` (never `NULL` or `0`) |
| Indent | 4 spaces (no tabs) |
