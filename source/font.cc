// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/font.hh>

#include <fmt/format.h>

namespace phoenix {
	font font::parse(buffer& in) {
		try {
			font fnt {};

			auto version = in.get_line();
			if (version != "1") {
				throw parser_error {"font", fmt::format("version mismatch: expected version '1', got '{}'", version)};
			}

			fnt._m_name = in.get_line(false);
			fnt._m_height = in.get_uint();
			fnt._m_glyphs.resize(in.get_uint());

			for (auto& _m_glyph : fnt._m_glyphs) {
				_m_glyph.width = in.get();
			}

			for (auto& _m_glyph : fnt._m_glyphs) {
				_m_glyph.uv[0] = in.get_vec2();
			}

			for (auto& _m_glyph : fnt._m_glyphs) {
				_m_glyph.uv[1] = in.get_vec2();
			}

			return fnt;
		} catch (const buffer_error& exc) {
			throw parser_error {"font", exc, "eof reached"};
		}
	}
} // namespace phoenix
