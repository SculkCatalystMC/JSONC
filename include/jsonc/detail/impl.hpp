#pragma once
#include "exception.hpp"
#include "serializer.hpp"
#include "type.hpp"
#include <format>

namespace jsonc {

JsoncType& Object::operator[](std::string_view index) {
    auto res = mStorage.find(index);
    if (res != mStorage.end()) { return res->second; }
    mInsertIndex[mNextInsertIndex] = index;
    mNextInsertIndex++;
    return mStorage.try_emplace(std::string(index)).first->second;
}
JSONC_RESULT(const JsoncType&) Object::operator[](std::string_view index) const {
    auto res = mStorage.find(index);
    if (res != mStorage.end()) { return _JSONC_MAKE_RESULT(res->second); }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

JSONC_RESULT(JsoncType&) Object::at(std::string_view index) {
    auto res = mStorage.find(index);
    if (res != mStorage.end()) { return _JSONC_MAKE_RESULT(res->second); }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}
JSONC_RESULT(const JsoncType&) Object::at(std::string_view index) const {
    auto res = mStorage.find(index);
    if (res != mStorage.end()) { return _JSONC_MAKE_RESULT(res->second); }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

JsoncType& Object::at(std::string_view index, JsoncType const& default_value) {
    auto res = mStorage.find(index);
    if (res != mStorage.end()) { return res->second; }
    return mStorage.try_emplace(std::string(index), default_value).first->second;
}

bool Object::contains(std::string_view index) const noexcept { return mStorage.contains(index); }

size_t Object::size() const noexcept { return mStorage.size(); }

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

std::vector<std::string>& Object::key_before_comments(std::string_view index) {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) {
        return res->second.mBeforeComments;
    } else {
        return mKeyComments.try_emplace(std::string(index)).first->second.mBeforeComments;
    }
}
JSONC_RESULT(const std::vector<std::string>&) Object::key_before_comments(std::string_view index) const {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { return res->second.mBeforeComments; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

std::vector<std::string>& Object::key_after_comments(std::string_view index) {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) {
        return res->second.mAfterComments;
    } else {
        return mKeyComments.try_emplace(std::string(index)).first->second.mAfterComments;
    }
}
JSONC_RESULT(const std::vector<std::string>&) Object::key_after_comments(std::string_view index) const {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { return res->second.mAfterComments; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

JSONC_RESULT(std::vector<std::string>) Object::get_key_before_comments(std::string_view index) const {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { return res->second.mBeforeComments; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}
JSONC_RESULT(std::vector<std::string>) Object::get_key_after_comments(std::string_view index) const {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { return res->second.mAfterComments; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

JSONC_RESULT(std::string) Object::get_key_before_comments(std::string_view index, size_t comment_index) const {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) {
        if (comment_index < res->second.mBeforeComments.size()) { return res->second.mBeforeComments[comment_index]; }
        _JSONC_OUT_OF_RANGE("comment index out of range");
    }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}
JSONC_RESULT(std::string) Object::get_key_after_comments(std::string_view index, size_t comment_index) const {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) {
        if (comment_index < res->second.mAfterComments.size()) { return res->second.mAfterComments[comment_index]; }
        _JSONC_OUT_OF_RANGE("comment index out of range");
    }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

bool Object::set_key_before_comments(std::string_view index, std::vector<std::string> const& comments) {
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
bool Object::set_key_after_comments(std::string_view index, std::vector<std::string> const& comments) {
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

bool Object::add_key_before_comment(std::string_view index, std::string_view comment) {
    if (contains(index)) {
        auto res = mKeyComments.find(index);
        if (res != mKeyComments.end()) {
            res->second.mBeforeComments.emplace_back(comment);
        } else {
            mKeyComments.try_emplace(std::string(index)).first->second.mBeforeComments.emplace_back(comment);
        }
        return true;
    }
    return false;
}
bool Object::add_key_after_comment(std::string_view index, std::string_view comment) {
    if (contains(index)) {
        auto res = mKeyComments.find(index);
        if (res != mKeyComments.end()) {
            res->second.mAfterComments.emplace_back(comment);
        } else {
            mKeyComments.try_emplace(std::string(index)).first->second.mAfterComments.emplace_back(comment);
        }
        return true;
    }
    return false;
}

void Object::clear_key_before_comments(std::string_view index) {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { res->second.mBeforeComments.clear(); }
}
void Object::clear_key_after_comments(std::string_view index) {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) { res->second.mAfterComments.clear(); }
}

bool Object::remove_key_before_comment(std::string_view index, size_t comment_index) {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) {
        if (comment_index < res->second.mBeforeComments.size()) {
            res->second.mAfterComments.erase(res->second.mBeforeComments.begin() + comment_index);
            return true;
        }
    }
    return false;
}
bool Object::remove_key_after_comment(std::string_view index, size_t comment_index) {
    auto res = mKeyComments.find(index);
    if (res != mKeyComments.end()) {
        if (comment_index < res->second.mAfterComments.size()) {
            res->second.mAfterComments.erase(res->second.mAfterComments.begin() + comment_index);
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

Object::const_iterator::const_iterator(const detail::StringHashMap<JsoncType>& storage) : mStorageRef(storage) {}

Object::const_iterator Object::const_iterator::make_begin(const Object& var) noexcept {
    const_iterator result{var.mStorage};
    result.mIterator = var.mInsertIndex.begin();
    return result;
}

Object::const_iterator Object::const_iterator::make_end(const Object& var) noexcept {
    const_iterator result{var.mStorage};
    result.mIterator = var.mInsertIndex.end();
    return result;
}

const std::pair<const std::string, JsoncType>& Object::const_iterator::operator*() const {
    auto result = mStorageRef.find(mIterator->second);
    if (result != mStorageRef.end()) { return *result; }
    _JSONC_TYPE_ERROR("iterator error");
}

const std::pair<const std::string, JsoncType>* Object::const_iterator::operator->() const { return &operator*(); }

Object::const_iterator& Object::const_iterator::operator++() noexcept {
    ++mIterator;
    return *this;
}

Object::const_iterator Object::const_iterator::operator++(int) noexcept {
    Object::const_iterator tmp = *this;
    ++*this;
    return tmp;
}

Object::const_iterator& Object::const_iterator::operator--() noexcept {
    --mIterator;
    return *this;
}

Object::const_iterator Object::const_iterator::operator--(int) noexcept {
    Object::const_iterator tmp = *this;
    --*this;
    return tmp;
}

bool Object::const_iterator::operator==(const_iterator const& rhs) const noexcept { return mIterator == rhs.mIterator; }

Object::iterator::iterator(detail::StringHashMap<JsoncType>& storage) : mStorageRef(storage) {}

Object::iterator Object::iterator::make_begin(Object& var) noexcept {
    iterator result{var.mStorage};
    result.mIterator = var.mInsertIndex.begin();
    return result;
}

Object::iterator Object::iterator::make_end(Object& var) noexcept {
    iterator result{var.mStorage};
    result.mIterator = var.mInsertIndex.end();
    return result;
}

std::pair<const std::string, JsoncType>& Object::iterator::operator*() const {
    auto result = mStorageRef.find(mIterator->second);
    if (result != mStorageRef.end()) { return *result; }
    _JSONC_TYPE_ERROR("iter orror");
}

std::pair<const std::string, JsoncType>* Object::iterator::operator->() const { return &operator*(); }

Object::iterator& Object::iterator::operator++() noexcept {
    ++mIterator;
    return *this;
}

Object::iterator Object::iterator::operator++(int) noexcept {
    Object::iterator tmp = *this;
    ++*this;
    return tmp;
}

Object::iterator& Object::iterator::operator--() noexcept {
    --mIterator;
    return *this;
}

Object::iterator Object::iterator::operator--(int) noexcept {
    Object::iterator tmp = *this;
    --*this;
    return tmp;
}

bool Object::iterator::operator==(iterator const& rhs) const noexcept { return mIterator == rhs.mIterator; }

Object::iterator Object::begin() noexcept { return iterator::make_begin(*this); }
Object::iterator Object::end() noexcept { return iterator::make_end(*this); }

Object::const_iterator Object::begin() const noexcept { return const_iterator::make_begin(*this); }
Object::const_iterator Object::end() const noexcept { return const_iterator::make_end(*this); }

Object::const_iterator Object::cbegin() const noexcept { return const_iterator::make_begin(*this); }
Object::const_iterator Object::cend() const noexcept { return const_iterator::make_end(*this); }

constexpr JsoncType& Array::operator[](size_t index) noexcept { return mStorage[index]; }
constexpr JSONC_RESULT(const JsoncType&) Array::operator[](size_t index) const noexcept { return _JSONC_MAKE_RESULT(mStorage[index]); }

constexpr JSONC_RESULT(JsoncType&) Array::at(size_t index) {
    if (index < mStorage.size()) { return _JSONC_MAKE_RESULT(mStorage[index]); }
    _JSONC_OUT_OF_RANGE("Index out of range");
}
constexpr JSONC_RESULT(const JsoncType&) Array::at(size_t index) const {
    if (index < mStorage.size()) { return _JSONC_MAKE_RESULT(mStorage[index]); }
    _JSONC_OUT_OF_RANGE("Index out of range");
}

constexpr size_t Array::size() const noexcept { return mStorage.size(); }

void Array::push_back(JsoncType const& val) { mStorage.push_back(val); }
void Array::push_back(JsoncType&& val) { mStorage.push_back(std::move(val)); }

Array::iterator Array::begin() noexcept { return mStorage.begin(); }
Array::iterator Array::end() noexcept { return mStorage.end(); }

Array::const_iterator Array::begin() const noexcept { return mStorage.begin(); }
Array::const_iterator Array::end() const noexcept { return mStorage.end(); }

Array::const_iterator Array::cbegin() const noexcept { return mStorage.cbegin(); }
Array::const_iterator Array::cend() const noexcept { return mStorage.cend(); }

Array::reverse_iterator Array::rbegin() noexcept { return mStorage.rbegin(); }
Array::reverse_iterator Array::rend() noexcept { return mStorage.rend(); }

Array::const_reverse_iterator Array::crbegin() const noexcept { return mStorage.crbegin(); }
Array::const_reverse_iterator Array::crend() const noexcept { return mStorage.crend(); }


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

std::string JsoncType::dump(int indent, bool ensure_ascii, bool global_comments) const {
    auto result = std::visit([&](auto const& val) { return detail::dump_typed(val, ensure_ascii, indent); }, mStorage);
    if (global_comments) {
        auto before = detail::format_comments(mBeforeComments);
        if (!before.empty()) { result = before + result; }
        auto after = detail::format_comments(mAfterComments);
        if (!after.empty()) {
            if (indent >= 0) { result.push_back(' '); }
            result.append(after);
        }
    }
    return result;
}

template <typename T>
    requires detail::jsonc_type_convertible_v<T>
JSONC_RESULT(T) JsoncType::get() const {
    return std::visit(
        [](auto const& val) -> JSONC_RESULT(T) {
            using Type = std::decay_t<decltype(val)>;
            if constexpr (std::is_convertible_v<Type, T>) { return static_cast<T>(val); }
            std::unreachable();
        },
        mStorage
    );
}

JSONC_RESULT(JsoncType&) JsoncType::operator[](std::string_view index) {
    if (hold(ValueType::Object)) { return std::get<Object>(mStorage)[index]; }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", std::string(type_name())));
}
JSONC_RESULT(const JsoncType&) JsoncType::operator[](std::string_view index) const {
    if (hold(ValueType::Object)) {
        auto& obj = std::get<Object>(mStorage);
        auto  res = obj.mStorage.find(index);
        if (res != obj.mStorage.end()) { return _JSONC_MAKE_RESULT(res->second); }
        _JSONC_OUT_OF_RANGE(std::format("Invalid key: {}", index));
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", std::string(type_name())));
}

JSONC_RESULT(JsoncType&) JsoncType::operator[](size_t index) {
    if (hold(ValueType::Array)) { return std::get<Array>(mStorage)[index]; }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", std::string(type_name())));
}
JSONC_RESULT(const JsoncType&) JsoncType::operator[](size_t index) const {
    if (hold(ValueType::Array)) { return std::get<Array>(mStorage)[index]; }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", std::string(type_name())));
}

JSONC_RESULT(JsoncType&) JsoncType::at(std::string_view index) {
    if (hold(ValueType::Object)) { return std::get<Object>(mStorage).at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", std::string(type_name())));
}
JSONC_RESULT(JsoncType&) JsoncType::at(std::string_view index, JsoncType const& default_value) {
    if (hold(ValueType::Object)) { return std::get<Object>(mStorage).at(index, default_value); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", std::string(type_name())));
}
JSONC_RESULT(const JsoncType&) JsoncType::at(std::string_view index) const {
    if (hold(ValueType::Object)) { return std::get<Object>(mStorage).at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", std::string(type_name())));
}

JSONC_RESULT(JsoncType&) JsoncType::at(size_t index) {
    if (hold(ValueType::Array)) { return std::get<Array>(mStorage).at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", std::string(type_name())));
}
JSONC_RESULT(const JsoncType&) JsoncType::at(size_t index) const {
    if (hold(ValueType::Array)) { return std::get<Array>(mStorage).at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", std::string(type_name())));
}

constexpr bool JsoncType::has_before_comments() const noexcept { return mBeforeComments.size() != 0; }
constexpr bool JsoncType::has_after_comments() const noexcept { return mAfterComments.size() != 0; }

std::vector<std::string>&       JsoncType::before_comments() noexcept { return mBeforeComments; }
const std::vector<std::string>& JsoncType::before_comments() const noexcept { return mBeforeComments; }

std::vector<std::string>&       JsoncType::after_comments() noexcept { return mAfterComments; }
const std::vector<std::string>& JsoncType::after_comments() const noexcept { return mAfterComments; }

std::vector<std::string> JsoncType::get_before_comments() const { return mBeforeComments; }
std::vector<std::string> JsoncType::get_after_comments() const { return mAfterComments; }

void JsoncType::set_before_comments(std::vector<std::string> const& comments) { mBeforeComments = comments; }
void JsoncType::set_after_comments(std::vector<std::string> const& comments) { mAfterComments = comments; }

void JsoncType::add_before_comment(std::string_view comment) { mBeforeComments.emplace_back(comment); }
void JsoncType::add_after_comment(std::string_view comment) { mAfterComments.emplace_back(comment); }

void JsoncType::clear_before_comments() { mBeforeComments.clear(); }
void JsoncType::clear_after_comments() { mBeforeComments.clear(); }

bool JsoncType::remove_before_comment(size_t comment_index) {
    if (comment_index < mBeforeComments.size()) {
        mBeforeComments.erase(mBeforeComments.begin() + comment_index);
        return true;
    }
    return false;
}
bool JsoncType::remove_after_comment(size_t comment_index) {
    if (comment_index < mAfterComments.size()) {
        mAfterComments.erase(mAfterComments.begin() + comment_index);
        return true;
    }
    return false;
}

constexpr size_t JsoncType::before_comments_size() const noexcept { return mBeforeComments.size(); }
constexpr size_t JsoncType::after_comments_size() const noexcept { return mAfterComments.size(); }

} // namespace jsonc