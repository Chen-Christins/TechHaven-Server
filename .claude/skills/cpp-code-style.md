---
name: cpp-code-style
description: C++ coding style conventions for this project. Use when writing or editing C++ code.
user-invocable: false
---

# C++ 编码规范

本规范适用于此项目的所有 C++ 代码。编写或修改代码时必须遵守。

## 控制流大括号

`if`、`for`、`while`、`else` 必须使用大括号，即使只有一行语句。大括号放在同一行（K&R 风格），后面紧跟换行。

```cpp
// 正确
if (!uid) {
    result->setResult(500, "not login");
    break;
}

// 正确 - 单行也要大括号
if (smtpPort > 0) {
    info->setSmtpPort(smtpPort);
}

// 错误 - 缺少大括号
if (!param.empty()) info->setSiteName(param);

// 错误 - 缺少大括号
if (request->checkGetParamAs("enableRegistration", bVal))
    info->setEnableRegistration(bVal ? 1 : 0);
```

## 缩进

使用 Tab 缩进（项目已有风格）。

## Include 顺序

1. 自身头文件（.h 对应 .cc）
2. 框架头文件（`<chen/...>`）
3. 项目其他头文件（相对路径，`"..."`）

```cpp
#include "category_create_servlet.h"

#include <chen/log/log.h>

#include "../../manager/category_manager.h"
#include "../../util.h"

```

## 命名空间

推荐使用 C++17 嵌套命名空间语法，简洁清晰：

```cpp
namespace blog::servlet {

// 代码...

} // namespace blog::servlet
```

## 函数参数对齐

续行参数以逗号开头，对齐到前一行参数起始位置：

```cpp
int32_t CategoryCreateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
```

## Servlet 模式

- 继承 `BlogServlet`（公开接口）或 `BlogLoginedServlet`（需要登录）
- 构造函数传入 servlet 名称：`BlogLoginedServlet("CategoryCreateServlet")`
- 使用 `do { ... } while (0)` + `break` 模式进行错误处理
- 通过 `result->setResult(code, msg)` 报告错误
- 使用 `DEFINE_AND_CHECK_STRING(result, var, "param_key")` 获取必填字符串参数
- 使用 `DEFINE_AND_CHECK_TYPE(result, type, var, "param_key")` 获取必填类型参数
- 框架已自动解析 JSON body，通过 `request->getParam("key")` 即可获取参数值
- 使用 `request->getParamAs<T>("key", default)` 获取带默认值的可选参数
- 使用 `request->checkGetParamAs("key", var)` 检查参数是否存在

## Logger 声明

每个 .cc 文件顶部声明静态 logger：

```cpp
static chen::Logger::ptr logger = LOG_ROOT();
```

## 类成员初始化

构造函数使用 `:BaseClass("name")` 初始化列表，冒号紧跟函数签名：

```cpp
CategoryCreateServlet::CategoryCreateServlet()
    : BlogLoginedServlet("CategoryCreateServlet") {
}
```
