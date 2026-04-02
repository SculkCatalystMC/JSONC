# Header-Only JSONC Library for Modern C++

<p align="center">
    <a href="./README.md"><img alt="English" src="https://img.shields.io/badge/Language-English-1f6feb"></a>
    <a href="./README.zh.md"><img alt="简体中文" src="https://img.shields.io/badge/Language-%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-0f766e"></a>
</p>

JSONC 是一个面向 C++23 的 JSON with Comments 库。
它的 C++ API 是纯头文件实现，同时仓库也提供了可选的 C API 静态库和动态库构建方案。
仓库中同时保留了 CMake 和 xmake 两套构建系统。

## ✨ 特性

- 纯头文件的 C++ API。
- 同时支持 JSON 和 JSONC 文档。
- 支持有序对象模型和无序对象模型。
- 可选的 JSONC 注释保留能力。
- 提供适合 C 或混合语言工程使用的可选编译型 C API。
- 可配置的错误处理模型：异常、std::expected 或无异常解析。
- 同时支持 CMake 和 xmake 构建。
- 支持将 C API 构建为静态库或动态库。

## 🧩 项目结构定位

仓库可以分为两层：

- 位于 `include/jsonc` 的纯头文件 C++ 库。
- 位于 `src/jsonc.cpp`、头文件在 `include/jsonc-c` 中的可选编译型 C API 桥接层。

如果你只使用 C++ API，则不需要构建项目。
只有在你需要导出的 C API 库时，才需要执行构建。

## 🧠 类型别名

主要的 C++ 入口定义在 `include/jsonc/jsonc.hpp` 中：

- `sculk::jsonc::json`：无序 JSON，不保留注释。
- `sculk::jsonc::ordered_json`：有序 JSON，不保留注释。
- `sculk::jsonc::jsonc`：无序 JSONC，支持注释。
- `sculk::jsonc::ordered_jsonc`：有序 JSONC，支持注释。

如果你需要保留或编辑注释，应使用 JSONC 变体。

## 🛡️ 错误处理

库支持三种错误处理方式：

- 默认基于异常。
- `JSONC_USE_EXPECTED`：返回基于 `std::expected` 的结果。
- `JSONC_NO_EXCEPTION`：关闭异常，并在支持的解析路径上使用非抛出式结果。

如果启用了 `JSONC_USE_EXPECTED`，则会自动启用 `JSONC_NO_EXCEPTION`。

## 🧰 构建要求

- CMake 3.24 或更新版本。
- 支持 C++23 的编译器。
- Windows：MSVC，或能配合合适生成器工作的 LLVM/Clang。
- Linux/macOS：从原始工程设置来看，Clang 是更符合预期的工具链。

## 🏗️ 构建系统

当前仓库同时保留 CMake 和 xmake 两套构建文件。

### 📘 纯头文件 C++ 用法

对于 C++ 用法，直接包含头文件即可：

```cpp
#include <jsonc/jsonc.hpp>
```

除非你需要 C API 库，否则不需要任何编译步骤。

## ⚙️ 使用 CMake 构建 C API

### 📦 配置静态 C API 库

```bash
cmake -S . -B build -DJSONC_BUILD_C_API=ON -DJSONC_LIBRARY_KIND=static -DJSONC_ENABLE_TEST=OFF
cmake --build build --config Release
```

### 🔌 配置动态 C API 库

```bash
cmake -S . -B build-shared -DJSONC_BUILD_C_API=ON -DJSONC_LIBRARY_KIND=shared -DJSONC_ENABLE_TEST=OFF
cmake --build build-shared --config Release
```

### 🧱 仅生成头文件目标

```bash
cmake -S . -B build-header -DJSONC_BUILD_C_API=OFF -DJSONC_ENABLE_TEST=OFF
```

### 🧪 构建测试可执行文件

```bash
cmake -S . -B build-test -DJSONC_BUILD_C_API=OFF -DJSONC_ENABLE_TEST=ON
cmake --build build-test --config Release
```

### 🔧 重要的 CMake 选项

- `JSONC_BUILD_C_API=ON|OFF`：是否构建编译型 C API 库。
- `JSONC_LIBRARY_KIND=static|shared`：选择库类型。
- `JSONC_ENABLE_TEST=ON|OFF`：启用或关闭 `test` 目标。

CMake 中可用的目标为：

- `JSONC::jsonc`：纯头文件 C++ 目标。
- `JSONC::capi`：编译型 C API 目标，仅在 `JSONC_BUILD_C_API=ON` 时可用。

当构建动态 C API 库时，构建脚本会将生成的二进制复制到 `bin/jsonc-<platform>-<arch>`，并在 `artifacts` 中生成 zip 包。

## ⚙️ 使用 xmake 构建 C API

### 📦 静态库

```bash
xmake f -m release --kind=static --enable_test=false
xmake
```

### 🔌 动态库

```bash
xmake f -m release --kind=shared --enable_test=false
xmake
```

### 🧪 测试可执行文件

```bash
xmake f -m release --kind=static --enable_test=true
xmake
```

## 🔗 在其他 CMake 工程中使用

对于纯头文件 C++ 用法：

```cmake
add_subdirectory(path/to/JSONC)
target_link_libraries(your_target PRIVATE JSONC::jsonc)
```

对于编译型 C API：

```cmake
add_subdirectory(path/to/JSONC)
target_link_libraries(your_target PRIVATE JSONC::capi)
```

## 🧾 C++ 示例

```cpp
#include <jsonc/jsonc.hpp>
#include <iostream>

int main() {
    auto result = sculk::jsonc::ordered_jsonc::parse(R"({
        // value comment
        "value": 42,
        "list": [1, 2, 3]
    })", false, false);

#ifdef JSONC_USE_EXPECTED
    if (!result) {
        std::cerr << result.error().mErrorInfo << '\n';
        return 1;
    }
    auto doc = *result;
#else
    auto doc = result;
#endif

    doc["value"] = 64;
    doc["added"] = sculk::jsonc::ordered_jsonc::array({1, 2, 3, 4});

    std::cout << doc.dump(4, false, false, true) << '\n';
}
```

## 🔩 C API

C API 位于 `include/jsonc-c/jsonc.h`。
它由编译生成的库提供，实现了解析、查询、修改、序列化以及对象、数组和标量值的内存管理接口。

典型的 C API 使用流程：

1. 使用 `jsonc_parse_content` 解析文本。
2. 使用对象或数组辅助函数查询和修改数据。
3. 使用 `jsonc_variant_dump`、`jsonc_object_dump` 或 `jsonc_array_dump` 序列化输出。
4. 使用对应的 `jsonc_free_*` 函数释放句柄。

## 🗂️ 仓库结构

- `include/jsonc`：纯头文件的公开 C++ 库。
- `include/jsonc-c`：公开的 C 头文件。
- `src`：编译型 C API 桥接层源码。
- `test`：测试和示例代码。
- `artifacts`：共享库打包产物。
- `bin`：复制后的运行产物。

## 📝 说明

- CMake 和 xmake 会继续同时保留。
- C++ API 是纯头文件实现；只有 C API 库和测试程序需要编译。
- C API 库的构建是可选的，默认不构建。

## 🤝 反馈与贡献

欢迎任何形式的反馈。

- 通过 Issues 提交 Bug、问题或改进建议：https://github.com/SculkCatalystMC/JSONC/issues
- 通过 Pull Requests 提交修复和增强：https://github.com/SculkCatalystMC/JSONC/pulls

PR 建议尽量保持改动聚焦，并附上简短说明。

## 📄 许可证

本项目采用 Mozilla Public License 2.0（MPL-2.0）开源。
完整许可证文本见 [LICENSE](LICENSE)。

MPL-2.0 核心要求（摘要）：

- 若你修改了受 MPL 覆盖的文件并进行分发，这些被修改的文件需继续以 MPL-2.0 许可发布。
- 需保留受覆盖源码文件中的版权声明与许可证声明。
- 若你分发二进制或可执行形式，需要提供对应的受覆盖源码获取方式。
- 你可以在更大的工程中与其他许可代码组合使用，但受覆盖文件仍需满足 MPL 义务。

以上仅为便于理解的摘要，法律效力以 [LICENSE](LICENSE) 正文为准。

### 版权所有 © 2026 SculkCatalystMC。保留所有权利。