// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <ctime>
#include <string_view>

namespace phoenix {
	enum class game_version { gothic_1, gothic_2 };

	/**
	 * @brief Converts a `DOS` timestamp into a unix timestamp.
	 *
	 * Adapted from PhysicFS' VDF implementation:
	 * https://hg.icculus.org/icculus/physfs/file/tip/src/physfs_archiver_vdf.c#l37
	 *
	 * Thanks to Francesco Bertolaccini for the original implementation!
	 *
	 * @param dos The timestamp to convert.
	 * @return The converted unix timestamp.
	 * @note Assumes the `DOS` timestamp is in the `GMT` timezone.
	 */
	std::time_t dos_to_unix_time(std::uint32_t dos) noexcept;

	/**
	 * @brief Converts a unix timestamp (std::time_t) to a `DOS` timestamp.
	 * @param tm The unix timestamp to convert
	 * @return The unix timestamp as a `DOS` timestamp.
	 * @note This will convert to a `DOS` timestamp in the `GMT` timezone.
	 */
	std::uint32_t unix_time_to_dos(std::time_t tm) noexcept;

	/**
	 * @brief Tests whether two strings are equal when ignoring case.
	 * @param a A string.
	 * @param b Another string.
	 * @return `true` if both strings are equal when ignoring case.
	 */
	bool iequals(std::string_view a, std::string_view b);
} // namespace phoenix
