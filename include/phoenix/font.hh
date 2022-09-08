// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/buffer.hh>

#include <glm/vec2.hpp>

#include <vector>

namespace phoenix {
	/// \brief A single font glyph.
	struct glyph {
		/// \brief The width of the glyph in pixels.
		std::uint8_t width;

		/// \brief The position of the top left and bottom right corners of the glyph in the font texture.
		/// \note These values are not stored as absolute pixels but rather in percent of the width and
		///       height of the image. Thus to calculate the real pixel position of the top right corner,
		///       one multiplies `uv[0].x` by the width of the font texture and `uv[0].y` by its height.
		glm::vec2 uv[2];
	};

	/// \brief Represents a *ZenGin* font file.
	///
	/// <p>Fonts in the *ZenGin* consist of a font definition file and a font texture. This class represents the former.
	/// Font definition files contain a set of glyphs which define the extents of a
	/// [Windows-1252](https://en.wikipedia.org/wiki/Windows-1252) encoded character within the font texture file. Font
	/// files can be identified most easily by their `.FNT` extension or alternatively through the `"1\n"` string at the
	/// beginning of the file.</p>
	class font {
	public:
		/// \brief Creates a new font from the given values.
		/// \param name The name of the font.
		/// \param height The height of each glyph in pixels.
		/// \param glyphs A list of glyphs of this font. There should always be 256 glyphs in a font.
		/// \warning While *phoenix* supports an arbitrary number of glyphs for fonts, Gothic and Gothic II always
		///          expect 256 glyphs for all fonts. Should you create a font a number of glyphs not equal to 256 and
		///          try to load it into *ZenGin*, it will fail.
		font(std::string name, std::uint32_t height, std::vector<glyph> glyphs);

		/// \brief Parses a font from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib). The only change to the parsing logic is, that this implementation
		/// dynamically allocates the number of glyphs from the glyph count stored within the font file. That change is
		/// incompatible with the original *ZenGin*.</p>
		///
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed font object.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] static font parse(buffer& buf);

		/// \brief Parses a font from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed font object.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] inline static font parse(buffer&& in) {
			return font::parse(in);
		}

		/// \brief Returns the name of this font.
		/// \return The name of this font.
		[[nodiscard]] inline const std::string& name() const noexcept {
			return _m_name;
		}

		/// \brief Returns the height of glyphs of this font in pixels.
		/// \return The height of glyphs of this font in pixels.
		[[nodiscard]] inline std::uint32_t height() const noexcept {
			return _m_height;
		}

		/// \brief Returns all glyphs of this font.
		///
		/// <p>*ZenGin* fonts contain characters present in the [Windows-1252 character
		/// encoding](https://en.wikipedia.org/wiki/Windows-1252) which is generally used by *Gothic* and *Gothic II*.
		/// The returned vector contains these glyphs in order.</p>
		///
		/// <p>Some glyphs may not actually be present in the font texture. For those glyphs, the stored UV-coordinates
		/// will be invalid in some way (ie. they may point to a pixel not actually in the texture or be negative).</p>
		///
		/// \note The glyphs UV-coordinates are not straightforward. Refer to glyph::uv for an explanation about to
		///       how to use them
		/// \return All glyphs of this font.
		[[nodiscard]] inline const std::vector<glyph>& glyphs() const noexcept {
			return _m_glyphs;
		}

	private:
		std::string _m_name;
		std::uint32_t _m_height;
		std::vector<glyph> _m_glyphs;
	};
} // namespace phoenix
