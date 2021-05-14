// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
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