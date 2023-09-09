// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"

#include <string_view>

namespace zenkit {
	/// \brief An enum for providing a game version hint to some functions
	enum class GameVersion {
		GOTHIC_1, ///< Represents any patch of Gothic
		GOTHIC_2, ///< Represents any patch of Gothic II, including _Night of the Raven_.

		gothic_1 ZKREM("renamed to GOTHIC_1") = GOTHIC_1, ///< Represents any patch of Gothic
		gothic_2 ZKREM("renamed to GOTHIC_2") =
		    GOTHIC_2, ///< Represents any patch of Gothic II, including _Night of the Raven_.
	};

	/// \brief Tests whether two strings are equal when ignoring case.
	///
	/// Internally, uses std::tolower to compare the strings character by character.
	///
	/// \param a A string.
	/// \param b Another string.
	/// \return ``true`` if both strings are equal when ignoring case.
	ZKAPI bool iequals(std::string_view a, std::string_view b);

	/// \brief Tests whether \p a is lexicographically less than \p b.
	///
	/// Internally, uses std::tolower to compare the strings character by character.
	///
	/// \param a A string.
	/// \param b Another string.
	/// \return ``true`` if \p a is lexicographically less than \p b.
	ZKAPI bool icompare(std::string_view a, std::string_view b);
} // namespace zenkit
