// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <cstdint>
#include <format>

#ifdef JSONC_USE_EXPECTED
#ifndef JSONC_NO_EXCEPTION
#define JSONC_NO_EXCEPTION
#endif
#include <expected>
#include <functional>
#endif

#ifndef JSONC_NO_EXCEPTION
#include <stdexcept>
#endif


#ifndef JSONC_NO_EXCEPTION
namespace sculk::jsonc::inline abi_v1_4_1::detail {

class out_of_range : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class type_error : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class key_error : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class parse_error : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

#define _JSONC_THROW_EXCEPTION(TYPE, EXCEPTION) throw TYPE(EXCEPTION)

#define JSONC_RESULT(...)       __VA_ARGS__
#define _JSONC_MAKE_RESULT(...) __VA_ARGS__
#define _JSONC_MAKE_VOID_RESULT()

} // namespace sculk::jsonc::inline abi_v1_4_1::detail
#else
#ifdef JSONC_USE_EXPECTED
namespace sculk::jsonc::inline abi_v1_4_1::detail {

enum class error_code : std::uint8_t {
    out_of_range = 0,
    type_error   = 1,
    key_error    = 2,
    parse_error  = 3,
};

#ifndef JSONC_EXPECTED_NO_DETAIL_INFO
struct ErrorInfo {
    error_code  mErrorCode;
    std::string mErrorInfo;

    explicit ErrorInfo(error_code error_code, std::string_view error_info) : mErrorCode(error_code), mErrorInfo(error_info) {}
};
#define _JSONC_THROW_EXCEPTION(TYPE, EXCEPTION) return std::unexpected(ErrorInfo(error_code::TYPE, EXCEPTION))
#else
using ErrorInfo = error_code;
#define _JSONC_THROW_EXCEPTION(TYPE, EXCEPTION) return std::unexpected(error_code::TYPE)
#endif

template <typename T = void>
using Result = std::expected<std::conditional_t<std::is_reference_v<T>, std::reference_wrapper<std::remove_reference_t<T>>, T>, ErrorInfo>;

template <typename T>
    requires(!std::is_void_v<T>)
[[nodiscard]] constexpr Result<T> make_result(T&& t) noexcept {
    if constexpr (std::is_reference_v<T>) {
        static_assert(!std::is_rvalue_reference_v<T&&>, "make_result does not accept rvalue references");
        return Result<T>{std::ref(t)};
    } else {
        return Result<T>{std::forward<T>(t)};
    }
}
template <typename T = void>
    requires std::is_void_v<T>
[[nodiscard]] constexpr Result<void> make_result() noexcept {
    return Result<void>{};
}

#define _JSONC_MAKE_RESULT(...)   make_result(__VA_ARGS__)
#define _JSONC_MAKE_VOID_RESULT() make_result()
#define JSONC_RESULT(...)         detail::Result<__VA_ARGS__>

} // namespace sculk::jsonc::inline abi_v1_4_1::detail

#else
#define _JSONC_THROW_EXCEPTION(TYPE, EXCEPTION) std::abort()

#define _JSONC_MAKE_RESULT(RESULT) RESULT
#define _JSONC_MAKE_VOID_RESULT()
#define JSONC_RESULT(...) __VA_ARGS__
#endif
#endif

#define _JSONC_OUT_OF_RANGE(ERROR) _JSONC_THROW_EXCEPTION(out_of_range, ERROR)
#define _JSONC_TYPE_ERROR(ERROR)   _JSONC_THROW_EXCEPTION(type_error, ERROR)
#define _JSONC_KEY_ERROR(ERROR)    _JSONC_THROW_EXCEPTION(parse_error, ERROR)
#if !defined(JSONC_NO_EXCEPTION) || defined(JSONC_USE_EXPECTED)
#define _JSONC_PARSE_ERROR(ERROR) _JSONC_THROW_EXCEPTION(key_error, std::format("jsonc parse error: {}", ERROR))
#define JSONC_PARSE_RESULT(...)   JSONC_RESULT(__VA_ARGS__)
#else
#include <optional>
#define _JSONC_PARSE_ERROR(ERROR) return std::nullopt
#define JSONC_PARSE_RESULT(...)   std::optional<__VA_ARGS__>
#endif