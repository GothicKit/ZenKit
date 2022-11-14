// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/buffer.hh>
#include <phoenix/phoenix.hh>

#include <fmt/format.h>

#include <utility>

namespace phoenix {
	error::error(std::string&& msg) : std::exception(), message(std::move(msg)) {}

	parser_error::parser_error(std::string&& type)
	    : error(fmt::format("failed parsing resource of type {}", type)), resource_type(type) {}

	parser_error::parser_error(std::string&& type, std::string&& ctx)
	    : error(fmt::format("failed parsing resource of type {} [context: {}]", type, ctx)),
	      resource_type(std::move(type)), context(std::move(ctx)) {}

	parser_error::parser_error(std::string&& type, const std::exception& other_exc)
	    : error(fmt::format("failed parsing resource of type {} due to [{}]", type, other_exc.what())),
	      resource_type(std::move(type)), cause(other_exc) {}

	parser_error::parser_error(std::string&& type, const std::exception& other_exc, std::string&& ctx)
	    : error(
	          fmt::format("failed parsing resource of type {} due to [{}] [context: {}]", type, other_exc.what(), ctx)),
	      resource_type(std::move(type)), context(std::move(ctx)), cause(other_exc) {}

	std::optional<std::function<void(logging::level, const std::string&)>> logging::callback {};

	void logging::use_logger(std::function<void(level, const std::string&)>&& cb) {
		logging::callback = std::forward<decltype(cb)>(cb);
	}

	void logging::use_default_logger() {
		logging::callback = [](level lvl, const std::string& message) {
			switch (lvl) {
			case level::error:
				fmt::print(stderr, "[phoenix] [error] {}\n", message);
				break;
			case level::warn:
				fmt::print(stderr, "[phoenix] [warn ] {}\n", message);
				break;
			case level::info:
				fmt::print(stderr, "[phoenix] [info ] {}\n", message);
				break;
			case level::debug:
				fmt::print(stderr, "[phoenix] [debug] {}\n", message);
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