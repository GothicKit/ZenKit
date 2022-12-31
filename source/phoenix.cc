// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/buffer.hh>
#include <phoenix/phoenix.hh>

#include <iostream>
#include <utility>

namespace phoenix {
	error::error(std::string&& msg) : std::exception(), message(std::move(msg)) {}

	parser_error::parser_error(std::string&& type)
	    : error("failed parsing resource of type " + type), resource_type(type) {}

	parser_error::parser_error(std::string&& type, std::string&& ctx)
	    : error("failed parsing resource of type " + type + " [context: " + ctx + "]"), resource_type(std::move(type)),
	      context(std::move(ctx)) {}

	parser_error::parser_error(std::string&& type, const std::exception& other_exc)
	    : error("failed parsing resource of type " + type + " due to [" + other_exc.what() + "]"),
	      resource_type(std::move(type)), cause(other_exc) {}

	parser_error::parser_error(std::string&& type, const std::exception& other_exc, std::string&& ctx)
	    : error("failed parsing resource of type " + type + " due to [" + other_exc.what() + "] [context: " + ctx +
	            "]"),
	      resource_type(std::move(type)), context(std::move(ctx)), cause(other_exc) {}

	std::optional<std::function<void(logging::level, const std::string&)>> logging::callback {};

	void logging::use_logger(std::function<void(level, const std::string&)>&& cb) {
		logging::callback = std::forward<decltype(cb)>(cb);
	}

	void logging::use_default_logger() {
		logging::callback = [](level lvl, const std::string& message) {
			switch (lvl) {
			case level::error:
				std::cerr << "[phoenix] [error] " << message << "\n";
				break;
			case level::warn:
				std::cerr << "[phoenix] [warn ] " << message << "\n";
				break;
			case level::info:
				std::cerr << "[phoenix] [info ] " << message << "\n";
				break;
			case level::debug:
				std::cerr << "[phoenix] [debug] " << message << "\n";
				break;
			}
		};
	}

	bool iequals(std::string_view a, std::string_view b) {
		return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](char c1, char c2) {
			return std::tolower(c1) == std::tolower(c2);
		});
	}

	bool icompare(std::string_view a, std::string_view b) {
		return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), [](char c1, char c2) {
			return std::tolower(c1) < std::tolower(c2);
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
