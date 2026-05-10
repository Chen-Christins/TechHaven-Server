# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```sh
# Full build (creates build/ dir, runs cmake, then make)
make xx

# Build a specific target (e.g., blog, blog_server, gen)
make blog_server

# Regenerate ORM data classes from XML table definitions
make orm
# Equivalent to: bin/orm dbproxy/xml dbproxy/data

# Clean build artifacts (removes build/ directory)
make clean

# Manual CMake build (alternative to the Makefile)
mkdir -p build && cd build && cmake .. && make -j
```

The convenience Makefile in the project root wraps all CMake operations and prints build timing. Outputs: shared library `bin/module/libblog.so`, executable `bin/blog_server`, and ORM generator `bin/gen`.

## Run the Server

```sh
# Foreground
./bin/blog_server

# Daemon mode
./bin/blog_server -d
```

Server listens on `0.0.0.0:8090` (configured in `bin/conf/server.yml`).

## Project Architecture

This is a C++17 blog backend built on **chen-sdk**, a proprietary application framework that provides HTTP serving, database access, configuration, logging, coroutines, and module lifecycle management. The project compiles into a shared library (`libblog.so`) loaded by the framework runtime, plus a thin `blog_server` executable entry point.

### Layer Stack (top to bottom)

```
HTTP Request → Servlet → Manager (in-memory cache) → ORM Data Class → SQLite3
```

### 1. ORM Layer (`dbproxy/`)

Database tables are defined as XML files in `dbproxy/xml/` (e.g., `user.xml`, `article.xml`). The code generator `generator/generator.cc` (built as `bin/gen`) reads these XMLs and generates C++ data access classes into `dbproxy/data/`. Each table gets:
- An info struct (e.g., `UserInfo`) with getters/setters for each column
- A DAO class (e.g., `UserInfoDao`) with static methods for CRUD (`Insert`, `Update`, `Delete`, `Query`, `CreateTableSQLite3`)

Run `make orm` after modifying any XML table definition.

### 2. Manager Layer (`src/manager/`)

Each domain has a singleton manager class (via `chen::Singleton<T>`) that:
- Loads all rows from its table into an in-memory `std::map<int64_t, Info::ptr>` on startup (`loadAll()`)
- Provides query/filter methods operating on the in-memory cache
- Writes changes through to SQLite via the DAO classes

Convention: `typedef chen::Singleton<FooManager> FooMgr;` — access the singleton via `FooMgr::GetInstance()`.

Key managers: `UserMgr`, `ArticleMgr`, `CategoryMgr`, `LabelMgr`, `OrganizationMgr`, `AssignmentMgr`, `ResourceMgr`, `ChunkUploadMgr` (plus relationship managers like `ArticleCategoryRelMgr`).

### 3. Servlet Layer (`src/servlets/`)

HTTP endpoints are servlet classes organized by domain in subdirectories (`article/`, `user/`, `category/`, `label/`, `file/`, `organization/`, `assignment/`, `resource/`).

**Servlet hierarchy:**
- `BlogServlet` (in `src/struct.h`) — base class. Provides `handle()` that wraps `handlePre()` → `handle(..., Result::ptr)` → `handlePost()`. Subclasses override `handle(request, response, session, result)`.
- `BlogLoginedServlet` extends `BlogServlet` — its `handlePre()` calls `initLogin()` to enforce authentication. Use this for endpoints that require a logged-in user.

All servlets return JSON via `Result` struct (`{code, msg, used, data}`). Request parameters are extracted using `DEFINE_AND_CHECK_STRING(result, varname, "param_key")` and `DEFINE_AND_CHECK_TYPE(result, type, varname, "param_key")` macros from `src/util.h`.

Servlets are registered in `BlogModule::registerServlets()` in `src/blog_module.cc` with URL paths like `/user/login`, `/article/create`, etc.

### 4. Module Lifecycle (`src/blog_module.cc`)

`BlogModule` (extends `chen::Module`) is the plugin entry point. Exported via `extern "C" CreateModule()/DestroyModule()`. Lifecycle:

1. `onLoad()` — module loaded
2. `onServerReady()` — **main initialization**:
   - `initDB()` — opens/creates SQLite database, creates tables if missing
   - `loadAllData()` — loads all tables into in-memory manager caches
   - `registerServlets()` — maps URL paths to servlet instances on the HTTP server
3. `onServerUp()` — server running

### Search Index (`src/index.cc`)

Bitmap-based multi-index for article search. Indexes articles by user, category, label, state, year-month, channel, plus full-text word index.

### Key Patterns

- **In-memory reads, write-through**: All reads hit the manager's in-memory map. Writes go to SQLite first, then update the in-memory cache on success.
- **Include aggregation**: `src/include/tables.h`, `managers.h`, `servlets.h` aggregate all headers. Source files typically include these aggregates plus `chen/` framework headers and `src/util.h`.
- **Error handling**: Servlets use `do { ... } while (0)` + `break` pattern for flow control. Errors are reported via `result->setResult(code, msg)`. Logging uses `chen::Logger` with `LOG_ROOT()`-created loggers.
- **File uploads**: Supports chunked upload via `ChunkUploadServlet` (`/upload/init`, `/upload/chunk`, `/upload/complete`, `/upload/cancel`, `/upload/status`).

## Configuration

All config in `bin/conf/` (YAML format):
- `server.yml` — HTTP bind address/port, keepalive, timeouts
- `system.yml` — work path, PID file, email service
- `sqlite3.yml` — database path and pragmas
- `redis.yml` — Redis connection
- `log.yml` — loggers and appenders (root, system, access)
- `worker.yml` / `fox_thread.yml` — thread pool sizes

## External Dependencies

**System packages**: g++ (C++20), cmake, libboost-all-dev, libsqlite3-dev, libssl-dev, libevent-dev

**Built from source** (by `build.sh`): yaml-cpp, ragel, tinyxml2, hiredis-vip, jsoncpp, protobuf

**Bundled**: chen-sdk (`chen-sdk-1.0.0/`), ONNX Runtime (`3rdparty/onnxruntime/`)

## Testing

No automated test framework. Test data lives in `tests/db/` as SQL files. Load them with:
```sh
tests/load_db_data.sh
```

## Deployment

```sh
# Docker
docker build -t blog-backend .
docker run -p 8080:8080 blog-backend

# SCP-based (requires expect)
./deploy.sh <remote-ip> <username> <password>
```
