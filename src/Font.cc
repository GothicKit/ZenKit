// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Font.hh"
#include "zenkit/Error.hh"
#include "zenkit/Stream.hh"

namespace zenkit {
	bool FontGlyph::operator==(FontGlyph const& g) const noexcept {
		return this->width == g.width && this->uv[0] == g.uv[0] && this->uv[1] == g.uv[1];
	}

	Font::Font(std::string font_name, std::uint32_t font_height, std::vector<FontGlyph> font_glyphs)
	    : name(std::move(font_name)), height(font_height), glyphs(std::move(font_glyphs)) {}

	void Font::load(Read* r) {
		if (auto version = r->read_line(true); version != "1") {
			throw ParserError {"Font", "version mismatch: expected version 1, got " + version};
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

	void Font::save(Write* w) const {
		w->write_line("1");
		w->write_line(this->name);
		w->write_uint(this->height);

		w->write_uint(static_cast<uint32_t>(this->glyphs.size()));
		for (auto& glyph : glyphs) {
			w->write_ubyte(glyph.width);
		}

		for (auto& glyph : glyphs) {
			w->write_vec2(glyph.uv[0]);
		}

		for (auto& glyph : glyphs) {
			w->write_vec2(glyph.uv[1]);
		}
	}
} // namespace zenkit
