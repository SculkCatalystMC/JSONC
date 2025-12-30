#pragma once
#include "detail/deserializer.hpp"
#include "detail/impl.hpp"
#include "detail/type.hpp"

namespace jsonc {

using object = Object;
using array  = Array;
using jsonc  = JsoncType;

inline JSONC_PARSE_RESULT(JsoncType) parse(std::string_view content, bool allow_trailing_comma = false) {
    return detail::parse_jsonc_type(content, allow_trailing_comma);
}

} // namespace jsonc
