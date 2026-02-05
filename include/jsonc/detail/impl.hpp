#pragma once
#include "exception.hpp"
#include "serializer.hpp"
#include "type.hpp"
#include <algorithm>
#include <format>
#include <ranges>

namespace jsonc::inline abi_v1_2_0::detail {

inline std::vector<std::string> split_comments(std::string_view comment) noexcept {
    std::vector<std::string> result{};
    if (!comment.empty()) {
        std::size_t pos = 0;
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

inline basic_jsonc::basic_object::basic_object(std::initializer_list<std::pair<std::string, basic_jsonc>> val) JSONC_EXCEPTION_TYPE : storage_(val) {}

inline basic_jsonc& basic_jsonc::basic_object::operator[](std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = storage_.find(index);
    if (res != storage_.end()) { return res->second; }
    return storage_.try_emplace(index).first->second;
}
inline JSONC_RESULT(const basic_jsonc&) basic_jsonc::basic_object::operator[](std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = storage_.find(index);
    if (res != storage_.end()) { return _JSONC_MAKE_RESULT(res->second); }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

inline JSONC_RESULT(basic_jsonc&) basic_jsonc::basic_object::at(std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = storage_.find(index);
    if (res != storage_.end()) { return _JSONC_MAKE_RESULT(res->second); }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}
inline JSONC_RESULT(const basic_jsonc&) basic_jsonc::basic_object::at(std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = storage_.find(index);
    if (res != storage_.end()) { return _JSONC_MAKE_RESULT(res->second); }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

inline basic_jsonc& basic_jsonc::basic_object::at(std::string_view index, const basic_jsonc& default_value) JSONC_EXCEPTION_TYPE {
    auto res = storage_.find(index);
    if (res != storage_.end()) { return res->second; }
    return storage_.try_emplace(index, default_value).first->second;
}

inline bool basic_jsonc::basic_object::contains(std::string_view index) const noexcept { return storage_.contains(index); }
inline bool basic_jsonc::basic_object::contains(std::string_view index, value_type type) const noexcept {
    auto result = storage_.find(index);
    if (result != storage_.end()) { return result->second.type() == type; }
    return false;
}

inline std::size_t basic_jsonc::basic_object::size() const noexcept { return storage_.size(); }

inline bool basic_jsonc::basic_object::empty() const noexcept { return storage_.empty(); }

inline bool basic_jsonc::basic_object::erase(std::string_view index) noexcept {
    key_comments_.erase(std::string(index));
    return storage_.erase(index);
}

inline void basic_jsonc::basic_object::clear() noexcept {
    storage_.clear();
    key_comments_.clear();
}

inline std::string
basic_jsonc::basic_object::dump(int indent, bool ensure_ascii, bool ignore_comments, bool multi_line_comments_format) const JSONC_EXCEPTION_TYPE {
    return dump_typed(*this, ensure_ascii, indent, ignore_comments, multi_line_comments_format);
}

inline const std::string& basic_jsonc::basic_object::key_index(std::size_t index) const noexcept { return storage_.key_index(index); }

inline bool basic_jsonc::basic_object::has_key_before_comments(std::string_view index) const noexcept {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return !res->second.before_comments_.empty(); }
    return false;
}
inline bool basic_jsonc::basic_object::has_key_after_comments(std::string_view index) const noexcept {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return !res->second.after_comments_.empty(); }
    return false;
}

inline std::vector<std::string>& basic_jsonc::basic_object::key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) {
        return res->second.before_comments_;
    } else {
        return key_comments_.try_emplace(std::string(index)).first->second.before_comments_;
    }
}
inline JSONC_RESULT(const std::vector<std::string>&) basic_jsonc::basic_object::key_before_comments(
    std::string_view index
) const JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.before_comments_; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

inline std::vector<std::string>& basic_jsonc::basic_object::key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) {
        return res->second.after_comments_;
    } else {
        return key_comments_.try_emplace(std::string(index)).first->second.after_comments_;
    }
}
inline JSONC_RESULT(const std::vector<std::string>&) basic_jsonc::basic_object::key_after_comments(
    std::string_view index
) const JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.after_comments_; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

inline JSONC_RESULT(std::vector<std::string>) basic_jsonc::basic_object::get_key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.before_comments_; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}
inline JSONC_RESULT(std::vector<std::string>) basic_jsonc::basic_object::get_key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.after_comments_; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

inline JSONC_RESULT(
    std::string
) basic_jsonc::basic_object::get_key_before_comment(std::string_view index, std::size_t comment_index) const JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) {
        if (comment_index < res->second.before_comments_.size()) { return res->second.before_comments_[comment_index]; }
        _JSONC_OUT_OF_RANGE("comment index out of range");
    }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}
inline JSONC_RESULT(
    std::string
) basic_jsonc::basic_object::get_key_after_comment(std::string_view index, std::size_t comment_index) const JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) {
        if (comment_index < res->second.after_comments_.size()) { return res->second.after_comments_[comment_index]; }
        _JSONC_OUT_OF_RANGE("comment index out of range");
    }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

inline bool
basic_jsonc::basic_object::set_key_before_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE {
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
inline bool basic_jsonc::basic_object::set_key_after_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE {
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

inline bool basic_jsonc::basic_object::add_key_before_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE {
    if (contains(index)) {
        auto res = key_comments_.find(index);
        if (res != key_comments_.end()) {
            res->second.before_comments_.append_range(split_comments(comment));
        } else {
            key_comments_.try_emplace(std::string(index)).first->second.before_comments_.append_range(split_comments(comment));
        }
        return true;
    }
    return false;
}
inline bool basic_jsonc::basic_object::add_key_after_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE {
    if (contains(index)) {
        auto res = key_comments_.find(index);
        if (res != key_comments_.end()) {
            res->second.after_comments_.append_range(split_comments(comment));
        } else {
            key_comments_.try_emplace(std::string(index)).first->second.after_comments_.append_range(split_comments(comment));
        }
        return true;
    }
    return false;
}

inline void basic_jsonc::basic_object::clear_key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { res->second.before_comments_.clear(); }
}
inline void basic_jsonc::basic_object::clear_key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { res->second.after_comments_.clear(); }
}

inline bool basic_jsonc::basic_object::remove_key_before_comment(std::string_view index, std::size_t comment_index) JSONC_EXCEPTION_TYPE {
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
inline bool basic_jsonc::basic_object::remove_key_after_comment(std::string_view index, std::size_t comment_index) JSONC_EXCEPTION_TYPE {
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

inline std::size_t basic_jsonc::basic_object::key_before_comments_size(std::string_view index) const noexcept {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.before_comments_.size(); }
    return 0;
}
inline std::size_t basic_jsonc::basic_object::key_after_comments_size(std::string_view index) const noexcept {
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.after_comments_.size(); }
    return 0;
}

inline basic_jsonc::basic_object::iterator basic_jsonc::basic_object::begin() noexcept { return storage_.begin(); }
inline basic_jsonc::basic_object::iterator basic_jsonc::basic_object::end() noexcept { return storage_.end(); }

inline basic_jsonc::basic_object::const_iterator basic_jsonc::basic_object::begin() const noexcept { return storage_.begin(); }
inline basic_jsonc::basic_object::const_iterator basic_jsonc::basic_object::end() const noexcept { return storage_.end(); }

inline basic_jsonc::basic_object::const_iterator basic_jsonc::basic_object::cbegin() const noexcept { return storage_.cbegin(); }
inline basic_jsonc::basic_object::const_iterator basic_jsonc::basic_object::cend() const noexcept { return storage_.cend(); }

inline basic_jsonc::basic_object::reverse_iterator basic_jsonc::basic_object::rbegin() noexcept { return storage_.rbegin(); }
inline basic_jsonc::basic_object::reverse_iterator basic_jsonc::basic_object::rend() noexcept { return storage_.rend(); }

inline basic_jsonc::basic_object::const_reverse_iterator basic_jsonc::basic_object::rbegin() const noexcept { return storage_.rbegin(); }
inline basic_jsonc::basic_object::const_reverse_iterator basic_jsonc::basic_object::rend() const noexcept { return storage_.rend(); }

inline basic_jsonc::basic_object::const_reverse_iterator basic_jsonc::basic_object::crbegin() const noexcept { return storage_.crbegin(); }
inline basic_jsonc::basic_object::const_reverse_iterator basic_jsonc::basic_object::crend() const noexcept { return storage_.crend(); }

inline void basic_jsonc::basic_object::merge_patch(const basic_object& other, bool merge_list) JSONC_EXCEPTION_TYPE {
    for (const auto& [key, val] : other) { operator[](key).merge_patch(val, merge_list); }
}
inline JSONC_RESULT(void) basic_jsonc::basic_object::merge_patch(const basic_jsonc& other, bool merge_list) JSONC_EXCEPTION_TYPE {
    if (auto* rhs = std::get_if<basic_object>(&other.storage_)) {
        merge_patch(*rhs, merge_list);
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", other.type_name()));
}

inline void basic_jsonc::basic_object::merge_comments(const basic_object& other) JSONC_EXCEPTION_TYPE {
    for (const auto& [key, val] : other) {
        if (contains(key)) {
            operator[](key).set_before_comments(val.get_before_comments());
            operator[](key).set_after_comments(val.get_after_comments());
#ifdef JSONC_USE_EXPECTED
            if (other.has_key_before_comments(key)) { set_key_before_comments(key, *other.get_key_before_comments(key)); }
            if (other.has_key_after_comments(key)) { set_key_after_comments(key, *other.get_key_after_comments(key)); }
#else
            if (other.has_key_before_comments(key)) { set_key_before_comments(key, other.get_key_before_comments(key)); }
            if (other.has_key_after_comments(key)) { set_key_after_comments(key, other.get_key_after_comments(key)); }
#endif
            operator[](key).merge_comments(val);
        }
    }
}

inline void basic_jsonc::basic_object::move_comments_to_before() JSONC_EXCEPTION_TYPE {
    for (const auto& [key, val] : *this) {
        if (has_key_after_comments(key)) {
#ifdef JSONC_USE_EXPECTED
            key_before_comments(key).append_range(*get_key_after_comments(key));
#else
            key_before_comments(key).append_range(get_key_after_comments(key));
#endif
            clear_key_after_comments(key);
        }
        key_before_comments(key).append_range(operator[](key).get_before_comments());
        operator[](key).clear_before_comments();
        key_before_comments(key).append_range(operator[](key).get_after_comments());
        operator[](key).clear_after_comments();
        operator[](key).move_comments_to_before();
    }
}

inline bool basic_jsonc::basic_object::operator==(const basic_object& other) const JSONC_EXCEPTION_TYPE { return storage_ == other.storage_; }
inline bool basic_jsonc::basic_object::operator==(const basic_jsonc& other) const JSONC_EXCEPTION_TYPE {
    if (auto* rhs = std::get_if<basic_object>(&other.storage_)) { return storage_ == rhs->storage_; }
    return false;
}


inline basic_jsonc::basic_array::basic_array(std::initializer_list<basic_jsonc> val) JSONC_EXCEPTION_TYPE : storage_(val) {}

inline constexpr basic_jsonc& basic_jsonc::basic_array::operator[](std::size_t index) noexcept { return storage_[index]; }
inline constexpr JSONC_RESULT(const basic_jsonc&) basic_jsonc::basic_array::operator[](std::size_t index) const noexcept {
    return _JSONC_MAKE_RESULT(storage_[index]);
}

inline constexpr JSONC_RESULT(basic_jsonc&) basic_jsonc::basic_array::at(std::size_t index) JSONC_EXCEPTION_TYPE {
    if (index < storage_.size()) { return _JSONC_MAKE_RESULT(storage_[index]); }
    _JSONC_OUT_OF_RANGE("Index out of range");
}
inline constexpr JSONC_RESULT(const basic_jsonc&) basic_jsonc::basic_array::at(std::size_t index) const JSONC_EXCEPTION_TYPE {
    if (index < storage_.size()) { return _JSONC_MAKE_RESULT(storage_[index]); }
    _JSONC_OUT_OF_RANGE("Index out of range");
}

inline constexpr std::size_t basic_jsonc::basic_array::size() const noexcept { return storage_.size(); }

inline constexpr bool basic_jsonc::basic_array::empty() const noexcept { return storage_.empty(); }

inline void basic_jsonc::basic_array::clear() noexcept { storage_.clear(); }

inline bool basic_jsonc::basic_array::erase(std::size_t where) {
    if (where < storage_.size()) {
        storage_.erase(storage_.begin() + static_cast<decltype(storage_)::difference_type>(where));
        return true;
    }
    return false;
}
inline bool basic_jsonc::basic_array::erase(std::size_t first, std::size_t last) {
    if (first < last && last < storage_.size()) {
        storage_.erase(
            storage_.begin() + static_cast<decltype(storage_)::difference_type>(first),
            storage_.begin() + static_cast<decltype(storage_)::difference_type>(last)
        );
        return true;
    }
    return false;
}

inline basic_jsonc::basic_array::iterator basic_jsonc::basic_array::erase(const_iterator where) JSONC_EXCEPTION_TYPE { return storage_.erase(where); }
inline basic_jsonc::basic_array::iterator basic_jsonc::basic_array::erase(const_iterator first, const_iterator last) JSONC_EXCEPTION_TYPE {
    return storage_.erase(first, last);
}

inline void basic_jsonc::basic_array::push_back(const basic_jsonc& val) JSONC_EXCEPTION_TYPE { storage_.push_back(val); }
inline void basic_jsonc::basic_array::push_back(basic_jsonc&& val) JSONC_EXCEPTION_TYPE { storage_.push_back(std::move(val)); }

inline const basic_jsonc& basic_jsonc::basic_array::front() const noexcept { return storage_.front(); }
inline basic_jsonc&       basic_jsonc::basic_array::front() noexcept { return storage_.front(); }

inline const basic_jsonc& basic_jsonc::basic_array::back() const noexcept { return storage_.back(); }
inline basic_jsonc&       basic_jsonc::basic_array::back() noexcept { return storage_.back(); }

inline std::string
basic_jsonc::basic_array::dump(int indent, bool ensure_ascii, bool ignore_comments, bool multi_line_comments_format) const JSONC_EXCEPTION_TYPE {
    return dump_typed(*this, ensure_ascii, indent, ignore_comments, multi_line_comments_format);
}

inline basic_jsonc::basic_array::iterator basic_jsonc::basic_array::begin() noexcept { return storage_.begin(); }
inline basic_jsonc::basic_array::iterator basic_jsonc::basic_array::end() noexcept { return storage_.end(); }

inline basic_jsonc::basic_array::const_iterator basic_jsonc::basic_array::begin() const noexcept { return storage_.begin(); }
inline basic_jsonc::basic_array::const_iterator basic_jsonc::basic_array::end() const noexcept { return storage_.end(); }

inline basic_jsonc::basic_array::const_iterator basic_jsonc::basic_array::cbegin() const noexcept { return storage_.cbegin(); }
inline basic_jsonc::basic_array::const_iterator basic_jsonc::basic_array::cend() const noexcept { return storage_.cend(); }

inline basic_jsonc::basic_array::reverse_iterator basic_jsonc::basic_array::rbegin() noexcept { return storage_.rbegin(); }
inline basic_jsonc::basic_array::reverse_iterator basic_jsonc::basic_array::rend() noexcept { return storage_.rend(); }

inline basic_jsonc::basic_array::const_reverse_iterator basic_jsonc::basic_array::rbegin() const noexcept { return storage_.rbegin(); }
inline basic_jsonc::basic_array::const_reverse_iterator basic_jsonc::basic_array::rend() const noexcept { return storage_.rend(); }

inline basic_jsonc::basic_array::const_reverse_iterator basic_jsonc::basic_array::crbegin() const noexcept { return storage_.crbegin(); }
inline basic_jsonc::basic_array::const_reverse_iterator basic_jsonc::basic_array::crend() const noexcept { return storage_.crend(); }

inline void basic_jsonc::basic_array::merge_patch(const basic_array& other) JSONC_EXCEPTION_TYPE {
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
inline JSONC_RESULT(void) basic_jsonc::basic_array::merge_patch(const basic_jsonc& other) JSONC_EXCEPTION_TYPE {
    if (auto* rhs = std::get_if<basic_array>(&other.storage_)) {
        merge_patch(*rhs);
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", other.type_name()));
}

inline void basic_jsonc::basic_array::merge_comments(const basic_array& other) JSONC_EXCEPTION_TYPE {
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

inline void basic_jsonc::basic_array::move_comments_to_before() JSONC_EXCEPTION_TYPE {
    for (auto& val : *this) {
        val.before_comments_.append_range(val.after_comments_);
        val.after_comments_.clear();
    }
}

inline bool basic_jsonc::basic_array::operator==(const basic_array& other) const JSONC_EXCEPTION_TYPE { return storage_ == other.storage_; }
inline bool basic_jsonc::basic_array::operator==(const basic_jsonc& other) const JSONC_EXCEPTION_TYPE {
    if (auto* rhs = std::get_if<basic_array>(&other.storage_)) { return storage_ == rhs->storage_; }
    return false;
}


inline constexpr value_type       basic_jsonc::type() const noexcept { return static_cast<value_type>(storage_.index()); }
inline constexpr std::string_view basic_jsonc::type_name() const noexcept {
    switch (type()) {
    case value_type::null:
        return "null";
    case value_type::boolean:
        return "boolean";
    case value_type::number_integer_signed:
        return "signed-inteager";
    case value_type::number_integer_unsigned:
        return "unsigned-integer";
    case value_type::string:
        return "string";
    case value_type::number_floating_point:
        return "floating-point";
    case value_type::object:
        return "object";
    case value_type::array:
        return "array";
    case value_type::number_big_integer:
        return "big-inteager";
    default:
        std::unreachable();
    }
}

inline constexpr bool basic_jsonc::hold(value_type value_type) const noexcept { return type() == value_type; }

inline constexpr bool basic_jsonc::is_null() const noexcept { return hold(value_type::null); }
inline constexpr bool basic_jsonc::is_boolean() const noexcept { return hold(value_type::boolean); }
inline constexpr bool basic_jsonc::is_number_signed() const noexcept { return hold(value_type::number_integer_signed); }
inline constexpr bool basic_jsonc::is_number_unsigned() const noexcept { return hold(value_type::number_integer_unsigned); }
inline constexpr bool basic_jsonc::is_number_integer() const noexcept { return is_number_signed() || is_number_unsigned(); }
inline constexpr bool basic_jsonc::is_number_big_inteager() const noexcept { return hold(value_type::number_big_integer); }
inline constexpr bool basic_jsonc::is_number_any_inteager() const noexcept { return is_number_integer() || is_number_big_inteager(); }
inline constexpr bool basic_jsonc::is_number_float() const noexcept { return hold(value_type::number_floating_point); }
inline constexpr bool basic_jsonc::is_number() const noexcept { return is_number_float() || is_number_any_inteager(); }
inline constexpr bool basic_jsonc::is_string() const noexcept { return hold(value_type::string); }
inline constexpr bool basic_jsonc::is_object() const noexcept { return hold(value_type::object); }
inline constexpr bool basic_jsonc::is_array() const noexcept { return hold(value_type::array); }
inline constexpr bool basic_jsonc::is_primitive() const noexcept { return is_null() || is_string() || is_number(); }
inline constexpr bool basic_jsonc::is_structured() const noexcept { return is_array() || is_object(); }

inline constexpr std::size_t basic_jsonc::size() const noexcept {
    return std::visit(
        [](const auto& val) -> std::size_t {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, basic_object> || std::is_same_v<T, basic_array>) {
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

inline JSONC_RESULT(bool) basic_jsonc::contains(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->contains(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline JSONC_RESULT(bool) basic_jsonc::contains(std::string_view index, value_type type) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->contains(index, type); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline JSONC_RESULT(bool) basic_jsonc::empty() const JSONC_EXCEPTION_TYPE {
    return std::visit(
        [&](const auto& val) -> JSONC_RESULT(bool) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, basic_object> || std::is_same_v<T, basic_array>) {
                return val.empty();
            } else {
                _JSONC_TYPE_ERROR(std::format("Type must be an object or array, but is {}", type_name()));
            }
        },
        storage_
    );
}

inline JSONC_RESULT(bool) basic_jsonc::erase(std::string_view index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->erase(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline JSONC_RESULT(bool) basic_jsonc::erase(std::size_t where) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->erase(where); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

inline JSONC_RESULT(bool) basic_jsonc::erase(std::size_t first, std::size_t last) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->erase(first, last); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

template <typename T>
    requires std::is_arithmetic_v<T>
inline basic_jsonc::operator T() const JSONC_EXCEPTION_TYPE {
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
inline basic_jsonc::operator T() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<std::string>(&storage_)) { return *storage; }
#ifdef JSONC_NO_EXCEPTION
    std::unreachable();
#else
    _JSONC_TYPE_ERROR("bad type cast");
#endif
}

template <is_array_like T>
inline basic_jsonc::operator T() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return T(storage->begin(), storage->end()); }
#ifdef JSONC_NO_EXCEPTION
    std::unreachable();
#else
    _JSONC_TYPE_ERROR("bad type cast");
#endif
}

template <is_object_like T>
inline basic_jsonc::operator T() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return T(storage->begin(), storage->end()); }
#ifdef JSONC_NO_EXCEPTION
    std::unreachable();
#else
    _JSONC_TYPE_ERROR("bad type cast");
#endif
}

inline std::string basic_jsonc::dump(
    int  indent,
    bool ensure_ascii,
    bool ignore_comments,
    bool multi_line_comments_format,
    bool global_comments
) const JSONC_EXCEPTION_TYPE {
    auto result =
        std::visit([&](const auto& val) { return dump_typed(val, ensure_ascii, indent, ignore_comments, multi_line_comments_format); }, storage_);
    if (!ignore_comments && global_comments) {
        auto before = format_comments(before_comments_, "", false, multi_line_comments_format);
        if (!before.empty()) { result = before + result; }
        auto after = format_comments(after_comments_, "", false, multi_line_comments_format);
        if (!after.empty()) {
            if (indent >= 0) { result.push_back(' '); }
            result.append(after);
            if (result.back() == '\n') { result.pop_back(); }
        }
    }
    return result;
}

template <typename T>
constexpr bool always_false_v = false;

template <typename T>
constexpr bool is_jsonc_type_convertible_v = [] {
    return []<std::size_t... I>(std::index_sequence<I...>) {
        return (std::same_as<std::variant_alternative_t<I, basic_jsonc::type_variant>, T> || ...);
    }(std::make_index_sequence<std::variant_size_v<basic_jsonc::type_variant>>{});
}();

template <typename T>
inline JSONC_RESULT(T&) basic_jsonc::as() JSONC_EXCEPTION_TYPE {
    if constexpr (is_jsonc_type_convertible_v<T>) {
        return std::visit(
            [](auto& val) -> JSONC_RESULT(T&) {
                using Type = std::decay_t<decltype(val)>;
                if constexpr (std::same_as<Type, T>) {
                    return val;
                } else {
                    _JSONC_TYPE_ERROR("bad type cast");
                }
            },
            storage_
        );
    } else {
        static_assert(always_false_v<T>, "Type is not any jsonc internal Type, use get<T> instead of as<T>");
    }
}

template <typename T>
inline JSONC_RESULT(const T&) basic_jsonc::as() const JSONC_EXCEPTION_TYPE {
    if constexpr (is_jsonc_type_convertible_v<T>) {
        return std::visit(
            [](const auto& val) -> JSONC_RESULT(const T&) {
                using Type = std::decay_t<decltype(val)>;
                if constexpr (std::same_as<Type, T>) {
                    return val;
                } else {
                    _JSONC_TYPE_ERROR("bad type cast");
                }
            },
            storage_
        );
    } else {
        static_assert(always_false_v<T>, "Type is not any jsonc internal Type, use get<T> instead of as<T>");
    }
}

template <typename T>
    requires std::is_arithmetic_v<T>
inline JSONC_RESULT(T) basic_jsonc::get() const JSONC_EXCEPTION_TYPE {
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
inline JSONC_RESULT(T) basic_jsonc::get() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<std::string>(&storage_)) { return *storage; }
    _JSONC_TYPE_ERROR("bad type cast");
}

template <is_array_like T>
inline JSONC_RESULT(T) basic_jsonc::get() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return T(storage->begin(), storage->end()); }
    _JSONC_TYPE_ERROR("bad type cast");
}

template <is_object_like T>
inline JSONC_RESULT(T) basic_jsonc::get() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return T(storage->begin(), storage->end()); }
    _JSONC_TYPE_ERROR("bad type cast");
}

inline JSONC_RESULT(std::string) basic_jsonc::get_big_int_view() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_big_int>(&storage_)) { return storage->view_; }
    _JSONC_TYPE_ERROR(std::format("Type must be a big integer, but is {}", type_name()));
}

inline JSONC_RESULT(std::string) basic_jsonc::get_any_int_view() const JSONC_EXCEPTION_TYPE {
    return std::visit(
        [&](const auto& val) -> JSONC_RESULT(std::string) {
            using Type = std::decay_t<decltype(val)>;
            if constexpr (std::same_as<basic_big_int, Type>) {
                return val.view_;
            } else if constexpr (std::same_as<std::int64_t, Type> || std::same_as<std::uint64_t, Type>) {
                return std::to_string(val);
            } else {
                _JSONC_TYPE_ERROR(std::format("Type must be a any integer type, but is {}", type_name()));
            }
        },
        storage_
    );
}

inline JSONC_RESULT(basic_jsonc&) basic_jsonc::operator[](std::string_view index) JSONC_EXCEPTION_TYPE {
    if (hold(value_type::null)) { storage_.emplace<6>(); }
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return (*storage)[index]; }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
inline JSONC_RESULT(const basic_jsonc&) basic_jsonc::operator[](std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) {
        auto res = storage->storage_.find(index);
        if (res != storage->storage_.end()) { return _JSONC_MAKE_RESULT(res->second); }
        _JSONC_OUT_OF_RANGE(std::format("Invalid key: {}", index));
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline JSONC_RESULT(basic_jsonc&) basic_jsonc::operator[](std::size_t index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return (*storage)[index]; }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}
inline JSONC_RESULT(const basic_jsonc&) basic_jsonc::operator[](std::size_t index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return (*storage)[index]; }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

inline JSONC_RESULT(basic_jsonc&) basic_jsonc::at(std::string_view index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
inline JSONC_RESULT(basic_jsonc&) basic_jsonc::at(std::string_view index, const basic_jsonc& default_value) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->at(index, default_value); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
inline JSONC_RESULT(const basic_jsonc&) basic_jsonc::at(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline JSONC_RESULT(basic_jsonc&) basic_jsonc::at(std::size_t index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}
inline JSONC_RESULT(const basic_jsonc&) basic_jsonc::at(std::size_t index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

inline JSONC_RESULT(void) basic_jsonc::push_back(const basic_jsonc& val) JSONC_EXCEPTION_TYPE {
    if (hold(value_type::null)) { storage_.emplace<7>(); }
    if (auto* storage = std::get_if<basic_array>(&storage_)) {
        storage->push_back(std::move(val));
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}
inline JSONC_RESULT(void) basic_jsonc::push_back(basic_jsonc&& val) JSONC_EXCEPTION_TYPE {
    if (hold(value_type::null)) { storage_.emplace<7>(); }
    if (auto* storage = std::get_if<basic_array>(&storage_)) {
        storage->push_back(std::move(val));
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

inline JSONC_RESULT(const basic_jsonc&) basic_jsonc::front() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->front(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}
inline JSONC_RESULT(basic_jsonc&) basic_jsonc::front() JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->front(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

inline JSONC_RESULT(const basic_jsonc&) basic_jsonc::back() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->back(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}
inline JSONC_RESULT(basic_jsonc&) basic_jsonc::back() JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->back(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

inline JSONC_RESULT(basic_jsonc::iterator_proxy) basic_jsonc::items() JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return iterator_proxy(*storage); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
inline JSONC_RESULT(basic_jsonc::const_iterator_proxy) basic_jsonc::items() const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return const_iterator_proxy(*storage); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline JSONC_RESULT(void) basic_jsonc::clear() JSONC_EXCEPTION_TYPE {
    return std::visit(
        [&](auto& val) -> JSONC_RESULT(void) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, basic_object> || std::is_same_v<T, basic_array>) {
                val.clear();
                return _JSONC_MAKE_VOID_RESULT();
            } else {
                _JSONC_TYPE_ERROR(std::format("Type must be an array or object, but is {}", type_name()));
            }
        },
        storage_
    );
}

inline basic_jsonc::iterator basic_jsonc::begin() noexcept { return iterator::make_begin<false>(*this); }
inline basic_jsonc::iterator basic_jsonc::end() noexcept { return iterator::make_end<false>(*this); }

inline basic_jsonc::const_iterator basic_jsonc::begin() const noexcept { return const_iterator::make_begin<false>(*this); }
inline basic_jsonc::const_iterator basic_jsonc::end() const noexcept { return const_iterator::make_end<false>(*this); }

inline basic_jsonc::const_iterator basic_jsonc::cbegin() const noexcept { return const_iterator::make_begin<false>(*this); }
inline basic_jsonc::const_iterator basic_jsonc::cend() const noexcept { return const_iterator::make_end<false>(*this); }

inline basic_jsonc::reverse_iterator basic_jsonc::rbegin() noexcept { return reverse_iterator::make_begin<true>(*this); }
inline basic_jsonc::reverse_iterator basic_jsonc::rend() noexcept { return reverse_iterator::make_end<true>(*this); }

inline basic_jsonc::const_reverse_iterator basic_jsonc::rbegin() const noexcept { return const_reverse_iterator::make_begin<true>(*this); }
inline basic_jsonc::const_reverse_iterator basic_jsonc::rend() const noexcept { return const_reverse_iterator::make_end<true>(*this); }

inline basic_jsonc::const_reverse_iterator basic_jsonc::crbegin() const noexcept { return const_reverse_iterator::make_begin<true>(*this); }
inline basic_jsonc::const_reverse_iterator basic_jsonc::crend() const noexcept { return const_reverse_iterator::make_end<true>(*this); }

inline void basic_jsonc::merge_patch(const basic_jsonc& other, bool merge_list) JSONC_EXCEPTION_TYPE {
    if (is_object() && other.is_object()) {
#ifdef JSONC_USE_EXPECTED
        as<basic_object>()->get().merge_patch(other.as<basic_object>()->get(), merge_list);
#else
        as<basic_object>().merge_patch(other.as<basic_object>(), merge_list);
#endif
    } else if (is_array() && other.is_array() && merge_list) {
#ifdef JSONC_USE_EXPECTED
        as<basic_array>()->get().merge_patch(other.as<basic_array>()->get());
#else
        as<basic_array>().merge_patch(other.as<basic_array>());
#endif
    } else {
        operator=(other);
    }
}

inline void basic_jsonc::merge_comments(const basic_jsonc& other) JSONC_EXCEPTION_TYPE {
    if (is_object() && other.is_object()) {
#ifdef JSONC_USE_EXPECTED
        as<basic_object>()->get().merge_comments(other.as<basic_object>()->get());
#else
        as<basic_object>().merge_comments(other.as<basic_object>());
#endif
    } else if (is_array() && other.is_array()) {
#ifdef JSONC_USE_EXPECTED
        as<basic_array>()->get().merge_comments(other.as<basic_array>()->get());
#else
        as<basic_array>().merge_comments(other.as<basic_array>());
#endif
    }
    set_before_comments(other.get_before_comments());
    set_after_comments(other.get_after_comments());
}

inline void basic_jsonc::move_comments_to_before() JSONC_EXCEPTION_TYPE {
    if (is_object()) {
#ifdef JSONC_USE_EXPECTED
        as<basic_object>()->get().move_comments_to_before();
#else
        as<basic_object>().move_comments_to_before();
#endif
    } else if (is_array()) {
#ifdef JSONC_USE_EXPECTED
        as<basic_array>()->get().move_comments_to_before();
#else
        as<basic_array>().move_comments_to_before();
#endif
    }
    before_comments_.append_range(after_comments_);
    after_comments_.clear();
}

inline bool basic_jsonc::operator==(const basic_jsonc& other) const JSONC_EXCEPTION_TYPE { return storage_ == other.storage_; }

inline constexpr bool basic_jsonc::has_before_comments() const noexcept { return before_comments_.size() != 0; }
inline constexpr bool basic_jsonc::has_after_comments() const noexcept { return after_comments_.size() != 0; }

inline std::vector<std::string>&       basic_jsonc::before_comments() noexcept { return before_comments_; }
inline const std::vector<std::string>& basic_jsonc::before_comments() const noexcept { return before_comments_; }

inline std::vector<std::string>&       basic_jsonc::after_comments() noexcept { return after_comments_; }
inline const std::vector<std::string>& basic_jsonc::after_comments() const noexcept { return after_comments_; }

inline std::vector<std::string> basic_jsonc::get_before_comments() const JSONC_EXCEPTION_TYPE { return before_comments_; }
inline std::vector<std::string> basic_jsonc::get_after_comments() const JSONC_EXCEPTION_TYPE { return after_comments_; }

inline void basic_jsonc::set_before_comments(const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE { before_comments_ = comments; }
inline void basic_jsonc::set_after_comments(const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE { after_comments_ = comments; }

inline void basic_jsonc::add_before_comment(std::string_view comment) JSONC_EXCEPTION_TYPE { before_comments_.append_range(split_comments(comment)); }
inline void basic_jsonc::add_after_comment(std::string_view comment) JSONC_EXCEPTION_TYPE { after_comments_.append_range(split_comments(comment)); }

inline void basic_jsonc::clear_before_comments() JSONC_EXCEPTION_TYPE { before_comments_.clear(); }
inline void basic_jsonc::clear_after_comments() JSONC_EXCEPTION_TYPE { after_comments_.clear(); }

inline bool basic_jsonc::remove_before_comment(std::size_t comment_index) JSONC_EXCEPTION_TYPE {
    if (comment_index < before_comments_.size()) {
        before_comments_.erase(before_comments_.begin() + static_cast<decltype(before_comments_)::difference_type>(comment_index));
        return true;
    }
    return false;
}
inline bool basic_jsonc::remove_after_comment(std::size_t comment_index) JSONC_EXCEPTION_TYPE {
    if (comment_index < after_comments_.size()) {
        after_comments_.erase(after_comments_.begin() + static_cast<decltype(after_comments_)::difference_type>(comment_index));
        return true;
    }
    return false;
}

inline constexpr std::size_t basic_jsonc::before_comments_size() const noexcept { return before_comments_.size(); }
inline constexpr std::size_t basic_jsonc::after_comments_size() const noexcept { return after_comments_.size(); }

inline JSONC_RESULT(bool) basic_jsonc::has_key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->has_key_before_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
inline JSONC_RESULT(bool) basic_jsonc::has_key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->has_key_after_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline JSONC_RESULT(std::vector<std::string>&) basic_jsonc::key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->key_before_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
inline JSONC_RESULT(const std::vector<std::string>&) basic_jsonc::key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->key_before_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline JSONC_RESULT(std::vector<std::string>&) basic_jsonc::key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->key_after_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
inline JSONC_RESULT(const std::vector<std::string>&) basic_jsonc::key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->key_after_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline JSONC_RESULT(std::vector<std::string>) basic_jsonc::get_key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->get_key_before_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
inline JSONC_RESULT(std::vector<std::string>) basic_jsonc::get_key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->get_key_after_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline JSONC_RESULT(std::string) basic_jsonc::get_key_before_comment(std::string_view index, std::size_t comment_index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->get_key_before_comment(index, comment_index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
inline JSONC_RESULT(std::string) basic_jsonc::get_key_after_comment(std::string_view index, std::size_t comment_index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->get_key_after_comment(index, comment_index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline JSONC_RESULT(
    bool
) basic_jsonc::set_key_before_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->set_key_before_comments(index, comments); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
inline JSONC_RESULT(bool) basic_jsonc::set_key_after_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->set_key_after_comments(index, comments); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline JSONC_RESULT(bool) basic_jsonc::add_key_before_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->add_key_before_comment(index, comment); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
inline JSONC_RESULT(bool) basic_jsonc::add_key_after_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->add_key_after_comment(index, comment); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline JSONC_RESULT(void) basic_jsonc::clear_key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) {
        storage->clear_key_before_comments(index);
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
inline JSONC_RESULT(void) basic_jsonc::clear_key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) {
        storage->clear_key_after_comments(index);
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline JSONC_RESULT(bool) basic_jsonc::remove_key_before_comment(std::string_view index, std::size_t comment_index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->remove_key_before_comment(index, comment_index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
inline JSONC_RESULT(bool) basic_jsonc::remove_key_after_comment(std::string_view index, std::size_t comment_index) JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->remove_key_after_comment(index, comment_index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline JSONC_RESULT(std::size_t) basic_jsonc::key_before_comments_size(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->key_before_comments_size(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}
inline JSONC_RESULT(std::size_t) basic_jsonc::key_after_comments_size(std::string_view index) const JSONC_EXCEPTION_TYPE {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->key_after_comments_size(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline bool is_int(std::string_view view) {
    if (view.starts_with('-') || view.starts_with('+')) { view.remove_prefix(1); }
    return std::ranges::all_of(view, [](unsigned char c) { return std::isdigit(c); });
}

inline std::optional<basic_jsonc> basic_jsonc::from_big_int(std::string_view view) noexcept {
    if (view.empty()) { return std::nullopt; }
    if (view.starts_with('-')) {
        std::int64_t res{};
        auto [ptr, ec] = std::from_chars(view.data(), view.data() + view.size(), res);
        if (ec != std::errc() || ptr != view.data() + view.size()) {
            if (is_int(view)) { return basic_big_int(view); }
            return std::nullopt;
        }
        return res;
    }
    std::uint64_t res{};
    auto [ptr, ec] = std::from_chars(view.data(), view.data() + view.size(), res);
    if (ec != std::errc() || ptr != view.data() + view.size()) {
        if (is_int(view)) { return basic_big_int(view); }
        return std::nullopt;
    }
    return res;
}

} // namespace jsonc::inline abi_v1_2_0::detail