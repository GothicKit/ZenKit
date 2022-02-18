// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/detail/error.hh>
#include <phoenix/texture.hh>

#include <fmt/format.h>
#include <squish.h>

namespace phoenix {
	/**
	 * @brief Calculates the size in bytes of the texture at the given mipmap level.
	 *
	 * Adapted from
	 * https://github.com/ataulien/ZenLib/blob/e1a5e1b12e71690a5470f3be2aa3d0d6419f5191/zenload/ztex2dds.cpp#L39.
	 *
	 * @return The size in bytes of the texture at the given mipmap level.
	 */
	static u32 _ztex_mipmap_size(texture_format format, unsigned long width, unsigned long height, int level) {
		unsigned long x = std::max(1ul, width);
		unsigned long y = std::max(1ul, height);

		for (int i = 0; i < level; i++) {
			if (x > 1)
				x >>= 1;
			if (y > 1)
				y >>= 1;
		}

		switch (format) {
		case tex_B8G8R8A8:
		case tex_R8G8B8A8:
		case tex_A8B8G8R8:
		case tex_A8R8G8B8:
			return x * y * 4;
		case tex_B8G8R8:
		case tex_R8G8B8:
			return x * y * 3;
		case tex_A4R4G4B4:
		case tex_A1R5G5B5:
		case tex_R5G6B5:
			return x * y * 2;
		case tex_p8:
			return x * y;
		case tex_dxt1:
			return std::max(1ul, x / 4) * std::max(1ul, y / 4) * 8;
		case tex_dxt2:
		case tex_dxt3:
		case tex_dxt4:
		case tex_dxt5:
			return std::max(1ul, x / 4) * std::max(1ul, y / 4) * 16;
		default:
			return 0;
		}
	}

	texture texture::parse(reader& in) {
		texture tex;

		if (in.read_string(4) != ZTEX_SIGNATURE) {
			throw parser_error("not a texture: invalid signature");
		}

		auto version = in.read_u32();

		if (version != 0) {
			throw parser_error("not a texture: invalid version");
		}

		tex._m_format = static_cast<texture_format>(in.read_u32());
		tex._m_width = in.read_u32();
		tex._m_height = in.read_u32();
		tex._m_mipmap_count = std::max(1u, in.read_u32());
		tex._m_reference_width = in.read_u32();
		tex._m_reference_height = in.read_u32();
		tex._m_average_color = {in.read_u8(), in.read_u8(), in.read_u8(), in.read_u8()};

		if (tex._m_format == tex_p8) {
			for (int i = 0; i < ZTEX_PALETTE_ENTRIES; ++i) {
				tex._m_palette[i].b = in.read_u8();
				tex._m_palette[i].g = in.read_u8();
				tex._m_palette[i].r = in.read_u8();
				tex._m_palette[i].a = in.read_u8();
			}
		}

		bool dxt_decompressed = false;

		// Lowest mipmap-level first
		for (int level = (int) tex._m_mipmap_count - 1; level >= 0; --level) {
			auto size = _ztex_mipmap_size(tex._m_format, tex._m_width, tex._m_height, level);

			std::vector<u8> mipmap;
			mipmap.resize(size);
			in.read(mipmap.data(), size);

			switch (tex._m_format) {
			case tex_dxt1:
			case tex_dxt3:
			case tex_dxt5: {
				std::vector<u8> v;
				auto w = tex.mipmap_width(level);
				auto h = tex.mipmap_height(level);
				v.resize(w * h * 4);

				auto flag = tex._m_format == tex_dxt1 ? squish::kDxt1
				                                      : (tex._m_format == tex_dxt3 ? squish::kDxt3 : squish::kDxt5);

				squish::DecompressImage(v.data(), w, h, mipmap.data(), flag);
				tex._m_textures.emplace_back(std::move(v));
				dxt_decompressed = true;
				break;
			}
			default:
				tex._m_textures.emplace_back(std::move(mipmap));
				break;
			}
		}

		if (dxt_decompressed) {
			tex._m_format = tex_R8G8B8A8;
		}

		return tex;
	}

	texture texture::parse(const std::string& path) {
		reader rdr {path};
		return parse(rdr);
	}

#pragma pack(push, 1)
	struct r5g5b5 {
		u8 r : 5;
		u8 g : 5;
		u8 b : 5;
	};
#pragma pack(pop)

	static_assert(sizeof(r5g5b5) == 2);

	std::vector<u8> texture::as_rgba8(u32 mipmap_level) const {
		std::vector<u8> conv;

		switch (_m_format) {
		case tex_B8G8R8A8: {
			const auto& map = data(mipmap_level);
			conv.resize(map.size());

			for (u32 i = 0; i < map.size(); i += 4) {
				conv[i + 0] = map[i + 2];
				conv[i + 1] = map[i + 1];
				conv[i + 2] = map[i + 0];
				conv[i + 3] = map[i + 3];
			}

			return conv;
		}
		case tex_R8G8B8A8:
			return data(mipmap_level);
		case tex_A8B8G8R8: {
			const auto& map = data(mipmap_level);
			conv.resize(map.size());

			for (u32 i = 0; i < map.size(); i += 4) {
				conv[i + 0] = map[i + 3];
				conv[i + 1] = map[i + 2];
				conv[i + 2] = map[i + 1];
				conv[i + 3] = map[i + 0];
			}

			return conv;
		}
		case tex_A8R8G8B8: {
			const auto& map = data(mipmap_level);
			conv.resize(map.size());

			for (u32 i = 0; i < map.size(); i += 4) {
				conv[i + 0] = map[i + 1];
				conv[i + 1] = map[i + 2];
				conv[i + 2] = map[i + 3];
				conv[i + 3] = map[i + 0];
			}

			return conv;
		}
		case tex_B8G8R8: {
			const auto& map = data(mipmap_level);
			conv.resize(map.size());

			for (u32 i = 0; i < map.size(); i += 3) {
				conv[i + 0] = map[i + 2];
				conv[i + 1] = map[i + 1];
				conv[i + 2] = map[i + 0];
				conv[i + 3] = 0;
			}

			return conv;
		}
		case tex_R8G8B8: {
			const auto& map = data(mipmap_level);
			conv.resize(map.size());

			for (u32 i = 0; i < map.size(); i += 3) {
				conv[i + 0] = map[i + 0];
				conv[i + 1] = map[i + 1];
				conv[i + 2] = map[i + 2];
				conv[i + 3] = 0;
			}

			return conv;
		}
		case tex_R5G6B5: {
			const auto& map = data(mipmap_level);
			conv.resize(map.size() * 2);

			u32 idx = 0;
			for (u32 i = 0; i < map.size(); i += 2) {
				r5g5b5* rgb = (r5g5b5*) &map[i];
				conv[idx++] = rgb->r;
				conv[idx++] = rgb->g;
				conv[idx++] = rgb->b;
				conv[idx++] = 0xff;
			}

			return conv;
		}
		case tex_p8: {
			const auto& map = data(mipmap_level);
			conv.resize(map.size() * sizeof(u8) * 4);

			for (u32 i = 0; i < map.size(); ++i) {
				auto palentry = _m_palette[map[i]];
				conv[i + 0] = palentry.r;
				conv[i + 1] = palentry.g;
				conv[i + 2] = palentry.b;
				conv[i + 3] = palentry.a;
			}

			return conv;
		}
		default:
			throw parser_error(fmt::format("texture: cannot convert format to rgba: {}", _m_format));
		}

		return std::vector<u8>();
	}
} // namespace phoenix
