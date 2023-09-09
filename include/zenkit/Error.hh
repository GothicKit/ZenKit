// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"

#include <exception>
#include <optional>
#include <string>

namespace zenkit {
	/// \brief Base class for all exceptions.
	class Error : public std::exception {
	public:
		ZKAPI explicit Error(std::string&& message);

		[[nodiscard]] ZKAPI const char* what() const noexcept override {
			return message.c_str();
		}

	public:
		const std::string message;
	};

	/// \brief An error representing a parsing failure of any kind.
	class ParserError : public Error {
	public:
		ZKINT explicit ParserError(std::string&& resource_type);
		ZKAPI explicit ParserError(std::string&& resource_type, std::string&& context);
		ZKINT explicit ParserError(std::string&& resource_type, const std::exception& cause);
		ZKINT explicit ParserError(std::string&& resource_type, const std::exception& cause, std::string&& context);

	public:
		const std::string resource_type;
		const std::optional<std::string> context {std::nullopt};
		const std::optional<std::exception> cause {std::nullopt};
	};
} // namespace zenkit
