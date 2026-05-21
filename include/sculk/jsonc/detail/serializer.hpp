// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "type.hpp"

namespace sculk::jsonc::inline abi_v1_4_1::detail {

inline std::string
format_comments(const std::vector<std::string>& comments, std::string_view indent_space, bool nolinefeed, bool multi_line_comments_format) {
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
            if (nolinefeed) {
                result.append("/*\n");
                for (const auto& comment : comments) {
                    result.append(indent_space);
                    result.append("* ");
                    result.append(comment);
                    result.push_back('\n');
                }
                result.append(indent_space);
                result.append("*/");
            } else {
                if (multi_line_comments_format) {
                    result.append("/*\n");
                    for (const auto& comment : comments) {
                        result.append(indent_space);
                        result.append("* ");
                        result.append(comment);
                        result.push_back('\n');
                    }
                    result.append(indent_space);
                    result.append("*/");
                    result.push_back('\n');
                } else {
                    bool is_first = true;
                    for (const auto& comment : comments) {
                        if (!is_first) { result.append(indent_space); }
                        result.append("// ");
                        result.append(comment);
                        result.push_back('\n');
                        is_first = false;
                    }
                }
            }
        }
    }
    return result;
}

inline std::string& fix_indent(std::string& str, std::string_view indent_space) {
    if (!indent_space.empty()) {
        for (std::string::size_type pos(0); pos != std::string::npos; pos += indent_space.length()) {
            if ((pos = str.find('\n', pos)) != std::string::npos) {
                str.replace(pos + 1, 0, indent_space);
            } else {
                break;
            }
        }
    }
    return str;
}

template <bool B, bool A>
inline std::string dump_typed(const std::monostate&, bool, int, bool, bool) {
    return "null";
}

template <bool B, bool A>
inline std::string dump_typed(const std::string& str, bool ensure_ascii, int, bool, bool) {
    if (str.empty()) { return "\"\""; }

    std::string result{};
    result.reserve(static_cast<std::size_t>(static_cast<double>(str.size()) * 1.2));

    if (ensure_ascii) {
        auto it = str.begin();
        while (it != str.end()) {
            const std::uint8_t c         = static_cast<std::uint8_t>(*it++);
            std::uint32_t      codepoint = c;
            std::size_t        char_len  = 1;

            if (c >= 0xC0) {
                char_len = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : 2;

                codepoint = c & (0xFFu >> (char_len + 1));
                for (std::size_t i = 1; i < char_len; ++i) { codepoint = (codepoint << 6) | (static_cast<std::uint8_t>(*it++) & 0x3F); }
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
                if (static_cast<std::uint8_t>(c) <= 0x1F) {
                    result.append(std::format("\\u{:04x}", c));
                } else {
                    result.push_back(c);
                }
            }
        }
    }

    return "\"" + result + "\"";
}

template <bool B, bool A>
inline std::string
dump_typed(const typename basic_jsonc<B, A>::array_type& val, bool ensure_ascii, int indent, bool ignore_comments, bool multi_line_comments_format) {
    std::string result{"["};

    std::size_t i = val.size();
    std::string indent_space{};
    if (indent > 0) { indent_space.resize(static_cast<std::size_t>(indent), ' '); }

    bool line_feed = (indent >= 0);

    if (line_feed && val.size() > 0) { result += '\n'; }

    for (auto& element : val) {
        i--;
        if (line_feed) { result += indent_space; }

        if constexpr (A) {
            if (!ignore_comments && element.has_before_comments()) {
                result += format_comments(element.before_comments(), indent_space, !line_feed, multi_line_comments_format);
                result += indent_space;
            }
        }
        auto value  = element.dump(indent, ensure_ascii, ignore_comments, multi_line_comments_format, false);
        result     += fix_indent(value, indent_space);

        if (i > 0) { result.push_back(','); }

        if constexpr (A) {
            if (!ignore_comments && element.has_after_comments()) {
                if (line_feed) { result.push_back(' '); }
                result += format_comments(element.after_comments(), indent_space, !line_feed, multi_line_comments_format);
                if (line_feed) { result.pop_back(); }
            }
        }

        if (line_feed) { result.push_back('\n'); }
    }
    result.push_back(']');

    return result;
}

template <bool B, bool A>
inline std::string
dump_typed(const typename basic_jsonc<B, A>::object_type& val, bool ensure_ascii, int indent, bool ignore_comments, bool multi_line_comments_format) {
    std::string result{"{"};

    std::size_t i = val.size();
    std::string indent_space{};
    if (indent > 0) { indent_space.resize(static_cast<std::size_t>(indent), ' '); }

    bool line_feed = (indent >= 0);

    if (line_feed && val.size() > 0) { result += '\n'; }

    for (auto& [k, v] : val) {
        i--;
        if (line_feed) { result += indent_space; }

        if constexpr (A) {
            if (!ignore_comments && val.has_key_before_comments(k)) {
#ifdef JSONC_USE_EXPECTED
                result += format_comments(val.key_before_comments(k)->get(), indent_space, !line_feed, multi_line_comments_format);
#else
                result += format_comments(val.key_before_comments(k), indent_space, !line_feed, multi_line_comments_format);
#endif
                if (line_feed) { result += indent_space; }
            }
        }

        result += dump_typed<B, A>(k, indent, ensure_ascii, ignore_comments, multi_line_comments_format);

        if constexpr (A) {
            if (!ignore_comments && val.has_key_after_comments(k)) {
#ifdef JSONC_USE_EXPECTED
                auto after_comments = val.key_after_comments(k)->get();
#else
                auto after_comments = val.key_after_comments(k);
#endif
                if (line_feed) { result.push_back(' '); }
                result += format_comments(after_comments, indent_space, true, multi_line_comments_format);
                if (line_feed && after_comments.size() > 1) { result += indent_space; }
            }
        }
        result.push_back(':');

        if (indent >= 0) { result += ' '; }

        std::string value{};
        if constexpr (A) {
            if (!ignore_comments && v.has_before_comments()) {
                value += format_comments(v.before_comments(), indent_space, true, multi_line_comments_format);
                if (line_feed) { value.push_back(' '); }
            }
        }
        value  += v.dump(indent, ensure_ascii, ignore_comments, multi_line_comments_format, false);
        result += fix_indent(value, indent_space);

        if (i > 0) { result.push_back(','); }

        if constexpr (A) {
            if (!ignore_comments && v.has_after_comments()) {
                if (line_feed) { result.push_back(' '); }
                result += format_comments(v.after_comments(), indent_space, !line_feed, multi_line_comments_format);
                if (line_feed) { result.pop_back(); }
            }
        }

        if (line_feed) { result.push_back('\n'); }
    }

    result.push_back('}');

    return result;
}

template <bool B, bool A, typename T>
    requires std::is_arithmetic_v<T>
inline std::string dump_typed(T val, bool, int, bool, bool) {
    return std::format("{}", val);
}

template <bool B, bool A>
inline std::string dump_typed(basic_big_integer val, bool, int, bool, bool) {
    return val.view_;
}

template <bool B, bool A>
inline std::string dump_typed(basic_high_precision_float val, bool, int, bool, bool) {
    return val.view_;
}

} // namespace sculk::jsonc::inline abi_v1_4_1::detail
