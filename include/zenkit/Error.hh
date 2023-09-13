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

		[[nodiscard]] ZKAPI char const* what() const noexcept override {
			return message.c_str();
		}

	public:
		std::string const message;
	};

	/// \brief An error representing a parsing failure of any kind.
	class ParserError : public Error {
	public:
		ZKINT explicit ParserError(std::string&& resource_type);
		ZKAPI explicit ParserError(std::string&& resource_type, std::string&& context);
		ZKINT explicit ParserError(std::string&& resource_type, std::exception const& cause);
		ZKINT explicit ParserError(std::string&& resource_type, std::exception const& cause, std::string&& context);

	public:
		std::string const resource_type;
		std::optional<std::string> const context {std::nullopt};
		std::optional<std::exception> const cause {std::nullopt};
	};
} // namespace zenkit
