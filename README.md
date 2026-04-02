# Header-Only JSONC Library for Modern C++

<p align="center">
	<a href="./README.md"><img alt="English" src="https://img.shields.io/badge/Language-English-1f6feb"></a>
	<a href="./README.zh.md"><img alt="Simplified Chinese" src="https://img.shields.io/badge/Language-%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-0f766e"></a>
</p>

JSONC is a C++23 JSON with Comments library.
The C++ API is header-only, while the repository also provides optional compiled static or shared libraries for the C API.
Both CMake and xmake build systems are kept in the repository.

## ✨ Features

- Header-only C++ API.
- JSON and JSONC document support.
- Ordered and unordered object models.
- Optional comment preservation for JSONC documents.
- Optional compiled C API for embedding in C or mixed-language projects.
- Configurable error model: exceptions, std::expected, or no-exception parsing.
- CMake and xmake build support.
- Optional static and shared library outputs for the C API.

## 🧩 Project Model

The repository contains two layers:

- Header-only C++ library in `include/jsonc`.
- Optional compiled C API bridge in `src/jsonc.cpp` with headers in `include/jsonc-c`.

If you only use the C++ API, you do not need to build the project.
Build output is only required when you want the exported C API library.

## 🧠 Type Aliases

The main C++ entry points are defined in `include/jsonc/jsonc.hpp`:

- `sculk::jsonc::json`: unordered JSON without comment support.
- `sculk::jsonc::ordered_json`: ordered JSON without comment support.
- `sculk::jsonc::jsonc`: unordered JSONC with comment support.
- `sculk::jsonc::ordered_jsonc`: ordered JSONC with comment support.

Use the JSONC variants when you need to preserve or edit comments.

## 🛡️ Error Handling

The library supports three error-handling styles:

- Default exception-based behavior.
- `JSONC_USE_EXPECTED`: returns `std::expected`-based results.
- `JSONC_NO_EXCEPTION`: disables exceptions for parse paths that support non-throwing results.

If `JSONC_USE_EXPECTED` is enabled, `JSONC_NO_EXCEPTION` is enabled automatically.

## 🧰 Build Requirements

- CMake 3.24 or newer.
- A compiler with C++23 support.
- Windows: MSVC or LLVM/Clang with a compatible generator.
- Linux/macOS: Clang is the intended toolchain in the original project settings.

## 🏗️ Build Systems

This repository keeps both CMake and xmake build files.

### 📘 C++ Header-Only Usage

For C++ usage, the library can be consumed directly from the headers:

```cpp
#include <jsonc/jsonc.hpp>
```

No compilation step is required unless you need the C API library.

## ⚙️ Build the C API with CMake

### 📦 Configure a Static C API Library Build

```bash
cmake -S . -B build -DJSONC_BUILD_C_API=ON -DJSONC_LIBRARY_KIND=static -DJSONC_ENABLE_TEST=OFF
cmake --build build --config Release
```

### 🔌 Configure a Shared C API Library Build

```bash
cmake -S . -B build-shared -DJSONC_BUILD_C_API=ON -DJSONC_LIBRARY_KIND=shared -DJSONC_ENABLE_TEST=OFF
cmake --build build-shared --config Release
```

### 🧱 Header-Only Target Only

```bash
cmake -S . -B build-header -DJSONC_BUILD_C_API=OFF -DJSONC_ENABLE_TEST=OFF
```

### 🧪 Build the Test Executable

```bash
cmake -S . -B build-test -DJSONC_BUILD_C_API=OFF -DJSONC_ENABLE_TEST=ON
cmake --build build-test --config Release
```

### 🔧 Important CMake Options

- `JSONC_BUILD_C_API=ON|OFF`: builds or skips the compiled C API library.
- `JSONC_LIBRARY_KIND=static|shared`: selects the library type.
- `JSONC_ENABLE_TEST=ON|OFF`: enables or disables the `test` target.

The CMake targets are:

- `JSONC::jsonc`: header-only C++ target.
- `JSONC::capi`: compiled C API target, available when `JSONC_BUILD_C_API=ON`.

When building the shared C API library, the build copies the produced binary to `bin/jsonc-<platform>-<arch>` and creates a zip package in `artifacts`.

## ⚙️ Build the C API with xmake

### 📦 Static Library

```bash
xmake f -m release --kind=static --enable_test=false
xmake
```

### 🔌 Shared Library

```bash
xmake f -m release --kind=shared --enable_test=false
xmake
```

### 🧪 Test Executable

```bash
xmake f -m release --kind=static --enable_test=true
xmake
```

## 🔗 Use from Another CMake Project

For header-only C++ usage:

```cmake
add_subdirectory(path/to/JSONC)
target_link_libraries(your_target PRIVATE JSONC::jsonc)
```

For the compiled C API library:

```cmake
add_subdirectory(path/to/JSONC)
target_link_libraries(your_target PRIVATE JSONC::capi)
```

## 🧾 C++ Example

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

The C API is available in `include/jsonc-c/jsonc.h`.
It is implemented in the compiled library target and exposes parsing, querying, mutation, dumping, and memory management helpers for objects, arrays, and scalar values.

Typical C API flow:

1. Parse content with `jsonc_parse_content`.
2. Query or mutate values through the object/array helpers.
3. Serialize with `jsonc_variant_dump`, `jsonc_object_dump`, or `jsonc_array_dump`.
4. Release allocated handles with the matching `jsonc_free_*` function.

## 🗂️ Repository Layout

- `include/jsonc`: header-only public C++ library.
- `include/jsonc-c`: public C headers.
- `src`: compiled C API bridge sources.
- `test`: test and usage examples.
- `artifacts`: packaged shared-library outputs.
- `bin`: copied runtime outputs.

## 📝 Notes

- Both CMake and xmake are intentionally retained.
- The C++ API is header-only; building is only required for the C API library and test executable.
- Building the C API library is optional and disabled by default.

## 🤝 Feedback & Contributions

Feedback is very welcome.

- Report bugs, ask questions, or suggest improvements via Issues: https://github.com/SculkCatalystMC/JSONC/issues
- Submit fixes and enhancements via Pull Requests: https://github.com/SculkCatalystMC/JSONC/pulls

For PRs, focused changes with a short explanation are preferred.

## 📄 License

This project is licensed under the Mozilla Public License 2.0 (MPL-2.0).
See the full text in [LICENSE](LICENSE).

Core MPL-2.0 requirements (summary):

- If you modify MPL-covered files and distribute them, those modified files must remain under MPL-2.0.
- You must keep existing copyright and license notices in covered source files.
- If you distribute binaries/executables, you must make the corresponding covered source code available.
- You may combine this project with code under other licenses in a larger work, as long as MPL obligations for covered files are respected.

This summary is for convenience only. The [LICENSE](LICENSE) file is the authoritative legal text.

### Copyright © 2026 SculkCatalystMC. All rights reserved.