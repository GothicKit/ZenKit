// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Date.hh"
#include "zenkit/Error.hh"
#include "zenkit/Logger.hh"
#include "zenkit/Misc.hh"

#include <filesystem>
#include <functional>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace phoenix {
	class buffer;

	using game_version ZKREM("renamed to zenkit::GameVersion") = zenkit::GameVersion;
	using error ZKREM("renamed to zenkit::Error") = zenkit::Error;
	using parser_error ZKREM("renamed to zenkit::ParserError") = zenkit::ParserError;

	inline bool iequals(std::string_view a, std::string_view b) {
		return zenkit::iequals(a, b);
	}

	inline bool icompare(std::string_view a, std::string_view b) {
		return zenkit::icompare(a, b);
	}

	using date ZKREM("renamed to zenkit::Date") = zenkit::Date;
	using logging ZKREM("renamed to zenkit::Logger") = zenkit::Logger;
} // namespace phoenix
