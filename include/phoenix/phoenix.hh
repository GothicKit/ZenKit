// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <filesystem>
#include <stdexcept>
#include <string>
#include <utility>

namespace phoenix {
	/// \brief Base class for all exceptions.
	class error : public std::exception {
	public:
		explicit error(std::string&& message);

		[[nodiscard]] inline const char* what() const noexcept override {
			return message.c_str();
		}

	public:
		[[maybe_unused]] const std::string message;
	};
} // namespace phoenix
