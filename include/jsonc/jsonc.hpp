#pragma once
#include "detail/deserializer.hpp"
#include "detail/impl.hpp"
#include "detail/type.hpp"

namespace jsonc {

using object = Object;
using array  = Array;
using jsonc  = JsoncType;

inline JSONC_RESULT(JsoncType) parse(std::string_view content) { return detail::parse_jsonc_type(content); }

} // namespace jsonc
