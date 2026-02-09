#pragma once
#include "detail/deserializer.hpp"
#include "detail/impl.hpp"
#include "detail/type.hpp"

namespace jsonc {

using json         = detail::basic_jsonc<false, false>;
using ordered_json = detail::basic_jsonc<true, false>;

using jsonc         = detail::basic_jsonc<false, true>;
using ordered_jsonc = detail::basic_jsonc<true, true>;

using json_with_comments         = detail::basic_jsonc<false, true>;
using ordered_json_with_comments = detail::basic_jsonc<true, true>;

} // namespace jsonc
