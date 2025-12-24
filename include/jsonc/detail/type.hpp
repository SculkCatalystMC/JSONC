#pragma once
#include "container.hpp"
#include <cstdint>
#include <map>
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
    friend class JsoncType;

private:
    struct Comments {
        std::vector<std::string> mBeforeComments;
        std::vector<std::string> mAfterComments;
    };

    detail::StringHashMap<JsoncType> mStorage;
    detail::StringHashMap<Comments>  mKeyComments;
    std::map<size_t, std::string>    mInsertIndex;

public:
    using iterator             = detail::StringHashMap<JsoncType>::iterator;
    using const_iterator       = detail::StringHashMap<JsoncType>::const_iterator;
    using local_iterator       = detail::StringHashMap<JsoncType>::local_iterator;
    using const_local_iterator = detail::StringHashMap<JsoncType>::const_local_iterator;

public:
    [[nodiscard]] JsoncType& operator[](std::string_view index);
    [[nodiscard]] JSONC_RESULT(const JsoncType&) operator[](std::string_view index) const;

    [[nodiscard]] JSONC_RESULT(JsoncType&) at(std::string_view index);
    [[nodiscard]] JsoncType& at(std::string_view index, JsoncType const& default_value);
    [[nodiscard]] JSONC_RESULT(const JsoncType&) at(std::string_view index) const;

    [[nodiscard]] bool contains(std::string_view index) const noexcept;

    [[nodiscard]] size_t size() const noexcept;

    [[nodiscard]] iterator begin() noexcept;
    [[nodiscard]] iterator end() noexcept;

    [[nodiscard]] const_iterator begin() const noexcept;
    [[nodiscard]] const_iterator end() const noexcept;

    [[nodiscard]] const_iterator cbegin() const noexcept;
    [[nodiscard]] const_iterator cend() const noexcept;

    [[nodiscard]] bool has_key_before_comments(std::string_view index) const noexcept;
    [[nodiscard]] bool has_key_after_comments(std::string_view index) const noexcept;

    [[nodiscard]] std::vector<std::string>& key_before_comments(std::string_view index);
    [[nodiscard]] JSONC_RESULT(const std::vector<std::string>&) key_before_comments(std::string_view index) const;

    [[nodiscard]] std::vector<std::string>& key_after_comments(std::string_view index);
    [[nodiscard]] JSONC_RESULT(const std::vector<std::string>&) key_after_comments(std::string_view index) const;

    [[nodiscard]] JSONC_RESULT(std::vector<std::string>) get_key_before_comments(std::string_view index) const;
    [[nodiscard]] JSONC_RESULT(std::vector<std::string>) get_key_after_comments(std::string_view index) const;

    [[nodiscard]] JSONC_RESULT(std::string) get_key_before_comments(std::string_view index, size_t comment_index) const;
    [[nodiscard]] JSONC_RESULT(std::string) get_key_after_comments(std::string_view index, size_t comment_index) const;

    bool set_key_before_comments(std::string_view index, std::vector<std::string> const& comments);
    bool set_key_after_comments(std::string_view index, std::vector<std::string> const& comments);

    bool add_key_before_comment(std::string_view index, std::string_view comment);
    bool add_key_after_comment(std::string_view index, std::string_view comment);

    void clear_key_before_comments(std::string_view index);
    void clear_key_after_comments(std::string_view index);

    bool remove_key_before_comment(std::string_view index, size_t comment_index);
    bool remove_key_after_comment(std::string_view index, size_t comment_index);

    [[nodiscard]] size_t key_before_comments_size(std::string_view index) const noexcept;
    [[nodiscard]] size_t key_after_comments_size(std::string_view index) const noexcept;
};

class Array {
    friend class JsoncType;

private:
    std::vector<JsoncType> mStorage;

public:
    using iterator               = std::vector<JsoncType>::iterator;
    using const_iterator         = std::vector<JsoncType>::const_iterator;
    using reverse_iterator       = std::vector<JsoncType>::reverse_iterator;
    using const_reverse_iterator = std::vector<JsoncType>::const_reverse_iterator;

public:
    [[nodiscard]] constexpr JsoncType& operator[](size_t index) noexcept;
    [[nodiscard]] constexpr JSONC_RESULT(const JsoncType&) operator[](size_t index) const noexcept;

    [[nodiscard]] constexpr JSONC_RESULT(JsoncType&) at(size_t index);
    [[nodiscard]] constexpr JSONC_RESULT(const JsoncType&) at(size_t index) const;

    [[nodiscard]] constexpr size_t size() const noexcept;

    void push_back(JsoncType const& val);
    void push_back(JsoncType&& val);

    [[nodiscard]] iterator begin() noexcept;
    [[nodiscard]] iterator end() noexcept;

    [[nodiscard]] const_iterator begin() const noexcept;
    [[nodiscard]] const_iterator end() const noexcept;

    [[nodiscard]] const_iterator cbegin() const noexcept;
    [[nodiscard]] const_iterator cend() const noexcept;

    [[nodiscard]] reverse_iterator rbegin() noexcept;
    [[nodiscard]] reverse_iterator rend() noexcept;

    [[nodiscard]] const_reverse_iterator crbegin() const noexcept;
    [[nodiscard]] const_reverse_iterator crend() const noexcept;
};

namespace detail {
using TypeVariant = std::variant<std::monostate, bool, int64_t, uint64_t, double, std::string, Object, Array>;
template <class T>
inline constexpr bool jsonc_type_convertible_v = [] {
    return []<std::size_t... I>(std::index_sequence<I...>) {
        return (std::is_convertible_v<std::variant_alternative_t<I, TypeVariant>, T> || ...);
    }(std::make_index_sequence<std::variant_size_v<TypeVariant>>{});
}();
} // namespace detail

class JsoncType {
private:
    detail::TypeVariant      mStorage;
    std::vector<std::string> mBeforeComments;
    std::vector<std::string> mAfterComments;

public:
    JsoncType() = default;
    constexpr JsoncType(std::nullptr_t) : mStorage(std::monostate()) {};
    constexpr JsoncType(bool val) : mStorage(val) {};

    template <std::signed_integral T>
    constexpr JsoncType(T val) : mStorage(static_cast<int64_t>(val)){};
    template <std::unsigned_integral T>
        requires(!std::same_as<T, bool>)
    constexpr JsoncType(T val) : mStorage(static_cast<uint64_t>(val)){};
    constexpr JsoncType(std::string val) : mStorage(std::move(val)) {};
    template <std::floating_point T>
    constexpr JsoncType(T val) : mStorage(static_cast<double>(val)){};

    template <size_t N>
    [[nodiscard]] JsoncType(char const (&val)[N]) : mStorage(std::string{val, N - 1}) {}

    constexpr JsoncType(Object const& val) : mStorage(val) {};
    constexpr JsoncType(Array const& val) : mStorage(val) {};

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

    [[nodiscard]] std::string dump(int indent = 4, bool ensure_ascii = false) const;

    template <typename T>
        requires detail::jsonc_type_convertible_v<T>
    [[nodiscard]] JSONC_RESULT(T) get() const;

    [[nodiscard]] JSONC_RESULT(JsoncType&) operator[](std::string_view index);
    [[nodiscard]] JSONC_RESULT(const JsoncType&) operator[](std::string_view index) const;

    [[nodiscard]] JSONC_RESULT(JsoncType&) operator[](size_t index);
    [[nodiscard]] JSONC_RESULT(const JsoncType&) operator[](size_t index) const;

    [[nodiscard]] JSONC_RESULT(JsoncType&) at(std::string_view index);
    [[nodiscard]] JSONC_RESULT(JsoncType&) at(std::string_view index, JsoncType const& default_value);
    [[nodiscard]] JSONC_RESULT(const JsoncType&) at(std::string_view index) const;

    [[nodiscard]] JSONC_RESULT(JsoncType&) at(size_t index);
    [[nodiscard]] JSONC_RESULT(const JsoncType&) at(size_t index) const;

    [[nodiscard]] constexpr bool has_before_comments() const noexcept;
    [[nodiscard]] constexpr bool has_after_comments() const noexcept;

    [[nodiscard]] std::vector<std::string>&       before_comments() noexcept;
    [[nodiscard]] const std::vector<std::string>& before_comments() const noexcept;

    [[nodiscard]] std::vector<std::string>&       after_comments() noexcept;
    [[nodiscard]] const std::vector<std::string>& after_comments() const noexcept;

    [[nodiscard]] std::vector<std::string> get_before_comments() const;
    [[nodiscard]] std::vector<std::string> get_after_comments() const;

    void set_before_comments(std::vector<std::string> const& comments);
    void set_after_comments(std::vector<std::string> const& comments);

    void add_before_comment(std::string_view comment);
    void add_after_comment(std::string_view comment);

    void clear_before_comments();
    void clear_after_comments();

    bool remove_before_comment(size_t comment_index);
    bool remove_after_comment(size_t comment_index);

    [[nodiscard]] constexpr size_t before_comments_size() const noexcept;
    [[nodiscard]] constexpr size_t after_comments_size() const noexcept;
};

} // namespace jsonc