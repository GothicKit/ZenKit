// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Texture.hh"
#include "zenkit/Stream.hh"

#include "phoenix/phoenix.hh"

#include "squish.h"

#include <cstring>

namespace zenkit {
	constexpr std::string_view ZTEX_SIGNATURE = "ZTEX";

#pragma pack(push, 1)
	struct r5g6b5 {
		std::uint16_t b : 5;
		std::uint16_t g : 6;
		std::uint16_t r : 5;
	};

	struct a4r4g4b4 {
		std::uint16_t b : 4;
		std::uint16_t g : 4;
		std::uint16_t r : 4;
		std::uint16_t a : 4;
	};

	struct a1r5g5b5 {
		std::uint16_t b : 5;
		std::uint16_t g : 5;
		std::uint16_t r : 5;
		std::uint16_t a : 1;
	};
#pragma pack(pop)

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

	std::vector<std::uint8_t> _ztex_to_rgba(uint8_t const* bytes, uint32_t width, uint32_t height, TextureFormat src) {
		std::vector<std::uint8_t> conv;
		conv.resize(width * height * 4);

		switch (src) {
		case TextureFormat::DXT1:
			squish::DecompressImage( //
			    conv.data(),
			    static_cast<int>(width),
			    static_cast<int>(height),
			    bytes,
			    squish::kDxt1);
			break;
		case TextureFormat::DXT3:
			squish::DecompressImage( //
			    conv.data(),
			    static_cast<int>(width),
			    static_cast<int>(height),
			    bytes,
			    squish::kDxt3);
			break;
		case TextureFormat::DXT5: {
			squish::DecompressImage( //
			    conv.data(),
			    static_cast<int>(width),
			    static_cast<int>(height),
			    bytes,
			    squish::kDxt5);
			break;
		}
		case TextureFormat::B8G8R8A8:
			for (auto i = 0u; i < width * height; ++i) {
				conv[i * 4 + 0] = bytes[i * 4 + 2];
				conv[i * 4 + 1] = bytes[i * 4 + 1];
				conv[i * 4 + 2] = bytes[i * 4 + 0];
				conv[i * 4 + 3] = bytes[i * 4 + 3];
			}
			break;
		case TextureFormat::R8G8B8A8:
			conv.assign(bytes, bytes + width * height * 4);
			break;
		case TextureFormat::A8B8G8R8:
			for (auto i = 0u; i < width * height; ++i) {
				conv[i * 4 + 0] = bytes[i * 4 + 3];
				conv[i * 4 + 1] = bytes[i * 4 + 2];
				conv[i * 4 + 2] = bytes[i * 4 + 1];
				conv[i * 4 + 3] = bytes[i * 4 + 0];
			}

			break;
		case TextureFormat::A8R8G8B8: {
			for (auto i = 0u; i < width * height; ++i) {
				conv[i * 4 + 0] = bytes[i * 4 + 1];
				conv[i * 4 + 1] = bytes[i * 4 + 2];
				conv[i * 4 + 2] = bytes[i * 4 + 3];
				conv[i * 4 + 3] = bytes[i * 4 + 0];
			}

			break;
		}
		case TextureFormat::B8G8R8: {
			for (std::uint32_t i = 0; i < width * height; ++i) {
				conv[i * 4 + 0] = bytes[i * 3 + 2];
				conv[i * 4 + 1] = bytes[i * 3 + 1];
				conv[i * 4 + 2] = bytes[i * 3 + 0];
				conv[i * 4 + 3] = 0xff;
			}

			break;
		}
		case TextureFormat::R8G8B8: {
			for (std::uint32_t i = 0; i < width * height; ++i) {
				conv[i * 4 + 0] = bytes[i * 3 + 0];
				conv[i * 4 + 1] = bytes[i * 3 + 1];
				conv[i * 4 + 2] = bytes[i * 3 + 2];
				conv[i * 4 + 3] = 0xff;
			}

			break;
		}
		case TextureFormat::R5G6B5: {
			for (auto i = 0u; i < width * height; ++i) {
				auto* rgb = reinterpret_cast<r5g6b5 const*>(&bytes[i * 2]);
				conv[i * 4 + 0] = static_cast<uint8_t>(static_cast<float>(rgb->r) * 8.225806452f);
				conv[i * 4 + 1] = static_cast<uint8_t>(static_cast<float>(rgb->g) * 4.047619048f);
				conv[i * 4 + 2] = static_cast<uint8_t>(static_cast<float>(rgb->b) * 8.225806452f);
				conv[i * 4 + 3] = 0xff;
			}

			break;
		}
		case TextureFormat::A4R4G4B4: {
			for (auto i = 0u; i < width * height; ++i) {
				conv[i * 4 + 2] = ((bytes[i * 2 + 0] >> 0) & 0x0F) * 17;
				conv[i * 4 + 1] = ((bytes[i * 2 + 0] >> 4) & 0x0F) * 17;
				conv[i * 4 + 0] = ((bytes[i * 2 + 1] >> 0) & 0x0F) * 17;
				conv[i * 4 + 3] = ((bytes[i * 2 + 1] >> 4) & 0x0F) * 17;
			}

			break;
		}
		default:
			throw ParserError {"texture",
			                   "cannot convert format to rgba: " + std::to_string(static_cast<int32_t>(src))};
		}

		return conv;
	}

	std::vector<std::uint8_t>
	_ztex_from_rgba(uint8_t const* bytes, uint32_t width, uint32_t height, TextureFormat dest) {
		std::vector<std::uint8_t> conv;

		switch (dest) {
		case TextureFormat::B8G8R8A8:
			conv.resize(width * height * 4);
			for (auto i = 0u; i < width * height; ++i) {
				conv[i + 0] = bytes[i + 2];
				conv[i + 1] = bytes[i + 1];
				conv[i + 2] = bytes[i + 0];
				conv[i + 3] = bytes[i + 3];
			}

			break;
		case TextureFormat::R8G8B8A8:
			conv.assign(bytes, bytes + width * height * 4);
			break;
		case TextureFormat::A8B8G8R8:
			conv.resize(width * height * 4);
			for (auto i = 0u; i < width * height; ++i) {
				conv[i * 4 + 0] = bytes[i * 4 + 3];
				conv[i * 4 + 1] = bytes[i * 4 + 2];
				conv[i * 4 + 2] = bytes[i * 4 + 1];
				conv[i * 4 + 3] = bytes[i * 4 + 0];
			}
			break;
		case TextureFormat::A8R8G8B8:
			conv.resize(width * height * 4);
			for (auto i = 0u; i < width * height; ++i) {
				conv[i * 4 + 0] = bytes[i * 4 + 1];
				conv[i * 4 + 1] = bytes[i * 4 + 2];
				conv[i * 4 + 2] = bytes[i * 4 + 3];
				conv[i * 4 + 3] = bytes[i * 4 + 0];
			}
			break;
		case TextureFormat::B8G8R8:
			conv.resize(width * height * 3);
			for (std::uint32_t i = 0; i < width * height; ++i) {
				conv[i * 3 + 0] = bytes[i * 4 + 2];
				conv[i * 3 + 1] = bytes[i * 4 + 1];
				conv[i * 3 + 2] = bytes[i * 4 + 0];
			}
			break;
		case TextureFormat::R8G8B8:
			conv.resize(width * height * 3);
			for (std::uint32_t i = 0; i < width * height; ++i) {
				conv[i * 3 + 0] = bytes[i * 4 + 0];
				conv[i * 3 + 1] = bytes[i * 4 + 1];
				conv[i * 3 + 2] = bytes[i * 4 + 2];
			}
			break;
		case TextureFormat::R5G6B5:
			conv.resize(width * height * 2);

			for (std::uint32_t i = 0; i < width * height; ++i) {
				r5g6b5 rgb {
				    static_cast<uint8_t>(static_cast<double>(bytes[i * 4 + 0]) / (5. / 8.)),
				    static_cast<uint8_t>(static_cast<double>(bytes[i * 4 + 1]) / (6. / 8.)),
				    static_cast<uint8_t>(static_cast<double>(bytes[i * 4 + 2]) / (5. / 8.)),
				};

				memcpy(conv.data() + i * 2, &rgb, sizeof(rgb));
			}

			break;
		case TextureFormat::DXT1:
			squish::CompressImage(bytes, static_cast<int>(width), static_cast<int>(height), conv.data(), squish::kDxt1);
			break;
		case TextureFormat::DXT3:
			squish::CompressImage(bytes, static_cast<int>(width), static_cast<int>(height), conv.data(), squish::kDxt3);
			break;
		case TextureFormat::DXT5:
			squish::CompressImage(bytes, static_cast<int>(width), static_cast<int>(height), conv.data(), squish::kDxt5);
			break;
		default:
			throw ParserError {"texture",
			                   "cannot convert format from rgba: " + std::to_string(static_cast<int32_t>(dest))};
		}

		return conv;
	}

	std::vector<std::uint8_t> _ztex_convert_format(uint8_t const* bytes,
	                                               uint32_t width,
	                                               uint32_t height,
	                                               TextureFormat from,
	                                               TextureFormat into) {
		if (from == into) {
			std::vector<std::uint8_t> conv;
			conv.assign(bytes, bytes + _ztex_mipmap_size(from, width, height, 0));
			return conv;
		}

		auto rgba = _ztex_to_rgba(bytes, width, height, from);
		return _ztex_from_rgba(rgba.data(), width, height, into);
	}

	Texture Texture::parse(phoenix::buffer& in) {
		Texture tex;

		auto r = Read::from(&in);
		tex.load(r.get());

		return tex;
	}

	Texture Texture::parse(phoenix::buffer&& in) {
		return parse(in);
	}

	void Texture::load(Read* r) {
		if (r->read_string(4) != ZTEX_SIGNATURE) {
			throw ParserError {"texture", "invalid signature"};
		}

		if (r->read_uint() != 0) {
			throw ParserError {"texture", "invalid version"};
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
			auto size =
			    _ztex_mipmap_size(this->_m_format, this->_m_width, this->_m_height, static_cast<uint32_t>(level));

			std::vector<std::uint8_t> mipmap;
			mipmap.resize(size);
			r->read(mipmap.data(), size);

			this->_m_textures.emplace_back(std::move(mipmap));
		}
	}

	std::vector<std::uint8_t> Texture::as_rgba8(std::uint32_t mipmap_level) const {

		auto const& map = data(mipmap_level);
		auto width = mipmap_width(mipmap_level);
		auto height = mipmap_height(mipmap_level);

		if (_m_format == TextureFormat::P8) {
			std::vector<std::uint8_t> conv;
			for (auto i = 0u; i < width * height; ++i) {
				auto palentry = _m_palette[map[i]];
				conv[i * 4 + 0] = palentry.r;
				conv[i * 4 + 1] = palentry.g;
				conv[i * 4 + 2] = palentry.b;
				conv[i * 4 + 3] = palentry.a;
			}
			return conv;
		}

		return _ztex_to_rgba(map.data(), width, height, _m_format);
	}

	void Texture::save(Write* w) const {
		w->write_string(ZTEX_SIGNATURE);
		w->write_uint(0);
		w->write_uint(static_cast<uint32_t>(this->format()));
		w->write_uint(this->width());
		w->write_uint(this->height());
		w->write_uint(this->mipmaps());
		w->write_uint(this->ref_width());
		w->write_uint(this->ref_height());
		w->write_uint(this->average_color());

		if (this->format() == TextureFormat::P8) {
			for (auto& v : this->_m_palette) {
				w->write_ubyte(v.b);
				w->write_ubyte(v.g);
				w->write_ubyte(v.r);
				w->write_ubyte(v.a);
			}
		}

		for (auto& m : this->_m_textures) {
			w->write(m.data(), m.size());
		}
	}

	TextureBuilder::TextureBuilder(uint32_t width, uint32_t height) : _m_width(width), _m_height(height) {}

	TextureBuilder& TextureBuilder::add_mipmap(std::vector<uint8_t> bytes, TextureFormat format) {
		auto expected_size = _ztex_mipmap_size(format, _m_width, _m_height, _m_mipmaps.size());
		if (bytes.size() != expected_size) {
			throw InvalidMipmapSize {expected_size, bytes.size()};
		}

		_m_mipmaps.emplace_back(std::move(bytes), format);
		return *this;
	}

	Texture TextureBuilder::build(TextureFormat format) {
		if (format == TextureFormat::P8) {
			throw UnsupportedFormatError {"P8"};
		}

		Texture tex;
		tex._m_format = format;
		tex._m_width = _m_width;
		tex._m_height = _m_height;
		tex._m_reference_width = _m_width;
		tex._m_reference_height = _m_height;
		tex._m_mipmap_count = _m_mipmaps.size();
		tex._m_average_color = {}; // FIXME(lmichaelis): Calculate the average color

		for (auto i = 0u; i < _m_mipmaps.size(); ++i) {
			auto& [data, fmt] = _m_mipmaps[i];
			tex._m_textures.push_back(_ztex_convert_format(data.data(), _m_width >> i, _m_height >> i, fmt, format));
		}

		return tex;
	}
} // namespace zenkit
