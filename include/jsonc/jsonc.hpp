#pragma once
#include "detail/impl.hpp"
#include "detail/type.hpp"

namespace sculk::jsonc {

using json         = detail::basic_jsonc<false, false>;
using ordered_json = detail::basic_jsonc<true, false>;

using jsonc         = detail::basic_jsonc<false, true>;
using ordered_jsonc = detail::basic_jsonc<true, true>;

} // namespace sculk::jsonc
