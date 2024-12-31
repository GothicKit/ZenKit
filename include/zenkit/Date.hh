// Copyright © 2023-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"

#include <cstdint>

namespace zenkit {
	class Read;
	class Write;

	/// \brief A basic date and time structure used by the *ZenGin*.
	struct Date {
		ZKAPI void load(Read* r);
		ZKAPI void save(Write* w) const;

		std::uint32_t year;
		std::uint16_t month;
		std::uint16_t day;
		std::uint16_t hour;
		std::uint16_t minute;
		std::uint16_t second;
	};
} // namespace zenkit
