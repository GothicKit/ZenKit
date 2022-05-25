// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/detail/buffer.hh>

#include <glm/vec2.hpp>

#include <vector>

namespace phoenix {
	/**
	 * @brief Represents one glyph.
	 */
	struct glyph {
		std::uint8_t width;
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
		 * @brief Parses in a font from the given reader.
		 * @param in The reader to read from
		 * @return The font parsed.
		 */
		static font parse(buffer& in);

		/**
		 * @return The name of the font
		 */
		[[nodiscard]] inline const std::string& name() const noexcept {
			return _m_name;
		}

		/**
		 * @return The height of the font
		 */
		[[nodiscard]] inline std::uint32_t height() const noexcept {
			return _m_height;
		}

		/**
		 * @return The glyphs in the font (length of FONT_MAX_GLYPHS)
		 */
		[[nodiscard]] inline const std::vector<glyph>& glyphs() const noexcept {
			return _m_glyphs;
		}

	protected:
		font() = default;

	private:
		std::string _m_name;
		std::uint32_t _m_height;
		std::vector<glyph> _m_glyphs;
	};

} // namespace phoenix
