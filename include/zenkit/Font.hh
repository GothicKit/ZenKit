// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"

#include <glm/vec2.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace phoenix {
	class buffer;
}

namespace zenkit {
	class Read;
	class Write;

	/// \brief A single font glyph.
	struct FontGlyph {
		/// \brief The width of the glyph in pixels.
		std::uint8_t width;

		/// \brief The position of the top left and bottom right corners of the glyph in the font texture.
		/// \note These values are not stored as absolute pixels but rather in percent of the width and
		///       height of the image. Thus to calculate the real pixel position of the top left corner,
		///       one multiplies `uv[0].x` by the width of the font texture and `uv[0].y` by its height.
		glm::vec2 uv[2];

		[[nodiscard]] ZKAPI bool operator==(FontGlyph const& g) const noexcept;
	};

	/// \brief Represents a *ZenGin* font file.
	///
	/// <p>Fonts in the *ZenGin* consist of a font definition file and a font texture. This class represents the former.
	/// Font definition files contain a set of glyphs which define the extents of a
	/// [Windows-1252](https://en.wikipedia.org/wiki/Windows-1252) encoded character within the font texture file. Font
	/// files can be identified most easily by their `.FNT` extension or alternatively through the `"1\n"` string at the
	/// beginning of the file.</p>
	///
	/// \see https://zk.gothickit.dev/library/api/font/
	class Font {
	public:
		ZKAPI Font() = default;

		/// \brief Creates a new font from the given values.
		/// \param name The name of the font.
		/// \param height The height of each glyph in pixels.
		/// \param glyphs A list of glyphs of this font. There should always be 256 glyphs in a font.
		/// \warning While *ZenKit* supports an arbitrary number of glyphs for fonts, Gothic and Gothic II always
		///          expect 256 glyphs for all fonts. Should you create a font a number of glyphs not equal to 256 and
		///          try to load it into *ZenGin*, it will fail.
		ZKAPI Font(std::string name, std::uint32_t height, std::vector<FontGlyph> glyphs);

		[[nodiscard]] ZKREM("use ::load()") ZKAPI static Font parse(phoenix::buffer& buf);
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static Font parse(phoenix::buffer&& in);

		ZKAPI void load(Read* r);
		ZKAPI void save(Write* w) const;

		/// \brief The name of this font.
		std::string name;

		/// \brief The height of each glyph of this font in pixels.
		std::uint32_t height {};

		/// \brief All glyphs of this font.
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
		std::vector<FontGlyph> glyphs {};
	};
} // namespace zenkit
