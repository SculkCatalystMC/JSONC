// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "detail/impl.hpp"
#include "detail/type.hpp"

namespace sculk::jsonc {

using json         = detail::basic_jsonc<false, false>;
using ordered_json = detail::basic_jsonc<true, false>;

using jsonc         = detail::basic_jsonc<false, true>;
using ordered_jsonc = detail::basic_jsonc<true, true>;

} // namespace sculk::jsonc
