#pragma once
#include "exception.hpp"
#include "serializer.hpp"
#include "type.hpp"
#include <format>

namespace jsonc {

namespace detail {
inline std::vector<std::string> split_comments(std::string_view comment) noexcept {
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
} // namespace detail

Object::Object(std::initializer_list<std::pair<std::string, JsoncType>> val) JSONC_EXCEPTION_TYPE : mStorage(val) {}

JsoncType& Object::operator[](std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = mStorage.find(index);
    if (res != mStorage.end()) { return res->second; }
    return mStorage.try_emplace(index).first->second;
}
JSONC_RESULT(const JsoncType&) Object::operator[](std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = mStorage.find(index);
    if (res != mStorage.end()) { return _JSONC_MAKE_RESULT(res->second); }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

JSONC_RESULT(JsoncType&) Object::at(std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = mStorage.find(index);
    if (res != mStorage.end()) { return _JSONC_MAKE_RESULT(res->second); }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}
JSONC_RESULT(const JsoncType&) Object::at(std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = mStorage.find(index);
    if (res != mStorage.end()) { return _JSONC_MAKE_RESULT(res->second); }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

JsoncType& Object::at(std::string_view index, const JsoncType& default_value) JSONC_EXCEPTION_TYPE {
    auto res = mStorage.find(index);
    if (res != mStorage.end()) { return res->second; }
    return mStorage.try_emplace(index, default_value).first->second;
}

bool Object::contains(std::string_view index) const noexcept { return mStorage.contains(index); }
bool Object::contains(std::string_view index, ValueType type) const noexcept {
    auto result = mStorage.find(index);
    if (result != mStorage.end()) { return result->second.type() == type; }
    return false;
}

size_t Object::size() const noexcept { return mStorage.size(); }

bool Object::empty() const noexcept { return mStorage.empty(); }

bool Object::erase(std::string_view index) noexcept {
    mKeyComments.erase(std::string(index));
    return mStorage.erase(index);
}

void Object::clear() noexcept {
    mStorage.clear();
    mKeyComments.clear();
}

std::string Object::dump(int indent, bool ensure_ascii, bool ignore_comments) const JSONC_EXCEPTION_TYPE {
    return detail::dump_typed(*this, ensure_ascii, indent, ignore_comments);
}

const std::string& Object::key_index(size_t index) const noexcept { return mStorage.key_index(index); }

bool Object::has_key_before_comments(std::string_view index) const noexcept {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { return !res->second.mBeforeComments.empty(); }
    return false;
}
bool Object::has_key_after_comments(std::string_view index) const noexcept {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { return !res->second.mAfterComments.empty(); }
    return false;
}

std::vector<std::string>& Object::key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) {
        return res->second.mBeforeComments;
    } else {
        return mKeyComments.try_emplace(std::string(index)).first->second.mBeforeComments;
    }
}
JSONC_RESULT(const std::vector<std::string>&) Object::key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { return res->second.mBeforeComments; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

std::vector<std::string>& Object::key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) {
        return res->second.mAfterComments;
    } else {
        return mKeyComments.try_emplace(std::string(index)).first->second.mAfterComments;
    }
}
JSONC_RESULT(const std::vector<std::string>&) Object::key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { return res->second.mAfterComments; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

JSONC_RESULT(std::vector<std::string>) Object::get_key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { return res->second.mBeforeComments; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}
JSONC_RESULT(std::vector<std::string>) Object::get_key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { return res->second.mAfterComments; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

JSONC_RESULT(std::string) Object::get_key_before_comments(std::string_view index, size_t comment_index) const JSONC_EXCEPTION_TYPE {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) {
        if (comment_index < res->second.mBeforeComments.size()) { return res->second.mBeforeComments[comment_index]; }
        _JSONC_OUT_OF_RANGE("comment index out of range");
    }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}
JSONC_RESULT(std::string) Object::get_key_after_comments(std::string_view index, size_t comment_index) const JSONC_EXCEPTION_TYPE {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) {
        if (comment_index < res->second.mAfterComments.size()) { return res->second.mAfterComments[comment_index]; }
        _JSONC_OUT_OF_RANGE("comment index out of range");
    }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

bool Object::set_key_before_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE {
    if (contains(index)) {
        auto res = mKeyComments.find(index);
        if (res != mKeyComments.end()) {
            res->second.mBeforeComments = comments;
        } else {
            mKeyComments.try_emplace(std::string(index)).first->second.mBeforeComments = comments;
        }
        return true;
    }
    return false;
}
bool Object::set_key_after_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE {
    if (contains(index)) {
        auto res = mKeyComments.find(index);
        if (res != mKeyComments.end()) {
            res->second.mAfterComments = comments;
        } else {
            mKeyComments.try_emplace(std::string(index)).first->second.mAfterComments = comments;
        }
        return true;
    }
    return false;
}

bool Object::add_key_before_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE {
    if (contains(index)) {
        auto res = mKeyComments.find(index);
        if (res != mKeyComments.end()) {
            res->second.mBeforeComments.append_range(detail::split_comments(comment));
        } else {
            mKeyComments.try_emplace(std::string(index)).first->second.mBeforeComments.append_range(detail::split_comments(comment));
        }
        return true;
    }
    return false;
}
bool Object::add_key_after_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE {
    if (contains(index)) {
        auto res = mKeyComments.find(index);
        if (res != mKeyComments.end()) {
            res->second.mAfterComments.append_range(detail::split_comments(comment));
        } else {
            mKeyComments.try_emplace(std::string(index)).first->second.mAfterComments.append_range(detail::split_comments(comment));
        }
        return true;
    }
    return false;
}

void Object::clear_key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { res->second.mBeforeComments.clear(); }
}
void Object::clear_key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { res->second.mAfterComments.clear(); }
}

bool Object::remove_key_before_comment(std::string_view index, size_t comment_index) JSONC_EXCEPTION_TYPE {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) {
        if (comment_index < res->second.mBeforeComments.size()) {
            res->second.mAfterComments.erase(
                res->second.mBeforeComments.begin() + static_cast<decltype(res->second.mBeforeComments)::difference_type>(comment_index)
            );
            return true;
        }
    }
    return false;
}
bool Object::remove_key_after_comment(std::string_view index, size_t comment_index) JSONC_EXCEPTION_TYPE {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) {
        if (comment_index < res->second.mAfterComments.size()) {
            res->second.mAfterComments.erase(
                res->second.mAfterComments.begin() + static_cast<decltype(res->second.mAfterComments)::difference_type>(comment_index)
            );
            return true;
        }
    }
    return false;
}

size_t Object::key_before_comments_size(std::string_view index) const noexcept {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { return res->second.mBeforeComments.size(); }
    return 0;
}
size_t Object::key_after_comments_size(std::string_view index) const noexcept {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { return res->second.mAfterComments.size(); }
    return 0;
}

Object::iterator Object::begin() noexcept { return mStorage.begin(); }
Object::iterator Object::end() noexcept { return mStorage.end(); }

Object::const_iterator Object::begin() const noexcept { return mStorage.begin(); }
Object::const_iterator Object::end() const noexcept { return mStorage.end(); }

Object::const_iterator Object::cbegin() const noexcept { return mStorage.cbegin(); }
Object::const_iterator Object::cend() const noexcept { return mStorage.cend(); }

Object::reverse_iterator Object::rbegin() noexcept { return mStorage.rbegin(); }
Object::reverse_iterator Object::rend() noexcept { return mStorage.rend(); }

Object::const_reverse_iterator Object::rbegin() const noexcept { return mStorage.rbegin(); }
Object::const_reverse_iterator Object::rend() const noexcept { return mStorage.rend(); }

Object::const_reverse_iterator Object::crbegin() const noexcept { return mStorage.crbegin(); }
Object::const_reverse_iterator Object::crend() const noexcept { return mStorage.crend(); }

void Object::merge_patch(const Object& other, bool merge_list) JSONC_EXCEPTION_TYPE {
    for (auto const& [key, val] : other) { operator[](key).merge_patch(val, merge_list); }
}
JSONC_RESULT(void) Object::merge_patch(const JsoncType& other, bool merge_list) JSONC_EXCEPTION_TYPE {
    if (auto* rhs = std::get_if<Object>(&other.mStorage)) { return merge_patch(*rhs, merge_list); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", other.type_name()));
}

bool Object::operator==(const Object& other) const JSONC_EXCEPTION_TYPE { return mStorage == other.mStorage; }
bool Object::operator==(const JsoncType& other) const JSONC_EXCEPTION_TYPE {
    if (auto* rhs = std::get_if<Object>(&other.mStorage)) { return mStorage == rhs->mStorage; }
    return false;
}


Array::Array(std::initializer_list<JsoncType> val) JSONC_EXCEPTION_TYPE : mStorage(val) {}

constexpr JsoncType& Array::operator[](size_t index) noexcept { return mStorage[index]; }
constexpr JSONC_RESULT(const JsoncType&) Array::operator[](size_t index) const noexcept { return _JSONC_MAKE_RESULT(mStorage[index]); }

constexpr JSONC_RESULT(JsoncType&) Array::at(size_t index) JSONC_EXCEPTION_TYPE {
    if (index < mStorage.size()) { return _JSONC_MAKE_RESULT(mStorage[index]); }
    _JSONC_OUT_OF_RANGE("Index out of range");
}
constexpr JSONC_RESULT(const JsoncType&) Array::at(size_t index) const JSONC_EXCEPTION_TYPE {
    if (index < mStorage.size()) { return _JSONC_MAKE_RESULT(mStorage[index]); }
    _JSONC_OUT_OF_RANGE("Index out of range");
}

constexpr size_t Array::size() const noexcept { return mStorage.size(); }

constexpr bool Array::empty() const noexcept { return mStorage.empty(); }

void Array::clear() noexcept { mStorage.clear(); }

bool Array::erase(size_t where) {
    if (where < mStorage.size()) {
        mStorage.erase(mStorage.begin() + static_cast<decltype(mStorage)::difference_type>(where));
        return true;
    }
    return false;
}
bool Array::erase(size_t first, size_t last) {
    if (first < last && last < mStorage.size()) {
        mStorage.erase(
            mStorage.begin() + static_cast<decltype(mStorage)::difference_type>(first),
            mStorage.begin() + static_cast<decltype(mStorage)::difference_type>(last)
        );
        return true;
    }
    return false;
}

Array::iterator Array::erase(const_iterator where) JSONC_EXCEPTION_TYPE { return mStorage.erase(where); }
Array::iterator Array::erase(const_iterator first, const_iterator last) JSONC_EXCEPTION_TYPE { return mStorage.erase(first, last); }

void Array::push_back(const JsoncType& val) JSONC_EXCEPTION_TYPE { mStorage.push_back(val); }
void Array::push_back(JsoncType&& val) JSONC_EXCEPTION_TYPE { mStorage.push_back(std::move(val)); }

const JsoncType& Array::front() const noexcept { return mStorage.front(); }
JsoncType&       Array::front() noexcept { return mStorage.front(); }

const JsoncType& Array::back() const noexcept { return mStorage.back(); }
JsoncType&       Array::back() noexcept { return mStorage.back(); }

std::string Array::dump(int indent, bool ensure_ascii, bool ignore_comments) const JSONC_EXCEPTION_TYPE {
    return detail::dump_typed(*this, ensure_ascii, indent, ignore_comments);
}

Array::iterator Array::begin() noexcept { return mStorage.begin(); }
Array::iterator Array::end() noexcept { return mStorage.end(); }

Array::const_iterator Array::begin() const noexcept { return mStorage.begin(); }
Array::const_iterator Array::end() const noexcept { return mStorage.end(); }

Array::const_iterator Array::cbegin() const noexcept { return mStorage.cbegin(); }
Array::const_iterator Array::cend() const noexcept { return mStorage.cend(); }

Array::reverse_iterator Array::rbegin() noexcept { return mStorage.rbegin(); }
Array::reverse_iterator Array::rend() noexcept { return mStorage.rend(); }

Array::const_reverse_iterator Array::rbegin() const noexcept { return mStorage.rbegin(); }
Array::const_reverse_iterator Array::rend() const noexcept { return mStorage.rend(); }

Array::const_reverse_iterator Array::crbegin() const noexcept { return mStorage.crbegin(); }
Array::const_reverse_iterator Array::crend() const noexcept { return mStorage.crend(); }

void Array::merge_patch(const Array& other) JSONC_EXCEPTION_TYPE {
    if (other.empty()) { return; }
    for (auto const& val : other.mStorage) {
        bool exist = false;
        for (const auto& tag : mStorage) {
            if (tag == val) {
                exist = true;
                break;
            }
        }
        if (!exist) { mStorage.push_back(val); }
    }
}
JSONC_RESULT(void) Array::merge_patch(const JsoncType& other) JSONC_EXCEPTION_TYPE {
    if (auto* rhs = std::get_if<Array>(&other.mStorage)) {
        { return merge_patch(*rhs); }
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", other.type_name()));
}

bool Array::operator==(const Array& other) const JSONC_EXCEPTION_TYPE { return mStorage == other.mStorage; }
bool Array::operator==(const JsoncType& other) const JSONC_EXCEPTION_TYPE {
    if (auto* rhs = std::get_if<Array>(&other.mStorage)) { return mStorage == rhs->mStorage; }
    return false;
}


constexpr ValueType        JsoncType::type() const noexcept { return static_cast<ValueType>(mStorage.index()); }
constexpr std::string_view JsoncType::type_name() const noexcept {
    switch (type()) {
    case ValueType::Null:
        return "null";
    case ValueType::Boolean:
        return "boolean";
    case ValueType::Signed:
    case ValueType::Unsigned:
        return "inteager";
    case ValueType::String:
        return "string";
    case ValueType::Float:
        return "floating-point";
    case ValueType::Object:
        return "object";
    case ValueType::Array:
        return "array";
    default:
        std::unreachable();
    }
}

constexpr bool JsoncType::hold(ValueType value_type) const noexcept { return type() == value_type; }

constexpr bool JsoncType::is_null() const noexcept { return hold(ValueType::Null); }
constexpr bool JsoncType::is_boolean() const noexcept { return hold(ValueType::Boolean); }
constexpr bool JsoncType::is_number_signed() const noexcept { return hold(ValueType::Signed); }
constexpr bool JsoncType::is_number_unsigned() const noexcept { return hold(ValueType::Unsigned); }
constexpr bool JsoncType::is_number_integer() const noexcept { return is_number_signed() || is_number_unsigned(); }
constexpr bool JsoncType::is_number_float() const noexcept { return hold(ValueType::Float); }
constexpr bool JsoncType::is_number() const noexcept { return is_number_integer() || is_number_float(); }
constexpr bool JsoncType::is_string() const noexcept { return hold(ValueType::String); }
constexpr bool JsoncType::is_object() const noexcept { return hold(ValueType::Object); }
constexpr bool JsoncType::is_array() const noexcept { return hold(ValueType::Array); }
constexpr bool JsoncType::is_primitive() const noexcept { return is_null() || is_string() || is_number(); }
constexpr bool JsoncType::is_structured() const noexcept { return is_array() || is_object(); }

constexpr size_t JsoncType::size() const noexcept {
    return std::visit(
        [](const auto& val) -> size_t {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, Object> || std::is_same_v<T, Array>) {
                return val.size();
            } else if constexpr (std::is_same_v<T, std::monostate>) {
                return 0;
            } else {
                return 1;
            }
        },
        mStorage
    );
}

JSONC_RESULT(bool) JsoncType::contains(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&mStorage)) { return storage->contains(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

JSONC_RESULT(bool) JsoncType::contains(std::string_view index, ValueType type) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&mStorage)) { return storage->contains(index, type); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

JSONC_RESULT(bool) JsoncType::empty() const JSONC_EXCEPTION_TYPE {
    return std::visit(
        [](const auto& val) -> JSONC_RESULT(bool) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, Object> || std::is_same_v<T, Array>) {
                return val.empty();
            } else {
                _JSONC_TYPE_ERROR(std::format("Type must be an object or array, but is {}", type_name()));
            }
        },
        mStorage
    );
}

JSONC_RESULT(bool) JsoncType::erase(std::string_view index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&mStorage)) { return storage->erase(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

JSONC_RESULT(bool) JsoncType::erase(size_t where) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&mStorage)) { return storage->erase(where); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

JSONC_RESULT(bool) JsoncType::erase(size_t first, size_t last) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&mStorage)) { return storage->erase(first, last); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

template <typename T>
    requires std::is_arithmetic_v<T>
JsoncType::operator T() const JSONC_EXCEPTION_TYPE {
    return std::visit(
        [](const auto& val) -> T {
            using Type = std::decay_t<decltype(val)>;
            if constexpr (std::is_convertible_v<Type, T>) {
                return static_cast<T>(val);
            } else {
#ifdef JSONC_NO_EXCEPTION
                std::unreachable();
#else
                _JSONC_TYPE_ERROR("bad type cast");
#endif
            }
        },
        mStorage
    );
}

template <typename T>
    requires std::is_convertible_v<T, std::string>
JsoncType::operator T() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<std::string>(&mStorage)) { return *storage; }
#ifdef JSONC_NO_EXCEPTION
    std::unreachable();
#else
    _JSONC_TYPE_ERROR("bad type cast");
#endif
}

template <detail::is_array_like T>
JsoncType::operator T() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&mStorage)) { return T(storage->begin(), storage->end()); }
#ifdef JSONC_NO_EXCEPTION
    std::unreachable();
#else
    _JSONC_TYPE_ERROR("bad type cast");
#endif
}

template <detail::is_object_like T>
JsoncType::operator T() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&mStorage)) { return T(storage->begin(), storage->end()); }
#ifdef JSONC_NO_EXCEPTION
    std::unreachable();
#else
    _JSONC_TYPE_ERROR("bad type cast");
#endif
}

std::string JsoncType::dump(int indent, bool ensure_ascii, bool ignore_comments, bool global_comments) const JSONC_EXCEPTION_TYPE {
    auto result = std::visit([&](const auto& val) { return detail::dump_typed(val, ensure_ascii, indent, ignore_comments); }, mStorage);
    if (!ignore_comments && global_comments) {
        auto before = detail::format_comments(mBeforeComments);
        if (!before.empty()) { result = before + result; }
        auto after = detail::format_comments(mAfterComments);
        if (!after.empty()) {
            if (indent >= 0) { result.push_back(' '); }
            result.append(after);
            if (result.back() == '\n') { result.pop_back(); }
        }
    }
    return result;
}

template <detail::is_jsonc_type_convertible T>
JSONC_RESULT(T&) JsoncType::as() JSONC_EXCEPTION_TYPE {
    return std::visit(
        [](auto& val) -> T& {
            using Type = std::decay_t<decltype(val)>;
            if constexpr (std::is_convertible_v<Type, T>) {
                return static_cast<T&>(val);
            } else {
                _JSONC_TYPE_ERROR("bad type cast");
            }
        },
        mStorage
    );
}

template <detail::is_jsonc_type_convertible T>
JSONC_RESULT(const T&) JsoncType::as() const JSONC_EXCEPTION_TYPE {
    return std::visit(
        [](const auto& val) -> const T& {
            using Type = std::decay_t<decltype(val)>;
            if constexpr (std::is_convertible_v<Type, T>) {
                return static_cast<const T&>(val);
            } else {
                _JSONC_TYPE_ERROR("bad type cast");
            }
        },
        mStorage
    );
}

template <typename T>
    requires std::is_arithmetic_v<T>
JSONC_RESULT(T) JsoncType::get() const JSONC_EXCEPTION_TYPE {
    return std::visit(
        [](const auto& val) -> JSONC_RESULT(T) {
            using Type = std::decay_t<decltype(val)>;
            if constexpr (std::is_convertible_v<Type, T>) {
                return static_cast<T>(val);
            } else {
                _JSONC_TYPE_ERROR("bad type cast");
            }
        },
        mStorage
    );
}

template <typename T>
    requires std::is_convertible_v<T, std::string>
JSONC_RESULT(T) JsoncType::get() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<std::string>(&mStorage)) { return *storage; }
    _JSONC_TYPE_ERROR("bad type cast");
}

template <detail::is_array_like T>
JSONC_RESULT(T) JsoncType::get() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&mStorage)) { return T(storage->begin(), storage->end()); }
    _JSONC_TYPE_ERROR("bad type cast");
}

template <detail::is_object_like T>
JSONC_RESULT(T) JsoncType::get() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&mStorage)) { return T(storage->begin(), storage->end()); }
    _JSONC_TYPE_ERROR("bad type cast");
}

JSONC_RESULT(JsoncType&) JsoncType::operator[](std::string_view index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&mStorage)) { return (*storage)[index]; }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
JSONC_RESULT(const JsoncType&) JsoncType::operator[](std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&mStorage)) {
        auto res = storage->mStorage.find(index);
        if (res != storage->mStorage.end()) { return _JSONC_MAKE_RESULT(res->second); }
        _JSONC_OUT_OF_RANGE(std::format("Invalid key: {}", index));
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

JSONC_RESULT(JsoncType&) JsoncType::operator[](size_t index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&mStorage)) { return (*storage)[index]; }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}
JSONC_RESULT(const JsoncType&) JsoncType::operator[](size_t index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&mStorage)) { return (*storage)[index]; }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

JSONC_RESULT(JsoncType&) JsoncType::at(std::string_view index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&mStorage)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
JSONC_RESULT(JsoncType&) JsoncType::at(std::string_view index, const JsoncType& default_value) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&mStorage)) { return storage->at(index, default_value); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
JSONC_RESULT(const JsoncType&) JsoncType::at(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&mStorage)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

JSONC_RESULT(JsoncType&) JsoncType::at(size_t index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&mStorage)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}
JSONC_RESULT(const JsoncType&) JsoncType::at(size_t index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&mStorage)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

JSONC_RESULT(void) JsoncType::push_back(const JsoncType& val) JSONC_EXCEPTION_TYPE {
    if (hold(ValueType::Null)) { mStorage.emplace<7>(); }
    if (auto* storage = std::get_if<Array>(&mStorage)) {
        storage->push_back(std::move(val));
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}
JSONC_RESULT(void) JsoncType::push_back(JsoncType&& val) JSONC_EXCEPTION_TYPE {
    if (hold(ValueType::Null)) { mStorage.emplace<7>(); }
    if (auto* storage = std::get_if<Array>(&mStorage)) {
        storage->push_back(std::move(val));
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

JSONC_RESULT(const JsoncType&) JsoncType::front() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&mStorage)) { return storage->front(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}
JSONC_RESULT(JsoncType&) JsoncType::front() JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&mStorage)) { return storage->front(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

JSONC_RESULT(const JsoncType&) JsoncType::back() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&mStorage)) { return storage->back(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}
JSONC_RESULT(JsoncType&) JsoncType::back() JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&mStorage)) { return storage->back(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

JSONC_RESULT(JsoncType::IteratorProxy) JsoncType::items() JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&mStorage)) { return IteratorProxy(*storage); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
JSONC_RESULT(JsoncType::IteratorProxyConst) JsoncType::items() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&mStorage)) { return IteratorProxyConst(*storage); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

JSONC_RESULT(void) JsoncType::clear() JSONC_EXCEPTION_TYPE {
    return std::visit(
        [&](auto& val) -> JSONC_RESULT(void) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, Object> || std::is_same_v<T, Array>) {
                val.clear();
                return _JSONC_MAKE_VOID_RESULT();
            } else {
                _JSONC_TYPE_ERROR(std::format("Type must be an array or object, but is {}", type_name()));
            }
        },
        mStorage
    );
}

JsoncType::iterator JsoncType::begin() noexcept { return iterator::make_begin<false>(*this); }
JsoncType::iterator JsoncType::end() noexcept { return iterator::make_end<false>(*this); }

JsoncType::const_iterator JsoncType::begin() const noexcept { return const_iterator::make_begin<false>(*this); }
JsoncType::const_iterator JsoncType::end() const noexcept { return const_iterator::make_end<false>(*this); }

JsoncType::const_iterator JsoncType::cbegin() const noexcept { return const_iterator::make_begin<false>(*this); }
JsoncType::const_iterator JsoncType::cend() const noexcept { return const_iterator::make_end<false>(*this); }

JsoncType::reverse_iterator JsoncType::rbegin() noexcept { return reverse_iterator::make_begin<true>(*this); }
JsoncType::reverse_iterator JsoncType::rend() noexcept { return reverse_iterator::make_end<true>(*this); }

JsoncType::const_reverse_iterator JsoncType::rbegin() const noexcept { return const_reverse_iterator::make_begin<true>(*this); }
JsoncType::const_reverse_iterator JsoncType::rend() const noexcept { return const_reverse_iterator::make_end<true>(*this); }

JsoncType::const_reverse_iterator JsoncType::crbegin() const noexcept { return const_reverse_iterator::make_begin<true>(*this); }
JsoncType::const_reverse_iterator JsoncType::crend() const noexcept { return const_reverse_iterator::make_end<true>(*this); }

void JsoncType::merge_patch(const JsoncType& other, bool merge_list) JSONC_EXCEPTION_TYPE {
    if (is_object() && other.is_object()) {
        as<Object>().merge_patch(other.as<Object>(), merge_list);
    } else if (is_array() && other.is_array() && merge_list) {
        as<Array>().merge_patch(other.as<Array>());
    } else {
        operator=(other);
    }
}

bool JsoncType::operator==(const JsoncType& other) const JSONC_EXCEPTION_TYPE { return mStorage == other.mStorage; }

constexpr bool JsoncType::has_before_comments() const noexcept { return mBeforeComments.size() != 0; }
constexpr bool JsoncType::has_after_comments() const noexcept { return mAfterComments.size() != 0; }

std::vector<std::string>&       JsoncType::before_comments() noexcept { return mBeforeComments; }
const std::vector<std::string>& JsoncType::before_comments() const noexcept { return mBeforeComments; }

std::vector<std::string>&       JsoncType::after_comments() noexcept { return mAfterComments; }
const std::vector<std::string>& JsoncType::after_comments() const noexcept { return mAfterComments; }

std::vector<std::string> JsoncType::get_before_comments() const JSONC_EXCEPTION_TYPE { return mBeforeComments; }
std::vector<std::string> JsoncType::get_after_comments() const JSONC_EXCEPTION_TYPE { return mAfterComments; }

void JsoncType::set_before_comments(const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE { mBeforeComments = comments; }
void JsoncType::set_after_comments(const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE { mAfterComments = comments; }

void JsoncType::add_before_comment(std::string_view comment) JSONC_EXCEPTION_TYPE { mBeforeComments.append_range(detail::split_comments(comment)); }
void JsoncType::add_after_comment(std::string_view comment) JSONC_EXCEPTION_TYPE { mAfterComments.append_range(detail::split_comments(comment)); }

void JsoncType::clear_before_comments() JSONC_EXCEPTION_TYPE { mBeforeComments.clear(); }
void JsoncType::clear_after_comments() JSONC_EXCEPTION_TYPE { mBeforeComments.clear(); }

bool JsoncType::remove_before_comment(size_t comment_index) JSONC_EXCEPTION_TYPE {
    if (comment_index < mBeforeComments.size()) {
        mBeforeComments.erase(mBeforeComments.begin() + static_cast<decltype(mBeforeComments)::difference_type>(comment_index));
        return true;
    }
    return false;
}
bool JsoncType::remove_after_comment(size_t comment_index) JSONC_EXCEPTION_TYPE {
    if (comment_index < mAfterComments.size()) {
        mAfterComments.erase(mAfterComments.begin() + static_cast<decltype(mAfterComments)::difference_type>(comment_index));
        return true;
    }
    return false;
}

constexpr size_t JsoncType::before_comments_size() const noexcept { return mBeforeComments.size(); }
constexpr size_t JsoncType::after_comments_size() const noexcept { return mAfterComments.size(); }

} // namespace jsonc