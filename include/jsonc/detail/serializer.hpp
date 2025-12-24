#pragma once
#include "type.hpp"

namespace jsonc::detail {

inline std::string format_comments(std::vector<std::string> const& comments, std::string_view indent_space = "", bool nolinefeed = false) {
    std::string result{};
    if (comments.size() > 0) {
        if (comments.size() == 1) {
            if (nolinefeed) {
                result.append("/* ");
                result.append(comments.front());
                result.append(" */");
            } else {
                result.append("// ");
                result.append(comments.front());
                result.push_back('\n');
            }
        } else {
            result.append("/*\n");
            for (auto const& comment : comments) {
                result.append(indent_space);
                result.append("* ");
                result.append(comment);
                result.push_back('\n');
            }
            result.append(indent_space);
            result.append("*/");
            if (!nolinefeed) { result.push_back('\n'); }
        }
    }
    return result;
}

std::string& fix_indent(std::string& str, std::string_view indent_space) {
    for (std::string::size_type pos(0); pos != std::string::npos; pos += indent_space.length()) {
        if ((pos = str.find('\n', pos)) != std::string::npos) {
            str.replace(pos + 1, 0, indent_space);
        } else {
            break;
        }
    }
    return str;
}

inline std::string dump_typed(std::monostate const&, bool, int) { return "null"; }

inline std::string dump_typed(std::string const& str, bool ensure_ascii, int) {
    if (str.empty()) { return "\"\""; }

    std::string result{};
    result.reserve(static_cast<size_t>(static_cast<double>(str.size()) * 1.2));

    if (ensure_ascii) {
        auto it = str.begin();
        while (it != str.end()) {
            const uint8_t c         = static_cast<uint8_t>(*it++);
            uint32_t      codepoint = c;
            size_t        char_len  = 1;

            if (c >= 0xC0) {
                char_len = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : 2;

                codepoint = c & (0xFFu >> (char_len + 1));
                for (size_t i = 1; i < char_len; ++i) { codepoint = (codepoint << 6) | (static_cast<uint8_t>(*it++) & 0x3F); }
            }

            switch (codepoint) {
            case '"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\b':
                result += "\\b";
                break;
            case '\f':
                result += "\\f";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                if (codepoint <= 0x1F) {
                    result += std::format("\\u{:04x}", codepoint);
                } else if (codepoint <= 0x7F) {
                    result += static_cast<char>(codepoint);
                } else {
                    if (codepoint <= 0xFFFF) {
                        result += std::format("\\u{:04x}", codepoint);
                    } else {
                        codepoint -= 0x10000;
                        result    += std::format("\\u{:04x}\\u{:04x}", 0xD800 + (codepoint >> 10), 0xDC00 + (codepoint & 0x3FF));
                    }
                }
            }
        }
    } else {
        for (char c : str) {
            switch (c) {
            case '"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\b':
                result += "\\b";
                break;
            case '\f':
                result += "\\f";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                if (static_cast<uint8_t>(c) <= 0x1F) {
                    result.append(std::format("\\u{:04x}", c));
                } else {
                    result.push_back(c);
                }
            }
        }
    }

    return "\"" + result + "\"";
}

inline std::string dump_typed(Array const& val, bool ensure_ascii, int indent) {
    std::string result{"["};

    size_t      i = val.size();
    std::string indent_space(indent, ' ');
    bool        line_feed = (indent >= 0);

    if (line_feed && val.size() > 0) { result += '\n'; }

    for (auto& element : val) {
        i--;
        if (line_feed) { result += indent_space; }

        auto value  = element.dump(indent, ensure_ascii);
        result     += fix_indent(value, indent_space);

        if (i > 0) { result += ','; }
        if (line_feed) { result += '\n'; }
    }
    result += "]";

    return result;
}

inline std::string dump_typed(Object const& val, bool ensure_ascii, int indent) {
    std::string result{"{"};

    size_t      i = val.size();
    std::string indent_space(indent, ' ');

    bool line_feed = (indent >= 0);

    if (line_feed && val.size() > 0) { result += '\n'; }

    for (auto& [k, v] : val) {
        i--;
        if (line_feed) { result += indent_space; }

        if (val.has_key_before_comments(k)) {
#ifdef JSONC_USE_EXPECTED
            result += format_comments(val.key_before_comments(k)->get(), indent_space);
#else
            result += format_comments(val.key_before_comments(k), indent_space);
#endif
            if (line_feed) { result += indent_space; }
        }

        result += dump_typed(k, indent, ensure_ascii);

        if (val.has_key_before_comments(k)) {
#ifdef JSONC_USE_EXPECTED
            auto after_comments = val.key_after_comments(k)->get();
#else
            auto after_comments = val.key_after_comments(k);
#endif
            if (after_comments.size() > 0) {
                result += format_comments(after_comments, indent_space, true);
                if (line_feed && after_comments.size() > 1) { result += indent_space; }
            }
        }
        result += ':';

        if (indent >= 0) { result += ' '; }

        auto value  = v.dump(indent, ensure_ascii);
        result     += fix_indent(value, indent_space);

        if (i > 0) { result += ','; }
        if (line_feed) { result += '\n'; }
    }

    result += '}';

    return result;
}

inline std::string dump_typed(bool val, bool ensure_ascii, int indent) { return val ? "true" : "false"; }

template <typename T>
    requires std::is_arithmetic_v<T> && (!std::same_as<T, bool>)
inline std::string dump_typed(T val, bool ensure_ascii, int indent) {
    return std::to_string(val);
}

} // namespace jsonc::detail
