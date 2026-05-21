// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "deserializer.hpp"
#include "exception.hpp"
#include "serializer.hpp"
#include "type.hpp"
#include <algorithm>
#include <limits>
#include <ranges>

namespace sculk::jsonc::inline abi_v1_4_1::detail {

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

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_object::basic_object(
    std::initializer_list<std::pair<std::string, basic_jsonc<_IsOrdered, _AllowComments>>> val
) {
    for (const auto& [k, v] : val) { storage_.try_emplace(k, v); }
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>& basic_jsonc<_IsOrdered, _AllowComments>::basic_object::operator[](std::string_view index) {
    auto res = storage_.find(index);
    if (res != storage_.end()) { return res->second; }
    return storage_.try_emplace(std::string(index)).first->second;
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(const basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::basic_object::operator[](
    std::string_view index
) const {
    auto res = storage_.find(index);
    if (res != storage_.end()) { return _JSONC_MAKE_RESULT(res->second); }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::basic_object::at(std::string_view index) {
    auto res = storage_.find(index);
    if (res != storage_.end()) { return _JSONC_MAKE_RESULT(res->second); }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(const basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::basic_object::at(
    std::string_view index
) const {
    auto res = storage_.find(index);
    if (res != storage_.end()) { return _JSONC_MAKE_RESULT(res->second); }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>&
basic_jsonc<_IsOrdered, _AllowComments>::basic_object::at(std::string_view index, const basic_jsonc<_IsOrdered, _AllowComments>& default_value) {
    auto res = storage_.find(index);
    if (res != storage_.end()) { return res->second; }
    return storage_.try_emplace(index, default_value).first->second;
}

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_object::contains(std::string_view index) const noexcept {
    return storage_.contains(index);
}

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_object::contains(std::string_view index, value_type type) const noexcept {
    auto result = storage_.find(index);
    if (result != storage_.end()) { return result->second.type() == type; }
    return false;
}

template <bool _IsOrdered, bool _AllowComments>
inline std::size_t basic_jsonc<_IsOrdered, _AllowComments>::basic_object::size() const noexcept {
    return storage_.size();
}

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_object::empty() const noexcept {
    return storage_.empty();
}

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_object::erase(std::string_view index) noexcept {
    key_comments_.erase(std::string(index));
    if constexpr (_IsOrdered) {
        return storage_.erase(index);
    } else {
        return storage_.erase(std::string(index));
    }
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::basic_object::clear() noexcept {
    storage_.clear();
    key_comments_.clear();
}

template <bool _IsOrdered, bool _AllowComments>
inline std::string basic_jsonc<_IsOrdered, _AllowComments>::basic_object::dump(
    int  indent,
    bool ensure_ascii,
    bool ignore_comments,
    bool multi_line_comments_format
) const {
    return dump_typed<_IsOrdered, _AllowComments>(*this, ensure_ascii, indent, ignore_comments, multi_line_comments_format);
}

template <bool _IsOrdered, bool _AllowComments>
inline const std::string& basic_jsonc<_IsOrdered, _AllowComments>::basic_object::key_index(std::size_t index) const noexcept {
    if constexpr (_IsOrdered) {
        return storage_.key_index(index);
    } else {
        auto it = std::next(storage_.begin(), static_cast<std::map<std::string, basic_jsonc<_IsOrdered, _AllowComments>>::difference_type>(index));
        if (it != storage_.end()) { return it->first; }
        std::abort();
    }
}

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_object::has_key_before_comments(std::string_view index) const noexcept
    requires(_AllowComments)
{
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return !res->second.before_comments_.empty(); }
    return false;
}

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_object::has_key_after_comments(std::string_view index) const noexcept
    requires(_AllowComments)
{
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return !res->second.after_comments_.empty(); }
    return false;
}

template <bool _IsOrdered, bool _AllowComments>
inline std::vector<std::string>& basic_jsonc<_IsOrdered, _AllowComments>::basic_object::key_before_comments(std::string_view index)
    requires(_AllowComments)
{
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) {
        return res->second.before_comments_;
    } else {
        return key_comments_.try_emplace(std::string(index)).first->second.before_comments_;
    }
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(const std::vector<std::string>&) basic_jsonc<_IsOrdered, _AllowComments>::basic_object::key_before_comments(
    std::string_view index
) const
    requires(_AllowComments)
{
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.before_comments_; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

template <bool _IsOrdered, bool _AllowComments>
inline std::vector<std::string>& basic_jsonc<_IsOrdered, _AllowComments>::basic_object::key_after_comments(std::string_view index)
    requires(_AllowComments)
{
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) {
        return res->second.after_comments_;
    } else {
        return key_comments_.try_emplace(std::string(index)).first->second.after_comments_;
    }
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(const std::vector<std::string>&) basic_jsonc<_IsOrdered, _AllowComments>::basic_object::key_after_comments(
    std::string_view index
) const
    requires(_AllowComments)
{
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.after_comments_; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(std::vector<std::string>) basic_jsonc<_IsOrdered, _AllowComments>::basic_object::get_key_before_comments(
    std::string_view index
) const
    requires(_AllowComments)
{
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.before_comments_; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(std::vector<std::string>) basic_jsonc<_IsOrdered, _AllowComments>::basic_object::get_key_after_comments(
    std::string_view index
) const
    requires(_AllowComments)
{
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.after_comments_; }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(
    std::string
) basic_jsonc<_IsOrdered, _AllowComments>::basic_object::get_key_before_comment(std::string_view index, std::size_t comment_index) const
    requires(_AllowComments)
{
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) {
        if (comment_index < res->second.before_comments_.size()) { return res->second.before_comments_[comment_index]; }
        _JSONC_OUT_OF_RANGE("comment index out of range");
    }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(
    std::string
) basic_jsonc<_IsOrdered, _AllowComments>::basic_object::get_key_after_comment(std::string_view index, std::size_t comment_index) const
    requires(_AllowComments)
{
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) {
        if (comment_index < res->second.after_comments_.size()) { return res->second.after_comments_[comment_index]; }
        _JSONC_OUT_OF_RANGE("comment index out of range");
    }
    _JSONC_KEY_ERROR(std::format("Invalid key: {}", index));
}


template <bool _IsOrdered, bool _AllowComments>
inline bool
basic_jsonc<_IsOrdered, _AllowComments>::basic_object::set_key_before_comments(std::string_view index, const std::vector<std::string>& comments)
    requires(_AllowComments)
{
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

template <bool _IsOrdered, bool _AllowComments>
inline bool
basic_jsonc<_IsOrdered, _AllowComments>::basic_object::set_key_after_comments(std::string_view index, const std::vector<std::string>& comments)
    requires(_AllowComments)
{
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

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_object::add_key_before_comment(std::string_view index, std::string_view comment)
    requires(_AllowComments)
{
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

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_object::add_key_after_comment(std::string_view index, std::string_view comment)
    requires(_AllowComments)
{
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

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::basic_object::clear_key_before_comments(std::string_view index)
    requires(_AllowComments)
{
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { res->second.before_comments_.clear(); }
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::basic_object::clear_key_after_comments(std::string_view index)
    requires(_AllowComments)
{
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { res->second.after_comments_.clear(); }
}

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_object::remove_key_before_comment(std::string_view index, std::size_t comment_index)
    requires(_AllowComments)
{
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

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_object::remove_key_after_comment(std::string_view index, std::size_t comment_index)
    requires(_AllowComments)
{
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

template <bool _IsOrdered, bool _AllowComments>
inline std::size_t basic_jsonc<_IsOrdered, _AllowComments>::basic_object::key_before_comments_size(std::string_view index) const noexcept
    requires(_AllowComments)
{
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.before_comments_.size(); }
    return 0;
}

template <bool _IsOrdered, bool _AllowComments>
inline std::size_t basic_jsonc<_IsOrdered, _AllowComments>::basic_object::key_after_comments_size(std::string_view index) const noexcept
    requires(_AllowComments)
{
    auto res = key_comments_.find(index);
    if (res != key_comments_.end()) { return res->second.after_comments_.size(); }
    return 0;
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_object::iterator basic_jsonc<_IsOrdered, _AllowComments>::basic_object::begin() noexcept {
    return storage_.begin();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_object::iterator basic_jsonc<_IsOrdered, _AllowComments>::basic_object::end() noexcept {
    return storage_.end();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_object::const_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_object::begin() const noexcept {
    return storage_.begin();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_object::const_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_object::end() const noexcept {
    return storage_.end();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_object::const_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_object::cbegin() const noexcept {
    return storage_.cbegin();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_object::const_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_object::cend() const noexcept {
    return storage_.cend();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_object::reverse_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_object::rbegin() noexcept {
    return storage_.rbegin();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_object::reverse_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_object::rend() noexcept {
    return storage_.rend();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_object::const_reverse_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_object::rbegin() const noexcept {
    return storage_.rbegin();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_object::const_reverse_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_object::rend() const noexcept {
    return storage_.rend();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_object::const_reverse_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_object::crbegin() const noexcept {
    return storage_.crbegin();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_object::const_reverse_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_object::crend() const noexcept {
    return storage_.crend();
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::basic_object::merge_patch(const basic_object& other, bool merge_list) {
    for (const auto& [key, val] : other) { operator[](key).merge_patch(val, merge_list); }
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(
    void
) basic_jsonc<_IsOrdered, _AllowComments>::basic_object::merge_patch(const basic_jsonc<_IsOrdered, _AllowComments>& other, bool merge_list) {
    if (auto* rhs = std::get_if<basic_object>(&other.storage_)) {
        merge_patch(*rhs, merge_list);
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", other.type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::basic_object::merge_comments(const basic_object& other)
    requires(_AllowComments)
{
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

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::basic_object::move_comments_to_before()
    requires(_AllowComments)
{
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

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_object::operator==(const basic_object& other) const {
    return storage_ == other.storage_;
}

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_object::operator==(const basic_jsonc<_IsOrdered, _AllowComments>& other) const {
    if (auto* rhs = std::get_if<basic_object>(&other.storage_)) { return storage_ == rhs->storage_; }
    return false;
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_array::basic_array(std::initializer_list<basic_jsonc<_IsOrdered, _AllowComments>> val)
: storage_(val) {}


template <bool _IsOrdered, bool _AllowComments>
inline constexpr basic_jsonc<_IsOrdered, _AllowComments>&
basic_jsonc<_IsOrdered, _AllowComments>::basic_array::operator[](std::size_t index) noexcept {
    return storage_[index];
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr JSONC_RESULT(const basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::basic_array::operator[](
    std::size_t index
) const noexcept {
    return _JSONC_MAKE_RESULT(storage_[index]);
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr JSONC_RESULT(basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::basic_array::at(std::size_t index) {
    if (index < storage_.size()) { return _JSONC_MAKE_RESULT(storage_[index]); }
    _JSONC_OUT_OF_RANGE("Index out of range");
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr JSONC_RESULT(const basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::basic_array::at(
    std::size_t index
) const {
    if (index < storage_.size()) { return _JSONC_MAKE_RESULT(storage_[index]); }
    _JSONC_OUT_OF_RANGE("Index out of range");
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr std::size_t basic_jsonc<_IsOrdered, _AllowComments>::basic_array::size() const noexcept {
    return storage_.size();
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::basic_array::empty() const noexcept {
    return storage_.empty();
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::basic_array::clear() noexcept {
    storage_.clear();
}

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_array::erase(std::size_t where) {
    if (where < storage_.size()) {
        storage_.erase(storage_.begin() + static_cast<decltype(storage_)::difference_type>(where));
        return true;
    }
    return false;
}

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_array::erase(std::size_t first, std::size_t last) {
    if (first < last && last < storage_.size()) {
        storage_.erase(
            storage_.begin() + static_cast<decltype(storage_)::difference_type>(first),
            storage_.begin() + static_cast<decltype(storage_)::difference_type>(last)
        );
        return true;
    }
    return false;
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_array::iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_array::erase(const_iterator where) {
    return storage_.erase(where);
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_array::iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_array::erase(const_iterator first, const_iterator last) {
    return storage_.erase(first, last);
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::basic_array::push_back(const basic_jsonc<_IsOrdered, _AllowComments>& val) {
    storage_.push_back(val);
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::basic_array::push_back(basic_jsonc<_IsOrdered, _AllowComments>&& val) {
    storage_.push_back(std::move(val));
}

template <bool _IsOrdered, bool _AllowComments>
inline const basic_jsonc<_IsOrdered, _AllowComments>& basic_jsonc<_IsOrdered, _AllowComments>::basic_array::front() const noexcept {
    return storage_.front();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>& basic_jsonc<_IsOrdered, _AllowComments>::basic_array::front() noexcept {
    return storage_.front();
}

template <bool _IsOrdered, bool _AllowComments>
inline const basic_jsonc<_IsOrdered, _AllowComments>& basic_jsonc<_IsOrdered, _AllowComments>::basic_array::back() const noexcept {
    return storage_.back();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>& basic_jsonc<_IsOrdered, _AllowComments>::basic_array::back() noexcept {
    return storage_.back();
}

template <bool _IsOrdered, bool _AllowComments>
inline std::string basic_jsonc<_IsOrdered, _AllowComments>::basic_array::dump(
    int  indent,
    bool ensure_ascii,
    bool ignore_comments,
    bool multi_line_comments_format
) const {
    return dump_typed<_IsOrdered, _AllowComments>(*this, ensure_ascii, indent, ignore_comments, multi_line_comments_format);
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_array::iterator basic_jsonc<_IsOrdered, _AllowComments>::basic_array::begin() noexcept {
    return storage_.begin();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_array::iterator basic_jsonc<_IsOrdered, _AllowComments>::basic_array::end() noexcept {
    return storage_.end();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_array::const_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_array::begin() const noexcept {
    return storage_.begin();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_array::const_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_array::end() const noexcept {
    return storage_.end();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_array::const_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_array::cbegin() const noexcept {
    return storage_.cbegin();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_array::const_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_array::cend() const noexcept {
    return storage_.cend();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_array::reverse_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_array::rbegin() noexcept {
    return storage_.rbegin();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_array::reverse_iterator basic_jsonc<_IsOrdered, _AllowComments>::basic_array::rend() noexcept {
    return storage_.rend();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_array::const_reverse_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_array::rbegin() const noexcept {
    return storage_.rbegin();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_array::const_reverse_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_array::rend() const noexcept {
    return storage_.rend();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_array::const_reverse_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_array::crbegin() const noexcept {
    return storage_.crbegin();
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::basic_array::const_reverse_iterator
basic_jsonc<_IsOrdered, _AllowComments>::basic_array::crend() const noexcept {
    return storage_.crend();
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::basic_array::merge_patch(const basic_array& other) {
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

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(void) basic_jsonc<_IsOrdered, _AllowComments>::basic_array::merge_patch(const basic_jsonc<_IsOrdered, _AllowComments>& other) {
    if (auto* rhs = std::get_if<basic_array>(&other.storage_)) {
        merge_patch(*rhs);
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", other.type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::basic_array::merge_comments(const basic_array& other)
    requires(_AllowComments)
{
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

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::basic_array::move_comments_to_before()
    requires(_AllowComments)
{
    for (auto& val : *this) {
        val.before_comments_.append_range(val.after_comments_);
        val.after_comments_.clear();
    }
}

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_array::operator==(const basic_array& other) const {
    return storage_ == other.storage_;
}

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::basic_array::operator==(const basic_jsonc<_IsOrdered, _AllowComments>& other) const {
    if (auto* rhs = std::get_if<basic_array>(&other.storage_)) { return storage_ == rhs->storage_; }
    return false;
}

template <bool _IsOrdered, bool _AllowComments>
template <bool IsOrdered, bool AllowComments>
inline constexpr basic_jsonc<_IsOrdered, _AllowComments>::basic_jsonc(const basic_jsonc<IsOrdered, AllowComments>& other) noexcept {
    if constexpr (_IsOrdered == IsOrdered && _AllowComments == AllowComments) {
        storage_ = other.storage_;
        if constexpr (_AllowComments && AllowComments) {
            before_comments_ = other.before_comments_;
            after_comments_  = other.after_comments_;
        }
    } else {
        std::visit(
            [&](const auto val) {
                using VT = std::remove_cvref_t<decltype(val)>;
                if constexpr (std::same_as<VT, typename basic_jsonc<IsOrdered, AllowComments>::object_type>) {
                    auto value = object_type();
                    for (const auto& [k, v] : val.storage_) { value[k] = basic_jsonc(v); }
                    if constexpr (_AllowComments && AllowComments) { value.key_comments_ = other.key_comments_; }
                    storage_ = std::move(value);
                } else if constexpr (std::same_as<VT, typename basic_jsonc<IsOrdered, AllowComments>::array_type>) {
                    auto value = array_type();
                    for (const auto& ele : val.storage_) { value.push_back(basic_jsonc(ele)); }
                    storage_ = std::move(value);
                } else {
                    storage_ = val;
                }
            },
            other.storage_
        );
        if constexpr (_AllowComments && AllowComments) {
            before_comments_ = other.before_comments_;
            after_comments_  = other.after_comments_;
        }
    }
}

template <bool _IsOrdered, bool _AllowComments>
template <bool IsOrdered, bool AllowComments>
inline constexpr basic_jsonc<_IsOrdered, _AllowComments>::basic_jsonc(basic_jsonc<IsOrdered, AllowComments>&& other) noexcept {
    if constexpr (_IsOrdered == IsOrdered && _AllowComments == AllowComments) {
        storage_ = std::move(other.storage_);
        if constexpr (_AllowComments && AllowComments) {
            before_comments_ = std::move(other.before_comments_);
            after_comments_  = std::move(other.after_comments_);
        }
    } else {
        std::visit(
            [&](const auto val) {
                using VT = std::remove_cvref_t<decltype(val)>;
                if constexpr (std::same_as<VT, typename basic_jsonc<IsOrdered, AllowComments>::object_type>) {
                    auto value = object_type();
                    for (auto&& [k, v] : val.storage_) { value[k] = basic_jsonc(std::move(v)); }
                    if constexpr (_AllowComments && AllowComments) { value.key_comments_ = std::move(other.key_comments_); }
                    storage_ = std::move(value);
                } else if constexpr (std::same_as<VT, typename basic_jsonc<IsOrdered, AllowComments>::array_type>) {
                    auto value = array_type();
                    for (auto&& ele : val.storage_) { value.push_back(basic_jsonc(std::move(ele))); }
                    storage_ = std::move(value);
                } else {
                    storage_ = std::move(val);
                }
            },
            other.storage_
        );
        if constexpr (_AllowComments && AllowComments) {
            before_comments_ = std::move(other.before_comments_);
            after_comments_  = std::move(other.after_comments_);
        }
    }
}

template <bool _IsOrdered, bool _AllowComments>
template <bool IsOrdered, bool AllowComments>
inline constexpr basic_jsonc<_IsOrdered, _AllowComments>&
basic_jsonc<_IsOrdered, _AllowComments>::operator=(const basic_jsonc<IsOrdered, AllowComments>& other) noexcept {
    if constexpr (_IsOrdered == IsOrdered && _AllowComments == AllowComments) {
        storage_ = other.storage_;
        if constexpr (_AllowComments && AllowComments) {
            before_comments_ = other.before_comments_;
            after_comments_  = other.after_comments_;
        }
    } else {
        std::visit(
            [&](const auto val) {
                using VT = std::remove_cvref_t<decltype(val)>;
                if constexpr (std::same_as<VT, typename basic_jsonc<IsOrdered, AllowComments>::object_type>) {
                    auto value = object_type();
                    for (const auto& [k, v] : val.storage_) { value[k] = basic_jsonc(v); }
                    if constexpr (_AllowComments && AllowComments) { value.key_comments_ = other.key_comments_; }
                    storage_ = std::move(value);
                } else if constexpr (std::same_as<VT, typename basic_jsonc<IsOrdered, AllowComments>::array_type>) {
                    auto value = array_type();
                    for (const auto& ele : val.storage_) { value.push_back(basic_jsonc(ele)); }
                    storage_ = std::move(value);
                } else {
                    storage_ = val;
                }
            },
            other.storage_
        );
        if constexpr (_AllowComments && AllowComments) {
            before_comments_ = other.before_comments_;
            after_comments_  = other.after_comments_;
        }
    }
    return *this;
}

template <bool _IsOrdered, bool _AllowComments>
template <bool IsOrdered, bool AllowComments>
inline constexpr basic_jsonc<_IsOrdered, _AllowComments>&
basic_jsonc<_IsOrdered, _AllowComments>::operator=(basic_jsonc<IsOrdered, AllowComments>&& other) noexcept {
    if constexpr (_IsOrdered == IsOrdered && _AllowComments == AllowComments) {
        storage_ = std::move(other.storage_);
        if constexpr (_AllowComments && AllowComments) {
            before_comments_ = std::move(other.before_comments_);
            after_comments_  = std::move(other.after_comments_);
        }
    } else {
        std::visit(
            [&](const auto val) {
                using VT = std::remove_cvref_t<decltype(val)>;
                if constexpr (std::same_as<VT, typename basic_jsonc<IsOrdered, AllowComments>::object_type>) {
                    auto value = object_type();
                    for (auto&& [k, v] : val.storage_) { value[k] = basic_jsonc(std::move(v)); }
                    if constexpr (_AllowComments && AllowComments) { value.key_comments_ = std::move(other.key_comments_); }
                    storage_ = std::move(value);
                } else if constexpr (std::same_as<VT, typename basic_jsonc<IsOrdered, AllowComments>::array_type>) {
                    auto value = array_type();
                    for (auto&& ele : val.storage_) { value.push_back(basic_jsonc(std::move(ele))); }
                    storage_ = std::move(value);
                } else {
                    storage_ = std::move(val);
                }
            },
            other.storage_
        );
        if constexpr (_AllowComments && AllowComments) {
            before_comments_ = std::move(other.before_comments_);
            after_comments_  = std::move(other.after_comments_);
        }
    }
    return *this;
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr value_type basic_jsonc<_IsOrdered, _AllowComments>::type() const noexcept {
    return static_cast<value_type>(storage_.index());
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr std::string_view basic_jsonc<_IsOrdered, _AllowComments>::type_name() const noexcept {
    switch (type()) {
    case value_type::null:
        return "null";
    case value_type::boolean:
        return "boolean";
    case value_type::number_integer_signed:
        return "signed inteager";
    case value_type::number_integer_unsigned:
        return "unsigned integer";
    case value_type::string:
        return "string";
    case value_type::number_floating_point:
        return "floating point";
    case value_type::object:
        return "object";
    case value_type::array:
        return "array";
    case value_type::number_big_integer:
        return "big inteager";
    case value_type::number_high_precision_float:
        return "high-precision float";
    }
    std::unreachable();
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::hold(value_type value_type) const noexcept {
    return type() == value_type;
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_null() const noexcept {
    return hold(value_type::null);
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_boolean() const noexcept {
    return hold(value_type::boolean);
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_number_signed() const noexcept {
    return hold(value_type::number_integer_signed);
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_number_unsigned() const noexcept {
    return hold(value_type::number_integer_unsigned);
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_number_normal_integer() const noexcept {
    return is_number_signed() || is_number_unsigned();
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_number_big_inteager() const noexcept {
    return hold(value_type::number_big_integer);
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_number_integer() const noexcept {
    return is_number_normal_integer() || is_number_big_inteager();
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_number_normal_float() const noexcept {
    return hold(value_type::number_floating_point);
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_number_high_precision_float() const noexcept {
    return hold(value_type::number_high_precision_float);
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_number_float() const noexcept {
    return is_number_normal_float() || is_number_high_precision_float();
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_normal_number() const noexcept {
    return is_number_normal_float() || is_number_normal_integer();
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_high_precision_number() const noexcept {
    return is_number_high_precision_float() || is_number_big_inteager();
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_number() const noexcept {
    return is_normal_number() || is_high_precision_number();
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_string() const noexcept {
    return hold(value_type::string);
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_object() const noexcept {
    return hold(value_type::object);
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_array() const noexcept {
    return hold(value_type::array);
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_primitive() const noexcept {
    return is_null() || is_string() || is_number();
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::is_structured() const noexcept {
    return is_array() || is_object();
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr std::size_t basic_jsonc<_IsOrdered, _AllowComments>::size() const noexcept {
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

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(bool) basic_jsonc<_IsOrdered, _AllowComments>::contains(std::string_view index) const {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->contains(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(bool) basic_jsonc<_IsOrdered, _AllowComments>::contains(std::string_view index, value_type type) const {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->contains(index, type); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(bool) basic_jsonc<_IsOrdered, _AllowComments>::empty() const {
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

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(bool) basic_jsonc<_IsOrdered, _AllowComments>::erase(std::string_view index) {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->erase(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(bool) basic_jsonc<_IsOrdered, _AllowComments>::erase(std::size_t where) {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->erase(where); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(bool) basic_jsonc<_IsOrdered, _AllowComments>::erase(std::size_t first, std::size_t last) {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->erase(first, last); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

template <typename T>
    requires(std::is_arithmetic_v<T> && !std::same_as<T, bool>)
constexpr T truncate_high_precision_number(std::string_view num_str) {
    T value{};
    auto [ptr, ec] = std::from_chars(num_str.data(), num_str.data() + num_str.size(), value);
    if (ec == std::errc{}) { return value; }
    return num_str.starts_with('-') ? std::numeric_limits<T>::lowest() : std::numeric_limits<T>::max();
}

template <bool _IsOrdered, bool _AllowComments>
template <typename T>
    requires std::is_arithmetic_v<T>
inline constexpr basic_jsonc<_IsOrdered, _AllowComments>::operator T() const {
    return std::visit(
        [](const auto& val) -> T {
            using Type = std::decay_t<decltype(val)>;
            if constexpr (std::is_convertible_v<Type, T>) {
                return static_cast<T>(val);
            } else if constexpr (std::same_as<Type, basic_big_integer>) {
                if constexpr (std::same_as<T, bool>) {
                    return val.view_ != "0";
                } else {
                    return truncate_high_precision_number<T>(val.view_);
                }
            } else if constexpr (std::same_as<Type, basic_high_precision_float>) {
                if constexpr (std::same_as<T, bool>) {
                    return val.view_ != "0";
                } else {
                    return static_cast<T>(truncate_high_precision_number<double>(val.view_));
                }
            } else {
#ifdef JSONC_NO_EXCEPTION
                std::abort();
#else
                _JSONC_TYPE_ERROR("bad type cast");
#endif
            }
        },
        storage_
    );
}

template <bool _IsOrdered, bool _AllowComments>
template <typename T>
    requires std::is_convertible_v<T, std::string>
inline constexpr basic_jsonc<_IsOrdered, _AllowComments>::operator T() const {
    if (auto* storage = std::get_if<std::string>(&storage_)) { return *storage; }
#ifdef JSONC_NO_EXCEPTION
    std::abort();
#else
    _JSONC_TYPE_ERROR("bad type cast");
#endif
}

template <bool _IsOrdered, bool _AllowComments>
template <is_array_like T>
inline constexpr basic_jsonc<_IsOrdered, _AllowComments>::operator T() const {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return T(storage->begin(), storage->end()); }
#ifdef JSONC_NO_EXCEPTION
    std::abort();
#else
    _JSONC_TYPE_ERROR("bad type cast");
#endif
}

template <bool _IsOrdered, bool _AllowComments>
template <is_object_like T>
inline constexpr basic_jsonc<_IsOrdered, _AllowComments>::operator T() const {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return T(storage->begin(), storage->end()); }
#ifdef JSONC_NO_EXCEPTION
    std::abort();
#else
    _JSONC_TYPE_ERROR("bad type cast");
#endif
}

template <bool _IsOrdered, bool _AllowComments>
template <bool IsOrdered, bool AllowComments>
[[nodiscard]] inline constexpr basic_jsonc<_IsOrdered, _AllowComments>::operator basic_jsonc<IsOrdered, AllowComments>() noexcept {
    return basic_jsonc<IsOrdered, AllowComments>(*this);
}

template <bool _IsOrdered, bool _AllowComments>
inline std::string basic_jsonc<_IsOrdered, _AllowComments>::dump(
    int  indent,
    bool ensure_ascii,
    bool ignore_comments,
    bool multi_line_comments_format,
    bool global_comments
) const {
    if constexpr (_AllowComments) {
        auto result = std::visit(
            [&](const auto& val) {
                return dump_typed<_IsOrdered, _AllowComments>(val, ensure_ascii, indent, ignore_comments, multi_line_comments_format);
            },
            storage_
        );
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
    } else {
        return std::visit(
            [&](const auto& val) {
                return dump_typed<_IsOrdered, _AllowComments>(val, ensure_ascii, indent, ignore_comments, multi_line_comments_format);
            },
            storage_
        );
    }
}

template <typename T>
constexpr bool always_false_v = false;

template <typename T, bool _IsOrdered, bool _AllowComments>
constexpr bool is_jsonc_type_convertible_v = [] {
    return []<std::size_t... I>(std::index_sequence<I...>) {
        return (std::same_as<std::variant_alternative_t<I, typename basic_jsonc<_IsOrdered, _AllowComments>::type_variant>, T> || ...);
    }(std::make_index_sequence<std::variant_size_v<typename basic_jsonc<_IsOrdered, _AllowComments>::type_variant>>{});
}();

template <bool _IsOrdered, bool _AllowComments>
template <typename T>
inline JSONC_RESULT(T&) basic_jsonc<_IsOrdered, _AllowComments>::as() {
    if constexpr (is_jsonc_type_convertible_v<T, _IsOrdered, _AllowComments>) {
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

template <bool _IsOrdered, bool _AllowComments>
template <typename T>
inline JSONC_RESULT(const T&) basic_jsonc<_IsOrdered, _AllowComments>::as() const {
    if constexpr (is_jsonc_type_convertible_v<T, _IsOrdered, _AllowComments>) {
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

template <bool _IsOrdered, bool _AllowComments>
template <typename T>
    requires std::is_arithmetic_v<T>
inline JSONC_RESULT(T) basic_jsonc<_IsOrdered, _AllowComments>::get() const {
    return std::visit(
        [](const auto& val) -> JSONC_RESULT(T) {
            using Type = std::decay_t<decltype(val)>;
            if constexpr (std::is_convertible_v<Type, T>) {
                return static_cast<T>(val);
            } else if constexpr (std::same_as<Type, basic_big_integer>) {
                if constexpr (std::same_as<T, bool>) {
                    return val.view_ != "0";
                } else {
                    return truncate_high_precision_number<T>(val.view_);
                }
            } else if constexpr (std::same_as<Type, basic_high_precision_float>) {
                if constexpr (std::same_as<T, bool>) {
                    return val.view_ != "0";
                } else {
                    return static_cast<T>(truncate_high_precision_number<double>(val.view_));
                }
            } else {
                _JSONC_TYPE_ERROR("bad type cast");
            }
        },
        storage_
    );
}

template <bool _IsOrdered, bool _AllowComments>
template <typename T>
    requires std::is_convertible_v<T, std::string>
inline JSONC_RESULT(T) basic_jsonc<_IsOrdered, _AllowComments>::get() const {
    if (auto* storage = std::get_if<std::string>(&storage_)) { return *storage; }
    _JSONC_TYPE_ERROR("bad type cast");
}

template <bool _IsOrdered, bool _AllowComments>
template <is_array_like T>
inline JSONC_RESULT(T) basic_jsonc<_IsOrdered, _AllowComments>::get() const {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return T(storage->begin(), storage->end()); }
    _JSONC_TYPE_ERROR("bad type cast");
}

template <bool _IsOrdered, bool _AllowComments>
template <is_object_like T>
inline JSONC_RESULT(T) basic_jsonc<_IsOrdered, _AllowComments>::get() const {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return T(storage->begin(), storage->end()); }
    _JSONC_TYPE_ERROR("bad type cast");
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(std::string) basic_jsonc<_IsOrdered, _AllowComments>::get_big_int_view() const {
    if (auto* storage = std::get_if<basic_big_integer>(&storage_)) { return storage->view_; }
    _JSONC_TYPE_ERROR(std::format("Type must be a big integer, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(std::string) basic_jsonc<_IsOrdered, _AllowComments>::get_any_int_view() const {
    return std::visit(
        [&](const auto& val) -> JSONC_RESULT(std::string) {
            using Type = std::decay_t<decltype(val)>;
            if constexpr (std::same_as<basic_big_integer, Type>) {
                return val.view_;
            } else if constexpr (std::same_as<std::int64_t, Type> || std::same_as<std::uint64_t, Type>) {
                return std::to_string(val);
            } else {
                _JSONC_TYPE_ERROR(std::format("Type must be any integer type, but is {}", type_name()));
            }
        },
        storage_
    );
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(std::string) basic_jsonc<_IsOrdered, _AllowComments>::get_high_precision_float_view() const {
    if (auto* storage = std::get_if<basic_high_precision_float>(&storage_)) { return storage->view_; }
    _JSONC_TYPE_ERROR(std::format("Type must be a high-precision float, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(std::string) basic_jsonc<_IsOrdered, _AllowComments>::get_any_float_view() const {
    return std::visit(
        [&](const auto& val) -> JSONC_RESULT(std::string) {
            using Type = std::decay_t<decltype(val)>;
            if constexpr (std::same_as<basic_high_precision_float, Type>) {
                return val.view_;
            } else if constexpr (std::same_as<double, Type>) {
                return std::format("{}", val);
            } else {
                _JSONC_TYPE_ERROR(std::format("Type must be any floating-point type, but is {}", type_name()));
            }
        },
        storage_
    );
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(std::string) basic_jsonc<_IsOrdered, _AllowComments>::get_any_number_view() const {
    return std::visit(
        [&](const auto& val) -> JSONC_RESULT(std::string) {
            using Type = std::decay_t<decltype(val)>;
            if constexpr (std::same_as<basic_high_precision_float, Type> || std::same_as<basic_big_integer, Type>) {
                return val.view_;
            } else if constexpr (std::same_as<double, Type> || std::same_as<std::int64_t, Type> || std::same_as<std::uint64_t, Type>) {
                return std::format("{}", val);
            } else {
                _JSONC_TYPE_ERROR(std::format("Type must be any number type, but is {}", type_name()));
            }
        },
        storage_
    );
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::operator[](std::string_view index) {
    if (hold(value_type::null)) { storage_.template emplace<6>(); }
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return (*storage)[index]; }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(const basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::operator[](
    std::string_view index
) const {
    if (auto* storage = std::get_if<basic_object>(&storage_)) {
        auto res = storage->storage_.find(index);
        if (res != storage->storage_.end()) { return _JSONC_MAKE_RESULT(res->second); }
        _JSONC_OUT_OF_RANGE(std::format("Invalid key: {}", index));
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::operator[](std::size_t index) {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return (*storage)[index]; }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(const basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::operator[](std::size_t index) const {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return (*storage)[index]; }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::at(std::string_view index) {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(
    basic_jsonc<_IsOrdered, _AllowComments>&
) basic_jsonc<_IsOrdered, _AllowComments>::at(std::string_view index, const basic_jsonc<_IsOrdered, _AllowComments>& default_value) {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->at(index, default_value); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(const basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::at(std::string_view index) const {
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::at(std::size_t index) {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(const basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::at(std::size_t index) const {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->at(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(void) basic_jsonc<_IsOrdered, _AllowComments>::push_back(const basic_jsonc<_IsOrdered, _AllowComments>& val) {
    if (hold(value_type::null)) { storage_.template emplace<7>(); }
    if (auto* storage = std::get_if<basic_array>(&storage_)) {
        storage->push_back(std::move(val));
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(void) basic_jsonc<_IsOrdered, _AllowComments>::push_back(basic_jsonc<_IsOrdered, _AllowComments>&& val) {
    if (hold(value_type::null)) { storage_.template emplace<7>(); }
    if (auto* storage = std::get_if<basic_array>(&storage_)) {
        storage->push_back(std::move(val));
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(const basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::front() const {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->front(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::front() {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->front(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(const basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::back() const {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->back(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(basic_jsonc<_IsOrdered, _AllowComments>&) basic_jsonc<_IsOrdered, _AllowComments>::back() {
    if (auto* storage = std::get_if<basic_array>(&storage_)) { return storage->back(); }
    _JSONC_TYPE_ERROR(std::format("Type must be an array, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(void) basic_jsonc<_IsOrdered, _AllowComments>::clear() {
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

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::iterator basic_jsonc<_IsOrdered, _AllowComments>::begin() noexcept {
    return iterator::template make_begin<false>(*this);
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::iterator basic_jsonc<_IsOrdered, _AllowComments>::end() noexcept {
    return iterator::template make_end<false>(*this);
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::const_iterator basic_jsonc<_IsOrdered, _AllowComments>::begin() const noexcept {
    return const_iterator::template make_begin<false>(*this);
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::const_iterator basic_jsonc<_IsOrdered, _AllowComments>::end() const noexcept {
    return const_iterator::template make_end<false>(*this);
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::const_iterator basic_jsonc<_IsOrdered, _AllowComments>::cbegin() const noexcept {
    return const_iterator::template make_begin<false>(*this);
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::const_iterator basic_jsonc<_IsOrdered, _AllowComments>::cend() const noexcept {
    return const_iterator::template make_end<false>(*this);
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::reverse_iterator basic_jsonc<_IsOrdered, _AllowComments>::rbegin() noexcept {
    return reverse_iterator::template make_begin<true>(*this);
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::reverse_iterator basic_jsonc<_IsOrdered, _AllowComments>::rend() noexcept {
    return reverse_iterator::template make_end<true>(*this);
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::const_reverse_iterator basic_jsonc<_IsOrdered, _AllowComments>::rbegin() const noexcept {
    return const_reverse_iterator::template make_begin<true>(*this);
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::const_reverse_iterator basic_jsonc<_IsOrdered, _AllowComments>::rend() const noexcept {
    return const_reverse_iterator::template make_end<true>(*this);
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::const_reverse_iterator basic_jsonc<_IsOrdered, _AllowComments>::crbegin() const noexcept {
    return const_reverse_iterator::template make_begin<true>(*this);
}

template <bool _IsOrdered, bool _AllowComments>
inline basic_jsonc<_IsOrdered, _AllowComments>::const_reverse_iterator basic_jsonc<_IsOrdered, _AllowComments>::crend() const noexcept {
    return const_reverse_iterator::template make_end<true>(*this);
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::merge_patch(const basic_jsonc<_IsOrdered, _AllowComments>& other, bool merge_list) {
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

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::merge_comments(const basic_jsonc<_IsOrdered, _AllowComments>& other)
    requires(_AllowComments)
{
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

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::move_comments_to_before()
    requires(_AllowComments)
{
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

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::operator==(const basic_jsonc<_IsOrdered, _AllowComments>& other) const {
    return storage_ == other.storage_;
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::has_before_comments() const noexcept
    requires(_AllowComments)
{
    return before_comments_.size() != 0;
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr bool basic_jsonc<_IsOrdered, _AllowComments>::has_after_comments() const noexcept
    requires(_AllowComments)
{
    return after_comments_.size() != 0;
}

template <bool _IsOrdered, bool _AllowComments>
inline std::vector<std::string>& basic_jsonc<_IsOrdered, _AllowComments>::before_comments() noexcept
    requires(_AllowComments)
{
    return before_comments_;
}

template <bool _IsOrdered, bool _AllowComments>
inline const std::vector<std::string>& basic_jsonc<_IsOrdered, _AllowComments>::before_comments() const noexcept
    requires(_AllowComments)
{
    return before_comments_;
}

template <bool _IsOrdered, bool _AllowComments>
inline std::vector<std::string>& basic_jsonc<_IsOrdered, _AllowComments>::after_comments() noexcept
    requires(_AllowComments)
{
    return after_comments_;
}

template <bool _IsOrdered, bool _AllowComments>
inline const std::vector<std::string>& basic_jsonc<_IsOrdered, _AllowComments>::after_comments() const noexcept
    requires(_AllowComments)
{
    return after_comments_;
}

template <bool _IsOrdered, bool _AllowComments>
inline std::vector<std::string> basic_jsonc<_IsOrdered, _AllowComments>::get_before_comments() const
    requires(_AllowComments)
{
    return before_comments_;
}

template <bool _IsOrdered, bool _AllowComments>
inline std::vector<std::string> basic_jsonc<_IsOrdered, _AllowComments>::get_after_comments() const
    requires(_AllowComments)
{
    return after_comments_;
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::set_before_comments(const std::vector<std::string>& comments)
    requires(_AllowComments)
{
    before_comments_ = comments;
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::set_after_comments(const std::vector<std::string>& comments)
    requires(_AllowComments)
{
    after_comments_ = comments;
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::add_before_comment(std::string_view comment)
    requires(_AllowComments)
{
    before_comments_.append_range(split_comments(comment));
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::add_after_comment(std::string_view comment)
    requires(_AllowComments)
{
    after_comments_.append_range(split_comments(comment));
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::clear_before_comments()
    requires(_AllowComments)
{
    before_comments_.clear();
}

template <bool _IsOrdered, bool _AllowComments>
inline void basic_jsonc<_IsOrdered, _AllowComments>::clear_after_comments()
    requires(_AllowComments)
{
    after_comments_.clear();
}

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::remove_before_comment(std::size_t comment_index)
    requires(_AllowComments)
{
    if (comment_index < before_comments_.size()) {
        before_comments_.erase(before_comments_.begin() + static_cast<decltype(before_comments_)::difference_type>(comment_index));
        return true;
    }
    return false;
}

template <bool _IsOrdered, bool _AllowComments>
inline bool basic_jsonc<_IsOrdered, _AllowComments>::remove_after_comment(std::size_t comment_index)
    requires(_AllowComments)
{
    if (comment_index < after_comments_.size()) {
        after_comments_.erase(after_comments_.begin() + static_cast<decltype(after_comments_)::difference_type>(comment_index));
        return true;
    }
    return false;
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr std::size_t basic_jsonc<_IsOrdered, _AllowComments>::before_comments_size() const noexcept
    requires(_AllowComments)
{
    return before_comments_.size();
}

template <bool _IsOrdered, bool _AllowComments>
inline constexpr std::size_t basic_jsonc<_IsOrdered, _AllowComments>::after_comments_size() const noexcept
    requires(_AllowComments)
{
    return after_comments_.size();
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(bool) basic_jsonc<_IsOrdered, _AllowComments>::has_key_before_comments(std::string_view index) const
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->has_key_before_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(bool) basic_jsonc<_IsOrdered, _AllowComments>::has_key_after_comments(std::string_view index) const
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->has_key_after_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(std::vector<std::string>&) basic_jsonc<_IsOrdered, _AllowComments>::key_before_comments(std::string_view index)
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->key_before_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(const std::vector<std::string>&) basic_jsonc<_IsOrdered, _AllowComments>::key_before_comments(std::string_view index) const
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->key_before_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(std::vector<std::string>&) basic_jsonc<_IsOrdered, _AllowComments>::key_after_comments(std::string_view index)
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->key_after_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(const std::vector<std::string>&) basic_jsonc<_IsOrdered, _AllowComments>::key_after_comments(std::string_view index) const
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->key_after_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(std::vector<std::string>) basic_jsonc<_IsOrdered, _AllowComments>::get_key_before_comments(std::string_view index) const
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->get_key_before_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(std::vector<std::string>) basic_jsonc<_IsOrdered, _AllowComments>::get_key_after_comments(std::string_view index) const
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->get_key_after_comments(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(
    std::string
) basic_jsonc<_IsOrdered, _AllowComments>::get_key_before_comment(std::string_view index, std::size_t comment_index) const
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->get_key_before_comment(index, comment_index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(
    std::string
) basic_jsonc<_IsOrdered, _AllowComments>::get_key_after_comment(std::string_view index, std::size_t comment_index) const
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->get_key_after_comment(index, comment_index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(
    bool
) basic_jsonc<_IsOrdered, _AllowComments>::set_key_before_comments(std::string_view index, const std::vector<std::string>& comments)
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->set_key_before_comments(index, comments); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(
    bool
) basic_jsonc<_IsOrdered, _AllowComments>::set_key_after_comments(std::string_view index, const std::vector<std::string>& comments)
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->set_key_after_comments(index, comments); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(bool) basic_jsonc<_IsOrdered, _AllowComments>::add_key_before_comment(std::string_view index, std::string_view comment)
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->add_key_before_comment(index, comment); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(bool) basic_jsonc<_IsOrdered, _AllowComments>::add_key_after_comment(std::string_view index, std::string_view comment)
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->add_key_after_comment(index, comment); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(void) basic_jsonc<_IsOrdered, _AllowComments>::clear_key_before_comments(std::string_view index)
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) {
        storage->clear_key_before_comments(index);
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(void) basic_jsonc<_IsOrdered, _AllowComments>::clear_key_after_comments(std::string_view index)
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) {
        storage->clear_key_after_comments(index);
        return _JSONC_MAKE_VOID_RESULT();
    }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(bool) basic_jsonc<_IsOrdered, _AllowComments>::remove_key_before_comment(std::string_view index, std::size_t comment_index)
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->remove_key_before_comment(index, comment_index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(bool) basic_jsonc<_IsOrdered, _AllowComments>::remove_key_after_comment(std::string_view index, std::size_t comment_index)
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->remove_key_after_comment(index, comment_index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(std::size_t) basic_jsonc<_IsOrdered, _AllowComments>::key_before_comments_size(std::string_view index) const
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->key_before_comments_size(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_RESULT(std::size_t) basic_jsonc<_IsOrdered, _AllowComments>::key_after_comments_size(std::string_view index) const
    requires(_AllowComments)
{
    if (auto* storage = std::get_if<basic_object>(&storage_)) { return storage->key_after_comments_size(index); }
    _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
}

inline bool is_int(std::string_view view) {
    if (view.starts_with('-') || view.starts_with('+')) { view.remove_prefix(1); }
    return std::ranges::all_of(view, [](unsigned char c) { return std::isdigit(c); });
}

template <bool _IsOrdered, bool _AllowComments>
inline std::optional<basic_jsonc<_IsOrdered, _AllowComments>> basic_jsonc<_IsOrdered, _AllowComments>::from_any_int(std::string_view view) noexcept {
    bool is_int{true};
    bool is_negative{false};
    bool is_scientific{false};
    auto num_str = extract_jsonc_number(view, is_int, is_negative, is_scientific);
    if (is_int && view.empty()) {
        if (is_negative) {
            std::int64_t res{};
            auto [ptr, ec] = std::from_chars(num_str.data(), num_str.data() + num_str.size(), res);
            if (ec != std::errc() || ptr != num_str.data() + num_str.size()) { return basic_big_integer(num_str); }
            return res;
        } else {
            std::uint64_t res{};
            auto [ptr, ec] = std::from_chars(num_str.data(), num_str.data() + num_str.size(), res);
            if (ec != std::errc() || ptr != num_str.data() + num_str.size()) { return basic_big_integer(num_str); }
            return res;
        }
    }
    return std::nullopt;
}

template <bool _IsOrdered, bool _AllowComments>
inline std::optional<basic_jsonc<_IsOrdered, _AllowComments>>
basic_jsonc<_IsOrdered, _AllowComments>::from_any_float(std::string_view view, bool float_keep_precision) noexcept {
    bool is_int{true};
    bool is_negative{false};
    bool is_scientific{false};
    auto num_str = extract_jsonc_number(view, is_int, is_negative, is_scientific);
    if (!is_int && view.empty()) {
        double res{};
        auto [ptr, ec] = std::from_chars(num_str.data(), num_str.data() + num_str.size(), res);
        if (ec != std::errc() || ptr != num_str.data() + num_str.size() || std::isinf(res)) { return basic_high_precision_float(num_str); }
        if (float_keep_precision && !is_scientific && std::format("{}", res) != num_str) { return basic_high_precision_float(num_str); }
        return res;
    }
    return std::nullopt;
}

template <bool _IsOrdered, bool _AllowComments>
inline std::optional<basic_jsonc<_IsOrdered, _AllowComments>>
basic_jsonc<_IsOrdered, _AllowComments>::from_any_number(std::string_view view, bool float_keep_precision) noexcept {
    bool is_int{true};
    bool is_negative{false};
    bool is_scientific{false};
    auto num_str = extract_jsonc_number(view, is_int, is_negative, is_scientific);
    if (view.empty()) {
        if (is_int) {
            if (is_negative) {
                std::int64_t res{};
                auto [ptr, ec] = std::from_chars(num_str.data(), num_str.data() + num_str.size(), res);
                if (ec != std::errc() || ptr != num_str.data() + num_str.size()) { return basic_big_integer(num_str); }
                return res;
            } else {
                std::uint64_t res{};
                auto [ptr, ec] = std::from_chars(num_str.data(), num_str.data() + num_str.size(), res);
                if (ec != std::errc() || ptr != num_str.data() + num_str.size()) { return basic_big_integer(num_str); }
                return res;
            }
        } else {
            double res{};
            auto [ptr, ec] = std::from_chars(num_str.data(), num_str.data() + num_str.size(), res);
            if (ec != std::errc() || ptr != num_str.data() + num_str.size() || std::isinf(res)) { return basic_high_precision_float(num_str); }
            if (float_keep_precision && !is_scientific && std::format("{}", res) != num_str) { return basic_high_precision_float(num_str); }
            return res;
        }
    }
    return std::nullopt;
}

template <bool _IsOrdered, bool _AllowComments>
inline JSONC_PARSE_RESULT(basic_jsonc<_IsOrdered, _AllowComments>) basic_jsonc<_IsOrdered, _AllowComments>::parse(
    std::string_view content,
    bool             allow_trailing_comma,
    bool             ignore_comments,
    bool             float_keep_precision
) {
    return detail::parse_basic_jsonc<_IsOrdered, _AllowComments>(content, allow_trailing_comma, ignore_comments, float_keep_precision);
}

} // namespace sculk::jsonc::inline abi_v1_4_1::detail