// Copyright 2021 Luis Michaelis
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
// Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include <phoenix/detail/error.hh>
#include <phoenix/font.hh>

#include <fmt/format.h>

namespace phoenix {
	font font::read(reader& in) {
		font fnt {};

		auto version = in.read_line();
		if (version != "1") {
			throw parser_error(fmt::format("font: cannot parse font due to version mismatch: expected version '1', got '{}'", version));
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
}// namespace phoenix