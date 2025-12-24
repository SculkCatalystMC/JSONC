#pragma once
#include "exception.hpp"
#include "type.hpp"

namespace jsonc {

namespace detail {
inline JSONC_RESULT(JsoncType) parseJsonValue(std::string_view& str, std::vector<std::string>&& comments_before);
inline JSONC_RESULT(JsoncType) parseJsonValue(std::string_view& str);
} // namespace detail

namespace {

inline JSONC_RESULT(std::string_view) extractComment(std::string_view& s) {
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
    _JSONC_PARSE_ERROR("");
}

inline void scanSpaces(std::string_view& s) noexcept {
    size_t i = 0;
    while (i <= s.size() && std::isspace(s[i++])) {}
    s.remove_prefix(std::min(i - 1, s.size()));
}

std::vector<std::string> parse_comments(std::string_view comment) {
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

inline bool extractComments(std::string_view& s, std::vector<std::string>& comments) {
    scanSpaces(s);
    while (s.starts_with('/')) {
        s.remove_prefix(1);
        auto comment = extractComment(s);
#ifdef JSONC_USE_EXPECTED
        if (!comment) { return false; }
        comments.append_range(parse_comments(*comment));
#else
        comments.append_range(parse_comments(comment));
#endif
        scanSpaces(s);
    }
    return true;
}

inline constexpr char getCurrentChar(std::string_view& s) {
    if (s.empty()) { return '\0'; }
    char c = s.front();
    s.remove_prefix(1);
    return c;
}

inline JSONC_RESULT(JsoncType) parseNumber(std::string_view& str, std::vector<std::string>&& comments_before) {
    size_t length = 0;
    bool   isInt  = true;
    if (str.empty()) { _JSONC_PARSE_ERROR("illegal escape"); }

    auto it    = str.begin();
    auto end   = str.end();
    auto start = it;

    bool negative = false;

    if (*it == '-') {
        negative = true;
        ++it;
    }

    auto digit_start = it;
    while (it != end && std::isdigit(*it)) { ++it; }

    if (it != end && (*it == '.' || *it == 'e' && *it == 'E')) {
        isInt = false;
        ++it;
        if (*it != '+' && *it != '-') { _JSONC_PARSE_ERROR("invalid floating point value"); }
        while (it != end && std::isdigit(*it)) { ++it; }
    }

    length      = static_cast<size_t>(it - str.begin());
    auto number = str.substr(static_cast<size_t>(start - str.begin()), static_cast<size_t>(it - start));
    str.remove_prefix(length);

    JsoncType   result{};
    static int& errnoRef = errno;

    errnoRef        = 0;
    char const* ptr = number.data();
    char*       eptr{};

    if (isInt) {
        if (negative) {
            result = strtoll(ptr, &eptr, 10);
        } else {
            result = strtoull(ptr, &eptr, 10);
        }
    } else {
        result = strtod(ptr, &eptr);
    }

    std::vector<std::string> comments_after{};
    if (!extractComments(str, comments_after)) { _JSONC_PARSE_ERROR("failed to parse comments"); }
    result.before_comments() = std::move(comments_before);
    result.after_comments()  = std::move(comments_after);
    return result;
}

inline JSONC_RESULT(int) get_codepoint(std::string_view& s) {
    int codepoint = 0;
    for (const auto factor : {12u, 8u, 4u, 0u}) {
        auto current = getCurrentChar(s);
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

inline JSONC_RESULT(JsoncType) parseString(std::string_view& str, std::vector<std::string>&& comments_before) {
    str.remove_prefix(1);
    std::string              res{};
    std::vector<std::string> comments_after{};
    while (!str.empty()) {
        auto current = getCurrentChar(str);
        switch (current) {
        case '\"': {
            if (!extractComments(str, comments_after)) { _JSONC_PARSE_ERROR("failed to parse comments"); }
            JsoncType result{res};
            result.before_comments() = std::move(comments_before);
            result.after_comments()  = std::move(comments_after);
            return result;
        }
        case '\\': {
            switch (getCurrentChar(str)) {
            case '\r':
            case '\n': {
                _JSONC_PARSE_ERROR("");
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
#ifdef JSONC_USE_EXPECTED
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
                    if (getCurrentChar(str) == '\\' && getCurrentChar(str) == 'u') {
#ifdef JSONC_USE_EXPECTED
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
                            codepoint = static_cast<int>(
                                (static_cast<uint32_t>(codepoint1) << 10u) + static_cast<uint32_t>(codepoint2)
                                - 0x35FDC00u
                            );
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

inline JSONC_RESULT(JsoncType) parseList(std::string_view& str, std::vector<std::string>&& comments_before) {
    str.remove_prefix(1);
    Array res{};
    bool  requre_value = false;
    while (!str.empty()) {
        std::vector<std::string> element_comment_before;
        if (!extractComments(str, element_comment_before)) { _JSONC_PARSE_ERROR(""); }

        if (str.starts_with(']')) {
            if (requre_value) { _JSONC_PARSE_ERROR("trailing comma"); }
            str.remove_prefix(1);
            std::vector<std::string> comments_after{};
            if (!extractComments(str, comments_after)) { _JSONC_PARSE_ERROR("failed to parse comments"); }
            JsoncType result{res};
            result.before_comments() = std::move(comments_before);
            result.after_comments()  = std::move(comments_after);
            return result;
        }

        auto value = detail::parseJsonValue(str, std::move(element_comment_before));
#ifdef JSONC_USE_EXPECTED
        if (!value) { _JSONC_PARSE_ERROR(value.error().mErrorInfo); }
        res.push_back(std::move(*value));
#else
        res.push_back(std::move(value));
#endif

        switch (str.front()) {
        case ']': {
            str.remove_prefix(1);
            std::vector<std::string> comments_after{};
            if (!extractComments(str, comments_after)) { _JSONC_PARSE_ERROR("failed to parse comments"); }
            JsoncType result{res};
            result.before_comments() = std::move(comments_before);
            result.after_comments()  = std::move(comments_after);
            return result;
        }
        case ',': {
            str.remove_prefix(1);
            requre_value = true;
        }
        default:
            break;
        }
    }
    _JSONC_PARSE_ERROR("");
}

inline JSONC_RESULT(JsoncType) parseObject(std::string_view& str, std::vector<std::string>&& comments_before) {
    str.remove_prefix(1);
    Object res{};
    bool   requre_value = false;
    while (!str.empty()) {

        std::vector<std::string> pair_comment_before;
        if (!extractComments(str, pair_comment_before)) { _JSONC_PARSE_ERROR(""); }

        if (str.starts_with('}')) {
            if (requre_value) { _JSONC_PARSE_ERROR("trailing comma"); }
            str.remove_prefix(1);
            std::vector<std::string> comments_after{};
            if (!extractComments(str, comments_after)) { _JSONC_PARSE_ERROR("failed to parse comments"); }
            JsoncType result{res};
            result.before_comments() = std::move(comments_before);
            result.after_comments()  = std::move(comments_after);
            return result;
        }


        if (str.front() != '\"') { _JSONC_PARSE_ERROR("key must be a string"); }
        auto key = parseString(str, std::move(pair_comment_before));

#ifdef JSONC_USE_EXPECTED
        if (!key) { _JSONC_PARSE_ERROR("key must be a string"); }
#endif

        if (getCurrentChar(str) != ':') { _JSONC_PARSE_ERROR("illegal key and value separator"); }
        auto value = detail::parseJsonValue(str);

#ifdef JSONC_USE_EXPECTED
        if (!value) { _JSONC_PARSE_ERROR("invalid object value"); }

        auto key_str  = key->get<std::string>();
        res[*key_str] = *value;

        res.key_before_comments(*key_str) = std::move(key->before_comments());
        res.key_after_comments(*key_str)  = std::move(key->after_comments());
        res[*key_str].before_comments()   = std::move(value->before_comments());
        res[*key_str].after_comments()    = std::move(value->after_comments());
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
            if (!extractComments(str, comments_after)) { _JSONC_PARSE_ERROR("failed to parse comments"); }
            JsoncType result{res};
            result.before_comments() = std::move(comments_before);
            result.after_comments()  = std::move(comments_after);
            return result;
        }
        case ',':
            str.remove_prefix(1);
            requre_value = true;
        default:
            break;
        }
    }
    _JSONC_PARSE_ERROR("");
}

} // namespace

namespace detail {

inline JSONC_RESULT(JsoncType) parseJsonValue(std::string_view& str, std::vector<std::string>&& comments_before) {
    if (str.empty()) { _JSONC_PARSE_ERROR("empty string"); }

    static auto parse_comments_after = [&](JSONC_RESULT(JsoncType) && result) -> JSONC_RESULT(JsoncType) {
        std::vector<std::string> comments_after{};
        if (!extractComments(str, comments_after)) { _JSONC_PARSE_ERROR("failed to parse comments"); }
#ifdef JSONC_USE_EXPECTED
        result->before_comments() = std::move(comments_before);
        result->after_comments()  = std::move(comments_after);
#else
        result.before_comments() = std::move(comments_before);
        result.after_comments()  = std::move(comments_after);
#endif
        return result;
    };

    switch (str.front()) {
    case 't':
        if (str.starts_with("true")) {
            str.remove_prefix(4);
            JsoncType result{true};
            return parse_comments_after(std::move(result));
        }
        break;
    case 'f':
        if (str.starts_with("false")) {
            str.remove_prefix(5);
            JsoncType result{false};
            return parse_comments_after(std::move(result));
        }
        break;
    case 'n':
        if (str.starts_with("null")) {
            str.remove_prefix(4);
            JsoncType result{nullptr};
            return parse_comments_after(std::move(result));
        }
        break;
    case ']':
    case '}':
        str.remove_prefix(1);
        _JSONC_PARSE_ERROR("unclosed bracket");
    case static_cast<char>(std::char_traits<char>::eof()):
    case '\0':
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
        return parseNumber(str, std::move(comments_before));
    case '[':
        return parseList(str, std::move(comments_before));
    case '{':
        return parseObject(str, std::move(comments_before));
    case '\"':
        return parseString(str, std::move(comments_before));
    default:
        break;
    }
    _JSONC_PARSE_ERROR("illegal escape");
}

inline JSONC_RESULT(JsoncType) parseJsonValue(std::string_view& str) {
    std::vector<std::string> comments_before{};
    if (!extractComments(str, comments_before)) { _JSONC_PARSE_ERROR("failed to parse comments"); }
    return parseJsonValue(str, std::move(comments_before));
}

} // namespace detail

} // namespace jsonc