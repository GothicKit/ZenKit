// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/phoenix.hh>

#include <utility>

namespace phoenix {
	error::error(std::string&& message) : std::exception(), message(std::move(message)) {}
} // namespace phoenix