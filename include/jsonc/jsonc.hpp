#pragma once
#include "detail/deserializer.hpp"
#include "detail/impl.hpp"
#include "detail/type.hpp"

namespace jsonc {

using jsonc = detail::basic_jsonc;

inline JSONC_PARSE_RESULT(jsonc) parse(std::string_view content, bool allow_trailing_comma = false, bool ignore_comments = false) {
    return detail::parse_jsonc_type(content, allow_trailing_comma, ignore_comments);
}

} // namespace jsonc
