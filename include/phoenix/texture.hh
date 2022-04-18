// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/detail/buffer.hh>

#include <vector>

namespace phoenix {
	constexpr const std::string_view ZTEX_SIGNATURE = "ZTEX";
	constexpr const std::uint16_t ZTEX_PALETTE_ENTRIES = 0x100;

	/**
	 * @brief Texture formats used by the ZenGin.
	 */
	enum texture_format {
		tex_B8G8R8A8, /* 0, 32-bit ARGB pixel format with alpha, using 8 bits per channel */
		tex_R8G8B8A8, /* 1, 32-bit ARGB pixel format with alpha, using 8 bits per channel */
		tex_A8B8G8R8, /* 2, 32-bit ARGB pixel format with alpha, using 8 bits per channel */
		tex_A8R8G8B8, /* 3, 32-bit ARGB pixel format with alpha, using 8 bits per channel */
		tex_B8G8R8,   /* 4, 24-bit RGB pixel format with 8 bits per channel */
		tex_R8G8B8,   /* 5, 24-bit RGB pixel format with 8 bits per channel */
		tex_A4R4G4B4, /* 6, 16-bit ARGB pixel format with 4 bits for each channel */
		tex_A1R5G5B5, /* 7, 16-bit pixel format where 5 bits are reserved for each glm::u8vec4 and 1 bit is reserved for
		               * alpha
		               */
		tex_R5G6B5,   /* 8, 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue */
		tex_p8,       /* 9, 8-bit glm::u8vec4 indexed */
		tex_dxt1,     /* A, DXT1 compression texture format */
		tex_dxt2,     /* B, DXT2 compression texture format */
		tex_dxt3,     /* C, DXT3 compression texture format */
		tex_dxt4,     /* D, DXT4 compression texture format */
		tex_dxt5,     /* E, DXT5 compression texture format */
	};

	/**
	 * @brief Simple ARGB quad.
	 */
	struct argb {
		std::uint8_t a, r, g, b;
	};

	/**
	 * @brief Represents a texture (.TEX).
	 *
	 * The reference implementation can be found on GitHub:
	 * https://github.com/ataulien/ZenLib/blob/e1a5e1b12e71690a5470f3be2aa3d0d6419f5191/zenload/ztex2dds.cpp and an
	 * adapted version used by OpenGothic was also referenced:
	 * https://github.com/Try/ZenLib/blob/732077c82589f5060d1762839293b996c8222c18/zenload/ztex2dds.cpp
	 *
	 * Thanks to the original authors, Nico Bendlin and Andre Taulien as well as Try for additional work on their
	 * ZenLib fork!
	 *
	 * @see https://github.com/ataulien/ZenLib
	 * @see https://github.com/Try/ZenLib
	 */
	class texture {
	public:
		/**
		 * @brief Parses a texture from the given reader.
		 *
		 * If the texture is compressed using DXT1, DXT3 or DXT5 it is automatically decompressed.
		 *
		 * @param in The reader to read from
		 * @return The texture.
		 */
		static texture parse(buffer& in);

		/**
		 * @brief Parses a texture from the given file.
		 * @param path The path to a file to read.
		 * @return The texture.
		 * @see #parse
		 */
		static texture parse(const std::string& path);

		/**
		 * @return The format of the texture.
		 */
		[[nodiscard]] inline texture_format format() const noexcept {
			return _m_format;
		}

		/**
		 * @return The width in pixels of the first mipmap level.
		 */
		[[nodiscard]] inline std::uint32_t width() const noexcept {
			return _m_width;
		}

		/**
		 * @return The height in pixels of the first mipmap level.
		 */
		[[nodiscard]] inline std::uint32_t height() const noexcept {
			return _m_height;
		}

		/**
		 * @param level The mipmap level to use (beginning from 0).
		 * @return The width in pixels of the given mipmap level.
		 */
		[[nodiscard]] inline std::uint32_t mipmap_width(std::uint32_t level) const noexcept {
			return _m_width >> level;
		}

		/**
		 * @param level The mipmap level to use (beginning from 0).
		 * @return The height in pixels of the given mipmap level.
		 */
		[[nodiscard]] inline std::uint32_t mipmap_height(std::uint32_t level) const noexcept {
			return _m_height >> level;
		}

		/**
		 * @return The width of the texture in-engine.
		 */
		[[nodiscard]] inline std::uint32_t ref_width() const noexcept {
			return _m_reference_width;
		}

		/**
		 * @return The height of the texture in-engine.
		 */
		[[nodiscard]] inline std::uint32_t ref_height() const noexcept {
			return _m_reference_height;
		}

		/**
		 * @return The number of mipmaps of the texture.
		 */
		[[nodiscard]] inline std::uint32_t mipmaps() const noexcept {
			return _m_mipmap_count;
		}

		/**
		 * @return The average glm::u8vec4 of the texture.
		 */
		[[nodiscard]] inline argb average_color() const noexcept {
			return _m_average_color;
		}

		/**
		 * @param mipmap_level The mipmap level to get.
		 * @return The texture data at the given mipmap level.
		 */
		[[nodiscard]] inline const std::vector<std::uint8_t>& data(std::uint32_t mipmap_level = 0) const noexcept {
			return _m_textures.at(_m_mipmap_count - 1 - mipmap_level);
		}

		/**
		 * @brief Converts the texture data of the given mipmap-level to RGBA8
		 * @param mipmap_level The mipmap level of the texture to convert
		 * @return The converted texture data.
		 * @attention This method is very expensive as it allocates a new buffer and copies the internal data into it.
		 */
		[[nodiscard]] std::vector<std::uint8_t> as_rgba8(std::uint32_t mipmap_level = 0) const;

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
		argb _m_average_color {};

		// Quirk: largest mipmap (level 0) stored at the end of the vector
		std::vector<std::vector<std::uint8_t>> _m_textures;
	};
} // namespace phoenix
