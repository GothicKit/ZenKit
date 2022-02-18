// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/detail/error.hh>
#include <phoenix/font.hh>

#include <fmt/format.h>

namespace phoenix {
	font font::parse(reader& in) {
		font fnt {};

		auto version = in.read_line();
		if (version != "1") {
			throw parser_error(
			    fmt::format("font: cannot parse font due to version mismatch: expected version '1', got '{}'",
			                version));
		}

		fnt._m_name = in.read_line(false);
		fnt._m_height = in.read_u32();

		// ZenLib says this is a magic number 0xFF however that is equal to FONT_MAX_GLYPHS so it
		// might be the number of glyphs stored in the font.
		(void) in.read_u32();

		for (auto& _m_glyph : fnt._m_glyphs) {
			_m_glyph.width = in.read_u8();
		}

		for (auto& _m_glyph : fnt._m_glyphs) {
			_m_glyph.uv[0] = in.read_vec2();
		}

		for (auto& _m_glyph : fnt._m_glyphs) {
			_m_glyph.uv[1] = in.read_vec2();
		}

		return fnt;
	}
} // namespace phoenix
