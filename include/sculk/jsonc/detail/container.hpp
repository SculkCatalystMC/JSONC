// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>

namespace sculk::jsonc::inline abi_v1_4_1::detail {

struct string_hash {
    using is_transparent = void;
    std::size_t operator()(std::string_view sv) const noexcept { return std::hash<std::string_view>{}(sv); }
    std::size_t operator()(const std::string& s) const noexcept { return std::hash<std::string>{}(s); }
    std::size_t operator()(const char* s) const noexcept { return std::hash<std::string_view>{}(s); }
};

struct string_equal {
    using is_transparent = void;
    bool operator()(std::string_view a, std::string_view b) const noexcept { return a == b; }
};

template <typename T>
using string_hash_map = std::unordered_map<std::string, T, string_hash, string_equal>;

template <typename T>
using string_map = std::map<std::string, T, std::less<>>;

template <typename T>
class ordered_string_map {
private:
    using storage_type  = std::list<std::pair<std::string, T>>;
    using iterator_type = storage_type::iterator;
    using entry_type    = std::unordered_map<std::string_view, iterator_type>;

public:
    using iterator               = storage_type::iterator;
    using const_iterator         = storage_type::const_iterator;
    using reverse_iterator       = storage_type::reverse_iterator;
    using const_reverse_iterator = storage_type::const_reverse_iterator;

public:
    [[nodiscard]] ordered_string_map() = default;
    [[nodiscard]] ordered_string_map(const ordered_string_map& other) : storage_(other.storage_) {
        for (auto it = storage_.begin(); it != storage_.end(); ++it) { entry_.emplace(it->first, it); }
    }
    ordered_string_map& operator=(const ordered_string_map& other) {
        storage_ = other.storage_;
        entry_.clear();
        for (auto it = storage_.begin(); it != storage_.end(); ++it) { entry_.emplace(it->first, it); }
        return *this;
    }

    [[nodiscard]] std::size_t size() const noexcept { return storage_.size(); }
    [[nodiscard]] bool        empty() const noexcept { return storage_.empty(); }

    [[nodiscard]] bool contains(std::string_view _Keyval) const noexcept { return entry_.contains(_Keyval); }

    [[nodiscard]] bool erase(std::string_view _Keyval) noexcept {
        auto it = entry_.find(_Keyval);
        if (it != entry_.end()) {
            storage_.erase(it->second);
            entry_.erase(it);
        }
        return false;
    }

    [[nodiscard]] iterator begin() noexcept { return storage_.begin(); }
    [[nodiscard]] iterator end() noexcept { return storage_.end(); }

    [[nodiscard]] const_iterator begin() const noexcept { return storage_.begin(); }
    [[nodiscard]] const_iterator end() const noexcept { return storage_.end(); }

    [[nodiscard]] const_iterator cbegin() const noexcept { return storage_.cbegin(); }
    [[nodiscard]] const_iterator cend() const noexcept { return storage_.cend(); }

    [[nodiscard]] reverse_iterator rbegin() noexcept { return storage_.rbegin(); }
    [[nodiscard]] reverse_iterator rend() noexcept { return storage_.rend(); }

    [[nodiscard]] const_reverse_iterator rbegin() const noexcept { return storage_.rbegin(); }
    [[nodiscard]] const_reverse_iterator rend() const noexcept { return storage_.rend(); }

    [[nodiscard]] const_reverse_iterator crbegin() const noexcept { return storage_.crbegin(); }
    [[nodiscard]] const_reverse_iterator crend() const noexcept { return storage_.crend(); }

    [[nodiscard]] const_iterator find(std::string_view _Keyval) const {
        auto it = entry_.find(_Keyval);
        if (it != entry_.end()) { return it->second; }
        return storage_.end();
    }
    [[nodiscard]] iterator find(std::string_view _Keyval) {
        auto it = entry_.find(_Keyval);
        if (it != entry_.end()) { return it->second; }
        return storage_.end();
    }

    template <typename... _Mappedty>
    std::pair<iterator, bool> try_emplace(std::string_view _Keyval, _Mappedty&&... _Mapval) {
        auto it = entry_.find(_Keyval);
        if (it != entry_.end()) { return {it->second, false}; }
        if constexpr (sizeof...(_Mappedty) > 0) {
            auto res           = storage_.emplace(storage_.end(), _Keyval, std::forward<_Mappedty>(_Mapval)...);
            entry_[res->first] = res;
            return {res, true};
        } else {
            auto res           = storage_.emplace(storage_.end(), _Keyval, T{});
            entry_[res->first] = res;
            return {res, true};
        }
    }

    void clear() noexcept {
        storage_.clear();
        entry_.clear();
    }

    [[nodiscard]] const std::string& key_index(std::size_t index) const noexcept {
        auto it = std::next(storage_.begin(), static_cast<storage_type::difference_type>(index));
        if (it != storage_.end()) { return it->first; }
        std::abort();
    }

    [[nodiscard]] bool operator==(const ordered_string_map& other) const { return storage_ == other.storage_; }

private:
    storage_type storage_;
    entry_type   entry_;
};

} // namespace sculk::jsonc::inline abi_v1_4_1::detail