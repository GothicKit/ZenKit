// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"

#include <cstdint>

namespace phoenix {
	class buffer;
}

namespace zenkit {
	class Read;

	/// \brief A basic date and time structure used by the *ZenGin*.
	struct Date {
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static Date parse(phoenix::buffer& buf);

		ZKAPI void load(Read* r);

		std::uint32_t year;
		std::uint16_t month;
		std::uint16_t day;
		std::uint16_t hour;
		std::uint16_t minute;
		std::uint16_t second;
	};
} // namespace zenkit
