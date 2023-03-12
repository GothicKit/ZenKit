// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"
#include <phoenix/buffer.hh>

#include <vector>

namespace phoenix {
	constexpr const std::string_view ZTEX_SIGNATURE = "ZTEX";
	constexpr const std::uint16_t ZTEX_PALETTE_ENTRIES = 0x100;

	/// \brief Texture formats used by the ZenGin.
	enum texture_format {
		tex_B8G8R8A8 = 0x0, ///< \brief 32-bit ARGB pixel format with alpha, using 8 bits per channel
		tex_R8G8B8A8 = 0x1, ///< \brief 32-bit ARGB pixel format with alpha, using 8 bits per channel
		tex_A8B8G8R8 = 0x2, ///< \brief 32-bit ARGB pixel format with alpha, using 8 bits per channel
		tex_A8R8G8B8 = 0x3, ///< \brief 32-bit ARGB pixel format with alpha, using 8 bits per channel
		tex_B8G8R8 = 0x4,   ///< \brief 24-bit RGB pixel format with 8 bits per channel
		tex_R8G8B8 = 0x5,   ///< \brief 24-bit RGB pixel format with 8 bits per channel
		tex_A4R4G4B4 = 0x6, ///< \brief 16-bit ARGB pixel format with 4 bits for each channel
		tex_A1R5G5B5 = 0x7, ///< \brief 16-bit pixel format where 5 bits are reserved for each color
		                    ///<        and 1 bit is reserved for alpha
		tex_R5G6B5 = 0x8,   ///< \brief 16-bit RGB pixel format with 5 bits for red, 6 bits for green,
		                    ///<        and 5 bits for blue
		tex_p8 = 0x9,       ///< \brief 8-bit color indexed
		tex_dxt1 = 0xA,     ///< \brief DXT1 compression texture format
		tex_dxt2 = 0xB,     ///< \brief DXT2 compression texture format
		tex_dxt3 = 0xC,     ///< \brief DXT3 compression texture format
		tex_dxt4 = 0xD,     ///< \brief DXT4 compression texture format
		tex_dxt5 = 0xE,     ///< \brief DXT5 compression texture format
	};

	/// \brief Simple ARGB quad.
	struct argb {
		std::uint8_t a, r, g, b;
	};

	/// \brief Represents a ZenGin texture.
	class texture {
	public:
		/// \brief Parses a texture from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).
		///
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed texture.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] PHOENIX_API static texture parse(buffer& in);

		/// \brief Parses a texture from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed texture.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] PHOENIX_API inline static texture parse(buffer&& in) {
			return parse(in);
		}

		/// \return The format of the texture.
		[[nodiscard]] PHOENIX_API inline texture_format format() const noexcept {
			return _m_format;
		}

		/// \return The width in pixels of the first mipmap level.
		[[nodiscard]] PHOENIX_API inline std::uint32_t width() const noexcept {
			return _m_width;
		}

		/// \return The height in pixels of the first mipmap level.
		[[nodiscard]] PHOENIX_API inline std::uint32_t height() const noexcept {
			return _m_height;
		}

		/// \param level The mipmap level to use (beginning from 0).
		/// \return The width in pixels of the given mipmap level.
		[[nodiscard]] PHOENIX_API inline std::uint32_t mipmap_width(std::uint32_t level) const noexcept {
			return _m_width >> level;
		}

		/// \param level The mipmap level to use (beginning from 0).
		/// \return The height in pixels of the given mipmap level.
		[[nodiscard]] PHOENIX_API inline std::uint32_t mipmap_height(std::uint32_t level) const noexcept {
			return _m_height >> level;
		}

		/// \return The width of the texture in-engine.
		[[nodiscard]] PHOENIX_API inline std::uint32_t ref_width() const noexcept {
			return _m_reference_width;
		}

		/// \return The height of the texture in-engine.
		[[nodiscard]] PHOENIX_API inline std::uint32_t ref_height() const noexcept {
			return _m_reference_height;
		}

		/// \return The number of mipmaps of the texture.
		[[nodiscard]] PHOENIX_API inline std::uint32_t mipmaps() const noexcept {
			return _m_mipmap_count;
		}

		/// \return The average color of the texture.
		[[nodiscard]] PHOENIX_API inline std::uint32_t average_color() const noexcept {
			return _m_average_color;
		}

		/// \return The palette of the texture.
		[[nodiscard]] PHOENIX_API inline argb* palette() const noexcept {
			return (argb*) _m_palette;
		}

		/// \param mipmap_level The mipmap level to get.
		/// \return The texture data at the given mipmap level.
		[[nodiscard]] PHOENIX_API inline const std::vector<std::uint8_t>&
		data(std::uint32_t mipmap_level = 0) const noexcept {
			return _m_textures.at(_m_mipmap_count - 1 - mipmap_level);
		}

		/// \brief Converts the texture data of the given mipmap-level to RGBA8
		/// \param mipmap_level The mipmap level of the texture to convert
		/// \return The converted texture data.
		/// \attention This method is very expensive as it allocates a new buffer and copies the internal data into it.
		[[nodiscard]] PHOENIX_API std::vector<std::uint8_t> as_rgba8(std::uint32_t mipmap_level = 0) const;

	private:
		texture() = default;

	private:
		texture_format _m_format {};
		argb _m_palette[ZTEX_PALETTE_ENTRIES] {};
		std::uint32_t _m_width {};
		std::uint32_t _m_height {};
		std::uint32_t _m_reference_width {};
		std::uint32_t _m_reference_height {};
		std::uint32_t _m_mipmap_count {};
		std::uint32_t _m_average_color {};

		// Quirk: largest mipmap (level 0) stored at the end of the vector
		std::vector<std::vector<std::uint8_t>> _m_textures;
	};
} // namespace phoenix
