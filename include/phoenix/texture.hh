// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Texture.hh"

namespace phoenix {
	constexpr zenkit::TextureFormat const tex_B8G8R8A8 ZKREM("use zenkit::TextureFormat") = zenkit::tex_B8G8R8A8;
	constexpr zenkit::TextureFormat const tex_R8G8B8A8 ZKREM("use zenkit::TextureFormat") = zenkit::tex_R8G8B8A8;
	constexpr zenkit::TextureFormat const tex_A8B8G8R8 ZKREM("use zenkit::TextureFormat") = zenkit::tex_A8B8G8R8;
	constexpr zenkit::TextureFormat const tex_A8R8G8B8 ZKREM("use zenkit::TextureFormat") = zenkit::tex_A8R8G8B8;
	constexpr zenkit::TextureFormat const tex_B8G8R8 ZKREM("use zenkit::TextureFormat") = zenkit::tex_B8G8R8;
	constexpr zenkit::TextureFormat const tex_R8G8B8 ZKREM("use zenkit::TextureFormat") = zenkit::tex_R8G8B8;
	constexpr zenkit::TextureFormat const tex_A4R4G4B4 ZKREM("use zenkit::TextureFormat") = zenkit::tex_A4R4G4B4;
	constexpr zenkit::TextureFormat const tex_A1R5G5B5 ZKREM("use zenkit::TextureFormat") = zenkit::tex_A1R5G5B5;
	constexpr zenkit::TextureFormat const tex_R5G6B5 ZKREM("use zenkit::TextureFormat") = zenkit::tex_R5G6B5;
	constexpr zenkit::TextureFormat const tex_p8 ZKREM("use zenkit::TextureFormat") = zenkit::tex_p8;
	constexpr zenkit::TextureFormat const tex_dxt1 ZKREM("use zenkit::TextureFormat") = zenkit::tex_dxt1;
	constexpr zenkit::TextureFormat const tex_dxt2 ZKREM("use zenkit::TextureFormat") = zenkit::tex_dxt2;
	constexpr zenkit::TextureFormat const tex_dxt3 ZKREM("use zenkit::TextureFormat") = zenkit::tex_dxt3;
	constexpr zenkit::TextureFormat const tex_dxt4 ZKREM("use zenkit::TextureFormat") = zenkit::tex_dxt4;
	constexpr zenkit::TextureFormat const tex_dxt5 ZKREM("use zenkit::TextureFormat") = zenkit::tex_dxt5;

	using texture ZKREM("renamed to zenkit::Texture") = zenkit::Texture;
	using texture_format ZKREM("renamed to zenkit::TextureFormat") = zenkit::TextureFormat;
	using argb ZKREM("renamed to zenkit::ColorARGB") = zenkit::ColorARGB;
} // namespace phoenix
