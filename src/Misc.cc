// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Misc.hh"

#include <algorithm>
#include <cctype>

namespace zenkit {
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
} // namespace zenkit
