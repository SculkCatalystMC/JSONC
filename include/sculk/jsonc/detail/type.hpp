// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "container.hpp"
#include <cmath>
#include <cstdint>
#include <format>
#include <optional>
#include <variant>
#include <vector>

namespace sculk::jsonc {
inline namespace abi_v1_4_1 {

enum class value_type : std::uint8_t {
    null                        = 0,
    boolean                     = 1,
    number_integer_signed       = 2,
    number_integer_unsigned     = 3,
    number_floating_point       = 4,
    string                      = 5,
    object                      = 6,
    array                       = 7,
    number_big_integer          = 8,
    number_high_precision_float = 9,
};

namespace detail {

template <typename T>
concept is_range_loopable = !std::is_constructible_v<std::string, T> && (std::is_bounded_array_v<std::remove_cvref_t<T>> || requires(T t) {
    t.begin();
    t.end();
});
template <typename T>
concept is_array_like = is_range_loopable<T> && !requires { typename std::remove_cvref_t<T>::mapped_type; };
template <typename T>
concept is_object_like = is_range_loopable<T> && requires {
    typename std::remove_cvref_t<T>::key_type;
    typename std::remove_cvref_t<T>::mapped_type;
};
template <typename Var, std::size_t... Is>
constexpr bool emplace_variant_impl(Var& v, std::size_t idx, std::index_sequence<Is...>) noexcept {
    using emplace_func             = void (*)(Var&);
    constexpr emplace_func table[] = {+[](Var& var) { var.template emplace<Is>(); }...};
    if (idx >= sizeof...(Is)) { return false; }
    table[idx](v);
    return true;
}
template <typename Var>
constexpr bool emplace_variant(Var& v, std::size_t idx) noexcept {
    constexpr std::size_t N = std::variant_size_v<std::remove_reference_t<Var>>;
    return emplace_variant_impl(v, idx, std::make_index_sequence<N>{});
}

struct basic_big_integer {
    basic_big_integer() noexcept = default;
    basic_big_integer(std::string_view val) noexcept : view_(val) {}
    bool        operator==(const basic_big_integer& other) const noexcept { return view_ == other.view_; }
    std::string view_;
};

struct basic_high_precision_float {
    basic_high_precision_float() noexcept = default;
    basic_high_precision_float(std::string_view val) noexcept : view_(val) {}
    bool        operator==(const basic_high_precision_float& other) const noexcept { return view_ == other.view_; }
    std::string view_;
};

template <bool _IsOrdered, bool _AllowComments>
class basic_jsonc {
public:
    class basic_object {
    public:
        using map_type               = std::conditional_t<_IsOrdered, ordered_string_map<basic_jsonc>, string_map<basic_jsonc>>;
        using jsonc_type             = basic_jsonc;
        using iterator               = map_type::iterator;
        using const_iterator         = map_type::const_iterator;
        using reverse_iterator       = map_type::reverse_iterator;
        using const_reverse_iterator = map_type::const_reverse_iterator;

    public:
        basic_object() = default;
        basic_object(std::initializer_list<std::pair<std::string, basic_jsonc>> val);

        [[nodiscard]] basic_jsonc& operator[](std::string_view index);
        [[nodiscard]] JSONC_RESULT(const basic_jsonc&) operator[](std::string_view index) const;

        [[nodiscard]] JSONC_RESULT(basic_jsonc&) at(std::string_view index);
        [[nodiscard]] basic_jsonc& at(std::string_view index, const basic_jsonc& default_value);
        [[nodiscard]] JSONC_RESULT(const basic_jsonc&) at(std::string_view index) const;

        [[nodiscard]] bool contains(std::string_view index) const noexcept;
        [[nodiscard]] bool contains(std::string_view index, value_type type) const noexcept;

        bool erase(std::string_view index) noexcept;

        [[nodiscard]] bool empty() const noexcept;

        void clear() noexcept;

        [[nodiscard]] std::size_t size() const noexcept;

        [[nodiscard]] std::string
        dump(int indent = 4, bool ensure_ascii = false, bool ignore_comments = false, bool multi_line_comments_format = true) const;

        [[nodiscard]] const std::string& key_index(std::size_t index) const noexcept;

        [[nodiscard]] iterator begin() noexcept;
        [[nodiscard]] iterator end() noexcept;

        [[nodiscard]] const_iterator begin() const noexcept;
        [[nodiscard]] const_iterator end() const noexcept;

        [[nodiscard]] const_iterator cbegin() const noexcept;
        [[nodiscard]] const_iterator cend() const noexcept;

        [[nodiscard]] reverse_iterator rbegin() noexcept;
        [[nodiscard]] reverse_iterator rend() noexcept;

        [[nodiscard]] const_reverse_iterator rbegin() const noexcept;
        [[nodiscard]] const_reverse_iterator rend() const noexcept;

        [[nodiscard]] const_reverse_iterator crbegin() const noexcept;
        [[nodiscard]] const_reverse_iterator crend() const noexcept;

        void merge_patch(const basic_object& other, bool merge_list = false);
        JSONC_RESULT(void) merge_patch(const basic_jsonc& other, bool merge_list = false);

        void merge_comments(const basic_object& other)
            requires(_AllowComments);
        void move_comments_to_before()
            requires(_AllowComments);

        [[nodiscard]] bool operator==(const basic_object& other) const;
        [[nodiscard]] bool operator==(const basic_jsonc& other) const;

        [[nodiscard]] bool has_key_before_comments(std::string_view index) const noexcept
            requires(_AllowComments);
        [[nodiscard]] bool has_key_after_comments(std::string_view index) const noexcept
            requires(_AllowComments);

        [[nodiscard]] std::vector<std::string>& key_before_comments(std::string_view index)
            requires(_AllowComments);
        [[nodiscard]] JSONC_RESULT(const std::vector<std::string>&) key_before_comments(std::string_view index) const
            requires(_AllowComments);

        [[nodiscard]] std::vector<std::string>& key_after_comments(std::string_view index)
            requires(_AllowComments);
        [[nodiscard]] JSONC_RESULT(const std::vector<std::string>&) key_after_comments(std::string_view index) const
            requires(_AllowComments);

        [[nodiscard]] JSONC_RESULT(std::vector<std::string>) get_key_before_comments(std::string_view index) const
            requires(_AllowComments);
        [[nodiscard]] JSONC_RESULT(std::vector<std::string>) get_key_after_comments(std::string_view index) const
            requires(_AllowComments);

        [[nodiscard]] JSONC_RESULT(std::string) get_key_before_comment(std::string_view index, std::size_t comment_index) const
            requires(_AllowComments);
        [[nodiscard]] JSONC_RESULT(std::string) get_key_after_comment(std::string_view index, std::size_t comment_index) const
            requires(_AllowComments);

        bool set_key_before_comments(std::string_view index, const std::vector<std::string>& comments)
            requires(_AllowComments);
        bool set_key_after_comments(std::string_view index, const std::vector<std::string>& comments)
            requires(_AllowComments);

        bool add_key_before_comment(std::string_view index, std::string_view comment)
            requires(_AllowComments);
        bool add_key_after_comment(std::string_view index, std::string_view comment)
            requires(_AllowComments);

        void clear_key_before_comments(std::string_view index)
            requires(_AllowComments);
        void clear_key_after_comments(std::string_view index)
            requires(_AllowComments);

        bool remove_key_before_comment(std::string_view index, std::size_t comment_index)
            requires(_AllowComments);
        bool remove_key_after_comment(std::string_view index, std::size_t comment_index)
            requires(_AllowComments);

        [[nodiscard]] std::size_t key_before_comments_size(std::string_view index) const noexcept
            requires(_AllowComments);
        [[nodiscard]] std::size_t key_after_comments_size(std::string_view index) const noexcept
            requires(_AllowComments);

    private:
        struct key_comments {
            std::vector<std::string> before_comments_{};
            std::vector<std::string> after_comments_{};
        };
        using key_comments_map_type = std::conditional_t<_AllowComments, string_hash_map<key_comments>, std::monostate>;
        template <bool O, bool A>
        friend class basic_jsonc;
        map_type                                    storage_{};
        [[no_unique_address]] key_comments_map_type key_comments_{};
    };

    class basic_array {
    public:
        using jsonc_type             = basic_jsonc;
        using iterator               = std::vector<basic_jsonc>::iterator;
        using const_iterator         = std::vector<basic_jsonc>::const_iterator;
        using reverse_iterator       = std::vector<basic_jsonc>::reverse_iterator;
        using const_reverse_iterator = std::vector<basic_jsonc>::const_reverse_iterator;

    public:
        basic_array() = default;
        basic_array(std::initializer_list<basic_jsonc> val);

        [[nodiscard]] constexpr basic_jsonc& operator[](std::size_t index) noexcept;
        [[nodiscard]] constexpr JSONC_RESULT(const basic_jsonc&) operator[](std::size_t index) const noexcept;

        [[nodiscard]] constexpr JSONC_RESULT(basic_jsonc&) at(std::size_t index);
        [[nodiscard]] constexpr JSONC_RESULT(const basic_jsonc&) at(std::size_t index) const;

        [[nodiscard]] constexpr std::size_t size() const noexcept;

        void clear() noexcept;

        [[nodiscard]] constexpr bool empty() const noexcept;

        void push_back(const basic_jsonc& val);
        void push_back(basic_jsonc&& val);

        bool erase(std::size_t where);
        bool erase(std::size_t first, std::size_t last);

        [[nodiscard]] const basic_jsonc& front() const noexcept;
        [[nodiscard]] basic_jsonc&       front() noexcept;

        [[nodiscard]] const basic_jsonc& back() const noexcept;
        [[nodiscard]] basic_jsonc&       back() noexcept;

        iterator erase(const_iterator where);
        iterator erase(const_iterator first, const_iterator last);

        [[nodiscard]] std::string
        dump(int indent = 4, bool ensure_ascii = false, bool ignore_comments = false, bool multi_line_comments_format = true) const;

        [[nodiscard]] iterator begin() noexcept;
        [[nodiscard]] iterator end() noexcept;

        [[nodiscard]] const_iterator begin() const noexcept;
        [[nodiscard]] const_iterator end() const noexcept;

        [[nodiscard]] const_iterator cbegin() const noexcept;
        [[nodiscard]] const_iterator cend() const noexcept;

        [[nodiscard]] reverse_iterator rbegin() noexcept;
        [[nodiscard]] reverse_iterator rend() noexcept;

        [[nodiscard]] const_reverse_iterator rbegin() const noexcept;
        [[nodiscard]] const_reverse_iterator rend() const noexcept;

        [[nodiscard]] const_reverse_iterator crbegin() const noexcept;
        [[nodiscard]] const_reverse_iterator crend() const noexcept;

        void merge_patch(const basic_array& other);
        JSONC_RESULT(void) merge_patch(const basic_jsonc& other);

        void merge_comments(const basic_array& other)
            requires(_AllowComments);
        void move_comments_to_before()
            requires(_AllowComments);

        [[nodiscard]] bool operator==(const basic_array& other) const;
        [[nodiscard]] bool operator==(const basic_jsonc& other) const;

    private:
        template <bool O, bool A>
        friend class basic_jsonc;
        std::vector<basic_jsonc> storage_{};
    };

public:
    using type_variant = std::variant<
        std::monostate,
        bool,
        std::int64_t,
        std::uint64_t,
        double,
        std::string,
        basic_object,
        basic_array,
        basic_big_integer,
        basic_high_precision_float>;

    using object_type = basic_object;
    using array_type  = basic_array;

public:
    template <bool _Const, bool _Reserve>
    class basic_iterator {
    public:
        using reference = std::conditional_t<_Const, const basic_jsonc, basic_jsonc>&;
        using pointer   = std::add_pointer_t<reference>;

    private:
        friend class basic_jsonc;
        using IteratorType = std::conditional_t<
            _Const,
            std::variant<
                const basic_jsonc*,
                std::conditional_t<_Reserve, typename basic_object::const_reverse_iterator, typename basic_object::const_iterator>,
                std::conditional_t<_Reserve, typename basic_array::const_reverse_iterator, typename basic_array::const_iterator>>,
            std::variant<
                basic_jsonc*,
                std::conditional_t<_Reserve, typename basic_object::reverse_iterator, typename basic_object::iterator>,
                std::conditional_t<_Reserve, typename basic_array::reverse_iterator, typename basic_array::iterator>>>;
        IteratorType iterator_;

        template <bool Reserve>
        [[nodiscard]] static basic_iterator make_begin(auto& var) noexcept {
            basic_iterator result{};
            std::visit(
                [&](auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, basic_object>) {
                        if constexpr (Reserve) {
                            result.iterator_.template emplace<1>(val.rbegin());
                        } else {
                            result.iterator_.template emplace<1>(val.begin());
                        }
                    } else if constexpr (std::is_same_v<T, basic_array>) {
                        if constexpr (Reserve) {
                            result.iterator_.template emplace<2>(val.rbegin());
                        } else {
                            result.iterator_.template emplace<2>(val.begin());
                        }
                    } else if constexpr (std::is_same_v<T, std::monostate>) {
                        result.iterator_.template emplace<0>(std::addressof(var) + 1);
                    } else {
                        result.iterator_.template emplace<0>(std::addressof(var));
                    }
                },
                var.storage_
            );
            return result;
        }

        template <bool Reserve>
        [[nodiscard]] static basic_iterator make_end(auto& var) noexcept {
            basic_iterator result{};
            std::visit(
                [&](auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, basic_object>) {
                        if constexpr (Reserve) {
                            result.iterator_.template emplace<1>(val.rend());
                        } else {
                            result.iterator_.template emplace<1>(val.end());
                        }
                    } else if constexpr (std::is_same_v<T, basic_array>) {
                        if constexpr (Reserve) {
                            result.iterator_.template emplace<2>(val.rend());
                        } else {
                            result.iterator_.template emplace<2>(val.end());
                        }
                    } else {
                        result.iterator_.template emplace<0>(std::addressof(var) + 1);
                    }
                },
                var.storage_
            );
            return result;
        }

    public:
        [[nodiscard]] reference operator*() const noexcept {
            return std::visit(
                [](auto& val) -> reference {
                    using T          = std::decay_t<decltype(val)>;
                    using ObjectType = std::conditional_t<
                        _Const,
                        std::conditional_t<_Reserve, typename basic_object::const_reverse_iterator, typename basic_object::const_iterator>,
                        std::conditional_t<_Reserve, typename basic_object::reverse_iterator, typename basic_object::iterator>>;
                    if constexpr (std::is_same_v<T, ObjectType>) {
                        return val->second;
                    } else {
                        return *val;
                    }
                },
                iterator_
            );
        }

        [[nodiscard]] pointer operator->() const noexcept { return std::addressof(**this); }

        basic_iterator& operator++() noexcept {
            std::visit([](auto& val) { ++val; }, iterator_);
            return *this;
        }

        basic_iterator operator++(int) noexcept {
            basic_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        basic_iterator& operator--() noexcept {
            std::visit([](auto& val) { --val; }, iterator_);
            return *this;
        }

        basic_iterator operator--(int) noexcept {
            basic_iterator tmp = *this;
            --*this;
            return tmp;
        }

        [[nodiscard]] bool operator==(basic_iterator const& r) const noexcept { return this->iterator_ == r.iterator_; }
    };

    class iterator_proxy {
    public:
        basic_object::iterator begin() noexcept { return self_.begin(); }
        basic_object::iterator end() noexcept { return self_.end(); }

    private:
        friend class basic_jsonc;
        basic_object& self_;
        iterator_proxy(basic_object& self) : self_(self) {}
    };

    class const_iterator_proxy {
    public:
        basic_object::const_iterator begin() const noexcept { return self_.begin(); }
        basic_object::const_iterator end() const noexcept { return self_.end(); }

    private:
        friend class basic_jsonc;
        const basic_object& self_;
        const_iterator_proxy(const basic_object& self) : self_(self) {}
    };

public:
    using iterator               = basic_iterator<false, false>;
    using const_iterator         = basic_iterator<true, false>;
    using reverse_iterator       = basic_iterator<false, true>;
    using const_reverse_iterator = basic_iterator<true, true>;

public:
    [[nodiscard]] basic_jsonc() = default;

    [[nodiscard]] constexpr basic_jsonc(value_type type) noexcept { emplace_variant(storage_, static_cast<std::size_t>(type)); };

    [[nodiscard]] constexpr basic_jsonc(std::nullptr_t) noexcept : storage_(std::monostate()) {};

    [[nodiscard]] constexpr basic_jsonc(bool val) noexcept : storage_(val) {};

    template <std::signed_integral T>
    [[nodiscard]] constexpr basic_jsonc(T val) noexcept : storage_(static_cast<std::int64_t>(val)) {};

    template <std::unsigned_integral T>
        requires(!std::same_as<T, bool>)
    [[nodiscard]] constexpr basic_jsonc(T val) noexcept : storage_(static_cast<std::uint64_t>(val)){};

    [[nodiscard]] constexpr basic_jsonc(std::string_view val) noexcept : storage_(std::string(val)) {};
    [[nodiscard]] constexpr basic_jsonc(const std::string& val) noexcept : storage_(val) {};

    [[nodiscard]] constexpr basic_jsonc(double val) noexcept : storage_(val) {};
    [[nodiscard]] constexpr basic_jsonc(float val) noexcept : storage_(std::round(val * 1e6) / 1e6) {};

    template <std::size_t N>
    [[nodiscard]] constexpr basic_jsonc(char const (&val)[N]) noexcept : storage_(std::string{val, N - 1}) {}

    [[nodiscard]] constexpr basic_jsonc(const basic_object& val) noexcept : storage_(val) {};
    [[nodiscard]] constexpr basic_jsonc(const basic_array& val) noexcept : storage_(val) {};
    [[nodiscard]] constexpr basic_jsonc(const basic_big_integer& val) noexcept : storage_(val) {};
    [[nodiscard]] constexpr basic_jsonc(const basic_high_precision_float& val) noexcept : storage_(val) {};

    [[nodiscard]] constexpr basic_jsonc(std::initializer_list<std::pair<std::string, basic_jsonc>> val) noexcept
    : storage_(std::in_place_type<basic_object>, val) {}

    template <bool IsOrdered, bool AllowComments>
    [[nodiscard]] constexpr basic_jsonc(const basic_jsonc<IsOrdered, AllowComments>& other) noexcept;

    template <bool IsOrdered, bool AllowComments>
    [[nodiscard]] constexpr basic_jsonc(basic_jsonc<IsOrdered, AllowComments>&& other) noexcept;

    template <bool IsOrdered, bool AllowComments>
    constexpr basic_jsonc& operator=(const basic_jsonc<IsOrdered, AllowComments>& other) noexcept;

    template <bool IsOrdered, bool AllowComments>
    constexpr basic_jsonc& operator=(basic_jsonc<IsOrdered, AllowComments>&& other) noexcept;

    constexpr void emplace(value_type type) noexcept { emplace_variant(storage_, static_cast<std::size_t>(type)); }

    [[nodiscard]] constexpr value_type       type() const noexcept;
    [[nodiscard]] constexpr std::string_view type_name() const noexcept;

    [[nodiscard]] constexpr bool hold(value_type value_type) const noexcept;

    [[nodiscard]] constexpr bool is_null() const noexcept;
    [[nodiscard]] constexpr bool is_boolean() const noexcept;
    [[nodiscard]] constexpr bool is_number_signed() const noexcept;
    [[nodiscard]] constexpr bool is_number_unsigned() const noexcept;
    [[nodiscard]] constexpr bool is_number_normal_integer() const noexcept;
    [[nodiscard]] constexpr bool is_number_big_inteager() const noexcept;
    [[nodiscard]] constexpr bool is_number_integer() const noexcept;
    [[nodiscard]] constexpr bool is_number_normal_float() const noexcept;
    [[nodiscard]] constexpr bool is_number_high_precision_float() const noexcept;
    [[nodiscard]] constexpr bool is_number_float() const noexcept;
    [[nodiscard]] constexpr bool is_normal_number() const noexcept;
    [[nodiscard]] constexpr bool is_high_precision_number() const noexcept;
    [[nodiscard]] constexpr bool is_number() const noexcept;
    [[nodiscard]] constexpr bool is_string() const noexcept;
    [[nodiscard]] constexpr bool is_object() const noexcept;
    [[nodiscard]] constexpr bool is_array() const noexcept;
    [[nodiscard]] constexpr bool is_primitive() const noexcept;
    [[nodiscard]] constexpr bool is_structured() const noexcept;

    [[nodiscard]] constexpr std::size_t size() const noexcept;

    [[nodiscard]] std::string dump(
        int  indent                     = 4,
        bool ensure_ascii               = false,
        bool ignore_comments            = false,
        bool multi_line_comments_format = true,
        bool global_comments            = true
    ) const;

    template <typename T>
    [[nodiscard]] JSONC_RESULT(T&) as();

    template <typename T>
    [[nodiscard]] JSONC_RESULT(const T&) as() const;

    template <typename T>
        requires std::is_arithmetic_v<T>
    [[nodiscard]] JSONC_RESULT(T) get() const;

    template <typename T>
        requires std::is_convertible_v<T, std::string>
    [[nodiscard]] JSONC_RESULT(T) get() const;

    template <is_array_like T>
    [[nodiscard]] JSONC_RESULT(T) get() const;

    template <is_object_like T>
    [[nodiscard]] JSONC_RESULT(T) get() const;

    [[nodiscard]] JSONC_RESULT(std::string) get_big_int_view() const;
    [[nodiscard]] JSONC_RESULT(std::string) get_any_int_view() const;

    [[nodiscard]] JSONC_RESULT(std::string) get_high_precision_float_view() const;
    [[nodiscard]] JSONC_RESULT(std::string) get_any_float_view() const;

    [[nodiscard]] JSONC_RESULT(std::string) get_any_number_view() const;

    [[nodiscard]] JSONC_RESULT(basic_jsonc&) operator[](std::string_view index);
    [[nodiscard]] JSONC_RESULT(const basic_jsonc&) operator[](std::string_view index) const;

    template <std::size_t N>
    [[nodiscard]] JSONC_RESULT(basic_jsonc&) operator[](char const (&index)[N]) {
        return operator[](std::string_view{index, N - 1});
    }
    template <std::size_t N>
    [[nodiscard]] JSONC_RESULT(const basic_jsonc&) operator[](char const (&index)[N]) const {
        return operator[](std::string_view{index, N - 1});
    }

    [[nodiscard]] JSONC_RESULT(basic_jsonc&) operator[](std::size_t index);
    [[nodiscard]] JSONC_RESULT(const basic_jsonc&) operator[](std::size_t index) const;

    [[nodiscard]] JSONC_RESULT(basic_jsonc&) at(std::string_view index);
    [[nodiscard]] JSONC_RESULT(basic_jsonc&) at(std::string_view index, const basic_jsonc& default_value);
    [[nodiscard]] JSONC_RESULT(const basic_jsonc&) at(std::string_view index) const;

    [[nodiscard]] JSONC_RESULT(basic_jsonc&) at(std::size_t index);
    [[nodiscard]] JSONC_RESULT(const basic_jsonc&) at(std::size_t index) const;

    [[nodiscard]] JSONC_RESULT(bool) contains(std::string_view index) const;
    [[nodiscard]] JSONC_RESULT(bool) contains(std::string_view index, value_type type) const;

    JSONC_RESULT(void) clear();

    [[nodiscard]] JSONC_RESULT(bool) empty() const;

    JSONC_RESULT(bool) erase(std::string_view index);
    JSONC_RESULT(bool) erase(std::size_t where);
    JSONC_RESULT(bool) erase(std::size_t first, std::size_t last);

    JSONC_RESULT(void) push_back(const basic_jsonc& val);
    JSONC_RESULT(void) push_back(basic_jsonc&& val);

    [[nodiscard]] JSONC_RESULT(const basic_jsonc&) front() const;
    [[nodiscard]] JSONC_RESULT(basic_jsonc&) front();

    [[nodiscard]] JSONC_RESULT(const basic_jsonc&) back() const;
    [[nodiscard]] JSONC_RESULT(basic_jsonc&) back();

    JSONC_RESULT(iterator_proxy) items() {
        if (auto* storage = std::get_if<basic_object>(&storage_)) { return iterator_proxy(*storage); }
        _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
    }

    JSONC_RESULT(const_iterator_proxy) items() const {
        if (auto* storage = std::get_if<basic_object>(&storage_)) { return const_iterator_proxy(*storage); }
        _JSONC_TYPE_ERROR(std::format("Type must be an object, but is {}", type_name()));
    }

    [[nodiscard]] iterator begin() noexcept;
    [[nodiscard]] iterator end() noexcept;

    [[nodiscard]] const_iterator begin() const noexcept;
    [[nodiscard]] const_iterator end() const noexcept;

    [[nodiscard]] const_iterator cbegin() const noexcept;
    [[nodiscard]] const_iterator cend() const noexcept;

    [[nodiscard]] reverse_iterator rbegin() noexcept;
    [[nodiscard]] reverse_iterator rend() noexcept;

    [[nodiscard]] const_reverse_iterator rbegin() const noexcept;
    [[nodiscard]] const_reverse_iterator rend() const noexcept;

    [[nodiscard]] const_reverse_iterator crbegin() const noexcept;
    [[nodiscard]] const_reverse_iterator crend() const noexcept;

    void merge_patch(const basic_jsonc& other, bool merge_list = false);
    void merge_comments(const basic_jsonc& other)
        requires(_AllowComments);
    void move_comments_to_before()
        requires(_AllowComments);

    [[nodiscard]] constexpr bool operator==(const basic_jsonc& other) const;

    template <typename T>
        requires std::is_arithmetic_v<T>
    [[nodiscard]] constexpr operator T() const;

    template <typename T>
        requires std::is_convertible_v<T, std::string>
    [[nodiscard]] constexpr operator T() const;

    template <is_array_like T>
    [[nodiscard]] constexpr operator T() const;

    template <is_object_like T>
    [[nodiscard]] constexpr operator T() const;

    template <bool IsOrdered, bool AllowComments>
    [[nodiscard]] constexpr operator basic_jsonc<IsOrdered, AllowComments>() noexcept;

    [[nodiscard]] constexpr bool has_before_comments() const noexcept
        requires(_AllowComments);
    [[nodiscard]] constexpr bool has_after_comments() const noexcept
        requires(_AllowComments);

    [[nodiscard]] std::vector<std::string>& before_comments() noexcept
        requires(_AllowComments);
    [[nodiscard]] const std::vector<std::string>& before_comments() const noexcept
        requires(_AllowComments);

    [[nodiscard]] std::vector<std::string>& after_comments() noexcept
        requires(_AllowComments);
    [[nodiscard]] const std::vector<std::string>& after_comments() const noexcept
        requires(_AllowComments);

    [[nodiscard]] std::vector<std::string> get_before_comments() const
        requires(_AllowComments);
    [[nodiscard]] std::vector<std::string> get_after_comments() const
        requires(_AllowComments);

    void set_before_comments(const std::vector<std::string>& comments)
        requires(_AllowComments);
    void set_after_comments(const std::vector<std::string>& comments)
        requires(_AllowComments);

    void add_before_comment(std::string_view comment)
        requires(_AllowComments);
    void add_after_comment(std::string_view comment)
        requires(_AllowComments);

    void clear_before_comments()
        requires(_AllowComments);
    void clear_after_comments()
        requires(_AllowComments);

    bool remove_before_comment(std::size_t comment_index)
        requires(_AllowComments);
    bool remove_after_comment(std::size_t comment_index)
        requires(_AllowComments);

    [[nodiscard]] constexpr std::size_t before_comments_size() const noexcept
        requires(_AllowComments);
    [[nodiscard]] constexpr std::size_t after_comments_size() const noexcept
        requires(_AllowComments);

    [[nodiscard]] JSONC_RESULT(bool) has_key_before_comments(std::string_view index) const
        requires(_AllowComments);
    [[nodiscard]] JSONC_RESULT(bool) has_key_after_comments(std::string_view index) const
        requires(_AllowComments);

    [[nodiscard]] JSONC_RESULT(std::vector<std::string>&) key_before_comments(std::string_view index)
        requires(_AllowComments);
    [[nodiscard]] JSONC_RESULT(const std::vector<std::string>&) key_before_comments(std::string_view index) const
        requires(_AllowComments);

    [[nodiscard]] JSONC_RESULT(std::vector<std::string>&) key_after_comments(std::string_view index)
        requires(_AllowComments);
    [[nodiscard]] JSONC_RESULT(const std::vector<std::string>&) key_after_comments(std::string_view index) const
        requires(_AllowComments);

    [[nodiscard]] JSONC_RESULT(std::vector<std::string>) get_key_before_comments(std::string_view index) const
        requires(_AllowComments);
    [[nodiscard]] JSONC_RESULT(std::vector<std::string>) get_key_after_comments(std::string_view index) const
        requires(_AllowComments);

    [[nodiscard]] JSONC_RESULT(std::string) get_key_before_comment(std::string_view index, std::size_t comment_index) const
        requires(_AllowComments);
    [[nodiscard]] JSONC_RESULT(std::string) get_key_after_comment(std::string_view index, std::size_t comment_index) const
        requires(_AllowComments);

    JSONC_RESULT(bool) set_key_before_comments(std::string_view index, const std::vector<std::string>& comments)  requires(_AllowComments);
    JSONC_RESULT(bool) set_key_after_comments(std::string_view index, const std::vector<std::string>& comments)  requires(_AllowComments);

    JSONC_RESULT(bool) add_key_before_comment(std::string_view index, std::string_view comment)  requires(_AllowComments);
    JSONC_RESULT(bool) add_key_after_comment(std::string_view index, std::string_view comment)  requires(_AllowComments);
    JSONC_RESULT(void) clear_key_before_comments(std::string_view index)  requires(_AllowComments);
    JSONC_RESULT(void) clear_key_after_comments(std::string_view index)  requires(_AllowComments);

    JSONC_RESULT(bool) remove_key_before_comment(std::string_view index, std::size_t comment_index)  requires(_AllowComments);
    JSONC_RESULT(bool) remove_key_after_comment(std::string_view index, std::size_t comment_index)  requires(_AllowComments);

    [[nodiscard]] JSONC_RESULT(std::size_t) key_before_comments_size(std::string_view index) const
        requires(_AllowComments);
    [[nodiscard]] JSONC_RESULT(std::size_t) key_after_comments_size(std::string_view index) const
        requires(_AllowComments);

public:
    static JSONC_PARSE_RESULT(
        basic_jsonc
    ) parse(std::string_view content, bool allow_trailing_comma = false, bool ignore_comments = false, bool float_keep_precision = true);

    static basic_jsonc object() { return basic_object(); }
    static basic_jsonc object(std::initializer_list<std::pair<std::string, basic_jsonc>> val) { return basic_object(val); }
    static basic_jsonc array() { return basic_array(); }
    static basic_jsonc array(std::initializer_list<basic_jsonc> val) { return basic_array(val); }

    static std::optional<basic_jsonc> from_any_int(std::string_view view) noexcept;
    static std::optional<basic_jsonc> from_any_float(std::string_view view, bool float_keep_precision = false) noexcept;
    static std::optional<basic_jsonc> from_any_number(std::string_view view, bool float_keep_precision = false) noexcept;

private:
    friend class basic_object;
    friend class basic_array;
    template <bool O, bool A>
    friend class basic_jsonc;
    using comments_type = std::conditional_t<_AllowComments, std::vector<std::string>, std::monostate>;
    type_variant                        storage_{};
    [[no_unique_address]] comments_type before_comments_{};
    [[no_unique_address]] comments_type after_comments_{};
};

} // namespace detail

} // namespace abi_v1_4_1

} // namespace sculk::jsonc