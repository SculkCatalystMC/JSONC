#pragma once
#include "container.hpp"
#include <cstdint>
#include <variant>
#include <vector>

namespace jsonc {

namespace detail {
struct BigInt {
    BigInt() noexcept = default;
    BigInt(std::string_view val) noexcept : view_(val) {}
    bool        operator==(const BigInt& other) const noexcept { return view_ == other.view_; }
    std::string view_;
};
} // namespace detail

enum class ValueType : uint8_t {
    Null     = 0,
    Boolean  = 1,
    Signed   = 2,
    Unsigned = 3,
    Float    = 4,
    String   = 5,
    Object   = 6,
    Array    = 7,
    BigInt   = 8,
};

class JsoncType;

class Object {
public:
    using iterator               = detail::OrderedStringHashMap<JsoncType>::iterator;
    using const_iterator         = detail::OrderedStringHashMap<JsoncType>::const_iterator;
    using reverse_iterator       = detail::OrderedStringHashMap<JsoncType>::reverse_iterator;
    using const_reverse_iterator = detail::OrderedStringHashMap<JsoncType>::const_reverse_iterator;

public:
    Object() JSONC_EXCEPTION_TYPE = default;
    inline Object(std::initializer_list<std::pair<std::string, JsoncType>> val) JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JsoncType& operator[](std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const JsoncType&) operator[](std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(JsoncType&) at(std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JsoncType& at(std::string_view index, const JsoncType& default_value) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const JsoncType&) at(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline bool contains(std::string_view index) const noexcept;
    [[nodiscard]] inline bool contains(std::string_view index, ValueType type) const noexcept;

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

    inline void merge_patch(const Object& other, bool merge_list = false) JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(void) merge_patch(const JsoncType& other, bool merge_list = false) JSONC_EXCEPTION_TYPE;

    inline void merge_comments(const Object& other) JSONC_EXCEPTION_TYPE;
    inline void move_comments_to_before() JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline bool operator==(const Object& other) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline bool operator==(const JsoncType& other) const JSONC_EXCEPTION_TYPE;

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
    struct Comments {
        std::vector<std::string> before_comments_{};
        std::vector<std::string> after_comments_{};
    };
    friend class JsoncType;
    detail::OrderedStringHashMap<JsoncType> storage_{};
    detail::StringHashMap<Comments>         key_comments_{};
};

class Array {
public:
    using iterator               = std::vector<JsoncType>::iterator;
    using const_iterator         = std::vector<JsoncType>::const_iterator;
    using reverse_iterator       = std::vector<JsoncType>::reverse_iterator;
    using const_reverse_iterator = std::vector<JsoncType>::const_reverse_iterator;

public:
    Array() JSONC_EXCEPTION_TYPE = default;
    inline Array(std::initializer_list<JsoncType> val) JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline constexpr JsoncType& operator[](size_t index) noexcept;
    [[nodiscard]] inline constexpr JSONC_RESULT(const JsoncType&) operator[](size_t index) const noexcept;

    [[nodiscard]] inline constexpr JSONC_RESULT(JsoncType&) at(size_t index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline constexpr JSONC_RESULT(const JsoncType&) at(size_t index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline constexpr size_t size() const noexcept;

    inline void clear() noexcept;

    [[nodiscard]] inline constexpr bool empty() const noexcept;

    inline void push_back(const JsoncType& val) JSONC_EXCEPTION_TYPE;
    inline void push_back(JsoncType&& val) JSONC_EXCEPTION_TYPE;

    inline bool erase(size_t where);
    inline bool erase(size_t first, size_t last);

    [[nodiscard]] inline const JsoncType& front() const noexcept;
    [[nodiscard]] inline JsoncType&       front() noexcept;

    [[nodiscard]] inline const JsoncType& back() const noexcept;
    [[nodiscard]] inline JsoncType&       back() noexcept;

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

    inline void merge_patch(const Array& other) JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(void) merge_patch(const JsoncType& other) JSONC_EXCEPTION_TYPE;

    inline void merge_comments(const Array& other) JSONC_EXCEPTION_TYPE;
    inline void move_comments_to_before() JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline bool operator==(const Array& other) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline bool operator==(const JsoncType& other) const JSONC_EXCEPTION_TYPE;

private:
    friend class JsoncType;
    std::vector<JsoncType> storage_{};
};

namespace detail {
using TypeVariant = std::variant<std::monostate, bool, int64_t, uint64_t, double, std::string, Object, Array, BigInt>;
template <class T>
inline constexpr bool is_jsonc_type_convertible_v = [] {
    return []<size_t... I>(std::index_sequence<I...>) {
        return (std::is_convertible_v<std::variant_alternative_t<I, TypeVariant>, T> || ...);
    }(std::make_index_sequence<std::variant_size_v<TypeVariant>>{});
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
} // namespace detail

class JsoncType {
public:
    template <bool _Const, bool _Reserve>
    class Iterator {
    public:
        using reference = std::conditional_t<_Const, const JsoncType, JsoncType>&;
        using pointer   = std::add_pointer_t<reference>;

    private:
        friend class JsoncType;
        using IteratorType = std::conditional_t<
            _Const,
            std::variant<
                const JsoncType*,
                std::conditional_t<_Reserve, Object::const_reverse_iterator, Object::const_iterator>,
                std::conditional_t<_Reserve, Array::const_reverse_iterator, Array::const_iterator>>,
            std::variant<
                JsoncType*,
                std::conditional_t<_Reserve, Object::reverse_iterator, Object::iterator>,
                std::conditional_t<_Reserve, Array::reverse_iterator, Array::iterator>>>;
        IteratorType iterator_;

        template <bool Reserve>
        [[nodiscard]] inline static Iterator make_begin(auto& var) noexcept {
            Iterator result{};
            std::visit(
                [&](auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, Object>) {
                        if constexpr (Reserve) {
                            result.iterator_.template emplace<1>(val.rbegin());
                        } else {
                            result.iterator_.template emplace<1>(val.begin());
                        }
                    } else if constexpr (std::is_same_v<T, Array>) {
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
        [[nodiscard]] inline static Iterator make_end(auto& var) noexcept {
            Iterator result{};
            std::visit(
                [&](auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, Object>) {
                        if constexpr (Reserve) {
                            result.iterator_.template emplace<1>(val.rend());
                        } else {
                            result.iterator_.template emplace<1>(val.end());
                        }
                    } else if constexpr (std::is_same_v<T, Array>) {
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
        [[nodiscard]] inline reference operator*() const noexcept {
            return std::visit(
                [](auto& val) -> reference {
                    using T          = std::decay_t<decltype(val)>;
                    using ObjectType = std::conditional_t<
                        _Const,
                        std::conditional_t<_Reserve, Object::const_reverse_iterator, Object::const_iterator>,
                        std::conditional_t<_Reserve, Object::reverse_iterator, Object::iterator>>;
                    if constexpr (std::is_same_v<T, ObjectType>) {
                        return val->second;
                    } else {
                        return *val;
                    }
                },
                iterator_
            );
        }

        [[nodiscard]] inline pointer operator->() const noexcept { return std::addressof(**this); }

        inline Iterator& operator++() noexcept {
            std::visit([](auto& val) { ++val; }, iterator_);
            return *this;
        }

        inline Iterator operator++(int) noexcept {
            Iterator tmp = *this;
            ++*this;
            return tmp;
        }

        inline Iterator& operator--() noexcept {
            std::visit([](auto& val) { --val; }, iterator_);
            return *this;
        }

        inline Iterator operator--(int) noexcept {
            Iterator tmp = *this;
            --*this;
            return tmp;
        }

        [[nodiscard]] inline bool operator==(Iterator const& r) const noexcept { return this->iterator_ == r.iterator_; }
    };

    class IteratorProxy {
    public:
        Object::iterator begin() noexcept { return mSelf.begin(); }
        Object::iterator end() noexcept { return mSelf.end(); }

    private:
        friend class JsoncType;
        Object& mSelf;
        IteratorProxy(Object& self) : mSelf(self) {}
    };

    class IteratorProxyConst {
    public:
        Object::const_iterator begin() const noexcept { return mSelf.begin(); }
        Object::const_iterator end() const noexcept { return mSelf.end(); }

    private:
        friend class JsoncType;
        const Object& mSelf;
        IteratorProxyConst(const Object& self) : mSelf(self) {}
    };

public:
    using iterator               = Iterator<false, false>;
    using const_iterator         = Iterator<true, false>;
    using reverse_iterator       = Iterator<false, true>;
    using const_reverse_iterator = Iterator<true, true>;

public:
    JsoncType() = default;

    inline constexpr JsoncType(ValueType type) noexcept { detail::emplace_variant(storage_, static_cast<size_t>(type)); };

    inline constexpr JsoncType(std::nullptr_t) noexcept : storage_(std::monostate()) {};

    inline constexpr JsoncType(bool val) noexcept : storage_(val) {};

    template <std::signed_integral T>
    inline constexpr JsoncType(T val) noexcept : storage_(static_cast<int64_t>(val)){};

    template <std::unsigned_integral T>
        requires(!std::same_as<T, bool>)
    inline constexpr JsoncType(T val) noexcept : storage_(static_cast<uint64_t>(val)){};

    inline constexpr JsoncType(std::string_view val) noexcept : storage_(std::string(val)) {};
    inline constexpr JsoncType(const std::string& val) noexcept : storage_(val) {};

    inline constexpr JsoncType(double val) noexcept : storage_(val) {};
    inline constexpr JsoncType(float val) noexcept : storage_(std::round(val * 1e6) / 1e6) {};

    template <size_t N>
    [[nodiscard]] inline JsoncType(char const (&val)[N]) noexcept : storage_(std::string{val, N - 1}) {}

    inline constexpr JsoncType(const Object& val) noexcept : storage_(val) {};
    inline constexpr JsoncType(const Array& val) noexcept : storage_(val) {};
    inline constexpr JsoncType(const detail::BigInt& val) noexcept : storage_(val) {};

    inline constexpr JsoncType(std::initializer_list<std::pair<std::string, JsoncType>> val) noexcept : storage_(std::in_place_type<Object>, val) {}

    inline constexpr void emplace(ValueType type) noexcept { detail::emplace_variant(storage_, static_cast<size_t>(type)); }

    [[nodiscard]] inline constexpr ValueType        type() const noexcept;
    [[nodiscard]] inline constexpr std::string_view type_name() const noexcept;

    [[nodiscard]] inline constexpr bool hold(ValueType value_type) const noexcept;

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

    template <detail::is_jsonc_type_convertible T>
    [[nodiscard]] inline JSONC_RESULT(T&) as() JSONC_EXCEPTION_TYPE;

    template <detail::is_jsonc_type_convertible T>
    [[nodiscard]] inline JSONC_RESULT(const T&) as() const JSONC_EXCEPTION_TYPE;

    template <typename T>
        requires std::is_arithmetic_v<T>
    [[nodiscard]] inline JSONC_RESULT(T) get() const JSONC_EXCEPTION_TYPE;

    template <typename T>
        requires std::is_convertible_v<T, std::string>
    [[nodiscard]] inline JSONC_RESULT(T) get() const JSONC_EXCEPTION_TYPE;

    template <detail::is_array_like T>
    [[nodiscard]] inline JSONC_RESULT(T) get() const JSONC_EXCEPTION_TYPE;

    template <detail::is_object_like T>
    [[nodiscard]] inline JSONC_RESULT(T) get() const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(std::string) get_big_int_view() const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(std::string) get_any_int_view() const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(JsoncType&) operator[](std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const JsoncType&) operator[](std::string_view index) const JSONC_EXCEPTION_TYPE;

    template <size_t N>
    [[nodiscard]] inline JSONC_RESULT(JsoncType&) operator[](char const (&index)[N]) JSONC_EXCEPTION_TYPE {
        return operator[](std::string_view{index, N - 1});
    }
    template <size_t N>
    [[nodiscard]] inline JSONC_RESULT(const JsoncType&) operator[](char const (&index)[N]) const JSONC_EXCEPTION_TYPE {
        return operator[](std::string_view{index, N - 1});
    }

    [[nodiscard]] inline JSONC_RESULT(JsoncType&) operator[](size_t index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const JsoncType&) operator[](size_t index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(JsoncType&) at(std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(JsoncType&) at(std::string_view index, const JsoncType& default_value) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const JsoncType&) at(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(JsoncType&) at(size_t index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(const JsoncType&) at(size_t index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(bool) contains(std::string_view index) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(bool) contains(std::string_view index, ValueType type) const JSONC_EXCEPTION_TYPE;

    JSONC_RESULT(void) clear() JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(bool) empty() const JSONC_EXCEPTION_TYPE;

    inline JSONC_RESULT(bool) erase(std::string_view index) JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(bool) erase(size_t where) JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(bool) erase(size_t first, size_t last) JSONC_EXCEPTION_TYPE;

    inline JSONC_RESULT(void) push_back(const JsoncType& val) JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(void) push_back(JsoncType&& val) JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(const JsoncType&) front() const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(JsoncType&) front() JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline JSONC_RESULT(const JsoncType&) back() const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline JSONC_RESULT(JsoncType&) back() JSONC_EXCEPTION_TYPE;

    inline JSONC_RESULT(IteratorProxy) items() JSONC_EXCEPTION_TYPE;
    inline JSONC_RESULT(IteratorProxyConst) items() const JSONC_EXCEPTION_TYPE;

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

    inline void merge_patch(const JsoncType& other, bool merge_list = false) JSONC_EXCEPTION_TYPE;
    inline void merge_comments(const JsoncType& other) JSONC_EXCEPTION_TYPE;
    inline void move_comments_to_before() JSONC_EXCEPTION_TYPE;

    [[nodiscard]] inline bool operator==(const JsoncType& other) const JSONC_EXCEPTION_TYPE;

    template <typename T>
        requires std::is_arithmetic_v<T>
    [[nodiscard]] inline operator T() const JSONC_EXCEPTION_TYPE;

    template <typename T>
        requires std::is_convertible_v<T, std::string>
    [[nodiscard]] inline operator T() const JSONC_EXCEPTION_TYPE;

    template <detail::is_array_like T>
    [[nodiscard]] inline operator T() const JSONC_EXCEPTION_TYPE;

    template <detail::is_object_like T>
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

    [[nodiscard]] inline bool has_key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline bool has_key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;

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

    [[nodiscard]] inline size_t key_before_comments_size(std::string_view index) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] inline size_t key_after_comments_size(std::string_view index) const JSONC_EXCEPTION_TYPE;

public:
    inline static JsoncType object() JSONC_EXCEPTION_TYPE { return Object(); }
    inline static JsoncType array() JSONC_EXCEPTION_TYPE { return Array(); }

    inline static std::optional<JsoncType> from_big_int(std::string_view view) noexcept;

private:
    friend class Object;
    friend class Array;
    detail::TypeVariant      storage_{};
    std::vector<std::string> before_comments_{};
    std::vector<std::string> after_comments_{};
};

} // namespace jsonc
