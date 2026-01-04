#ifndef JSONC_NO_EXCEPTION
#define JSONC_NO_EXCEPTION
#endif
#include "jsonc/jsonc.hpp"
#include "jsonc-c/jsonc.h"

inline const char* make_cstr(const std::string& s) noexcept {
    const size_t n   = s.size();
    char*        buf = new char[n + 1]{};
    if (n) std::memcpy(buf, s.data(), n);
    buf[n] = '\0';
    return buf;
}

inline std::vector<std::string> parse_comments(std::string_view comment) noexcept {
    std::vector<std::string> result{};
    if (!comment.empty()) {
        size_t pos = 0;
        while ((pos = comment.find('\n')) != std::string::npos) {
            if (pos != 0) {
                auto line = comment.substr(0, pos);
                if (!line.empty()) { result.emplace_back(line); }
            }
            comment.remove_prefix(pos + 1);
        }
        if (!comment.empty()) { result.emplace_back(comment); }
    }
    return result;
}

extern "C" {

JsoncTypeVariantHandle jsonc_parse_content(const char* content, bool allow_trailing_comma) {
    if (auto result = jsonc::parse(content, allow_trailing_comma)) { return new jsonc::JsoncType(result.value()); }
    return nullptr;
}

JsoncValueType jsonc_get_variant_type(JsoncTypeVariantHandle handle) {
    return static_cast<JsoncValueType>(static_cast<jsonc::JsoncType*>(handle)->type());
}

bool        jsonc_variant_to_bool(JsoncTypeVariantHandle handle) { return static_cast<jsonc::JsoncType*>(handle)->get<bool>(); }
int64_t     jsonc_variant_to_signed(JsoncTypeVariantHandle handle) { return static_cast<jsonc::JsoncType*>(handle)->get<int64_t>(); }
uint64_t    jsonc_variant_to_unsigned(JsoncTypeVariantHandle handle) { return static_cast<jsonc::JsoncType*>(handle)->get<uint64_t>(); }
double      jsonc_variant_to_float(JsoncTypeVariantHandle handle) { return static_cast<jsonc::JsoncType*>(handle)->get<double>(); }
const char* jsonc_variant_to_string(JsoncTypeVariantHandle handle) { return make_cstr(static_cast<jsonc::JsoncType*>(handle)->get<std::string>()); }

JsoncObjectHandle jsonc_variant_as_object(JsoncTypeVariantHandle handle) { return &static_cast<jsonc::JsoncType*>(handle)->as<jsonc::Object>(); }
JsoncArrayHandle  jsonc_variant_as_array(JsoncTypeVariantHandle handle) { return &static_cast<jsonc::JsoncType*>(handle)->as<jsonc::Array>(); }

const char* jsonc_variant_dump(JsoncTypeVariantHandle handle, int indent, bool ensure_ascii, bool ignore_comments) {
    return make_cstr(static_cast<jsonc::JsoncType*>(handle)->dump(indent, ensure_ascii, ignore_comments, true));
}

bool jsonc_object_contains(JsoncObjectHandle handle, const char* key) { return static_cast<jsonc::Object*>(handle)->contains(key); }

JsoncValueType jsonc_object_get_type(JsoncObjectHandle handle, const char* key) {
    return static_cast<JsoncValueType>(static_cast<jsonc::Object*>(handle)->operator[](key).type());
}

bool jsonc_object_get_bool(JsoncObjectHandle handle, const char* key) { return static_cast<jsonc::Object*>(handle)->operator[](key).get<bool>(); }
void jsonc_object_set_bool(JsoncObjectHandle handle, const char* key, bool value) { static_cast<jsonc::Object*>(handle)->operator[](key) = value; }

int64_t jsonc_object_get_signed(JsoncObjectHandle handle, const char* key) {
    return static_cast<jsonc::Object*>(handle)->operator[](key).get<int64_t>();
}
void jsonc_object_set_signed(JsoncObjectHandle handle, const char* key, int64_t value) {
    static_cast<jsonc::Object*>(handle)->operator[](key) = value;
}

uint64_t jsonc_object_get_unsigned(JsoncObjectHandle handle, const char* key) {
    return static_cast<jsonc::Object*>(handle)->operator[](key).get<uint64_t>();
}
void jsonc_object_set_unsigned(JsoncObjectHandle handle, const char* key, uint64_t value) {
    static_cast<jsonc::Object*>(handle)->operator[](key) = value;
}

double jsonc_object_get_float(JsoncObjectHandle handle, const char* key) {
    return static_cast<jsonc::Object*>(handle)->operator[](key).get<double>();
}
void jsonc_object_set_float(JsoncObjectHandle handle, const char* key, double value) { static_cast<jsonc::Object*>(handle)->operator[](key) = value; }

const char* jsonc_object_get_string(JsoncObjectHandle handle, const char* key) {
    return make_cstr(static_cast<jsonc::Object*>(handle)->operator[](key).get<std::string>());
}
void jsonc_object_set_string(JsoncObjectHandle handle, const char* key, const char* value) {
    static_cast<jsonc::Object*>(handle)->operator[](key) = std::string_view(value);
}

JsoncObjectHandle jsonc_object_get_object(JsoncObjectHandle handle, const char* key) {
    return &static_cast<jsonc::Object*>(handle)->operator[](key).as<jsonc::Object>();
}
JsoncObjectHandle jsonc_object_add_new_object(JsoncObjectHandle handle, const char* key) {
    auto& res = static_cast<jsonc::Object*>(handle)->operator[](key) = jsonc::Object();
    return &res.as<jsonc::Object>();
}
void jsonc_object_set_object(JsoncObjectHandle handle, const char* key, JsoncObjectHandle value) {
    static_cast<jsonc::Object*>(handle)->operator[](key) = *static_cast<jsonc::Object*>(value);
}

JsoncArrayHandle jsonc_object_get_array(JsoncObjectHandle handle, const char* key) {
    return &static_cast<jsonc::Object*>(handle)->operator[](key).as<jsonc::Array>();
}
JsoncArrayHandle jsonc_object_add_new_array(JsoncObjectHandle handle, const char* key) {
    auto& res = static_cast<jsonc::Object*>(handle)->operator[](key) = jsonc::Array();
    return &res.as<jsonc::Array>();
}
void jsonc_object_set_array(JsoncObjectHandle handle, const char* key, JsoncArrayHandle value) {
    static_cast<jsonc::Object*>(handle)->operator[](key) = *static_cast<jsonc::Array*>(value);
}

size_t jsonc_object_get_size(JsoncObjectHandle handle) { return static_cast<jsonc::Object*>(handle)->size(); }

const char* jsonc_object_get_key_at_index(JsoncObjectHandle handle, size_t index) {
    return make_cstr(static_cast<jsonc::Object*>(handle)->key_index(index));
}

void jsonc_object_clear(JsoncObjectHandle handle) { static_cast<jsonc::Object*>(handle)->clear(); }

bool jsonc_object_remove(JsoncObjectHandle handle, const char* key) { return static_cast<jsonc::Object*>(handle)->erase(key); }

const char* jsonc_object_dump(JsoncObjectHandle handle, int indent, bool ensure_ascii, bool ignore_comments) {
    return make_cstr(static_cast<jsonc::Object*>(handle)->dump(indent, ensure_ascii, ignore_comments));
}

const char* jsonc_object_get_key_comments_before(JsoncObjectHandle handle, const char* key) {
    if (static_cast<jsonc::Object*>(handle)->has_key_before_comments(key)) {
        std::string result{};
        for (auto& comment : static_cast<jsonc::Object*>(handle)->key_before_comments(key)) {
            result.append(comment);
            result.push_back('\n');
        }
        result.pop_back();
        return make_cstr(result);
    }
    return nullptr;
}
const char* jsonc_object_get_key_comments_after(JsoncObjectHandle handle, const char* key) {
    if (static_cast<jsonc::Object*>(handle)->has_key_after_comments(key)) {
        std::string result{};
        for (auto& comment : static_cast<jsonc::Object*>(handle)->key_after_comments(key)) {
            result.append(comment);
            result.push_back('\n');
        }
        result.pop_back();
        return make_cstr(result);
    }
    return nullptr;
}
const char* jsonc_object_get_value_comments_before(JsoncObjectHandle handle, const char* key) {
    if (static_cast<jsonc::Object*>(handle)->operator[](key).has_before_comments()) {
        std::string result{};
        for (auto& comment : static_cast<jsonc::Object*>(handle)->operator[](key).before_comments()) {
            result.append(comment);
            result.push_back('\n');
        }
        result.pop_back();
        return make_cstr(result);
    }
    return nullptr;
}
const char* jsonc_object_get_value_comments_after(JsoncObjectHandle handle, const char* key) {
    if (static_cast<jsonc::Object*>(handle)->operator[](key).has_after_comments()) {
        std::string result{};
        for (auto& comment : static_cast<jsonc::Object*>(handle)->operator[](key).after_comments()) {
            result.append(comment);
            result.push_back('\n');
        }
        result.pop_back();
        return make_cstr(result);
    }
    return nullptr;
}

void jsonc_object_set_key_comments_before(JsoncObjectHandle handle, const char* key, const char* comments) {
    static_cast<jsonc::Object*>(handle)->key_before_comments(key) = parse_comments(comments);
}
void jsonc_object_set_key_comments_after(JsoncObjectHandle handle, const char* key, const char* comments) {
    static_cast<jsonc::Object*>(handle)->key_after_comments(key) = parse_comments(comments);
}
void jsonc_object_set_value_comments_before(JsoncObjectHandle handle, const char* key, const char* comments) {
    static_cast<jsonc::Object*>(handle)->operator[](key).before_comments() = parse_comments(comments);
}
void jsonc_object_set_value_comments_after(JsoncObjectHandle handle, const char* key, const char* comments) {
    static_cast<jsonc::Object*>(handle)->operator[](key).after_comments() = parse_comments(comments);
}

bool jsonc_object_equals(JsoncObjectHandle lhs, JsoncObjectHandle rhs) {
    return (*static_cast<jsonc::Object*>(lhs)) == (*static_cast<jsonc::Object*>(rhs));
}

JsoncValueType jsonc_array_get_type(JsoncArrayHandle handle, size_t index) {
    return static_cast<JsoncValueType>(static_cast<jsonc::Array*>(handle)->operator[](index).type());
}

bool jsonc_array_get_bool(JsoncArrayHandle handle, size_t index) { return static_cast<jsonc::Array*>(handle)->operator[](index).get<bool>(); }
void jsonc_array_set_bool(JsoncArrayHandle handle, size_t index, bool value) { static_cast<jsonc::Array*>(handle)->operator[](index) = value; }
void jsonc_array_add_bool(JsoncArrayHandle handle, bool value) { static_cast<jsonc::Array*>(handle)->push_back(value); }

int64_t jsonc_array_get_signed(JsoncArrayHandle handle, size_t index) { return static_cast<jsonc::Array*>(handle)->operator[](index).get<int64_t>(); }
void jsonc_array_set_signed(JsoncArrayHandle handle, size_t index, int64_t value) { static_cast<jsonc::Array*>(handle)->operator[](index) = value; }
void jsonc_array_add_signed(JsoncArrayHandle handle, int64_t value) { static_cast<jsonc::Array*>(handle)->push_back(value); }

uint64_t jsonc_array_get_unsigned(JsoncArrayHandle handle, size_t index) {
    return static_cast<jsonc::Array*>(handle)->operator[](index).get<uint64_t>();
}
void jsonc_array_set_unsigned(JsoncArrayHandle handle, size_t index, uint64_t value) {
    static_cast<jsonc::Array*>(handle)->operator[](index) = value;
}
void jsonc_array_add_unsigend(JsoncArrayHandle handle, uint64_t value) { static_cast<jsonc::Array*>(handle)->push_back(value); }

double jsonc_array_get_float(JsoncArrayHandle handle, size_t index) { return static_cast<jsonc::Array*>(handle)->operator[](index).get<double>(); }
void   jsonc_array_set_float(JsoncArrayHandle handle, size_t index, double value) { static_cast<jsonc::Array*>(handle)->operator[](index) = value; }
void   jsonc_array_add_float(JsoncArrayHandle handle, double value) { static_cast<jsonc::Array*>(handle)->push_back(value); }

const char* jsonc_array_get_string(JsoncArrayHandle handle, size_t index) {
    return make_cstr(static_cast<jsonc::Array*>(handle)->operator[](index).get<std::string>());
}
void jsonc_array_set_string(JsoncArrayHandle handle, size_t index, const char* value) {
    static_cast<jsonc::Array*>(handle)->operator[](index) = std::string_view(value);
}
void jsonc_array_add_string(JsoncArrayHandle handle, const char* value) { static_cast<jsonc::Array*>(handle)->push_back(std::string_view(value)); }

JsoncObjectHandle jsonc_array_get_object(JsoncArrayHandle handle, size_t index) {
    return &static_cast<jsonc::Array*>(handle)->operator[](index).as<jsonc::Object>();
}
JsoncObjectHandle jsonc_array_add_new_object(JsoncArrayHandle handle) {
    auto& arr = *static_cast<jsonc::Array*>(handle);
    arr.push_back(jsonc::Object());
    return &arr.back().as<jsonc::Object>();
}
void jsonc_array_set_object(JsoncArrayHandle handle, size_t index, JsoncObjectHandle value) {
    static_cast<jsonc::Array*>(handle)->operator[](index) = *static_cast<jsonc::Object*>(value);
}

JsoncArrayHandle jsonc_array_get_array(JsoncArrayHandle handle, size_t index) {
    return &static_cast<jsonc::Array*>(handle)->operator[](index).as<jsonc::Array>();
}
JsoncArrayHandle jsonc_array_add_new_array(JsoncArrayHandle handle) {
    auto& arr = *static_cast<jsonc::Array*>(handle);
    arr.push_back(jsonc::Array());
    return &arr.back().as<jsonc::Array>();
}
void jsonc_array_set_array(JsoncArrayHandle handle, size_t index, JsoncArrayHandle value) {
    static_cast<jsonc::Array*>(handle)->operator[](index) = *static_cast<jsonc::Array*>(value);
}

size_t jsonc_array_get_size(JsoncArrayHandle handle) { return static_cast<jsonc::Array*>(handle)->size(); }

void jsonc_array_clear(JsoncArrayHandle handle) { return static_cast<jsonc::Array*>(handle)->clear(); }

bool jsonc_array_remove(JsoncArrayHandle handle, size_t index) { return static_cast<jsonc::Array*>(handle)->erase(index); }

const char* jsonc_array_dump(JsoncArrayHandle handle, int indent, bool ensure_ascii, bool ignore_comments) {
    return make_cstr(static_cast<jsonc::Array*>(handle)->dump(indent, ensure_ascii, ignore_comments));
}

const char* jsonc_array_get_comments_before(JsoncObjectHandle handle, size_t index) {
    if (static_cast<jsonc::Array*>(handle)->operator[](index).has_before_comments()) {
        std::string result{};
        for (auto& comment : static_cast<jsonc::Array*>(handle)->operator[](index).before_comments()) {
            result.append(comment);
            result.push_back('\n');
        }
        result.pop_back();
        return make_cstr(result);
    }
    return nullptr;
}
const char* jsonc_array_get_comments_after(JsoncObjectHandle handle, size_t index) {
    if (static_cast<jsonc::Array*>(handle)->operator[](index).has_after_comments()) {
        std::string result{};
        for (auto& comment : static_cast<jsonc::Array*>(handle)->operator[](index).after_comments()) {
            result.append(comment);
            result.push_back('\n');
        }
        result.pop_back();
        return make_cstr(result);
    }
    return nullptr;
}

void jsonc_array_set_comments_before(JsoncObjectHandle handle, size_t index, const char* comments) {
    static_cast<jsonc::Array*>(handle)->operator[](index).before_comments() = parse_comments(comments);
}
void jsonc_array_set_comments_after(JsoncObjectHandle handle, size_t index, const char* comments) {
    static_cast<jsonc::Array*>(handle)->operator[](index).after_comments() = parse_comments(comments);
}

bool jsonc_array_equals(JsoncArrayHandle lhs, JsoncArrayHandle rhs) {
    return (*static_cast<jsonc::Array*>(lhs)) == (*static_cast<jsonc::Array*>(rhs));
}

JsoncObjectHandle jsonc_create_object() { return new jsonc::Object(); }
JsoncArrayHandle  jsonc_create_array() { return new jsonc::Array(); }

void jsonc_free_object(JsoncObjectHandle handle) { delete static_cast<jsonc::Object*>(handle); }
void jsonc_free_array(JsoncArrayHandle handle) { delete static_cast<jsonc::Array*>(handle); }
void jsonc_free_type_variant(JsoncTypeVariantHandle handle) { delete static_cast<jsonc::JsoncType*>(handle); }
void jsonc_free_string(const char* str) { delete[] str; }
}