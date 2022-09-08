// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/buffer.hh>
#include <phoenix/phoenix.hh>

#include <fmt/format.h>

#include <utility>

namespace phoenix {
	error::error(std::string&& message) : std::exception(), message(std::move(message)) {}

	parser_error::parser_error(std::string&& resource_type)
	    : error(fmt::format("failed parsing resource of type {}", resource_type)), resource_type(resource_type) {}

	parser_error::parser_error(std::string&& resource_type, std::string&& context)
	    : error(fmt::format("failed parsing resource of type {} [context: {}]", resource_type, context)),
	      resource_type(std::move(resource_type)), context(std::move(context)) {}

	parser_error::parser_error(std::string&& resource_type, const std::exception& cause)
	    : error(fmt::format("failed parsing resource of type {} due to [{}]", resource_type, cause.what())),
	      resource_type(std::move(resource_type)), cause(cause) {}

	parser_error::parser_error(std::string&& resource_type, const std::exception& cause, std::string&& context)
	    : error(fmt::format("failed parsing resource of type {} due to [{}] [context: {}]",
	                        resource_type,
	                        cause.what(),
	                        context)),
	      resource_type(std::move(resource_type)), context(std::move(context)), cause(cause) {}

	bool iequals(std::string_view a, std::string_view b) {
		return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](char a, char b) {
			return std::tolower(a) == std::tolower(b);
		});
	}

	date date::parse(buffer& buf) {
		auto dt = date {buf.get_uint(),
		                buf.get_ushort(),
		                buf.get_ushort(),
		                buf.get_ushort(),
		                buf.get_ushort(),
		                buf.get_ushort()};
		(void) buf.get_ushort(); // padding
		return dt;
	}
} // namespace phoenix