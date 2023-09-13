// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Error.hh"

namespace zenkit {
	Error::Error(std::string&& msg) : std::exception(), message(std::move(msg)) {}

	ParserError::ParserError(std::string&& type)
	    : Error("failed parsing resource of type " + type), resource_type(type) {}

	ParserError::ParserError(std::string&& type, std::string&& ctx)
	    : Error("failed parsing resource of type " + type + " [context: " + ctx + "]"), resource_type(std::move(type)),
	      context(std::move(ctx)) {}

	ParserError::ParserError(std::string&& type, std::exception const& other_exc)
	    : Error("failed parsing resource of type " + type + " due to [" + other_exc.what() + "]"),
	      resource_type(std::move(type)), cause(other_exc) {}

	ParserError::ParserError(std::string&& type, std::exception const& other_exc, std::string&& ctx)
	    : Error("failed parsing resource of type " + type + " due to [" + other_exc.what() + "] [context: " + ctx +
	            "]"),
	      resource_type(std::move(type)), context(std::move(ctx)), cause(other_exc) {}
} // namespace zenkit
