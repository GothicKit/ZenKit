// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <stdexcept>

namespace phoenix {
	/**
	 * @brief Represents any parsing failure that may occur.
	 *
	 * A parsing failure can occur if when reading a file format, an unexpected or illegal
	 * value is read and can not be processed, when a version mismatch is detected or when
	 * the format actually present is not the format expected.
	 */
	class parser_error : public std::runtime_error {
		using std::runtime_error::runtime_error;
	};
} // namespace phoenix
