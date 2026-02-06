#pragma once
#include "detail/deserializer.hpp"
#include "detail/impl.hpp"
#include "detail/type.hpp"

namespace jsonc {

using jsonc         = detail::basic_jsonc<false>;
using ordered_jsonc = detail::basic_jsonc<true>;

} // namespace jsonc
