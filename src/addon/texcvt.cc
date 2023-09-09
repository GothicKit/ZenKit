// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/addon/texcvt.hh"
#include "zenkit/Stream.hh"

#include "phoenix/buffer.hh"

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                                                                                 \
	((uint32_t) (uint8_t) (ch0) | ((uint32_t) (uint8_t) (ch1) << 8) | ((uint32_t) (uint8_t) (ch2) << 16) |             \
	 ((uint32_t) (uint8_t) (ch3) << 24))

namespace zenkit {
	extern std::uint32_t _ztex_mipmap_size(TextureFormat, std::uint32_t, std::uint32_t, uint32_t);

	enum {
		DDSD_CAPS = 0x00000001l,
		DDSD_HEIGHT = 0x00000002l,
		DDSD_WIDTH = 0x00000004l,
		DDSD_PITCH = 0x00000008l,
		DDSD_ALPHABITDEPTH = 0x00000080l,
		DDSD_PIXELFORMAT = 0x00001000l,
		DDSD_MIPMAPCOUNT = 0x00020000l,
		DDSD_LINEARSIZE = 0x00080000l,
		DDSD_DEPTH = 0x00800000l
	};

	enum {
		DDSCAPS_ALPHA = 0x00000002l,   // alpha only surface
		DDSCAPS_COMPLEX = 0x00000008l, // complex surface structure
		DDSCAPS_TEXTURE = 0x00001000l, // used as texture (should always be set)
		DDSCAPS_MIPMAP = 0x00400000l   // Mipmap present
	};

	enum {
		DDPF_ALPHAPIXELS = 0x00000001l, // surface has alpha channel
		DDPF_ALPHA = 0x00000002l,       // alpha only
		DDPF_FOURCC = 0x00000004l,      // FOURCC available
		DDPF_RGB = 0x00000040l,         // RGB(A) bitmap
		DDPF_PALETTEINDEXED8 = 0x00000020l
	};

	typedef struct {
		uint32_t dwCaps1; // Zero or more of the DDSCAPS_* members
		uint32_t dwCaps2; // Zero or more of the DDSCAPS2_* members
		uint32_t dwReserved[2];
	} DDCAPS2;

	typedef struct {
		uint32_t dwSize;  // size of this structure (must be 32)
		uint32_t dwFlags; // see DDPF_*
		uint32_t dwFourCC;
		uint32_t dwRGBBitCount; // Total number of bits for RGB formats
		uint32_t dwRBitMask;
		uint32_t dwGBitMask;
		uint32_t dwBBitMask;
		uint32_t dwRGBAlphaBitMask;
	} DDPIXELFORMAT;

	typedef struct {
		uint32_t dwSize;  // size of this structure (must be 124)
		uint32_t dwFlags; // combination of the DDSS_* flags
		uint32_t dwHeight;
		uint32_t dwWidth;
		uint32_t dwPitchOrLinearSize;
		uint32_t dwDepth; // Depth of a volume texture
		uint32_t dwMipMapCount;
		uint32_t dwReserved1[11];
		DDPIXELFORMAT ddpfPixelFormat;
		DDCAPS2 ddsCaps;
		uint32_t dwReserved2;
	} DDSURFACEDESC2;

	static void write_dds_header(Write* w, zenkit::Texture const& tex) {
		DDSURFACEDESC2 header {};
		header.dwSize = sizeof(DDSURFACEDESC2);
		header.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_CAPS;
		header.ddsCaps.dwCaps1 = DDSCAPS_TEXTURE;
		header.dwHeight = tex.height();
		header.dwWidth = tex.width();

		if (tex.format() != TextureFormat::DXT1 && tex.format() != TextureFormat::DXT2 &&
		    tex.format() != TextureFormat::DXT3 && tex.format() != TextureFormat::DXT4 &&
		    tex.format() != TextureFormat::DXT5) {
			header.dwFlags |= DDSD_PITCH;
			header.dwPitchOrLinearSize = _ztex_mipmap_size(tex.format(), header.dwWidth, 1, 0);
		} else {
			header.dwFlags |= DDSD_LINEARSIZE;
			header.dwPitchOrLinearSize = _ztex_mipmap_size(tex.format(), header.dwWidth, header.dwHeight, 0);
		}

		if (tex.mipmaps() > 1) {
			header.dwFlags |= DDSD_MIPMAPCOUNT;
			header.ddsCaps.dwCaps1 |= DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
			header.dwMipMapCount = tex.mipmaps();
		}

		header.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		switch (tex.format()) {
		case TextureFormat::B8G8R8A8:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			header.ddpfPixelFormat.dwRGBBitCount = 32;
			header.ddpfPixelFormat.dwRBitMask = 0x0000FF00;
			header.ddpfPixelFormat.dwGBitMask = 0x00FF0000;
			header.ddpfPixelFormat.dwBBitMask = 0xFF000000;
			header.ddpfPixelFormat.dwRGBAlphaBitMask = 0x000000FF;
			break;
		case TextureFormat::R8G8B8A8:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			header.ddpfPixelFormat.dwRGBBitCount = 32;
			header.ddpfPixelFormat.dwRBitMask = 0xFF000000;
			header.ddpfPixelFormat.dwGBitMask = 0x00FF0000;
			header.ddpfPixelFormat.dwBBitMask = 0x0000FF00;
			header.ddpfPixelFormat.dwRGBAlphaBitMask = 0x000000FF;
			break;
		case TextureFormat::A8B8G8R8:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			header.ddpfPixelFormat.dwRGBBitCount = 32;
			header.ddpfPixelFormat.dwRBitMask = 0x000000FF;
			header.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
			header.ddpfPixelFormat.dwBBitMask = 0x00FF0000;
			header.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
			break;
		case TextureFormat::A8R8G8B8:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			header.ddpfPixelFormat.dwRGBBitCount = 32;
			header.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
			header.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
			header.ddpfPixelFormat.dwBBitMask = 0x000000FF;
			header.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
			break;
		case TextureFormat::B8G8R8:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB;
			header.ddpfPixelFormat.dwRGBBitCount = 24;
			header.ddpfPixelFormat.dwRBitMask = 0x000000FF;
			header.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
			header.ddpfPixelFormat.dwBBitMask = 0x00FF0000;
			break;
		case TextureFormat::R8G8B8:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB;
			header.ddpfPixelFormat.dwRGBBitCount = 24;
			header.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
			header.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
			header.ddpfPixelFormat.dwBBitMask = 0x000000FF;
			break;
		case TextureFormat::A4R4G4B4:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			header.ddpfPixelFormat.dwRGBBitCount = 16;
			header.ddpfPixelFormat.dwRBitMask = 0x00000F00;
			header.ddpfPixelFormat.dwGBitMask = 0x000000F0;
			header.ddpfPixelFormat.dwBBitMask = 0x0000000F;
			header.ddpfPixelFormat.dwRGBAlphaBitMask = 0x0000F000;
			break;
		case TextureFormat::A1R5G5B5:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			header.ddpfPixelFormat.dwRGBBitCount = 16;
			header.ddpfPixelFormat.dwRBitMask = 0x00007C00;
			header.ddpfPixelFormat.dwGBitMask = 0x000003E0;
			header.ddpfPixelFormat.dwBBitMask = 0x0000001F;
			header.ddpfPixelFormat.dwRGBAlphaBitMask = 0x00008000;
			break;
		case TextureFormat::R5G6B5:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB;
			header.ddpfPixelFormat.dwRGBBitCount = 16;
			header.ddpfPixelFormat.dwRBitMask = 0x0000F800;
			header.ddpfPixelFormat.dwGBitMask = 0x000007E0;
			header.ddpfPixelFormat.dwBBitMask = 0x0000001F;
			break;
		case TextureFormat::P8:
			header.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8;
			break;
		case TextureFormat::DXT1:
			header.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			header.ddpfPixelFormat.dwFourCC = MAKEFOURCC('D', 'X', 'T', '1');
			break;
		case TextureFormat::DXT2:
			header.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			header.ddpfPixelFormat.dwFourCC = MAKEFOURCC('D', 'X', 'T', '2');
			break;
		case TextureFormat::DXT3:
			header.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			header.ddpfPixelFormat.dwFourCC = MAKEFOURCC('D', 'X', 'T', '3');
			break;
		case TextureFormat::DXT4:
			header.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			header.ddpfPixelFormat.dwFourCC = MAKEFOURCC('D', 'X', 'T', '4');
			break;
		case TextureFormat::DXT5:
			header.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			header.ddpfPixelFormat.dwFourCC = MAKEFOURCC('D', 'X', 'T', '5');
			break;
		}

		w->write_uint(header.dwSize);
		w->write_uint(header.dwFlags);
		w->write_uint(header.dwHeight);
		w->write_uint(header.dwWidth);
		w->write_uint(header.dwPitchOrLinearSize);
		w->write_uint(header.dwDepth);
		w->write_uint(header.dwMipMapCount);

		for (auto i : header.dwReserved1) {
			w->write_uint(i);
		}

		w->write_uint(header.ddpfPixelFormat.dwSize);
		w->write_uint(header.ddpfPixelFormat.dwFlags);
		w->write_uint(header.ddpfPixelFormat.dwFourCC);
		w->write_uint(header.ddpfPixelFormat.dwRGBBitCount);
		w->write_uint(header.ddpfPixelFormat.dwRBitMask);
		w->write_uint(header.ddpfPixelFormat.dwGBitMask);
		w->write_uint(header.ddpfPixelFormat.dwBBitMask);
		w->write_uint(header.ddpfPixelFormat.dwRGBAlphaBitMask);
		w->write_uint(header.ddsCaps.dwCaps1);
		w->write_uint(header.ddsCaps.dwCaps2);
		w->write_uint(header.ddsCaps.dwReserved[0]);
		w->write_uint(header.ddsCaps.dwReserved[1]);
		w->write_uint(header.dwReserved2);
	}

	static void write_dds_palette(Write* w, zenkit::Texture const& tex) {
		for (auto i = 0; i < ZTEX_PALETTE_ENTRIES; ++i) {
			w->write_ubyte(tex.palette()[i].b);
			w->write_ubyte(tex.palette()[i].g);
			w->write_ubyte(tex.palette()[i].r);
			w->write_ubyte(tex.palette()[i].a);
		}
	}

	static void write_dds_data(Write* w, zenkit::Texture const& tex) {
		for (uint32_t level = 0; level < tex.mipmaps(); ++level) {
			auto& data = tex.data(level);
			w->write(data.data(), data.size());
		}
	}

	std::vector<std::byte> to_dds(zenkit::Texture const& tex) {
		std::vector<std::byte> buf {};
		auto w = Write::to(&buf);

		w->write_string("DDS ");
		write_dds_header(w.get(), tex);

		if (tex.format() == TextureFormat::P8) {
			write_dds_palette(w.get(), tex);
		}

		write_dds_data(w.get(), tex);
		return buf;
	}
} // namespace zenkit
