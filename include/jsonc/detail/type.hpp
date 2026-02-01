#pragma once
#include "container.hpp"
#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

namespace jsonc {
inline namespace abi_v1_1_1 {

enum class value_type : uint8_t {
    null                    = 0,
    boolean                 = 1,
    number_integer_signed   = 2,
    number_integer_unsigned = 3,
    number_floating_point   = 4,
    string                  = 5,
    object                  = 6,
    array                   = 7,
    number_big_integer      = 8,
};

namespace detail {

struct basic_big_int {
    basic_big_int() noexcept = default;
    basic_big_int(std::string_view val) noexcept : view_(val) {}
    bool        operator==(const basic_big_int& other) const noexcept { return view_ == other.view_; }
    std::string view_;
};

class basic_jsonc;

class basic_object {
public:
    using iterator               = ordered_string_hash_map<basic_jsonc>::iterator;
    using const_iterator         = ordered_string_hash_map<basic_jsonc>::const_iterator;
    using reverse_iterator       = ordered_string_hash_map<basic_jsonc>::reverse_iterator;
    using const_reverse_iterator = ordered_string_hash_map<basic_jsonc>::const_reverse_iterator;

public:
    basic_object() JSONC_EXCEPTION_TYPE = default;
    inline basic_object(std::initializer_list<std::pair<std::string, basic_jsonc>> val) JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline basic_jsonc& operator[](std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const basic_jsonc&) operator[](std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(basic_jsonc&) at(std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline basic_jsonc& at(std::string_view index, const basic_jsonc& default_value) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const basic_jsonc&) at(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline bool contains(std::string_view index) const noexcept;
    [[nodiscard]] inline bool contains(std::string_view index, value_type type) const noexcept;

    inline bool erase(std::string_view index) noexcept;

    [[nodiscard]] inline bool empty() const noexcept;

    inline void clear() noexcept;

    [[nodiscard]] inline size_t size() const noexcept;

    [[nodiscard]] inline std::string
    dump(int indent = 4, bool ensure_ascii = false, bool ignore_comments = false, bool multi_line_comments_format = true) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline const std::string& key_index(size_t index) const noexcept;

    [[nodiscard]] inline iterator begin() noexcept;
    [[nodiscard]] inline iterator end() noexcept;

    [[nodiscard]] inline const_iterator begin() const noexcept;
    [[nodiscard]] inline const_iterator end() const noexcept;

    [[nodiscard]] inline const_iterator cbegin() const noexcept;
    [[nodiscard]] inline const_iterator cend() const noexcept;

    [[nodiscard]] inline reverse_iterator rbegin() noexcept;
    [[nodiscard]] inline reverse_iterator rend() noexcept;

    [[nodiscard]] inline const_reverse_iterator rbegin() const noexcept;
    [[nodiscard]] inline const_reverse_iterator rend() const noexcept;

    [[nodiscard]] inline const_reverse_iterator crbegin() const noexcept;
    [[nodiscard]] inline const_reverse_iterator crend() const noexcept;

    inline void merge_patch(const basic_object& other, bool merge_list = false) JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(void) merge_patch(const basic_jsonc& other, bool merge_list = false) JSONC_EXCEPTION_TYPE;

    inline void merge_comments(const basic_object& other) JSONC_EXCEPTION_TYPE;
    inline void move_comments_to_before() JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline bool operator==(const basic_object& other) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline bool operator==(const basic_jsonc& other) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline bool has_key_before_comments(std::string_view index) const noexcept;
    [[nodiscard]] inline bool has_key_after_comments(std::string_view index) const noexcept;

    [[nodiscard]] inline std::vector<std::string>& key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const std::vector<std::string>&) key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline std::vector<std::string>& key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const std::vector<std::string>&) key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(std::vector<std::string>) get_key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(std::vector<std::string>) get_key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(std::string) get_key_before_comment(std::string_view index, size_t comment_index) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(std::string) get_key_after_comment(std::string_view index, size_t comment_index) const JSONC_EXCEPTION_TYPE;

    inline bool set_key_before_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE;
    inline bool set_key_after_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE;

    inline bool add_key_before_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE;
    inline bool add_key_after_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE;

    inline void clear_key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE;
    inline void clear_key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE;

    inline bool remove_key_before_comment(std::string_view index, size_t comment_index) JSONC_EXCEPTION_TYPE;
    inline bool remove_key_after_comment(std::string_view index, size_t comment_index) JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline size_t key_before_comments_size(std::string_view index) const noexcept;
    [[nodiscard]] inline size_t key_after_comments_size(std::string_view index) const noexcept;

private:
    struct key_comments {
        std::vector<std::string> before_comments_{};
        std::vector<std::string> after_comments_{};
    };
    friend class basic_jsonc;
    ordered_string_hash_map<basic_jsonc> storage_{};
    string_hash_map<key_comments>        key_comments_{};
};

class basic_array {
public:
    using iterator               = std::vector<basic_jsonc>::iterator;
    using const_iterator         = std::vector<basic_jsonc>::const_iterator;
    using reverse_iterator       = std::vector<basic_jsonc>::reverse_iterator;
    using const_reverse_iterator = std::vector<basic_jsonc>::const_reverse_iterator;

public:
    basic_array() JSONC_EXCEPTION_TYPE = default;
    inline basic_array(std::initializer_list<basic_jsonc> val) JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline constexpr basic_jsonc& operator[](size_t index) noexcept;
    [[nodiscard]] inline constexpr JSONC_RESULT(const basic_jsonc&) operator[](size_t index) const noexcept;

    [[nodiscard]] inline constexpr JSONC_RESULT(basic_jsonc&) at(size_t index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline constexpr JSONC_RESULT(const basic_jsonc&) at(size_t index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline constexpr size_t size() const noexcept;

    inline void clear() noexcept;

    [[nodiscard]] inline constexpr bool empty() const noexcept;

    inline void push_back(const basic_jsonc& val) JSONC_EXCEPTION_TYPE;
    inline void push_back(basic_jsonc&& val) JSONC_EXCEPTION_TYPE;

    inline bool erase(size_t where);
    inline bool erase(size_t first, size_t last);

    [[nodiscard]] inline const basic_jsonc& front() const noexcept;
    [[nodiscard]] inline basic_jsonc&       front() noexcept;

    [[nodiscard]] inline const basic_jsonc& back() const noexcept;
    [[nodiscard]] inline basic_jsonc&       back() noexcept;

    inline iterator erase(const_iterator where) JSONC_EXCEPTION_TYPE;
    inline iterator erase(const_iterator first, const_iterator last) JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline std::string
    dump(int indent = 4, bool ensure_ascii = false, bool ignore_comments = false, bool multi_line_comments_format = true) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline iterator begin() noexcept;
    [[nodiscard]] inline iterator end() noexcept;

    [[nodiscard]] inline const_iterator begin() const noexcept;
    [[nodiscard]] inline const_iterator end() const noexcept;

    [[nodiscard]] inline const_iterator cbegin() const noexcept;
    [[nodiscard]] inline const_iterator cend() const noexcept;

    [[nodiscard]] inline reverse_iterator rbegin() noexcept;
    [[nodiscard]] inline reverse_iterator rend() noexcept;

    [[nodiscard]] inline const_reverse_iterator rbegin() const noexcept;
    [[nodiscard]] inline const_reverse_iterator rend() const noexcept;

    [[nodiscard]] inline const_reverse_iterator crbegin() const noexcept;
    [[nodiscard]] inline const_reverse_iterator crend() const noexcept;

    inline void merge_patch(const basic_array& other) JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(void) merge_patch(const basic_jsonc& other) JSONC_EXCEPTION_TYPE;

    inline void merge_comments(const basic_array& other) JSONC_EXCEPTION_TYPE;
    inline void move_comments_to_before() JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline bool operator==(const basic_array& other) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline bool operator==(const basic_jsonc& other) const JSONC_EXCEPTION_TYPE;

private:
    friend class basic_jsonc;
    std::vector<basic_jsonc> storage_{};
};

using type_variant = std::variant<std::monostate, bool, int64_t, uint64_t, double, std::string, basic_object, basic_array, basic_big_int>;
template <class T>
inline constexpr bool is_jsonc_type_convertible_v = [] {
    return []<size_t... I>(std::index_sequence<I...>) {
        return (std::is_convertible_v<std::variant_alternative_t<I, type_variant>, T> || ...);
    }(std::make_index_sequence<std::variant_size_v<type_variant>>{});
}();
template <typename T>
concept is_jsonc_type_convertible = is_jsonc_type_convertible_v<T>;
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
template <typename Var, size_t... Is>
constexpr bool emplace_variant_impl(Var& v, size_t idx, std::index_sequence<Is...>) noexcept {
    using emplace_func             = void (*)(Var&);
    constexpr emplace_func table[] = {+[](Var& var) { var.template emplace<Is>(); }...};
    if (idx >= sizeof...(Is)) { return false; }
    table[idx](v);
    return true;
}
template <typename Var>
constexpr bool emplace_variant(Var& v, size_t idx) noexcept {
    constexpr size_t N = std::variant_size_v<std::remove_reference_t<Var>>;
    return emplace_variant_impl(v, idx, std::make_index_sequence<N>{});
}

class basic_jsonc {
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
                std::conditional_t<_Reserve, basic_object::const_reverse_iterator, basic_object::const_iterator>,
                std::conditional_t<_Reserve, basic_array::const_reverse_iterator, basic_array::const_iterator>>,
            std::variant<
                basic_jsonc*,
                std::conditional_t<_Reserve, basic_object::reverse_iterator, basic_object::iterator>,
                std::conditional_t<_Reserve, basic_array::reverse_iterator, basic_array::iterator>>>;
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
                        std::conditional_t<_Reserve, basic_object::const_reverse_iterator, basic_object::const_iterator>,
                        std::conditional_t<_Reserve, basic_object::reverse_iterator, basic_object::iterator>>;
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
    basic_jsonc() = default;

    inline constexpr basic_jsonc(value_type type) noexcept { emplace_variant(storage_, static_cast<size_t>(type)); };

    inline constexpr basic_jsonc(std::nullptr_t) noexcept : storage_(std::monostate()) {};

    inline constexpr basic_jsonc(bool val) noexcept : storage_(val) {};

    template <std::signed_integral T>
    inline constexpr basic_jsonc(T val) noexcept : storage_(static_cast<int64_t>(val)){};

    template <std::unsigned_integral T>
        requires(!std::same_as<T, bool>)
    inline constexpr basic_jsonc(T val) noexcept : storage_(static_cast<uint64_t>(val)){};

    inline constexpr basic_jsonc(std::string_view val) noexcept : storage_(std::string(val)) {};
    inline constexpr basic_jsonc(const std::string& val) noexcept : storage_(val) {};

    inline constexpr basic_jsonc(double val) noexcept : storage_(val) {};
    inline constexpr basic_jsonc(float val) noexcept : storage_(std::round(val * 1e6) / 1e6) {};

    template <size_t N>
    [[nodiscard]] inline basic_jsonc(char const (&val)[N]) noexcept : storage_(std::string{val, N - 1}) {}

    inline constexpr basic_jsonc(const basic_object& val) noexcept : storage_(val) {};
    inline constexpr basic_jsonc(const basic_array& val) noexcept : storage_(val) {};
    inline constexpr basic_jsonc(const basic_big_int& val) noexcept : storage_(val) {};

    inline constexpr basic_jsonc(std::initializer_list<std::pair<std::string, basic_jsonc>> val) noexcept
    : storage_(std::in_place_type<basic_object>, val) {}

    inline constexpr void emplace(value_type type) noexcept { emplace_variant(storage_, static_cast<size_t>(type)); }

    [[nodiscard]] inline constexpr value_type       type() const noexcept;
    [[nodiscard]] inline constexpr std::string_view type_name() const noexcept;

    [[nodiscard]] inline constexpr bool hold(value_type value_type) const noexcept;

    [[nodiscard]] inline constexpr bool is_null() const noexcept;
    [[nodiscard]] inline constexpr bool is_boolean() const noexcept;
    [[nodiscard]] inline constexpr bool is_number_signed() const noexcept;
    [[nodiscard]] inline constexpr bool is_number_unsigned() const noexcept;
    [[nodiscard]] inline constexpr bool is_number_integer() const noexcept;
    [[nodiscard]] inline constexpr bool is_number_big_inteager() const noexcept;
    [[nodiscard]] inline constexpr bool is_number_any_inteager() const noexcept;
    [[nodiscard]] inline constexpr bool is_number_float() const noexcept;
    [[nodiscard]] inline constexpr bool is_number() const noexcept;
    [[nodiscard]] inline constexpr bool is_string() const noexcept;
    [[nodiscard]] inline constexpr bool is_object() const noexcept;
    [[nodiscard]] inline constexpr bool is_array() const noexcept;
    [[nodiscard]] inline constexpr bool is_primitive() const noexcept;
    [[nodiscard]] inline constexpr bool is_structured() const noexcept;

    [[nodiscard]] inline constexpr size_t size() const noexcept;

    [[nodiscard]] inline std::string dump(
        int  indent                     = 4,
        bool ensure_ascii               = false,
        bool ignore_comments            = false,
        bool multi_line_comments_format = true,
        bool global_comments            = true
    ) const JSONC_EXCEPTION_TYPE;

    template <is_jsonc_type_convertible T>
    [[nodiscard]] inline JSONC_RESULT(T&) as() JSONC_EXCEPTION_TYPE;

    template <is_jsonc_type_convertible T>
    [[nodiscard]] inline JSONC_RESULT(const T&) as() const JSONC_EXCEPTION_TYPE;

    template <typename T>
        requires std::is_arithmetic_v<T>
    [[nodiscard]] inline JSONC_RESULT(T) get() const JSONC_EXCEPTION_TYPE;

    template <typename T>
        requires std::is_convertible_v<T, std::string>
    [[nodiscard]] inline JSONC_RESULT(T) get() const JSONC_EXCEPTION_TYPE;

    template <is_array_like T>
    [[nodiscard]] inline JSONC_RESULT(T) get() const JSONC_EXCEPTION_TYPE;

    template <is_object_like T>
    [[nodiscard]] inline JSONC_RESULT(T) get() const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(std::string) get_big_int_view() const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(std::string) get_any_int_view() const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(basic_jsonc&) operator[](std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const basic_jsonc&) operator[](std::string_view index) const JSONC_EXCEPTION_TYPE;

    template <size_t N>
    [[nodiscard]] inline JSONC_RESULT(basic_jsonc&) operator[](char const (&index)[N]) JSONC_EXCEPTION_TYPE {
        return operator[](std::string_view{index, N - 1});
    }
    template <size_t N>
    [[nodiscard]] inline JSONC_RESULT(const basic_jsonc&) operator[](char const (&index)[N]) const JSONC_EXCEPTION_TYPE {
        return operator[](std::string_view{index, N - 1});
    }

    [[nodiscard]] inline JSONC_RESULT(basic_jsonc&) operator[](size_t index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const basic_jsonc&) operator[](size_t index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(basic_jsonc&) at(std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(basic_jsonc&) at(std::string_view index, const basic_jsonc& default_value) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const basic_jsonc&) at(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(basic_jsonc&) at(size_t index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const basic_jsonc&) at(size_t index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(bool) contains(std::string_view index) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(bool) contains(std::string_view index, value_type type) const JSONC_EXCEPTION_TYPE;

    inline JSONC_RESULT(void) clear() JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(bool) empty() const JSONC_EXCEPTION_TYPE;

    inline JSONC_RESULT(bool) erase(std::string_view index) JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(bool) erase(size_t where) JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(bool) erase(size_t first, size_t last) JSONC_EXCEPTION_TYPE;

    inline JSONC_RESULT(void) push_back(const basic_jsonc& val) JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(void) push_back(basic_jsonc&& val) JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(const basic_jsonc&) front() const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(basic_jsonc&) front() JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(const basic_jsonc&) back() const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(basic_jsonc&) back() JSONC_EXCEPTION_TYPE;

    inline JSONC_RESULT(iterator_proxy) items() JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(const_iterator_proxy) items() const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline iterator begin() noexcept;
    [[nodiscard]] inline iterator end() noexcept;

    [[nodiscard]] inline const_iterator begin() const noexcept;
    [[nodiscard]] inline const_iterator end() const noexcept;

    [[nodiscard]] inline const_iterator cbegin() const noexcept;
    [[nodiscard]] inline const_iterator cend() const noexcept;

    [[nodiscard]] inline reverse_iterator rbegin() noexcept;
    [[nodiscard]] inline reverse_iterator rend() noexcept;

    [[nodiscard]] inline const_reverse_iterator rbegin() const noexcept;
    [[nodiscard]] inline const_reverse_iterator rend() const noexcept;

    [[nodiscard]] inline const_reverse_iterator crbegin() const noexcept;
    [[nodiscard]] inline const_reverse_iterator crend() const noexcept;

    inline void merge_patch(const basic_jsonc& other, bool merge_list = false) JSONC_EXCEPTION_TYPE;
    inline void merge_comments(const basic_jsonc& other) JSONC_EXCEPTION_TYPE;
    inline void move_comments_to_before() JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline bool operator==(const basic_jsonc& other) const JSONC_EXCEPTION_TYPE;

    template <typename T>
        requires std::is_arithmetic_v<T>
    [[nodiscard]] inline operator T() const JSONC_EXCEPTION_TYPE;

    template <typename T>
        requires std::is_convertible_v<T, std::string>
    [[nodiscard]] inline operator T() const JSONC_EXCEPTION_TYPE;

    template <is_array_like T>
    [[nodiscard]] inline operator T() const JSONC_EXCEPTION_TYPE;

    template <is_object_like T>
    [[nodiscard]] inline operator T() const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline constexpr bool has_before_comments() const noexcept;
    [[nodiscard]] inline constexpr bool has_after_comments() const noexcept;

    [[nodiscard]] inline std::vector<std::string>&       before_comments() noexcept;
    [[nodiscard]] inline const std::vector<std::string>& before_comments() const noexcept;

    [[nodiscard]] inline std::vector<std::string>&       after_comments() noexcept;
    [[nodiscard]] inline const std::vector<std::string>& after_comments() const noexcept;

    [[nodiscard]] inline std::vector<std::string> get_before_comments() const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline std::vector<std::string> get_after_comments() const JSONC_EXCEPTION_TYPE;

    inline void set_before_comments(const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE;
    inline void set_after_comments(const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE;

    inline void add_before_comment(std::string_view comment) JSONC_EXCEPTION_TYPE;
    inline void add_after_comment(std::string_view comment) JSONC_EXCEPTION_TYPE;

    inline void clear_before_comments() JSONC_EXCEPTION_TYPE;
    inline void clear_after_comments() JSONC_EXCEPTION_TYPE;

    inline bool remove_before_comment(size_t comment_index) JSONC_EXCEPTION_TYPE;
    inline bool remove_after_comment(size_t comment_index) JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline constexpr size_t before_comments_size() const noexcept;
    [[nodiscard]] inline constexpr size_t after_comments_size() const noexcept;

    [[nodiscard]] inline JSONC_RESULT(bool) has_key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(bool) has_key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(std::vector<std::string>&) key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const std::vector<std::string>&) key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(std::vector<std::string>&) key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const std::vector<std::string>&) key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(std::vector<std::string>) get_key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(std::vector<std::string>) get_key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(std::string) get_key_before_comment(std::string_view index, size_t comment_index) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(std::string) get_key_after_comment(std::string_view index, size_t comment_index) const JSONC_EXCEPTION_TYPE;

    inline JSONC_RESULT(bool) set_key_before_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(bool) set_key_after_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE;

    inline JSONC_RESULT(bool) add_key_before_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(bool) add_key_after_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(void) clear_key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(void) clear_key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE;

    inline JSONC_RESULT(bool) remove_key_before_comment(std::string_view index, size_t comment_index) JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(bool) remove_key_after_comment(std::string_view index, size_t comment_index) JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(size_t) key_before_comments_size(std::string_view index) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(size_t) key_after_comments_size(std::string_view index) const JSONC_EXCEPTION_TYPE;

public:
    inline static basic_jsonc object() JSONC_EXCEPTION_TYPE { return basic_object(); }
    inline static basic_jsonc object(std::initializer_list<std::pair<std::string, basic_jsonc>> val) JSONC_EXCEPTION_TYPE {
        return basic_object(val);
    }
    inline static basic_jsonc array() JSONC_EXCEPTION_TYPE { return basic_array(); }
    inline static basic_jsonc array(std::initializer_list<basic_jsonc> val) JSONC_EXCEPTION_TYPE { return basic_array(val); }

    inline static std::optional<basic_jsonc> from_big_int(std::string_view view) noexcept;

private:
    friend class basic_object;
    friend class basic_array;
    type_variant             storage_{};
    std::vector<std::string> before_comments_{};
    std::vector<std::string> after_comments_{};
};

} // namespace detail

} // namespace abi_v1_1_1

} // namespace jsonc