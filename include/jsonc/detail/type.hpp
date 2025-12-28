#pragma once
#include "container.hpp"
#include <cstdint>
#include <variant>
#include <vector>

namespace jsonc {

enum class ValueType : uint8_t {
    Null     = 0,
    Boolean  = 1,
    Signed   = 2,
    Unsigned = 3,
    Float    = 4,
    String   = 5,
    Object   = 6,
    Array    = 7,
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
    Object(std::initializer_list<std::pair<std::string, JsoncType>> val) JSONC_EXCEPTION_TYPE;

    [[nodiscard]] JsoncType& operator[](std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] JSONC_RESULT(const JsoncType&) operator[](std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] JSONC_RESULT(JsoncType&) at(std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] JsoncType& at(std::string_view index, const JsoncType& default_value) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] JSONC_RESULT(const JsoncType&) at(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] bool contains(std::string_view index) const noexcept;

    bool erase(std::string_view index) noexcept;

    void clear() noexcept;

    [[nodiscard]] size_t size() const noexcept;

    [[nodiscard]] std::string dump(int indent = 4, bool ensure_ascii = false, bool ignore_comments = false) const JSONC_EXCEPTION_TYPE;

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

    [[nodiscard]] bool has_key_before_comments(std::string_view index) const noexcept;
    [[nodiscard]] bool has_key_after_comments(std::string_view index) const noexcept;

    [[nodiscard]] std::vector<std::string>& key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] JSONC_RESULT(const std::vector<std::string>&) key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] std::vector<std::string>& key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] JSONC_RESULT(const std::vector<std::string>&) key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] JSONC_RESULT(std::vector<std::string>) get_key_before_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] JSONC_RESULT(std::vector<std::string>) get_key_after_comments(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] JSONC_RESULT(std::string) get_key_before_comments(std::string_view index, size_t comment_index) const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] JSONC_RESULT(std::string) get_key_after_comments(std::string_view index, size_t comment_index) const JSONC_EXCEPTION_TYPE;

    bool set_key_before_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE;
    bool set_key_after_comments(std::string_view index, const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE;

    bool add_key_before_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE;
    bool add_key_after_comment(std::string_view index, std::string_view comment) JSONC_EXCEPTION_TYPE;

    void clear_key_before_comments(std::string_view index) JSONC_EXCEPTION_TYPE;
    void clear_key_after_comments(std::string_view index) JSONC_EXCEPTION_TYPE;

    bool remove_key_before_comment(std::string_view index, size_t comment_index) JSONC_EXCEPTION_TYPE;
    bool remove_key_after_comment(std::string_view index, size_t comment_index) JSONC_EXCEPTION_TYPE;

    [[nodiscard]] size_t key_before_comments_size(std::string_view index) const noexcept;
    [[nodiscard]] size_t key_after_comments_size(std::string_view index) const noexcept;

private:
    struct Comments {
        std::vector<std::string> mBeforeComments{};
        std::vector<std::string> mAfterComments{};
    };
    friend class JsoncType;
    detail::OrderedStringHashMap<JsoncType> mStorage{};
    detail::StringHashMap<Comments>         mKeyComments{};
};

class Array {
public:
    using iterator               = std::vector<JsoncType>::iterator;
    using const_iterator         = std::vector<JsoncType>::const_iterator;
    using reverse_iterator       = std::vector<JsoncType>::reverse_iterator;
    using const_reverse_iterator = std::vector<JsoncType>::const_reverse_iterator;

public:
    Array() JSONC_EXCEPTION_TYPE = default;
    Array(std::initializer_list<JsoncType> val) JSONC_EXCEPTION_TYPE;

    [[nodiscard]] constexpr JsoncType& operator[](size_t index) noexcept;
    [[nodiscard]] constexpr JSONC_RESULT(const JsoncType&) operator[](size_t index) const noexcept;

    [[nodiscard]] constexpr JSONC_RESULT(JsoncType&) at(size_t index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] constexpr JSONC_RESULT(const JsoncType&) at(size_t index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] constexpr size_t size() const noexcept;

    void clear() noexcept;

    void push_back(const JsoncType& val) JSONC_EXCEPTION_TYPE;
    void push_back(JsoncType&& val) JSONC_EXCEPTION_TYPE;

    bool erase(size_t where);
    bool erase(size_t first, size_t last);

    iterator erase(const_iterator where);
    iterator erase(const_iterator first, const_iterator last);

    [[nodiscard]] std::string dump(int indent = 4, bool ensure_ascii = false, bool ignore_comments = false) const JSONC_EXCEPTION_TYPE;

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

private:
    friend class JsoncType;
    std::vector<JsoncType> mStorage{};
};

namespace detail {
using TypeVariant = std::variant<std::monostate, bool, int64_t, uint64_t, double, std::string, Object, Array>;
template <class T>
inline constexpr bool is_jsonc_type_convertible_v = [] {
    return []<std::size_t... I>(std::index_sequence<I...>) {
        return (std::is_convertible_v<std::variant_alternative_t<I, TypeVariant>, T> || ...);
    }(std::make_index_sequence<std::variant_size_v<TypeVariant>>{});
}();
template <typename T>
concept is_jsonc_type_convertible = is_jsonc_type_convertible_v<T>;
template <typename T>
concept is_range_loopable = (std::is_bounded_array_v<std::remove_cvref_t<T>> || requires(T t) {
    t.begin();
    t.end();
} || requires(T t) {
    begin(t);
    end(t);
}) && !std::is_constructible_v<std::string, T> ;
template <typename T>
concept is_array_like = is_range_loopable<T> && !requires { typename std::remove_cvref_t<T>::mapped_type; };
template <typename T>
concept is_object_like = is_range_loopable<T> && requires {
    typename std::remove_cvref_t<T>::key_type;
    typename std::remove_cvref_t<T>::mapped_type;
};
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
        IteratorType mIterator;

        template <bool Reserve>
        [[nodiscard]] static Iterator make_begin(auto& var) noexcept {
            Iterator result{};
            std::visit(
                [&](auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, Object>) {
                        if constexpr (Reserve) {
                            result.mIterator.template emplace<1>(val.rbegin());
                        } else {
                            result.mIterator.template emplace<1>(val.begin());
                        }
                    } else if constexpr (std::is_same_v<T, Array>) {
                        if constexpr (Reserve) {
                            result.mIterator.template emplace<2>(val.rbegin());
                        } else {
                            result.mIterator.template emplace<2>(val.begin());
                        }
                    } else if constexpr (std::is_same_v<T, std::monostate>) {
                        result.mIterator.template emplace<0>(std::addressof(var) + 1);
                    } else {
                        result.mIterator.template emplace<0>(std::addressof(var));
                    }
                },
                var.mStorage
            );
            return result;
        }

        template <bool Reserve>
        [[nodiscard]] static Iterator make_end(auto& var) noexcept {
            Iterator result{};
            std::visit(
                [&](auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, Object>) {
                        if constexpr (Reserve) {
                            result.mIterator.template emplace<1>(val.rend());
                        } else {
                            result.mIterator.template emplace<1>(val.end());
                        }
                    } else if constexpr (std::is_same_v<T, Array>) {
                        if constexpr (Reserve) {
                            result.mIterator.template emplace<2>(val.rend());
                        } else {
                            result.mIterator.template emplace<2>(val.end());
                        }
                    } else {
                        result.mIterator.template emplace<0>(std::addressof(var) + 1);
                    }
                },
                var.mStorage
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
                        std::conditional_t<_Reserve, Object::const_reverse_iterator, Object::const_iterator>,
                        std::conditional_t<_Reserve, Object::reverse_iterator, Object::iterator>>;
                    if constexpr (std::is_same_v<T, ObjectType>) {
                        return val->second;
                    } else {
                        return *val;
                    }
                },
                mIterator
            );
        }

        [[nodiscard]] pointer operator->() const noexcept { return std::addressof(**this); }

        Iterator& operator++() noexcept {
            std::visit([](auto& val) { ++val; }, mIterator);
            return *this;
        }

        Iterator operator++(int) noexcept {
            Iterator tmp = *this;
            ++*this;
            return tmp;
        }

        Iterator& operator--() noexcept {
            std::visit([](auto& val) { --val; }, mIterator);
            return *this;
        }

        Iterator operator--(int) noexcept {
            Iterator tmp = *this;
            --*this;
            return tmp;
        }

        [[nodiscard]] bool operator==(Iterator const& r) const noexcept { return this->mIterator == r.mIterator; }
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
    constexpr JsoncType(std::nullptr_t) JSONC_EXCEPTION_TYPE : mStorage(std::monostate()) {};

    constexpr JsoncType(bool val) JSONC_EXCEPTION_TYPE : mStorage(val) {};

    template <std::signed_integral T>
    constexpr JsoncType(T val) JSONC_EXCEPTION_TYPE : mStorage(static_cast<int64_t>(val)){};

    template <std::unsigned_integral T>
        requires(!std::same_as<T, bool>)
    constexpr JsoncType(T val) JSONC_EXCEPTION_TYPE : mStorage(static_cast<uint64_t>(val)){};

    constexpr JsoncType(std::string_view val) JSONC_EXCEPTION_TYPE : mStorage(std::string(val)) {};

    template <std::floating_point T>
    constexpr JsoncType(T val) JSONC_EXCEPTION_TYPE : mStorage(static_cast<double>(val)){};

    template <size_t N>
    [[nodiscard]] JsoncType(char const (&val)[N]) JSONC_EXCEPTION_TYPE : mStorage(std::string{val, N - 1}) {}

    constexpr JsoncType(const Object& val) JSONC_EXCEPTION_TYPE : mStorage(val) {};
    constexpr JsoncType(const Array& val) JSONC_EXCEPTION_TYPE : mStorage(val) {};

    constexpr JsoncType(std::initializer_list<std::pair<std::string, JsoncType>> val) JSONC_EXCEPTION_TYPE
    : mStorage(std::in_place_type<Object>, val) {}

    [[nodiscard]] constexpr ValueType        type() const noexcept;
    [[nodiscard]] constexpr std::string_view type_name() const noexcept;

    [[nodiscard]] constexpr bool hold(ValueType value_type) const noexcept;

    [[nodiscard]] constexpr bool is_null() const noexcept;
    [[nodiscard]] constexpr bool is_boolean() const noexcept;
    [[nodiscard]] constexpr bool is_number_signed() const noexcept;
    [[nodiscard]] constexpr bool is_number_unsigned() const noexcept;
    [[nodiscard]] constexpr bool is_number_integer() const noexcept;
    [[nodiscard]] constexpr bool is_number_float() const noexcept;
    [[nodiscard]] constexpr bool is_number() const noexcept;
    [[nodiscard]] constexpr bool is_string() const noexcept;
    [[nodiscard]] constexpr bool is_object() const noexcept;
    [[nodiscard]] constexpr bool is_array() const noexcept;
    [[nodiscard]] constexpr bool is_primitive() const noexcept;
    [[nodiscard]] constexpr bool is_structured() const noexcept;

    [[nodiscard]] std::string
    dump(int indent = 4, bool ensure_ascii = false, bool ignore_comments = false, bool global_comments = true) const JSONC_EXCEPTION_TYPE;

    template <detail::is_jsonc_type_convertible T>
    [[nodiscard]] JSONC_RESULT(T&) as() JSONC_EXCEPTION_TYPE;

    template <detail::is_jsonc_type_convertible T>
    [[nodiscard]] JSONC_RESULT(const T&) as() const JSONC_EXCEPTION_TYPE;

    template <typename T>
        requires std::is_arithmetic_v<T>
    [[nodiscard]] JSONC_RESULT(T) get() const JSONC_EXCEPTION_TYPE;

    template <typename T>
        requires std::is_convertible_v<T, std::string>
    [[nodiscard]] JSONC_RESULT(T) get() const JSONC_EXCEPTION_TYPE;

    template <detail::is_array_like T>
    [[nodiscard]] JSONC_RESULT(T) get() const JSONC_EXCEPTION_TYPE;

    template <detail::is_object_like T>
    [[nodiscard]] JSONC_RESULT(T) get() const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] JSONC_RESULT(JsoncType&) operator[](std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] JSONC_RESULT(const JsoncType&) operator[](std::string_view index) const JSONC_EXCEPTION_TYPE;

    template <size_t N>
    [[nodiscard]] JSONC_RESULT(JsoncType&) operator[](char const (&index)[N]) JSONC_EXCEPTION_TYPE {
        return operator[](std::string_view{index, N - 1});
    }
    template <size_t N>
    [[nodiscard]] JSONC_RESULT(const JsoncType&) operator[](char const (&index)[N]) const JSONC_EXCEPTION_TYPE {
        return operator[](std::string_view{index, N - 1});
    }

    [[nodiscard]] JSONC_RESULT(JsoncType&) operator[](size_t index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] JSONC_RESULT(const JsoncType&) operator[](size_t index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] JSONC_RESULT(JsoncType&) at(std::string_view index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] JSONC_RESULT(JsoncType&) at(std::string_view index, const JsoncType& default_value) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] JSONC_RESULT(const JsoncType&) at(std::string_view index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] JSONC_RESULT(JsoncType&) at(size_t index) JSONC_EXCEPTION_TYPE;
    [[nodiscard]] JSONC_RESULT(const JsoncType&) at(size_t index) const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] JSONC_RESULT(bool) contains(std::string_view index) JSONC_EXCEPTION_TYPE;

    JSONC_RESULT(void) clear() JSONC_EXCEPTION_TYPE;

    JSONC_RESULT(bool) erase(std::string_view index) JSONC_EXCEPTION_TYPE;
    JSONC_RESULT(bool) erase(size_t where);
    JSONC_RESULT(bool) erase(size_t first, size_t last);

    JSONC_RESULT(void) push_back(const JsoncType& val) JSONC_EXCEPTION_TYPE;
    JSONC_RESULT(void) push_back(JsoncType&& val) JSONC_EXCEPTION_TYPE;

    JSONC_RESULT(IteratorProxy) items() JSONC_EXCEPTION_TYPE;
    JSONC_RESULT(IteratorProxyConst) items() const JSONC_EXCEPTION_TYPE;

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

    template <typename T>
        requires std::is_arithmetic_v<T>
    [[nodiscard]] operator T() const JSONC_EXCEPTION_TYPE;

    template <typename T>
        requires std::is_convertible_v<T, std::string>
    [[nodiscard]] operator T() const JSONC_EXCEPTION_TYPE;

    template <detail::is_array_like T>
    [[nodiscard]] operator T() const JSONC_EXCEPTION_TYPE;

    template <detail::is_object_like T>
    [[nodiscard]] operator T() const JSONC_EXCEPTION_TYPE;

    [[nodiscard]] constexpr bool has_before_comments() const noexcept;
    [[nodiscard]] constexpr bool has_after_comments() const noexcept;

    [[nodiscard]] std::vector<std::string>&       before_comments() noexcept;
    [[nodiscard]] const std::vector<std::string>& before_comments() const noexcept;

    [[nodiscard]] std::vector<std::string>&       after_comments() noexcept;
    [[nodiscard]] const std::vector<std::string>& after_comments() const noexcept;

    [[nodiscard]] std::vector<std::string> get_before_comments() const JSONC_EXCEPTION_TYPE;
    [[nodiscard]] std::vector<std::string> get_after_comments() const JSONC_EXCEPTION_TYPE;

    void set_before_comments(const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE;
    void set_after_comments(const std::vector<std::string>& comments) JSONC_EXCEPTION_TYPE;

    void add_before_comment(std::string_view comment) JSONC_EXCEPTION_TYPE;
    void add_after_comment(std::string_view comment) JSONC_EXCEPTION_TYPE;

    void clear_before_comments() JSONC_EXCEPTION_TYPE;
    void clear_after_comments() JSONC_EXCEPTION_TYPE;

    bool remove_before_comment(size_t comment_index) JSONC_EXCEPTION_TYPE;
    bool remove_after_comment(size_t comment_index) JSONC_EXCEPTION_TYPE;

    [[nodiscard]] constexpr size_t before_comments_size() const noexcept;
    [[nodiscard]] constexpr size_t after_comments_size() const noexcept;

private:
    detail::TypeVariant      mStorage{};
    std::vector<std::string> mBeforeComments{};
    std::vector<std::string> mAfterComments{};
};

} // namespace jsonc
