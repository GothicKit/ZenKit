// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/font.hh>

#include <fmt/format.h>

namespace phoenix {
	font::font(std::string font_name, std::uint32_t font_height, std::vector<glyph> font_glyphs)
	    : name(std::move(font_name)), height(font_height), glyphs(std::move(font_glyphs)) {}

	font font::parse(buffer& in) {
		try {
			auto version = in.get_line();
			if (version != "1") {
				throw parser_error {"font", fmt::format("version mismatch: expected version '1', got '{}'", version)};
			}

			auto name = in.get_line(false);
			auto height = in.get_uint();

			std::vector<glyph> glyphs {};
			glyphs.resize(in.get_uint());

			for (auto& glyph : glyphs) {
				glyph.width = in.get();
			}

			for (auto& glyph : glyphs) {
				glyph.uv[0] = in.get_vec2();
			}

			for (auto& glyph : glyphs) {
				glyph.uv[1] = in.get_vec2();
			}

			return font {
			    name,
			    height,
			    std::move(glyphs),
			};
		} catch (const buffer_error& exc) {
			throw parser_error {"font", exc, "eof reached"};
		}
	}
} // namespace phoenix
