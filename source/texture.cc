// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/texture.hh>

#include <fmt/format.h>
#include <squish.h>

namespace phoenix {
	/// \brief Calculates the size in bytes of a texture at the given mipmap level.
	/// \return The size in bytes of a texture at the given mipmap level.
	std::uint32_t _ztex_mipmap_size(texture_format format, std::uint32_t width, std::uint32_t height, unsigned level) {
		std::uint32_t x = std::max(1u, width);
		std::uint32_t y = std::max(1u, height);

		for (unsigned i = 0; i < level; i++) {
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
			return std::max(1u, x / 4) * std::max(1u, y / 4) * 8;
		case tex_dxt2:
		case tex_dxt3:
		case tex_dxt4:
		case tex_dxt5:
			return std::max(1u, x / 4) * std::max(1u, y / 4) * 16;
		default:
			return 0;
		}
	}

	texture texture::parse(buffer& in) {
		texture tex;

		if (in.get_string(4) != ZTEX_SIGNATURE) {
			throw parser_error {"texture", "invalid signature"};
		}

		auto version = in.get_uint();
		if (version != 0) {
			throw parser_error {"texture", "invalid version"};
		}

		tex._m_format = static_cast<texture_format>(in.get_uint());
		tex._m_width = in.get_uint();
		tex._m_height = in.get_uint();
		tex._m_mipmap_count = std::max(1u, in.get_uint());
		tex._m_reference_width = in.get_uint();
		tex._m_reference_height = in.get_uint();
		tex._m_average_color = in.get_uint();

		if (tex._m_format == tex_p8) {
			for (auto& i : tex._m_palette) {
				i.b = in.get();
				i.g = in.get();
				i.r = in.get();
				i.a = in.get();
			}
		}

		// Lowest mipmap-level first
		for (std::int64_t level = tex._m_mipmap_count - 1; level >= 0; --level) {
			auto size = _ztex_mipmap_size(tex._m_format, tex._m_width, tex._m_height, level);

			std::vector<std::uint8_t> mipmap;
			mipmap.resize(size);
			in.get(mipmap.data(), size);

			tex._m_textures.emplace_back(std::move(mipmap));
		}

		return tex;
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

	std::vector<std::uint8_t> texture::as_rgba8(std::uint32_t mipmap_level) const {
		std::vector<std::uint8_t> conv;

		switch (_m_format) {
		case tex_dxt1:
		case tex_dxt3:
		case tex_dxt5: {
			const auto& map = data(mipmap_level);
			auto w = mipmap_width(mipmap_level);
			auto h = mipmap_height(mipmap_level);
			conv.resize(w * h * 4);

			auto flag = _m_format == tex_dxt1 ? squish::kDxt1 : (_m_format == tex_dxt3 ? squish::kDxt3 : squish::kDxt5);

			squish::DecompressImage(conv.data(), static_cast<int>(w), static_cast<int>(h), map.data(), flag);
			return conv;
		}
		case tex_B8G8R8A8: {
			const auto& map = data(mipmap_level);
			conv.resize(map.size());

			for (std::uint32_t i = 0; i < map.size(); i += 4) {
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

			for (std::uint32_t i = 0; i < map.size(); i += 4) {
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

			for (std::uint32_t i = 0; i < map.size(); i += 4) {
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

			for (std::uint32_t i = 0; i < map.size(); i += 3) {
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

			for (std::uint32_t i = 0; i < map.size(); i += 3) {
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
		case tex_p8: {
			const auto& map = data(mipmap_level);
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
			throw parser_error {"texture", fmt::format("cannot convert format to rgba: {}", int(_m_format))};
		}
	}
} // namespace phoenix
