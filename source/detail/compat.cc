// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/detail/compat.hh>

#include <algorithm>
#include <cctype>

namespace phoenix {
	std::time_t dos_to_unix_time(std::uint32_t dos) noexcept {
		struct tm t {};

		t.tm_year = ((int) ((dos >> 25) & 0x7F)) + 80;
		t.tm_mon = ((int) ((dos >> 21) & 0xF)) - 1;
		t.tm_mday = (int) ((dos >> 16) & 0x1F);
		t.tm_hour = (int) ((dos >> 11) & 0x1F);
		t.tm_min = (int) ((dos >> 5) & 0x3F);
		t.tm_sec = ((int) ((dos >> 0) & 0x1F)) * 2;

		return timegm(&t);
	}

	std::uint32_t unix_time_to_dos(std::time_t nix) noexcept {
		struct std::tm* t {std::gmtime(&nix)};
		std::uint32_t dos {0};

		dos |= (t->tm_year - 80) << 25;
		dos |= (t->tm_mon + 1) << 21;
		dos |= t->tm_mday << 16;
		dos |= t->tm_hour << 11;
		dos |= t->tm_min << 5;
		dos |= (t->tm_sec / 2) << 0;

		return dos;
	}

	bool iequals(std::string_view a, std::string_view b) {
		return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](char a, char b) {
			return tolower(a) == tolower(b);
		});
	}
} // namespace phoenix
