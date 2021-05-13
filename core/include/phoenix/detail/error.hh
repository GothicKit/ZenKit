// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <stdexcept>

namespace phoenix {
	/**
	 * @brief Represents any I/O failure that may occur.
	 *
	 * An I/O failure can occur when reading past the end of a file, trying to open file that
	 * doesn't exist, trying to open a directory as a file and others. Most commonly it will
	 * occur when reading past the end of a file.
	 */
	class io_error : public std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	/**
	 * @brief Represents any parsing failure that may occur.
	 *
	 * A parsing failure can occur if when reading a file format, an unexpected or illegal
	 * value is read and can not be processed, when a version mismatch is detected or when
	 * the format actually present is not the format expected.
	 */
	class parser_error : public io_error {
		using io_error::io_error;
	};
}