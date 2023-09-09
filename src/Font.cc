// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Font.hh"
#include "zenkit/Stream.hh"

#include "phoenix/phoenix.hh"

namespace zenkit {
	bool FontGlyph::operator==(FontGlyph const& g) const noexcept {
		return this->width == g.width && this->uv[0] == g.uv[0] && this->uv[1] == g.uv[1];
	}

	Font::Font(std::string font_name, std::uint32_t font_height, std::vector<FontGlyph> font_glyphs)
	    : name(std::move(font_name)), height(font_height), glyphs(std::move(font_glyphs)) {}

	Font Font::parse(phoenix::buffer& in) {
		Font fnt {};

		auto r = Read::from(&in);
		fnt.load(r.get());

		return fnt;
	}

	Font Font::parse(phoenix::buffer&& in) {
		return Font::parse(in);
	}

	void Font::load(Read* r) {
		auto version = r->read_line(true);
		if (version != "1") {
			throw zenkit::ParserError {"Font", "version mismatch: expected version 1, got " + version};
		}

		this->name = r->read_line(false);
		this->height = r->read_uint();

		this->glyphs.resize(r->read_uint());
		for (auto& glyph : glyphs) {
			glyph.width = r->read_ubyte();
		}

		for (auto& glyph : glyphs) {
			glyph.uv[0] = r->read_vec2();
		}

		for (auto& glyph : glyphs) {
			glyph.uv[1] = r->read_vec2();
		}
	}
} // namespace zenkit
