// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <filesystem>
#include <optional>
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

	/// \brief An error representing a parsing failure of any kind.
	class parser_error : public error {
	public:
		explicit parser_error(std::string&& resource_type);
		explicit parser_error(std::string&& resource_type, std::string&& context);
		explicit parser_error(std::string&& resource_type, const std::exception& cause);
		explicit parser_error(std::string&& resource_type, const std::exception& cause, std::string&& context);

	public:
		const std::string resource_type;
		const std::optional<std::string> context {std::nullopt};
		const std::optional<std::exception> cause {std::nullopt};
	};
} // namespace phoenix
