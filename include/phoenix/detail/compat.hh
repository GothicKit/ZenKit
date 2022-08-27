// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <ctime>
#include <string_view>

namespace phoenix {
	/// \brief An enum for providing a game version hint to some functions
	enum class game_version {
		gothic_1, ///< Represents any patch of Gothic
		gothic_2, ///< Represents any patch of Gothic II, including _Night of the Raven_.
	};

	/// \brief Converts a ``DOS`` timestamp into a unix timestamp.
	///
	/// This date format does not seem to be documented anywhere, so this implementation
	/// is based on the implementation in PhysicsFS
	/// ([here](https://github.com/icculus/physfs/blob/0d4e9aac4575744ddaae56b146f1be19f064f0e5/src/physfs_archiver_vdf.c#l37)).
	///
	/// \param dos The timestamp to convert.
	/// \return The converted unix timestamp.
	/// \note Assumes the ``DOS`` timestamp is in the ``GMT`` timezone.
	std::time_t dos_to_unix_time(std::uint32_t dos) noexcept;

	/// \brief Converts a unix timestamp (std::time_t) to a ``DOS`` timestamp.
	/// \param tm The unix timestamp to convert
	/// \return The unix timestamp as a ``DOS`` timestamp.
	/// \note This will convert to a ``DOS`` timestamp in the ``GMT`` timezone.
	std::uint32_t unix_time_to_dos(std::time_t tm) noexcept;

	/// \brief Tests whether two strings are equal when ignoring case.
	///
	/// Internally, uses std::tolower to compare the strings character by character.
	///
	/// \param a A string.
	/// \param b Another string.
	/// \return ``true`` if both strings are equal when ignoring case.
	bool iequals(std::string_view a, std::string_view b);
} // namespace phoenix
