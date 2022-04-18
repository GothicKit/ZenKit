// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/detail/error.hh>
#include <phoenix/font.hh>

#include <fmt/format.h>

namespace phoenix {
	font font::parse(buffer& in) {
		font fnt {};

		auto version = in.get_line();
		if (version != "1") {
			throw parser_error(
			    fmt::format("font: cannot parse font due to version mismatch: expected version '1', got '{}'",
			                version));
		}

		fnt._m_name = in.get_line(false);
		fnt._m_height = in.get_uint();

		// TODO: ZenLib says this is a magic number 0xFF however that is equal to FONT_MAX_GLYPHS so it
		//       might be the number of glyphs stored in the font.
		(void) in.get_uint();

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
	}
} // namespace phoenix
