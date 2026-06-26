// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#ifndef JSONC_NO_EXCEPTION
#define JSONC_NO_EXCEPTION
#endif
#ifdef JSONC_USE_EXPECTED
#undef JSONC_USE_EXPECTED
#endif
#include "sculk/jsonc/jsonc.hpp"
#include "sculk/jsonc-c/jsonc.h"
#include <cstring>
#include <mutex>

#define JSONC_VERSION_MAJOR 1
#define JSONC_VERSION_MINOR 4
#define JSONC_VERSION_PATCH 1

#define _STRINGIZE(S) #S
#define STRINGIZE(S)  _STRINGIZE(S)

inline size_t copy_string(char* data, size_t length, const std::string& str) {
    if (length > str.size()) {
        std::memcpy(data, str.data(), str.size());
        data[str.size()] = '\0';
        return str.size();
    }
    return 0;
}

struct ErrorManager {
    std::string        mLastError{};
    mutable std::mutex mMutex{};

    const char* getLastError() const {
        std::lock_guard lock{mMutex};
        return mLastError.c_str();
    }

    void setLastError(std::string_view error) {
        std::lock_guard lock{mMutex};
        mLastError = error;
    }

    static ErrorManager& getInstance() {
        static ErrorManager instance{};
        return instance;
    }
};

extern "C" {

const char* jsonc_get_library_version_string() {
    return "v" STRINGIZE(JSONC_VERSION_MAJOR) "." STRINGIZE(JSONC_VERSION_MINOR) "." STRINGIZE(JSONC_VERSION_PATCH);
}

uint8_t jsonc_get_library_version_major() { return JSONC_VERSION_MAJOR; }
uint8_t jsonc_get_library_version_minor() { return JSONC_VERSION_MINOR; }
uint8_t jsonc_get_library_version_patch() { return JSONC_VERSION_PATCH; }

const char* jsonc_get_last_error() { return ErrorManager::getInstance().getLastError(); }

jsonc_variant_t jsonc_parse_content(const char* content, bool allow_trailing_comma) {
    auto result = sculk::jsonc::ordered_jsonc::parse(content, allow_trailing_comma);
    if (result) { return new (std::nothrow) sculk::jsonc::ordered_jsonc(std::move(*result)); }
    ErrorManager::getInstance().setLastError(result.error());
    return nullptr;
}

int jsonc_get_variant_type(jsonc_variant_t handle) { return static_cast<int>(static_cast<sculk::jsonc::ordered_jsonc*>(handle)->type()); }

bool     jsonc_variant_to_bool(jsonc_variant_t handle) { return static_cast<sculk::jsonc::ordered_jsonc*>(handle)->get<bool>(); }
int64_t  jsonc_variant_to_signed(jsonc_variant_t handle) { return static_cast<sculk::jsonc::ordered_jsonc*>(handle)->get<int64_t>(); }
uint64_t jsonc_variant_to_unsigned(jsonc_variant_t handle) { return static_cast<sculk::jsonc::ordered_jsonc*>(handle)->get<uint64_t>(); }
double   jsonc_variant_to_float(jsonc_variant_t handle) { return static_cast<sculk::jsonc::ordered_jsonc*>(handle)->get<double>(); }

const char* jsonc_variant_as_string(jsonc_variant_t handle) { return static_cast<sculk::jsonc::ordered_jsonc*>(handle)->as<std::string>().c_str(); }
const char* jsonc_variant_as_big_int(jsonc_variant_t handle) {
    return static_cast<sculk::jsonc::ordered_jsonc*>(handle)->as<sculk::jsonc::detail::basic_big_integer>().view_.c_str();
}
const char* jsonc_variant_as_high_precision_float(jsonc_variant_t handle) {
    return static_cast<sculk::jsonc::ordered_jsonc*>(handle)->as<sculk::jsonc::detail::basic_high_precision_float>().view_.c_str();
}

jsonc_object_t jsonc_variant_as_object(jsonc_variant_t handle) {
    return &static_cast<sculk::jsonc::ordered_jsonc*>(handle)->as<sculk::jsonc::ordered_jsonc::object_type>();
}
jsonc_array_t jsonc_variant_as_array(jsonc_variant_t handle) {
    return &static_cast<sculk::jsonc::ordered_jsonc*>(handle)->as<sculk::jsonc::ordered_jsonc::array_type>();
}

size_t jsonc_variant_get_comments_before(jsonc_variant_t handle, char* data, size_t length) {
    if (static_cast<sculk::jsonc::ordered_jsonc*>(handle)->has_before_comments()) {
        std::string result{};
        for (auto& comment : static_cast<sculk::jsonc::ordered_jsonc*>(handle)->before_comments()) {
            result.append(comment);
            result.push_back('\n');
        }
        result.pop_back();
        return copy_string(data, length, result);
    }
    return 0;
}

size_t jsonc_variant_get_comments_after(jsonc_variant_t handle, char* data, size_t length) {
    if (static_cast<sculk::jsonc::ordered_jsonc*>(handle)->has_after_comments()) {
        std::string result{};
        for (auto& comment : static_cast<sculk::jsonc::ordered_jsonc*>(handle)->after_comments()) {
            result.append(comment);
            result.push_back('\n');
        }
        result.pop_back();
        return copy_string(data, length, result);
    }

    return 0;
}

void jsonc_variant_merge_comments(jsonc_variant_t lhs, jsonc_variant_t rhs) {
    static_cast<sculk::jsonc::ordered_jsonc*>(lhs)->merge_comments(*static_cast<sculk::jsonc::ordered_jsonc*>(rhs));
}

void jsonc_variant_set_comments_before(jsonc_variant_t handle, const char* comments) {
    static_cast<sculk::jsonc::ordered_jsonc*>(handle)->before_comments() = sculk::jsonc::detail::split_comments(comments);
}
void jsonc_variant_set_comments_after(jsonc_variant_t handle, const char* comments) {
    static_cast<sculk::jsonc::ordered_jsonc*>(handle)->after_comments() = sculk::jsonc::detail::split_comments(comments);
}

size_t jsonc_variant_dump(
    jsonc_variant_t handle,
    char*           data,
    size_t          length,
    int             indent,
    bool            ensure_ascii,
    bool            ignore_comments,
    bool            multi_line_comments_format
) {
    auto result = static_cast<sculk::jsonc::ordered_jsonc*>(handle)->dump(indent, ensure_ascii, ignore_comments, multi_line_comments_format);
    return copy_string(data, length, result);
}

bool jsonc_object_contains(jsonc_object_t handle, const char* key) {
    return static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->contains(key);
}

bool jsonc_object_value_is_any_int_type(jsonc_object_t handle, const char* key) {
    return static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).is_number_integer();
}

bool jsonc_object_value_is_any_float_type(jsonc_object_t handle, const char* key) {
    return static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).is_number_float();
}

bool jsonc_object_value_is_any_number_type(jsonc_object_t handle, const char* key) {
    return static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).is_number();
}

int jsonc_object_get_type(jsonc_object_t handle, const char* key) {
    return static_cast<int>(static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).type());
}

bool jsonc_object_get_bool(jsonc_object_t handle, const char* key) {
    return static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).get<bool>();
}
void jsonc_object_set_bool(jsonc_object_t handle, const char* key, bool value) {
    static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key) = value;
}

int64_t jsonc_object_get_signed(jsonc_object_t handle, const char* key) {
    return static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).get<int64_t>();
}
void jsonc_object_set_signed(jsonc_object_t handle, const char* key, int64_t value) {
    static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key) = value;
}

uint64_t jsonc_object_get_unsigned(jsonc_object_t handle, const char* key) {
    return static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).get<uint64_t>();
}
void jsonc_object_set_unsigned(jsonc_object_t handle, const char* key, uint64_t value) {
    static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key) = value;
}

size_t jsonc_object_get_any_int(jsonc_object_t handle, char* data, size_t length, const char* key) {
    auto result = static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).get_any_int_view();
    return copy_string(data, length, result);
}
bool jsonc_object_set_any_int(jsonc_object_t handle, const char* key, const char* value) {
    auto val = sculk::jsonc::ordered_jsonc::from_any_int(value);
    if (val) { static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key) = *val; }
    return val.has_value();
}

double jsonc_object_get_float(jsonc_object_t handle, const char* key) {
    return static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).get<double>();
}
void jsonc_object_set_float(jsonc_object_t handle, const char* key, double value) {
    static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key) = value;
}

size_t jsonc_object_get_any_float(jsonc_object_t handle, char* data, size_t length, const char* key) {
    auto result = static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).get_any_float_view();
    return copy_string(data, length, result);
}
bool jsonc_object_set_any_float(jsonc_object_t handle, const char* key, const char* value) {
    auto val = sculk::jsonc::ordered_jsonc::from_any_float(value);
    if (val) { static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key) = *val; }
    return val.has_value();
}

size_t jsonc_object_get_any_number(jsonc_object_t handle, char* data, size_t length, const char* key) {
    auto result = static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).get_any_number_view();
    return copy_string(data, length, result);
}
bool jsonc_object_set_any_number(jsonc_object_t handle, const char* key, const char* value) {
    auto val = sculk::jsonc::ordered_jsonc::from_any_number(value);
    if (val) { static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key) = *val; }
    return val.has_value();
}

const char* jsonc_object_get_string(jsonc_object_t handle, const char* key) {
    return static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).as<std::string>().c_str();
}
void jsonc_object_set_string(jsonc_object_t handle, const char* key, const char* value) {
    static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key) = std::string_view(value);
}

jsonc_object_t jsonc_object_get_object(jsonc_object_t handle, const char* key) {
    return &static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).as<sculk::jsonc::ordered_jsonc::object_type>();
}
jsonc_object_t jsonc_object_add_new_object(jsonc_object_t handle, const char* key) {
    auto& res = static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key) = sculk::jsonc::ordered_jsonc::object_type();
    return &res.as<sculk::jsonc::ordered_jsonc::object_type>();
}
void jsonc_object_set_object(jsonc_object_t handle, const char* key, jsonc_object_t value) {
    static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key) = *static_cast<sculk::jsonc::ordered_jsonc::object_type*>(value);
}

jsonc_array_t jsonc_object_get_array(jsonc_object_t handle, const char* key) {
    return &static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).as<sculk::jsonc::ordered_jsonc::array_type>();
}
jsonc_array_t jsonc_object_add_new_array(jsonc_object_t handle, const char* key) {
    auto& res = static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key) = sculk::jsonc::ordered_jsonc::array_type();
    return &res.as<sculk::jsonc::ordered_jsonc::array_type>();
}
void jsonc_object_set_array(jsonc_object_t handle, const char* key, jsonc_array_t value) {
    static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key) = *static_cast<sculk::jsonc::ordered_jsonc::array_type*>(value);
}

size_t jsonc_object_get_size(jsonc_object_t handle) { return static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->size(); }

const char* jsonc_object_get_key_at_index(jsonc_object_t handle, size_t index) {
    return static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->key_index(index).c_str();
}

void jsonc_object_clear(jsonc_object_t handle) { static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->clear(); }

bool jsonc_object_remove(jsonc_object_t handle, const char* key) {
    return static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->erase(key);
}

size_t jsonc_object_dump(
    jsonc_object_t handle,
    char*          data,
    size_t         length,
    int            indent,
    bool           ensure_ascii,
    bool           ignore_comments,
    bool           multi_line_comments_format
) {
    auto result =
        static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->dump(indent, ensure_ascii, ignore_comments, multi_line_comments_format);
    return copy_string(data, length, result);
}

size_t jsonc_object_get_key_comments_before(jsonc_object_t handle, char* data, size_t length, const char* key) {
    if (static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->has_key_before_comments(key)) {
        std::string result{};
        for (auto& comment : static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->key_before_comments(key)) {
            result.append(comment);
            result.push_back('\n');
        }
        result.pop_back();
        if (length > result.size()) {
            std::memcpy(data, result.data(), result.size());
            data[result.size()] = '\0';
            return result.size();
        }
    }
    return 0;
}
size_t jsonc_object_get_key_comments_after(jsonc_object_t handle, char* data, size_t length, const char* key) {
    if (static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->has_key_after_comments(key)) {
        std::string result{};
        for (auto& comment : static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->key_after_comments(key)) {
            result.append(comment);
            result.push_back('\n');
        }
        result.pop_back();
        return copy_string(data, length, result);
    }
    return 0;
}
size_t jsonc_object_get_value_comments_before(jsonc_object_t handle, char* data, size_t length, const char* key) {
    if (static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).has_before_comments()) {
        std::string result{};
        for (auto& comment : static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).before_comments()) {
            result.append(comment);
            result.push_back('\n');
        }
        result.pop_back();
        return copy_string(data, length, result);
    }
    return 0;
}
size_t jsonc_object_get_value_comments_after(jsonc_object_t handle, char* data, size_t length, const char* key) {
    if (static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).has_after_comments()) {
        std::string result{};
        for (auto& comment : static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).after_comments()) {
            result.append(comment);
            result.push_back('\n');
        }
        result.pop_back();
        return copy_string(data, length, result);
    }
    return 0;
}

void jsonc_object_set_key_comments_before(jsonc_object_t handle, const char* key, const char* comments) {
    static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->key_before_comments(key) = sculk::jsonc::detail::split_comments(comments);
}
void jsonc_object_set_key_comments_after(jsonc_object_t handle, const char* key, const char* comments) {
    static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->key_after_comments(key) = sculk::jsonc::detail::split_comments(comments);
}
void jsonc_object_set_value_comments_before(jsonc_object_t handle, const char* key, const char* comments) {
    static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).before_comments() =
        sculk::jsonc::detail::split_comments(comments);
}
void jsonc_object_set_value_comments_after(jsonc_object_t handle, const char* key, const char* comments) {
    static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle)->operator[](key).after_comments() = sculk::jsonc::detail::split_comments(comments);
}

bool jsonc_object_equals(jsonc_object_t lhs, jsonc_object_t rhs) {
    return (*static_cast<sculk::jsonc::ordered_jsonc::object_type*>(lhs)) == (*static_cast<sculk::jsonc::ordered_jsonc::object_type*>(rhs));
}

int jsonc_array_get_type(jsonc_array_t handle, size_t index) {
    return static_cast<int>(static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).type());
}

bool jsonc_array_value_is_any_int_type(jsonc_array_t handle, size_t index) {
    return static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).is_number_integer();
}

bool jsonc_array_value_is_any_float_type(jsonc_array_t handle, size_t index) {
    return static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).is_number_float();
}

bool jsonc_array_value_is_any_number_type(jsonc_array_t handle, size_t index) {
    return static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).is_number();
}

bool jsonc_array_get_bool(jsonc_array_t handle, size_t index) {
    return static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).get<bool>();
}
void jsonc_array_set_bool(jsonc_array_t handle, size_t index, bool value) {
    static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index) = value;
}
void jsonc_array_add_bool(jsonc_array_t handle, bool value) { static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->push_back(value); }

int64_t jsonc_array_get_signed(jsonc_array_t handle, size_t index) {
    return static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).get<int64_t>();
}
void jsonc_array_set_signed(jsonc_array_t handle, size_t index, int64_t value) {
    static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index) = value;
}
void jsonc_array_add_signed(jsonc_array_t handle, int64_t value) { static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->push_back(value); }

uint64_t jsonc_array_get_unsigned(jsonc_array_t handle, size_t index) {
    return static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).get<uint64_t>();
}
void jsonc_array_set_unsigned(jsonc_array_t handle, size_t index, uint64_t value) {
    static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index) = value;
}
void jsonc_array_add_unsigned(jsonc_array_t handle, uint64_t value) {
    static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->push_back(value);
}

size_t jsonc_array_get_any_int(jsonc_array_t handle, char* data, size_t length, size_t index) {
    auto result = static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).get_any_int_view();
    return copy_string(data, length, result);
}
bool jsonc_array_set_any_int(jsonc_array_t handle, size_t index, const char* value) {
    auto val = sculk::jsonc::ordered_jsonc::from_any_int(value);
    if (val) { static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index) = *val; }
    return val.has_value();
}
bool jsonc_array_add_any_int(jsonc_array_t handle, const char* value) {
    auto val = sculk::jsonc::ordered_jsonc::from_any_int(value);
    if (val) { static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->push_back(*val); }
    return val.has_value();
}

double jsonc_array_get_float(jsonc_array_t handle, size_t index) {
    return static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).get<double>();
}
void jsonc_array_set_float(jsonc_array_t handle, size_t index, double value) {
    static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index) = value;
}
void jsonc_array_add_float(jsonc_array_t handle, double value) { static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->push_back(value); }

size_t jsonc_array_get_any_float(jsonc_array_t handle, char* data, size_t length, size_t index) {
    auto result = static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).get_any_float_view();
    return copy_string(data, length, result);
}
bool jsonc_array_set_any_float(jsonc_array_t handle, size_t index, const char* value) {
    auto val = sculk::jsonc::ordered_jsonc::from_any_float(value);
    if (val) { static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index) = *val; }
    return val.has_value();
}
bool jsonc_array_add_any_float(jsonc_array_t handle, const char* value) {
    auto val = sculk::jsonc::ordered_jsonc::from_any_float(value);
    if (val) { static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->push_back(*val); }
    return val.has_value();
}

size_t jsonc_array_get_any_number(jsonc_array_t handle, char* data, size_t length, size_t index) {
    auto result = static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).get_any_number_view();
    return copy_string(data, length, result);
}
bool jsonc_array_set_any_number(jsonc_array_t handle, size_t index, const char* value) {
    auto val = sculk::jsonc::ordered_jsonc::from_any_number(value);
    if (val) { static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index) = *val; }
    return val.has_value();
}
bool jsonc_array_add_any_number(jsonc_array_t handle, const char* value) {
    auto val = sculk::jsonc::ordered_jsonc::from_any_number(value);
    if (val) { static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->push_back(*val); }
    return val.has_value();
}

const char* jsonc_array_get_string(jsonc_array_t handle, size_t index) {
    return static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).as<std::string>().c_str();
}
void jsonc_array_set_string(jsonc_array_t handle, size_t index, const char* value) {
    static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index) = std::string_view(value);
}
void jsonc_array_add_string(jsonc_array_t handle, const char* value) {
    static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->push_back(std::string_view(value));
}

jsonc_object_t jsonc_array_get_object(jsonc_array_t handle, size_t index) {
    return &static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).as<sculk::jsonc::ordered_jsonc::object_type>();
}
jsonc_object_t jsonc_array_add_new_object(jsonc_array_t handle) {
    auto& arr = *static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle);
    arr.push_back(sculk::jsonc::ordered_jsonc::object_type());
    return &arr.back().as<sculk::jsonc::ordered_jsonc::object_type>();
}
void jsonc_array_set_object(jsonc_array_t handle, size_t index, jsonc_object_t value) {
    static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index) = *static_cast<sculk::jsonc::ordered_jsonc::object_type*>(value);
}

jsonc_array_t jsonc_array_get_array(jsonc_array_t handle, size_t index) {
    return &static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).as<sculk::jsonc::ordered_jsonc::array_type>();
}
jsonc_array_t jsonc_array_add_new_array(jsonc_array_t handle) {
    auto& arr = *static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle);
    arr.push_back(sculk::jsonc::ordered_jsonc::array_type());
    return &arr.back().as<sculk::jsonc::ordered_jsonc::array_type>();
}
void jsonc_array_set_array(jsonc_array_t handle, size_t index, jsonc_array_t value) {
    static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index) = *static_cast<sculk::jsonc::ordered_jsonc::array_type*>(value);
}

size_t jsonc_array_get_size(jsonc_array_t handle) { return static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->size(); }

void jsonc_array_clear(jsonc_array_t handle) { return static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->clear(); }

bool jsonc_array_remove(jsonc_array_t handle, size_t index) { return static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->erase(index); }

size_t jsonc_array_dump(
    jsonc_array_t handle,
    char*         data,
    size_t        length,
    int           indent,
    bool          ensure_ascii,
    bool          ignore_comments,
    bool          multi_line_comments_format
) {
    auto result =
        static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->dump(indent, ensure_ascii, ignore_comments, multi_line_comments_format);
    return copy_string(data, length, result);
}

size_t jsonc_array_get_comments_before(jsonc_object_t handle, char* data, size_t length, size_t index) {
    if (static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).has_before_comments()) {
        std::string result{};
        for (auto& comment : static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).before_comments()) {
            result.append(comment);
            result.push_back('\n');
        }
        result.pop_back();
        return copy_string(data, length, result);
    }
    return 0;
}
size_t jsonc_array_get_comments_after(jsonc_object_t handle, char* data, size_t length, size_t index) {
    if (static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).has_after_comments()) {
        std::string result{};
        for (auto& comment : static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).after_comments()) {
            result.append(comment);
            result.push_back('\n');
        }
        result.pop_back();
        return copy_string(data, length, result);
    }
    return 0;
}

void jsonc_array_set_comments_before(jsonc_object_t handle, size_t index, const char* comments) {
    static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).before_comments() =
        sculk::jsonc::detail::split_comments(comments);
}
void jsonc_array_set_comments_after(jsonc_object_t handle, size_t index, const char* comments) {
    static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle)->operator[](index).after_comments() =
        sculk::jsonc::detail::split_comments(comments);
}

bool jsonc_array_equals(jsonc_array_t lhs, jsonc_array_t rhs) {
    return (*static_cast<sculk::jsonc::ordered_jsonc::array_type*>(lhs)) == (*static_cast<sculk::jsonc::ordered_jsonc::array_type*>(rhs));
}

jsonc_object_t jsonc_create_object() { return new (std::nothrow) sculk::jsonc::ordered_jsonc::object_type(); }
jsonc_array_t  jsonc_create_array() { return new (std::nothrow) sculk::jsonc::ordered_jsonc::array_type(); }

void jsonc_free_object(jsonc_object_t handle) { delete static_cast<sculk::jsonc::ordered_jsonc::object_type*>(handle); }
void jsonc_free_array(jsonc_array_t handle) { delete static_cast<sculk::jsonc::ordered_jsonc::array_type*>(handle); }
void jsonc_free_type_variant(jsonc_variant_t handle) { delete static_cast<sculk::jsonc::ordered_jsonc*>(handle); }
}
