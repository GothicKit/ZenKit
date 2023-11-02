// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Texture.hh"

namespace phoenix {
	constexpr zenkit::TextureFormat tex_B8G8R8A8 ZKREM("use zenkit::TextureFormat") = zenkit::TextureFormat::B8G8R8A8;
	constexpr zenkit::TextureFormat tex_R8G8B8A8 ZKREM("use zenkit::TextureFormat") = zenkit::TextureFormat::R8G8B8A8;
	constexpr zenkit::TextureFormat tex_A8B8G8R8 ZKREM("use zenkit::TextureFormat") = zenkit::TextureFormat::A8B8G8R8;
	constexpr zenkit::TextureFormat tex_A8R8G8B8 ZKREM("use zenkit::TextureFormat") = zenkit::TextureFormat::A8R8G8B8;
	constexpr zenkit::TextureFormat tex_B8G8R8 ZKREM("use zenkit::TextureFormat") = zenkit::TextureFormat::B8G8R8;
	constexpr zenkit::TextureFormat tex_R8G8B8 ZKREM("use zenkit::TextureFormat") = zenkit::TextureFormat::R8G8B8;
	constexpr zenkit::TextureFormat tex_A4R4G4B4 ZKREM("use zenkit::TextureFormat") = zenkit::TextureFormat::A4R4G4B4;
	constexpr zenkit::TextureFormat tex_A1R5G5B5 ZKREM("use zenkit::TextureFormat") = zenkit::TextureFormat::A1R5G5B5;
	constexpr zenkit::TextureFormat tex_R5G6B5 ZKREM("use zenkit::TextureFormat") = zenkit::TextureFormat::R5G6B5;
	constexpr zenkit::TextureFormat tex_p8 ZKREM("use zenkit::TextureFormat") = zenkit::TextureFormat::P8;
	constexpr zenkit::TextureFormat tex_dxt1 ZKREM("use zenkit::TextureFormat") = zenkit::TextureFormat::DXT1;
	constexpr zenkit::TextureFormat tex_dxt2 ZKREM("use zenkit::TextureFormat") = zenkit::TextureFormat::DXT2;
	constexpr zenkit::TextureFormat tex_dxt3 ZKREM("use zenkit::TextureFormat") = zenkit::TextureFormat::DXT3;
	constexpr zenkit::TextureFormat tex_dxt4 ZKREM("use zenkit::TextureFormat") = zenkit::TextureFormat::DXT4;
	constexpr zenkit::TextureFormat tex_dxt5 ZKREM("use zenkit::TextureFormat") = zenkit::TextureFormat::DXT5;

	using texture ZKREM("renamed to zenkit::Texture") = zenkit::Texture;
	using texture_format ZKREM("renamed to zenkit::TextureFormat") = zenkit::TextureFormat;
	using argb ZKREM("renamed to zenkit::ColorARGB") = zenkit::ColorARGB;
} // namespace phoenix
