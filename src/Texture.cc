// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Texture.hh"
#include "zenkit/Stream.hh"

#include "phoenix/phoenix.hh"

#include "squish.h"

namespace zenkit {
	constexpr std::string_view const ZTEX_SIGNATURE = "ZTEX";

	/// \brief Calculates the size in bytes of a texture at the given mipmap level.
	/// \return The size in bytes of a texture at the given mipmap level.
	std::uint32_t _ztex_mipmap_size(TextureFormat format, std::uint32_t width, std::uint32_t height, uint32_t level) {
		std::uint32_t x = std::max(1u, width);
		std::uint32_t y = std::max(1u, height);

		for (uint32_t i = 0; i < level; i++) {
			if (x > 1) x >>= 1;
			if (y > 1) y >>= 1;
		}

		switch (format) {
		case TextureFormat::B8G8R8A8:
		case TextureFormat::R8G8B8A8:
		case TextureFormat::A8B8G8R8:
		case TextureFormat::A8R8G8B8:
			return x * y * 4;
		case TextureFormat::B8G8R8:
		case TextureFormat::R8G8B8:
			return x * y * 3;
		case TextureFormat::A4R4G4B4:
		case TextureFormat::A1R5G5B5:
		case TextureFormat::R5G6B5:
			return x * y * 2;
		case TextureFormat::P8:
			return x * y;
		case TextureFormat::DXT1:
			return std::max(1u, x / 4) * std::max(1u, y / 4) * 8;
		case TextureFormat::DXT2:
		case TextureFormat::DXT3:
		case TextureFormat::DXT4:
		case TextureFormat::DXT5:
			return std::max(1u, x / 4) * std::max(1u, y / 4) * 16;
		default:
			return 0;
		}
	}

	Texture Texture::parse(phoenix::buffer& in) {
		Texture tex;

		auto r = Read::from(&in);
		tex.load(r.get());

		return tex;
	}

	Texture Texture::parse(phoenix::buffer&& in) {
		return Texture::parse(in);
	}

	void Texture::load(Read* r) {
		if (r->read_string(4) != ZTEX_SIGNATURE) {
			throw zenkit::ParserError {"texture", "invalid signature"};
		}

		auto version = r->read_uint();
		if (version != 0) {
			throw zenkit::ParserError {"texture", "invalid version"};
		}

		this->_m_format = static_cast<TextureFormat>(r->read_uint());
		this->_m_width = r->read_uint();
		this->_m_height = r->read_uint();
		this->_m_mipmap_count = std::max(1u, r->read_uint());
		this->_m_reference_width = r->read_uint();
		this->_m_reference_height = r->read_uint();
		this->_m_average_color = r->read_uint();

		if (this->_m_format == TextureFormat::P8) {
			for (auto& i : this->_m_palette) {
				i.b = r->read_ubyte();
				i.g = r->read_ubyte();
				i.r = r->read_ubyte();
				i.a = r->read_ubyte();
			}
		}

		// Lowest mipmap-level first
		for (std::int64_t level = this->_m_mipmap_count - 1; level >= 0; --level) {
			auto size = _ztex_mipmap_size(this->_m_format, this->_m_width, this->_m_height, level);

			std::vector<std::uint8_t> mipmap;
			mipmap.resize(size);
			r->read(mipmap.data(), size);

			this->_m_textures.emplace_back(std::move(mipmap));
		}
	}

#pragma pack(push, 1)
	struct r5g5b5 {
		std::uint16_t r : 5;
		std::uint16_t g : 5;
		std::uint16_t b : 5;
	};

	struct r5g6b5 {
		std::uint16_t r : 5;
		std::uint16_t g : 6;
		std::uint16_t b : 5;
	};
#pragma pack(pop)

	static_assert(sizeof(r5g5b5) == 2);

	std::vector<std::uint8_t> Texture::as_rgba8(std::uint32_t mipmap_level) const {
		std::vector<std::uint8_t> conv;

		switch (_m_format) {
		case TextureFormat::DXT1:
		case TextureFormat::DXT3:
		case TextureFormat::DXT5: {
			auto const& map = data(mipmap_level);
			auto w = mipmap_width(mipmap_level);
			auto h = mipmap_height(mipmap_level);
			conv.resize(w * h * 4);

			auto flag = _m_format == TextureFormat::DXT1
			    ? squish::kDxt1
			    : (_m_format == TextureFormat::DXT3 ? squish::kDxt3 : squish::kDxt5);

			squish::DecompressImage(conv.data(), static_cast<int>(w), static_cast<int>(h), map.data(), flag);
			return conv;
		}
		case TextureFormat::B8G8R8A8: {
			auto const& map = data(mipmap_level);
			conv.resize(map.size());

			for (std::uint32_t i = 0; i < map.size(); i += 4) {
				conv[i + 0] = map[i + 2];
				conv[i + 1] = map[i + 1];
				conv[i + 2] = map[i + 0];
				conv[i + 3] = map[i + 3];
			}

			return conv;
		}
		case TextureFormat::R8G8B8A8:
			return data(mipmap_level);
		case TextureFormat::A8B8G8R8: {
			auto const& map = data(mipmap_level);
			conv.resize(map.size());

			for (std::uint32_t i = 0; i < map.size(); i += 4) {
				conv[i + 0] = map[i + 3];
				conv[i + 1] = map[i + 2];
				conv[i + 2] = map[i + 1];
				conv[i + 3] = map[i + 0];
			}

			return conv;
		}
		case TextureFormat::A8R8G8B8: {
			auto const& map = data(mipmap_level);
			conv.resize(map.size());

			for (std::uint32_t i = 0; i < map.size(); i += 4) {
				conv[i + 0] = map[i + 1];
				conv[i + 1] = map[i + 2];
				conv[i + 2] = map[i + 3];
				conv[i + 3] = map[i + 0];
			}

			return conv;
		}
		case TextureFormat::B8G8R8: {
			auto const& map = data(mipmap_level);
			conv.resize(map.size());

			for (std::uint32_t i = 0; i < map.size(); i += 3) {
				conv[i + 0] = map[i + 2];
				conv[i + 1] = map[i + 1];
				conv[i + 2] = map[i + 0];
				conv[i + 3] = 0;
			}

			return conv;
		}
		case TextureFormat::R8G8B8: {
			auto const& map = data(mipmap_level);
			conv.resize(map.size());

			for (std::uint32_t i = 0; i < map.size(); i += 3) {
				conv[i + 0] = map[i + 0];
				conv[i + 1] = map[i + 1];
				conv[i + 2] = map[i + 2];
				conv[i + 3] = 0;
			}

			return conv;
		}
		case TextureFormat::R5G6B5: {
			auto const& map = data(mipmap_level);
			conv.resize(map.size() * 2);

			std::uint32_t idx = 0;
			for (std::uint32_t i = 0; i < map.size(); i += 2) {
				auto* rgb = (r5g6b5*) &map[i];
				conv[idx++] = static_cast<uint8_t>(static_cast<float>(rgb->b) * 8.225806452f);
				conv[idx++] = static_cast<uint8_t>(static_cast<float>(rgb->g) * 4.047619048f);
				conv[idx++] = static_cast<uint8_t>(static_cast<float>(rgb->r) * 8.225806452f);
				conv[idx++] = 0xff;
			}

			return conv;
		}
		case TextureFormat::P8: {
			auto const& map = data(mipmap_level);
			conv.resize(map.size() * sizeof(std::uint8_t) * 4);

			for (std::uint32_t i = 0; i < map.size(); ++i) {
				auto palentry = _m_palette[map[i]];
				conv[i + 0] = palentry.r;
				conv[i + 1] = palentry.g;
				conv[i + 2] = palentry.b;
				conv[i + 3] = palentry.a;
			}

			return conv;
		}
		default:
			throw zenkit::ParserError {"texture",
			                           "cannot convert format to rgba: " +
			                               std::to_string(static_cast<int32_t>(_m_format))};
		}
	}
} // namespace zenkit
