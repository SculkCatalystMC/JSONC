#pragma once
#include "exception.hpp"
#include "type.hpp"
#include <charconv>

namespace jsonc::inline abi_v1_1_0::detail {

JSONC_PARSE_RESULT(basic_jsonc) parse_jsonc_type(std::string_view& str, std::vector<std::string>&& comments_before, bool allow_trailing_comma, bool ignore_comments) JSONC_EXCEPTION_TYPE;
JSONC_PARSE_RESULT(basic_jsonc) parse_jsonc_type(std::string_view& str, bool allow_trailing_comma, bool ignore_comments) JSONC_EXCEPTION_TYPE;

namespace {

inline JSONC_PARSE_RESULT(std::string_view) extract_comment(std::string_view& s) JSONC_EXCEPTION_TYPE {
    size_t           i = 0;
    std::string_view result;
    switch (s[i++]) {
    case '*': {
        while (i < s.size()) {
            switch (s[i++]) {
            case static_cast<char>(std::char_traits<char>::eof()):
            case '\0':
                _JSONC_PARSE_ERROR("unexpected eof encountered");
            case '*': {
                switch (s[i]) {
                case '/':
                    result = s.substr(1, i - 2);
                    s.remove_prefix(std::min(i + 1, s.size()));
                    return result;
                default:
                    continue;
                }
            }
            default:
                continue;
            }
        }
        break;
    }
    case '/': {
        while (i < s.size()) {
            switch (s[i++]) {
            case static_cast<char>(std::char_traits<char>::eof()):
            case '\0':
                _JSONC_PARSE_ERROR("unexpected eof encountered");
            case '\r':
            case '\n': {
                result = s.substr(1, i - 2);
                s.remove_prefix(std::min(i, s.size()));
                return result;
            }
            default:
                continue;
            }
        }
        if (i == s.size()) {
            result = s.substr(1);
            s.remove_prefix(s.size());
            return result;
        }
        break;
    }
    default: {
        while (i < s.size()) {
            switch (s[i++]) {
            case static_cast<char>(std::char_traits<char>::eof()):
            case '\0':
                s.remove_prefix(std::min(i, s.size()));
                _JSONC_PARSE_ERROR("unexpected eof encountered");
            default:
                break;
            }
        }
        break;
    }
    }
    _JSONC_PARSE_ERROR("invalid comment format");
}

inline void skip_spaces(std::string_view& s) noexcept {
    size_t i = 0;
    while (i <= s.size() && std::isspace(s[i++])) {}
    s.remove_prefix(std::min(i - 1, s.size()));
}

inline void skip_spaces_nolinefeed(std::string_view& s) noexcept {
    size_t      i                  = 0;
    static auto isspace_nolinefeed = [](char c) { return c == ' ' || c == '\t' || c == '\v' || c == '\f'; };
    while (i <= s.size() && (isspace_nolinefeed(s[i++]))) {}
    s.remove_prefix(std::min(i - 1, s.size()));
}

inline std::vector<std::string> parse_comments(std::string_view comment) JSONC_EXCEPTION_TYPE {
    std::vector<std::string> result{};
    size_t                   pos = 0;
    while ((pos = comment.find('\n')) != std::string::npos) {
        if (pos != 0) {
            auto line = comment.substr(0, pos);
            while (!line.empty() && std::isspace(line.front())) { line.remove_prefix(1); }
            if (!line.empty() && line.starts_with('*')) { line.remove_prefix(1); }
            if (!line.empty() && std::isspace(line.front())) { line.remove_prefix(1); }
            if (!line.empty()) { result.emplace_back(line); }
        }
        comment.remove_prefix(pos + 1);
    }
    while (!comment.empty() && std::isspace(comment.front())) { comment.remove_prefix(1); }
    if (!comment.empty()) { result.emplace_back(comment); }
    return result;
}

inline bool extract_comments(std::string_view& s, std::vector<std::string>& comments, bool ignore_comments) JSONC_EXCEPTION_TYPE {
    skip_spaces(s);
    while (s.starts_with('/')) {
        s.remove_prefix(1);
        auto comment = extract_comment(s);
#ifdef JSONC_NO_EXCEPTION
        if (!comment) { return false; }
        if (!ignore_comments) { comments.append_range(parse_comments(*comment)); }
#else
        if (!ignore_comments) { comments.append_range(parse_comments(comment)); }
#endif
        skip_spaces(s);
    }
    return true;
}

inline bool extarct_comma_back_comments(std::string_view& s, std::vector<std::string>& comments, bool ignore_comments) JSONC_EXCEPTION_TYPE {
    skip_spaces_nolinefeed(s);
    if (s.starts_with('/')) {
        s.remove_prefix(1);
        auto comment = extract_comment(s);
#ifdef JSONC_NO_EXCEPTION
        if (!comment) { return false; }
        if (!ignore_comments) { comments.append_range(parse_comments(*comment)); }
#else
        if (!ignore_comments) { comments.append_range(parse_comments(comment)); }
#endif
    }
    return true;
}

inline constexpr char get_current_char(std::string_view& s) JSONC_EXCEPTION_TYPE {
    if (s.empty()) { return '\0'; }
    char c = s.front();
    s.remove_prefix(1);
    return c;
}

inline JSONC_PARSE_RESULT(
    basic_jsonc
) parse_null(std::string_view& str, std::vector<std::string>&& comments_before, bool ignore_comments) JSONC_EXCEPTION_TYPE {
    if (str.starts_with("null")) {
        str.remove_prefix(4);
        basic_jsonc              result{nullptr};
        std::vector<std::string> comments_after{};
        if (!extract_comments(str, comments_after, ignore_comments)) { _JSONC_PARSE_ERROR("invalid comments format"); }
        result.before_comments() = std::move(comments_before);
        result.after_comments()  = std::move(comments_after);
        return result;
    }
    _JSONC_PARSE_ERROR("invalid value");
}

template <bool Value>
inline JSONC_PARSE_RESULT(
    basic_jsonc
) parse_boolean(std::string_view& str, std::vector<std::string>&& comments_before, bool ignore_comments) JSONC_EXCEPTION_TYPE {
    if constexpr (Value == true) {
        if (str.starts_with("true")) {
            str.remove_prefix(4);
            basic_jsonc              result{true};
            std::vector<std::string> comments_after{};
            if (!extract_comments(str, comments_after, ignore_comments)) { _JSONC_PARSE_ERROR("invalid comments format"); }
            result.before_comments() = std::move(comments_before);
            result.after_comments()  = std::move(comments_after);
            return result;
        }
    } else {
        if (str.starts_with("false")) {
            str.remove_prefix(5);
            basic_jsonc              result{false};
            std::vector<std::string> comments_after{};
            if (!extract_comments(str, comments_after, ignore_comments)) { _JSONC_PARSE_ERROR("invalid comments format"); }
            result.before_comments() = std::move(comments_before);
            result.after_comments()  = std::move(comments_after);
            return result;
        }
    }
    _JSONC_PARSE_ERROR("invalid value");
}

inline JSONC_PARSE_RESULT(
    basic_jsonc
) parse_number(std::string_view& str, std::vector<std::string>&& comments_before, bool ignore_comments) JSONC_EXCEPTION_TYPE {
    size_t length = 0;
    bool   is_int = true;
    if (str.empty()) { _JSONC_PARSE_ERROR("illegal escape"); }

    auto it    = str.begin();
    auto end   = str.end();
    auto start = it;

    bool negative = false;

    if (*it == '-') {
        negative = true;
        ++it;
    }

    while (it != end && std::isdigit(*it)) { ++it; }

    if (it != end && *it == '.') {
        is_int = false;
        ++it;
        while (it != end && std::isdigit(*it)) { ++it; }
    }

    length      = static_cast<size_t>(it - str.begin());
    auto number = str.substr(static_cast<size_t>(start - str.begin()), static_cast<size_t>(it - start));
    str.remove_prefix(length);

    basic_jsonc result{};

    if (is_int) {
        if (negative) {
            int64_t res{};
            auto [ptr, ec] = std::from_chars(number.data(), number.data() + number.size(), res);
            if (ec != std::errc()) {
                result = basic_big_int(number);
            } else {
                result = res;
            }
        } else {
            uint64_t res{};
            auto [ptr, ec] = std::from_chars(number.data(), number.data() + number.size(), res);
            if (ec != std::errc()) {
                result = basic_big_int(number);
            } else {
                result = res;
            }
        }
    } else {
        double res{};
        std::from_chars(number.data(), number.data() + number.size(), res);
        result = res;
    }

    std::vector<std::string> comments_after{};
    if (!extract_comments(str, comments_after, ignore_comments)) { _JSONC_PARSE_ERROR("invalid comments format"); }
    result.before_comments() = std::move(comments_before);
    result.after_comments()  = std::move(comments_after);
    return result;
}

inline JSONC_PARSE_RESULT(int) get_codepoint(std::string_view& s) JSONC_EXCEPTION_TYPE {
    int codepoint = 0;
    for (const auto factor : {12u, 8u, 4u, 0u}) {
        auto current = get_current_char(s);
        if (current >= '0' && current <= '9') {
            codepoint += static_cast<int>((static_cast<uint32_t>(current) - 0x30u) << factor);
        } else if (current >= 'A' && current <= 'F') {
            codepoint += static_cast<int>((static_cast<uint32_t>(current) - 0x37u) << factor);
        } else if (current >= 'a' && current <= 'f') {
            codepoint += static_cast<int>((static_cast<uint32_t>(current) - 0x57u) << factor);
        } else {
            _JSONC_PARSE_ERROR("");
        }
    }
    return codepoint;
}

inline JSONC_PARSE_RESULT(
    basic_jsonc
) parse_string(std::string_view& str, std::vector<std::string>&& comments_before, bool ignore_comments) JSONC_EXCEPTION_TYPE {
    str.remove_prefix(1);
    std::string              res{};
    std::vector<std::string> comments_after{};
    while (!str.empty()) {
        auto current = get_current_char(str);
        switch (current) {
        case '\"': {
            if (!extract_comments(str, comments_after, ignore_comments)) { _JSONC_PARSE_ERROR("invalid comments format"); }
            basic_jsonc result{res};
            result.before_comments() = std::move(comments_before);
            result.after_comments()  = std::move(comments_after);
            return result;
        }
        case '\\': {
            switch (get_current_char(str)) {
            case '\r':
            case '\n': {
                _JSONC_PARSE_ERROR("unexpected line break encountered in string");
            }
            case '\"': {
                res.push_back('\"');
                break;
            }
            case '\'': {
                res.push_back('\'');
                break;
            }
            case '\\': {
                res.push_back('\\');
                break;
            }
            case '/': {
                res.push_back('/');
                break;
            }
            case 'b': {
                res.push_back('\b');
                break;
            }
            case 'f': {
                res.push_back('\f');
                break;
            }
            case 'n': {
                res.push_back('\n');
                break;
            }
            case 'r': {
                res.push_back('\r');
                break;
            }
            case 't': {
                res.push_back('\t');
                break;
            }
            case 'v': {
                res.push_back('\v');
                break;
            }
            case 'u': {
#ifdef JSONC_NO_EXCEPTION
                int codepoint1{};
                if (auto getted = get_codepoint(str); getted) {
                    codepoint1 = *getted;
                } else {
                    _JSONC_PARSE_ERROR("");
                }
#else
                int codepoint1 = get_codepoint(str);
#endif
                int codepoint = codepoint1;
                if (0xD800 <= codepoint1 && codepoint1 <= 0xDBFF) {
                    if (get_current_char(str) == '\\' && get_current_char(str) == 'u') {
#ifdef JSONC_NO_EXCEPTION
                        int codepoint2{};
                        if (auto getted = get_codepoint(str); getted) {
                            codepoint2 = *getted;
                        } else {
                            _JSONC_PARSE_ERROR("");
                        }
#else
                        int codepoint2 = get_codepoint(str);
#endif
                        if ((0xDC00 <= codepoint2 && codepoint2 <= 0xDFFF)) {
                            codepoint = static_cast<int>((static_cast<uint32_t>(codepoint1) << 10u) + static_cast<uint32_t>(codepoint2) - 0x35FDC00u);
                        } else {
                            _JSONC_PARSE_ERROR("");
                        }
                    } else {
                        _JSONC_PARSE_ERROR("");
                    }
                } else {
                    if (0xDC00 <= codepoint1 && codepoint1 <= 0xDFFF) { _JSONC_PARSE_ERROR(""); }
                }
                if (codepoint < 0x80) {
                    res.push_back(static_cast<char>(codepoint));
                } else if (codepoint <= 0x7FF) {
                    res.push_back(static_cast<char>(0xC0u | (static_cast<uint32_t>(codepoint) >> 6u)));
                    res.push_back(static_cast<char>(0x80u | (static_cast<uint32_t>(codepoint) & 0x3Fu)));
                } else if (codepoint <= 0xFFFF) {
                    res.push_back(static_cast<char>(0xE0u | (static_cast<uint32_t>(codepoint) >> 12u)));
                    res.push_back(static_cast<char>(0x80u | ((static_cast<uint32_t>(codepoint) >> 6u) & 0x3Fu)));
                    res.push_back(static_cast<char>(0x80u | (static_cast<uint32_t>(codepoint) & 0x3Fu)));
                } else {
                    res.push_back(static_cast<char>(0xF0u | (static_cast<uint32_t>(codepoint) >> 18u)));
                    res.push_back(static_cast<char>(0x80u | ((static_cast<uint32_t>(codepoint) >> 12u) & 0x3Fu)));
                    res.push_back(static_cast<char>(0x80u | ((static_cast<uint32_t>(codepoint) >> 6u) & 0x3Fu)));
                    res.push_back(static_cast<char>(0x80u | (static_cast<uint32_t>(codepoint) & 0x3Fu)));
                }
                break;
            }
            default:
                _JSONC_PARSE_ERROR("");
            }
            break;
        }
        default: {
            res.push_back(current);
            break;
        }
        }
    }
    _JSONC_PARSE_ERROR("");
}

inline JSONC_PARSE_RESULT(basic_jsonc) parse_list(
    std::string_view&          str,
    std::vector<std::string>&& comments_before,
    bool                       allow_trailing_comma,
    bool                       ignore_comments
) JSONC_EXCEPTION_TYPE {
    str.remove_prefix(1);
    basic_array res{};
    bool        requre_value = false;
    while (!str.empty()) {
        std::vector<std::string> element_comment_before;
        if (!extract_comments(str, element_comment_before, ignore_comments)) { _JSONC_PARSE_ERROR("invalid comments format"); }

        if (str.starts_with(']')) {
            if (requre_value) { _JSONC_PARSE_ERROR("trailing comma"); }
            str.remove_prefix(1);
            std::vector<std::string> comments_after{};
            if (!extract_comments(str, comments_after, ignore_comments)) { _JSONC_PARSE_ERROR("invalid comments format"); }
            basic_jsonc result{res};
            result.before_comments() = std::move(comments_before);
            result.after_comments()  = std::move(comments_after);
            return result;
        }

        auto value = parse_jsonc_type(str, std::move(element_comment_before), allow_trailing_comma, ignore_comments);

        switch (str.front()) {
        case ']': {
#ifdef JSONC_NO_EXCEPTION
            if (!value) { _JSONC_PARSE_ERROR(value.error().mErrorInfo); }
            res.push_back(std::move(*value));
#else
            res.push_back(std::move(value));
#endif
            str.remove_prefix(1);
            std::vector<std::string> comments_after{};
            if (!extract_comments(str, comments_after, ignore_comments)) { _JSONC_PARSE_ERROR("invalid comments format"); }
            basic_jsonc result{res};
            result.before_comments() = std::move(comments_before);
            result.after_comments()  = std::move(comments_after);
            return result;
        }
        case ',': {
            str.remove_prefix(1);
#ifdef JSONC_NO_EXCEPTION
            extarct_comma_back_comments(str, value->after_comments(), ignore_comments);
#else
            extarct_comma_back_comments(str, value.after_comments(), ignore_comments);
#endif
            if (!allow_trailing_comma) { requre_value = true; }
        }
        default:
            break;
        }
#ifdef JSONC_NO_EXCEPTION
        if (!value) { _JSONC_PARSE_ERROR(value.error().mErrorInfo); }
        res.push_back(std::move(*value));
#else
        res.push_back(std::move(value));
#endif
    }
    _JSONC_PARSE_ERROR("");
}

inline JSONC_PARSE_RESULT(basic_jsonc) parse_object(
    std::string_view&          str,
    std::vector<std::string>&& comments_before,
    bool                       allow_trailing_comma,
    bool                       ignore_comments
) JSONC_EXCEPTION_TYPE {
    str.remove_prefix(1);
    basic_object res{};
    bool         requre_value = false;
    while (!str.empty()) {

        std::vector<std::string> pair_comment_before;
        if (!extract_comments(str, pair_comment_before, ignore_comments)) { _JSONC_PARSE_ERROR(""); }

        if (str.starts_with('}')) {
            if (requre_value) { _JSONC_PARSE_ERROR("trailing comma"); }
            str.remove_prefix(1);
            std::vector<std::string> comments_after{};
            if (!extract_comments(str, comments_after, ignore_comments)) { _JSONC_PARSE_ERROR("invalid comments format"); }
            basic_jsonc result{res};
            result.before_comments() = std::move(comments_before);
            result.after_comments()  = std::move(comments_after);
            return result;
        }


        if (str.front() != '\"') { _JSONC_PARSE_ERROR("key must be a string"); }
        auto key = parse_string(str, std::move(pair_comment_before), ignore_comments);

#ifdef JSONC_NO_EXCEPTION
        if (!key) { _JSONC_PARSE_ERROR("key must be a string"); }
#endif

        if (get_current_char(str) != ':') { _JSONC_PARSE_ERROR("illegal key and value separator"); }
        auto value = parse_jsonc_type(str, allow_trailing_comma, ignore_comments);

#ifdef JSONC_NO_EXCEPTION
        if (!value) { _JSONC_PARSE_ERROR("invalid object value"); }

        auto key_str = key->get<std::string>();

#ifdef JSONC_USE_EXPECTED
        res[*key_str] = *value;

        res.key_before_comments(*key_str) = std::move(key->before_comments());
        res.key_after_comments(*key_str)  = std::move(key->after_comments());
        res[*key_str].before_comments()   = std::move(value->before_comments());
        res[*key_str].after_comments()    = std::move(value->after_comments());
#else
        res[key_str] = *value;

        res.key_before_comments(key_str) = std::move(key->before_comments());
        res.key_after_comments(key_str)  = std::move(key->after_comments());
        res[key_str].before_comments()   = std::move(value->before_comments());
        res[key_str].after_comments()    = std::move(value->after_comments());
#endif

#else
        auto key_str = key.get<std::string>();
        res[key_str] = value;

        res.key_before_comments(key_str) = std::move(key.before_comments());
        res.key_after_comments(key_str)  = std::move(key.after_comments());
        res[key_str].before_comments()   = std::move(value.before_comments());
        res[key_str].after_comments()    = std::move(value.after_comments());
#endif

        switch (str.front()) {
        case '}': {
            str.remove_prefix(1);
            std::vector<std::string> comments_after{};
            if (!extract_comments(str, comments_after, ignore_comments)) { _JSONC_PARSE_ERROR("invalid comments format"); }
            basic_jsonc result{res};
            result.before_comments() = std::move(comments_before);
            result.after_comments()  = std::move(comments_after);
            return result;
        }
        case ',':
            str.remove_prefix(1);
#ifdef JSONC_USE_EXPECTED
            extarct_comma_back_comments(str, res[*key_str].after_comments(), ignore_comments);
#else
            extarct_comma_back_comments(str, res[key_str].after_comments(), ignore_comments);
#endif
            if (!allow_trailing_comma) { requre_value = true; }
        default:
            break;
        }
    }
    _JSONC_PARSE_ERROR("invalid object");
}

} // namespace

inline JSONC_PARSE_RESULT(basic_jsonc) parse_jsonc_type(
    std::string_view&          str,
    std::vector<std::string>&& comments_before,
    bool                       allow_trailing_comma,
    bool                       ignore_comments
) JSONC_EXCEPTION_TYPE {
    if (str.empty()) { _JSONC_PARSE_ERROR("empty string"); }
    switch (str.front()) {
    case 't':
        return parse_boolean<true>(str, std::move(comments_before), ignore_comments);
    case 'f':
        return parse_boolean<false>(str, std::move(comments_before), ignore_comments);
    case 'n':
        return parse_null(str, std::move(comments_before), ignore_comments);
    case ']':
    case '}':
        _JSONC_PARSE_ERROR("unclosed bracket");
    case static_cast<char>(std::char_traits<char>::eof()):
        _JSONC_PARSE_ERROR("unexpected eof encountered");
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return parse_number(str, std::move(comments_before), ignore_comments);
    case '[':
        return parse_list(str, std::move(comments_before), allow_trailing_comma, ignore_comments);
    case '{':
        return parse_object(str, std::move(comments_before), allow_trailing_comma, ignore_comments);
    case '\"':
        return parse_string(str, std::move(comments_before), ignore_comments);
    default:
        break;
    }
    _JSONC_PARSE_ERROR("illegal escape");
}

inline JSONC_PARSE_RESULT(basic_jsonc) parse_jsonc_type(std::string_view& str, bool allow_trailing_comma, bool ignore_comments) JSONC_EXCEPTION_TYPE {
    std::vector<std::string> comments_before{};
    if (!extract_comments(str, comments_before, ignore_comments)) { _JSONC_PARSE_ERROR("invalid comments format"); }
    return parse_jsonc_type(str, std::move(comments_before), allow_trailing_comma, ignore_comments);
}

} // namespace jsonc::inline abi_v1_1_0::detail
