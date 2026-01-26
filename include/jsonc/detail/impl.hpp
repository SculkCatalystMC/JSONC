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

Object::Object(std::initializer_list<std::pair<std::string, JsoncType>> val) JSONC_EXCEPTION_TYPE : storage_(val) {}

JsoncType& Object::operator[](std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = storage_.find(index);
    if (res != storage_.end()) { return res->second; }
    return storage_.try_emplace(index).first->second;
}
JSONC_RESULT(const JsoncType&) Object::operator[](std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = storage_.find(index);
    if (res != storage_.end()) { return _JSONC_MAKE_RESULT(res->second); }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

JSONC_RESULT(JsoncType&) Object::at(std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = storage_.find(index);
    if (res != storage_.end()) { return _JSONC_MAKE_RESULT(res->second); }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}
JSONC_RESULT(const JsoncType&) Object::at(std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = storage_.find(index);
    if (res != storage_.end()) { return _JSONC_MAKE_RESULT(res->second); }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

JsoncType& Object::at(std::string_view index, const JsoncType& default_value) JSONC_EXCEPTION_TYPE {
    auto res = storage_.find(index);
    if (res != storage_.end()) { return res->second; }
    return storage_.try_emplace(index, default_value).first->second;
}

bool Object::contains(std::string_view index) const noexcept { return storage_.contains(index); }
bool Object::contains(std::string_view index, ValueType type) const noexcept {
    auto result = storage_.find(index);
    if (result != storage_.end()) { return result->second.type() == type; }
    return false;
}

size_t Object::size() const noexcept { return storage_.size(); }

bool Object::empty() const noexcept { return storage_.empty(); }

bool Object::erase(std::string_view index) noexcept {
    key_comments_.erase(std::string(index));
    return storage_.erase(index);
}

void Object::clear() noexcept {
    storage_.clear();
    key_comments_.clear();
}

std::string Object::dump(int indent, bool ensure_ascii, bool ignore_comments) const JSONC_EXCEPTION_TYPE {
    return detail::dump_typed(*this, ensure_ascii, indent, ignore_comments);
}

const std::string& Object::key_index(size_t index) const noexcept { return storage_.key_index(index); }

bool Object::has_key_before_comments(std::string_view index) const noexcept {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return !res->second.before_comments_.empty(); }
    return false;
}
bool Object::has_key_after_comments(std::string_view index) const noexcept {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return !res->second.after_comments_.empty(); }
    return false;
}

std::vector<std::string>& Object::key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) {
        return res->second.before_comments_;
    } else {
        return key_comments_.try_emplace(std::string(index)).first->second.before_comments_;
    }
}
JSONC_RESULT(const std::vector<std::string>&) Object::key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.before_comments_; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

std::vector<std::string>& Object::key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) {
        return res->second.after_comments_;
    } else {
        return key_comments_.try_emplace(std::string(index)).first->second.after_comments_;
    }
}
JSONC_RESULT(const std::vector<std::string>&) Object::key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.after_comments_; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

JSONC_RESULT(std::vector<std::string>) Object::get_key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.before_comments_; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}
JSONC_RESULT(std::vector<std::string>) Object::get_key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.after_comments_; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

JSONC_RESULT(std::string) Object::get_key_before_comment(std::string_view index, size_t comment_index) const JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) {
        if (comment_index < res->second.before_comments_.size()) { return res->second.before_comments_[comment_index]; }
        _JSONC_OUT_OF_RANGE("comment index out of range");
    }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}
JSONC_RESULT(std::string) Object::get_key_after_comment(std::string_view index, size_t comment_index) const JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) {
        if (comment_index < res->second.after_comments_.size()) { return res->second.after_comments_[comment_index]; }
        _JSONC_OUT_OF_RANGE("comment index out of range");
    }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

bool Object::set_key_before_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE {
    if (contains(index)) {
        auto res = key_comments_.find(index);
        if (res != key_comments_.end()) {
            res->second.before_comments_ = comments;
        } else {
            key_comments_.try_emplace(std::string(index)).first->second.before_comments_ = comments;
        }
        return true;
    }
    return false;
}
bool Object::set_key_after_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE {
    if (contains(index)) {
        auto res = key_comments_.find(index);
        if (res != key_comments_.end()) {
            res->second.after_comments_ = comments;
        } else {
            key_comments_.try_emplace(std::string(index)).first->second.after_comments_ = comments;
        }
        return true;
    }
    return false;
}

bool Object::add_key_before_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE {
    if (contains(index)) {
        auto res = key_comments_.find(index);
        if (res != key_comments_.end()) {
            res->second.before_comments_.append_range(detail::split_comments(comment));
        } else {
            key_comments_.try_emplace(std::string(index)).first->second.before_comments_.append_range(detail::split_comments(comment));
        }
        return true;
    }
    return false;
}
bool Object::add_key_after_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE {
    if (contains(index)) {
        auto res = key_comments_.find(index);
        if (res != key_comments_.end()) {
            res->second.after_comments_.append_range(detail::split_comments(comment));
        } else {
            key_comments_.try_emplace(std::string(index)).first->second.after_comments_.append_range(detail::split_comments(comment));
        }
        return true;
    }
    return false;
}

void Object::clear_key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { res->second.before_comments_.clear(); }
}
void Object::clear_key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { res->second.after_comments_.clear(); }
}

bool Object::remove_key_before_comment(std::string_view index, size_t comment_index) JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) {
        if (comment_index < res->second.before_comments_.size()) {
            res->second.after_comments_.erase(
                res->second.before_comments_.begin() + static_cast<decltype(res->second.before_comments_)::difference_type>(comment_index)
            );
            return true;
        }
    }
    return false;
}
bool Object::remove_key_after_comment(std::string_view index, size_t comment_index) JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) {
        if (comment_index < res->second.after_comments_.size()) {
            res->second.after_comments_.erase(
                res->second.after_comments_.begin() + static_cast<decltype(res->second.after_comments_)::difference_type>(comment_index)
            );
            return true;
        }
    }
    return false;
}

size_t Object::key_before_comments_size(std::string_view index) const noexcept {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.before_comments_.size(); }
    return 0;
}
size_t Object::key_after_comments_size(std::string_view index) const noexcept {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.after_comments_.size(); }
    return 0;
}

Object::iterator Object::begin() noexcept { return storage_.begin(); }
Object::iterator Object::end() noexcept { return storage_.end(); }

Object::const_iterator Object::begin() const noexcept { return storage_.begin(); }
Object::const_iterator Object::end() const noexcept { return storage_.end(); }

Object::const_iterator Object::cbegin() const noexcept { return storage_.cbegin(); }
Object::const_iterator Object::cend() const noexcept { return storage_.cend(); }

Object::reverse_iterator Object::rbegin() noexcept { return storage_.rbegin(); }
Object::reverse_iterator Object::rend() noexcept { return storage_.rend(); }

Object::const_reverse_iterator Object::rbegin() const noexcept { return storage_.rbegin(); }
Object::const_reverse_iterator Object::rend() const noexcept { return storage_.rend(); }

Object::const_reverse_iterator Object::crbegin() const noexcept { return storage_.crbegin(); }
Object::const_reverse_iterator Object::crend() const noexcept { return storage_.crend(); }

void Object::merge_patch(const Object& other, bool merge_list) JSONC_EXCEPTION_TYPE {
    for (const auto& [key, val] : other) { operator[](key).merge_patch(val, merge_list); }
}
JSONC_RESULT(void) Object::merge_patch(const JsoncType& other, bool merge_list) JSONC_EXCEPTION_TYPE {
    if (auto* rhs = std::get_if<Object>(&other.storage_)) { return merge_patch(*rhs, merge_list); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", other.type_name()));
}

void Object::merge_comments(const Object& other) JSONC_EXCEPTION_TYPE {
    for (const auto& [key, val] : other) {
        operator[](key).set_before_comments(val[key].get_before_comments());
        operator[](key).set_after_comments(val[key].get_after_comments());
        set_key_before_comments(key, val.get_key_before_comments(key));
        set_key_after_comments(key, val.get_key_after_comments(key));
    }
}

void Object::move_comments_to_before() JSONC_EXCEPTION_TYPE {
    for (const auto& [key, val] : *this) {
        key_before_comments(key).append_range(get_key_after_comments(key));
        clear_key_after_comments(key);
        key_before_comments(key).append_range(operator[](key).get_before_comments());
        operator[](key).clear_before_comments();
        key_before_comments(key).append_range(operator[](key).get_after_comments());
        operator[](key).clear_after_comments();
    }
}

bool Object::operator==(const Object& other) const JSONC_EXCEPTION_TYPE { return storage_ == other.storage_; }
bool Object::operator==(const JsoncType& other) const JSONC_EXCEPTION_TYPE {
    if (auto* rhs = std::get_if<Object>(&other.storage_)) { return storage_ == rhs->storage_; }
    return false;
}


Array::Array(std::initializer_list<JsoncType> val) JSONC_EXCEPTION_TYPE : storage_(val) {}

constexpr JsoncType& Array::operator[](size_t index) noexcept { return storage_[index]; }
constexpr JSONC_RESULT(const JsoncType&) Array::operator[](size_t index) const noexcept { return _JSONC_MAKE_RESULT(storage_[index]); }

constexpr JSONC_RESULT(JsoncType&) Array::at(size_t index) JSONC_EXCEPTION_TYPE {
    if (index < storage_.size()) { return _JSONC_MAKE_RESULT(storage_[index]); }
    _JSONC_OUT_OF_RANGE("Index out of range");
}
constexpr JSONC_RESULT(const JsoncType&) Array::at(size_t index) const JSONC_EXCEPTION_TYPE {
    if (index < storage_.size()) { return _JSONC_MAKE_RESULT(storage_[index]); }
    _JSONC_OUT_OF_RANGE("Index out of range");
}

constexpr size_t Array::size() const noexcept { return storage_.size(); }

constexpr bool Array::empty() const noexcept { return storage_.empty(); }

void Array::clear() noexcept { storage_.clear(); }

bool Array::erase(size_t where) {
    if (where < storage_.size()) {
        storage_.erase(storage_.begin() + static_cast<decltype(storage_)::difference_type>(where));
        return true;
    }
    return false;
}
bool Array::erase(size_t first, size_t last) {
    if (first < last && last < storage_.size()) {
        storage_.erase(
            storage_.begin() + static_cast<decltype(storage_)::difference_type>(first),
            storage_.begin() + static_cast<decltype(storage_)::difference_type>(last)
        );
        return true;
    }
    return false;
}

Array::iterator Array::erase(const_iterator where) JSONC_EXCEPTION_TYPE { return storage_.erase(where); }
Array::iterator Array::erase(const_iterator first, const_iterator last) JSONC_EXCEPTION_TYPE { return storage_.erase(first, last); }

void Array::push_back(const JsoncType& val) JSONC_EXCEPTION_TYPE { storage_.push_back(val); }
void Array::push_back(JsoncType&& val) JSONC_EXCEPTION_TYPE { storage_.push_back(std::move(val)); }

const JsoncType& Array::front() const noexcept { return storage_.front(); }
JsoncType&       Array::front() noexcept { return storage_.front(); }

const JsoncType& Array::back() const noexcept { return storage_.back(); }
JsoncType&       Array::back() noexcept { return storage_.back(); }

std::string Array::dump(int indent, bool ensure_ascii, bool ignore_comments) const JSONC_EXCEPTION_TYPE {
    return detail::dump_typed(*this, ensure_ascii, indent, ignore_comments);
}

Array::iterator Array::begin() noexcept { return storage_.begin(); }
Array::iterator Array::end() noexcept { return storage_.end(); }

Array::const_iterator Array::begin() const noexcept { return storage_.begin(); }
Array::const_iterator Array::end() const noexcept { return storage_.end(); }

Array::const_iterator Array::cbegin() const noexcept { return storage_.cbegin(); }
Array::const_iterator Array::cend() const noexcept { return storage_.cend(); }

Array::reverse_iterator Array::rbegin() noexcept { return storage_.rbegin(); }
Array::reverse_iterator Array::rend() noexcept { return storage_.rend(); }

Array::const_reverse_iterator Array::rbegin() const noexcept { return storage_.rbegin(); }
Array::const_reverse_iterator Array::rend() const noexcept { return storage_.rend(); }

Array::const_reverse_iterator Array::crbegin() const noexcept { return storage_.crbegin(); }
Array::const_reverse_iterator Array::crend() const noexcept { return storage_.crend(); }

void Array::merge_patch(const Array& other) JSONC_EXCEPTION_TYPE {
    if (other.empty()) { return; }
    for (const auto& val : other.storage_) {
        bool exist = false;
        for (const auto& tag : storage_) {
            if (tag == val) {
                exist = true;
                break;
            }
        }
        if (!exist) { storage_.push_back(val); }
    }
}
JSONC_RESULT(void) Array::merge_patch(const JsoncType& other) JSONC_EXCEPTION_TYPE {
    if (auto* rhs = std::get_if<Array>(&other.storage_)) {
        { return merge_patch(*rhs); }
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", other.type_name()));
}

void Array::merge_comments(const Array& other) JSONC_EXCEPTION_TYPE {
    if (other.empty()) { return; }
    for (const auto& val : other.storage_) {
        for (auto& tag : storage_) {
            if (tag == val) {
                tag.set_before_comments(val.get_before_comments());
                tag.set_after_comments(val.get_after_comments());
                break;
            }
        }
    }
}

void Array::move_comments_to_before() JSONC_EXCEPTION_TYPE {
    for (auto& val : *this) {
        val.before_comments_.append_range(val.after_comments_);
        val.after_comments_.clear();
    }
}

bool Array::operator==(const Array& other) const JSONC_EXCEPTION_TYPE { return storage_ == other.storage_; }
bool Array::operator==(const JsoncType& other) const JSONC_EXCEPTION_TYPE {
    if (auto* rhs = std::get_if<Array>(&other.storage_)) { return storage_ == rhs->storage_; }
    return false;
}


constexpr ValueType        JsoncType::type() const noexcept { return static_cast<ValueType>(storage_.index()); }
constexpr std::string_view JsoncType::type_name() const noexcept {
    switch (type()) {
    case ValueType::Null:
        return "null";
    case ValueType::Boolean:
        return "boolean";
    case ValueType::Signed:
    case ValueType::Unsigned:
    case ValueType::BigInt:
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
constexpr bool JsoncType::is_number() const noexcept { return is_number_integer() || is_number_float() || is_number_big_inteager(); }
constexpr bool JsoncType::is_string() const noexcept { return hold(ValueType::String); }
constexpr bool JsoncType::is_object() const noexcept { return hold(ValueType::Object); }
constexpr bool JsoncType::is_array() const noexcept { return hold(ValueType::Array); }
constexpr bool JsoncType::is_number_big_inteager() const noexcept { return hold(ValueType::BigInt); }
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
        storage_
    );
}

JSONC_RESULT(bool) JsoncType::contains(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->contains(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

JSONC_RESULT(bool) JsoncType::contains(std::string_view index, ValueType type) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->contains(index, type); }
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
        storage_
    );
}

JSONC_RESULT(bool) JsoncType::erase(std::string_view index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->erase(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

JSONC_RESULT(bool) JsoncType::erase(size_t where) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&storage_)) { return storage->erase(where); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

JSONC_RESULT(bool) JsoncType::erase(size_t first, size_t last) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&storage_)) { return storage->erase(first, last); }
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
        storage_
    );
}

template <typename T>
    requires std::is_convertible_v<T, std::string>
JsoncType::operator T() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<std::string>(&storage_)) { return *storage; }
#ifdef JSONC_NO_EXCEPTION
    std::unreachable();
#else
    _JSONC_TYPE_ERROR("bad type cast");
#endif
}

template <detail::is_array_like T>
JsoncType::operator T() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&storage_)) { return T(storage->begin(), storage->end()); }
#ifdef JSONC_NO_EXCEPTION
    std::unreachable();
#else
    _JSONC_TYPE_ERROR("bad type cast");
#endif
}

template <detail::is_object_like T>
JsoncType::operator T() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return T(storage->begin(), storage->end()); }
#ifdef JSONC_NO_EXCEPTION
    std::unreachable();
#else
    _JSONC_TYPE_ERROR("bad type cast");
#endif
}

std::string JsoncType::dump(int indent, bool ensure_ascii, bool ignore_comments, bool global_comments) const JSONC_EXCEPTION_TYPE {
    auto result = std::visit([&](const auto& val) { return detail::dump_typed(val, ensure_ascii, indent, ignore_comments); }, storage_);
    if (!ignore_comments && global_comments) {
        auto before = detail::format_comments(before_comments_);
        if (!before.empty()) { result = before + result; }
        auto after = detail::format_comments(after_comments_);
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
        storage_
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
        storage_
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
        storage_
    );
}

template <typename T>
    requires std::is_convertible_v<T, std::string>
JSONC_RESULT(T) JsoncType::get() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<std::string>(&storage_)) { return *storage; }
    _JSONC_TYPE_ERROR("bad type cast");
}

template <detail::is_array_like T>
JSONC_RESULT(T) JsoncType::get() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&storage_)) { return T(storage->begin(), storage->end()); }
    _JSONC_TYPE_ERROR("bad type cast");
}

template <detail::is_object_like T>
JSONC_RESULT(T) JsoncType::get() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return T(storage->begin(), storage->end()); }
    _JSONC_TYPE_ERROR("bad type cast");
}

JSONC_RESULT(std::string_view) JsoncType::get_big_int_view() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<detail::BigInt>(&storage_)) { return storage->view_; }
    _JSONC_TYPE_ERROR(std::format("Type must be a big integer, but is {}", type_name()));
}

JSONC_RESULT(JsoncType&) JsoncType::operator[](std::string_view index) JSONC_EXCEPTION_TYPE {
    if (hold(ValueType::Null)) { storage_.emplace<6>(); }
    if (auto* storage = std::get_if<Object>(&storage_)) { return (*storage)[index]; }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
JSONC_RESULT(const JsoncType&) JsoncType::operator[](std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) {
        auto res = storage->storage_.find(index);
        if (res != storage->storage_.end()) { return _JSONC_MAKE_RESULT(res->second); }
        _JSONC_OUT_OF_RANGE(std::format("Invalid key: {}", index));
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

JSONC_RESULT(JsoncType&) JsoncType::operator[](size_t index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&storage_)) { return (*storage)[index]; }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}
JSONC_RESULT(const JsoncType&) JsoncType::operator[](size_t index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&storage_)) { return (*storage)[index]; }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

JSONC_RESULT(JsoncType&) JsoncType::at(std::string_view index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
JSONC_RESULT(JsoncType&) JsoncType::at(std::string_view index, const JsoncType& default_value) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->at(index, default_value); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
JSONC_RESULT(const JsoncType&) JsoncType::at(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

JSONC_RESULT(JsoncType&) JsoncType::at(size_t index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&storage_)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}
JSONC_RESULT(const JsoncType&) JsoncType::at(size_t index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&storage_)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

JSONC_RESULT(void) JsoncType::push_back(const JsoncType& val) JSONC_EXCEPTION_TYPE {
    if (hold(ValueType::Null)) { storage_.emplace<7>(); }
    if (auto* storage = std::get_if<Array>(&storage_)) {
        storage->push_back(std::move(val));
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}
JSONC_RESULT(void) JsoncType::push_back(JsoncType&& val) JSONC_EXCEPTION_TYPE {
    if (hold(ValueType::Null)) { storage_.emplace<7>(); }
    if (auto* storage = std::get_if<Array>(&storage_)) {
        storage->push_back(std::move(val));
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

JSONC_RESULT(const JsoncType&) JsoncType::front() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&storage_)) { return storage->front(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}
JSONC_RESULT(JsoncType&) JsoncType::front() JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&storage_)) { return storage->front(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

JSONC_RESULT(const JsoncType&) JsoncType::back() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&storage_)) { return storage->back(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}
JSONC_RESULT(JsoncType&) JsoncType::back() JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Array>(&storage_)) { return storage->back(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

JSONC_RESULT(JsoncType::IteratorProxy) JsoncType::items() JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return IteratorProxy(*storage); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
JSONC_RESULT(JsoncType::IteratorProxyConst) JsoncType::items() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return IteratorProxyConst(*storage); }
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
        storage_
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

void JsoncType::merge_comments(const JsoncType& other) JSONC_EXCEPTION_TYPE {
    if (is_object() && other.is_object()) {
        as<Object>().merge_comments(other.as<Object>());
    } else if (is_array() && other.is_array()) {
        as<Array>().merge_comments(other.as<Array>());
    }
    set_before_comments(other.get_before_comments());
    set_after_comments(other.get_after_comments());
}

void JsoncType::move_comments_to_before() JSONC_EXCEPTION_TYPE {
    if (is_object()) {
        as<Object>().move_comments_to_before();
    } else if (is_array()) {
        as<Array>().move_comments_to_before();
    }
    before_comments_.append_range(after_comments_);
    after_comments_.clear();
}

bool JsoncType::operator==(const JsoncType& other) const JSONC_EXCEPTION_TYPE { return storage_ == other.storage_; }

constexpr bool JsoncType::has_before_comments() const noexcept { return before_comments_.size() != 0; }
constexpr bool JsoncType::has_after_comments() const noexcept { return after_comments_.size() != 0; }

std::vector<std::string>&       JsoncType::before_comments() noexcept { return before_comments_; }
const std::vector<std::string>& JsoncType::before_comments() const noexcept { return before_comments_; }

std::vector<std::string>&       JsoncType::after_comments() noexcept { return after_comments_; }
const std::vector<std::string>& JsoncType::after_comments() const noexcept { return after_comments_; }

std::vector<std::string> JsoncType::get_before_comments() const JSONC_EXCEPTION_TYPE { return before_comments_; }
std::vector<std::string> JsoncType::get_after_comments() const JSONC_EXCEPTION_TYPE { return after_comments_; }

void JsoncType::set_before_comments(const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE { before_comments_ = comments; }
void JsoncType::set_after_comments(const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE { after_comments_ = comments; }

void JsoncType::add_before_comment(std::string_view comment) JSONC_EXCEPTION_TYPE { before_comments_.append_range(detail::split_comments(comment)); }
void JsoncType::add_after_comment(std::string_view comment) JSONC_EXCEPTION_TYPE { after_comments_.append_range(detail::split_comments(comment)); }

void JsoncType::clear_before_comments() JSONC_EXCEPTION_TYPE { before_comments_.clear(); }
void JsoncType::clear_after_comments() JSONC_EXCEPTION_TYPE { after_comments_.clear(); }

bool JsoncType::remove_before_comment(size_t comment_index) JSONC_EXCEPTION_TYPE {
    if (comment_index < before_comments_.size()) {
        before_comments_.erase(before_comments_.begin() + static_cast<decltype(before_comments_)::difference_type>(comment_index));
        return true;
    }
    return false;
}
bool JsoncType::remove_after_comment(size_t comment_index) JSONC_EXCEPTION_TYPE {
    if (comment_index < after_comments_.size()) {
        after_comments_.erase(after_comments_.begin() + static_cast<decltype(after_comments_)::difference_type>(comment_index));
        return true;
    }
    return false;
}

constexpr size_t JsoncType::before_comments_size() const noexcept { return before_comments_.size(); }
constexpr size_t JsoncType::after_comments_size() const noexcept { return after_comments_.size(); }

bool JsoncType::has_key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->has_key_before_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
bool JsoncType::has_key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->has_key_after_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

std::vector<std::string>& JsoncType::key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->key_before_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
JSONC_RESULT(const std::vector<std::string>&) JsoncType::key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->key_before_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

std::vector<std::string>& JsoncType::key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->key_after_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
JSONC_RESULT(const std::vector<std::string>&) JsoncType::key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->key_after_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

JSONC_RESULT(std::vector<std::string>) JsoncType::get_key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->get_key_before_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
JSONC_RESULT(std::vector<std::string>) JsoncType::get_key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->get_key_after_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

JSONC_RESULT(std::string) JsoncType::get_key_before_comment(std::string_view index, size_t comment_index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->get_key_before_comment(index, comment_index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
JSONC_RESULT(std::string) JsoncType::get_key_after_comment(std::string_view index, size_t comment_index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->get_key_after_comment(index, comment_index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

bool JsoncType::set_key_before_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->set_key_before_comments(index, comments); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
bool JsoncType::set_key_after_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->set_key_after_comments(index, comments); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

bool JsoncType::add_key_before_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->add_key_before_comment(index, comment); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
bool JsoncType::add_key_after_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->add_key_after_comment(index, comment); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

void JsoncType::clear_key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->clear_key_before_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
void JsoncType::clear_key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->clear_key_after_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

bool JsoncType::remove_key_before_comment(std::string_view index, size_t comment_index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->remove_key_before_comment(index, comment_index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
bool JsoncType::remove_key_after_comment(std::string_view index, size_t comment_index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->remove_key_after_comment(index, comment_index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

size_t JsoncType::key_before_comments_size(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->key_before_comments_size(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
size_t JsoncType::key_after_comments_size(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<Object>(&storage_)) { return storage->key_after_comments_size(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

} // namespace jsonc