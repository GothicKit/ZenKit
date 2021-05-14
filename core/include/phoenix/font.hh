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
#pragma once
#include <phoenix/detail/stream.hh>
#include <phoenix/detail/types.hh>

#include <glm/vec2.hpp>

namespace phoenix {
	static const int FONT_MAX_GLYPHS = 256;

	/**
	 * @brief Represents one glyph.
	 */
	struct glyph {
		u8 width;
		glm::vec2 uv[2];
	};

	/**
	 * @brief Represents a font file (.FNT).
	 *
	 * Parses ZenGin font files. The reference implementation can be found on GitHub:
	 * https://github.com/Try/ZenLib/blob/732077c82589f5060d1762839293b996c8222c18/zenload/zCFont.cpp and
	 * the original version by Andre Taulien was also referenced:
	 * https://github.com/ataulien/ZenLib/blob/e1a5e1b12e71690a5470f3be2aa3d0d6419f5191/zenload/zCFont.cpp
	 *
	 * Thanks to the original authors, Andre Taulien and kapitaenzufall as well as Try for additional work on their
	 * ZenLib fork!
	 *
	 * @see https://github.com/Try/ZenLib
	 * @see https://github.com/ataulien/ZenLib
	 */
	class font {
	public:
		/**
		 * @brief Reads in a font from the given reader.
		 * @param in The reader to read from
		 * @return The font read.
		 */
		static font read(reader& in);

		/**
		 * @return The name of the font
		 */
		[[nodiscard]] inline const std::string& name() const noexcept { return _m_name; }

		/**
		 * @return The height of the font
		 */
		[[nodiscard]] inline u32 height() const noexcept { return _m_height; }

		/**
		 * @return The glyphs in the font (length of FONT_MAX_GLYPHS)
		 */
		[[nodiscard]] inline const glyph* glyphs() const noexcept { return _m_glyphs; }

	protected:
		font() = default;

	private:
		std::string _m_name;
		u32 _m_height;
		glyph _m_glyphs[FONT_MAX_GLYPHS];
	};

}// namespace phoenix