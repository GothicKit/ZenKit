// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/ext/dds_convert.hh>

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                                                                                 \
	((uint32_t) (uint8_t) (ch0) | ((uint32_t) (uint8_t) (ch1) << 8) | ((uint32_t) (uint8_t) (ch2) << 16) |             \
	 ((uint32_t) (uint8_t) (ch3) << 24))

namespace phoenix {
	extern std::uint32_t
	_ztex_mipmap_size(texture_format format, std::uint32_t width, std::uint32_t height, unsigned level);

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

	static void write_dds_header(buffer& out, const texture& tex) {
		DDSURFACEDESC2 header {};
		header.dwSize = sizeof(DDSURFACEDESC2);
		header.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_CAPS;
		header.ddsCaps.dwCaps1 = DDSCAPS_TEXTURE;
		header.dwHeight = tex.height();
		header.dwWidth = tex.width();

		if (tex.format() != tex_dxt1 && tex.format() != tex_dxt2 && tex.format() != tex_dxt3 &&
		    tex.format() != tex_dxt4 && tex.format() != tex_dxt5) {
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
		case tex_B8G8R8A8:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			header.ddpfPixelFormat.dwRGBBitCount = 32;
			header.ddpfPixelFormat.dwRBitMask = 0x0000FF00;
			header.ddpfPixelFormat.dwGBitMask = 0x00FF0000;
			header.ddpfPixelFormat.dwBBitMask = 0xFF000000;
			header.ddpfPixelFormat.dwRGBAlphaBitMask = 0x000000FF;
			break;
		case tex_R8G8B8A8:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			header.ddpfPixelFormat.dwRGBBitCount = 32;
			header.ddpfPixelFormat.dwRBitMask = 0xFF000000;
			header.ddpfPixelFormat.dwGBitMask = 0x00FF0000;
			header.ddpfPixelFormat.dwBBitMask = 0x0000FF00;
			header.ddpfPixelFormat.dwRGBAlphaBitMask = 0x000000FF;
			break;
		case tex_A8B8G8R8:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			header.ddpfPixelFormat.dwRGBBitCount = 32;
			header.ddpfPixelFormat.dwRBitMask = 0x000000FF;
			header.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
			header.ddpfPixelFormat.dwBBitMask = 0x00FF0000;
			header.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
			break;
		case tex_A8R8G8B8:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			header.ddpfPixelFormat.dwRGBBitCount = 32;
			header.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
			header.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
			header.ddpfPixelFormat.dwBBitMask = 0x000000FF;
			header.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
			break;
		case tex_B8G8R8:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB;
			header.ddpfPixelFormat.dwRGBBitCount = 24;
			header.ddpfPixelFormat.dwRBitMask = 0x000000FF;
			header.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
			header.ddpfPixelFormat.dwBBitMask = 0x00FF0000;
			break;
		case tex_R8G8B8:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB;
			header.ddpfPixelFormat.dwRGBBitCount = 24;
			header.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
			header.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
			header.ddpfPixelFormat.dwBBitMask = 0x000000FF;
			break;
		case tex_A4R4G4B4:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			header.ddpfPixelFormat.dwRGBBitCount = 16;
			header.ddpfPixelFormat.dwRBitMask = 0x00000F00;
			header.ddpfPixelFormat.dwGBitMask = 0x000000F0;
			header.ddpfPixelFormat.dwBBitMask = 0x0000000F;
			header.ddpfPixelFormat.dwRGBAlphaBitMask = 0x0000F000;
			break;
		case tex_A1R5G5B5:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			header.ddpfPixelFormat.dwRGBBitCount = 16;
			header.ddpfPixelFormat.dwRBitMask = 0x00007C00;
			header.ddpfPixelFormat.dwGBitMask = 0x000003E0;
			header.ddpfPixelFormat.dwBBitMask = 0x0000001F;
			header.ddpfPixelFormat.dwRGBAlphaBitMask = 0x00008000;
			break;
		case tex_R5G6B5:
			header.ddpfPixelFormat.dwFlags = DDPF_RGB;
			header.ddpfPixelFormat.dwRGBBitCount = 16;
			header.ddpfPixelFormat.dwRBitMask = 0x0000F800;
			header.ddpfPixelFormat.dwGBitMask = 0x000007E0;
			header.ddpfPixelFormat.dwBBitMask = 0x0000001F;
			break;
		case tex_p8:
			header.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8;
			break;
		case tex_dxt1:
			header.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			header.ddpfPixelFormat.dwFourCC = MAKEFOURCC('D', 'X', 'T', '1');
			break;
		case tex_dxt2:
			header.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			header.ddpfPixelFormat.dwFourCC = MAKEFOURCC('D', 'X', 'T', '2');
			break;
		case tex_dxt3:
			header.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			header.ddpfPixelFormat.dwFourCC = MAKEFOURCC('D', 'X', 'T', '3');
			break;
		case tex_dxt4:
			header.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			header.ddpfPixelFormat.dwFourCC = MAKEFOURCC('D', 'X', 'T', '4');
			break;
		case tex_dxt5:
			header.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			header.ddpfPixelFormat.dwFourCC = MAKEFOURCC('D', 'X', 'T', '5');
			break;
		}

		out.put_uint(header.dwSize);
		out.put_uint(header.dwFlags);
		out.put_uint(header.dwHeight);
		out.put_uint(header.dwWidth);
		out.put_uint(header.dwPitchOrLinearSize);
		out.put_uint(header.dwDepth);
		out.put_uint(header.dwMipMapCount);

		for (auto i : header.dwReserved1)
			out.put_uint(i);

		out.put_uint(header.ddpfPixelFormat.dwSize);
		out.put_uint(header.ddpfPixelFormat.dwFlags);
		out.put_uint(header.ddpfPixelFormat.dwFourCC);
		out.put_uint(header.ddpfPixelFormat.dwRGBBitCount);
		out.put_uint(header.ddpfPixelFormat.dwRBitMask);
		out.put_uint(header.ddpfPixelFormat.dwGBitMask);
		out.put_uint(header.ddpfPixelFormat.dwBBitMask);
		out.put_uint(header.ddpfPixelFormat.dwRGBAlphaBitMask);
		out.put_uint(header.ddsCaps.dwCaps1);
		out.put_uint(header.ddsCaps.dwCaps2);
		out.put_uint(header.ddsCaps.dwReserved[0]);
		out.put_uint(header.ddsCaps.dwReserved[1]);
		out.put_uint(header.dwReserved2);
	}

	void write_dds_palette(buffer& out, const texture& tex) {
		for (auto i = 0; i < ZTEX_PALETTE_ENTRIES; ++i) {
			out.put(tex.palette()[i].b);
			out.put(tex.palette()[i].g);
			out.put(tex.palette()[i].r);
			out.put(tex.palette()[i].a);
		}
	}

	void write_dds_data(buffer& out, const texture& tex) {
		for (uint32_t level = 0; level < tex.mipmaps(); ++level) {
			auto& data = tex.data(level);
			out.put(data);
		}
	}

	uint32_t calculate_total_buffer_size(const texture& tex) {
		uint32_t size = 0;

		for (int32_t level = static_cast<int32_t>(tex.mipmaps()) - 1; level >= 0; --level)
			size += _ztex_mipmap_size(tex.format(), tex.width(), tex.height(), level);

		return size;
	}

	buffer texture_to_dds(const texture& tex) {
		auto buf = buffer::allocate(4 + // FOURCC
		                            sizeof(DDSURFACEDESC2) + (tex.format() == tex_p8 ? ZTEX_PALETTE_ENTRIES * 4 : 0) +
		                            calculate_total_buffer_size(tex));
		buf.put_string("DDS ");
		write_dds_header(buf, tex);

		if (tex.format() == tex_p8) {
			write_dds_palette(buf, tex);
		}

		write_dds_data(buf, tex);
		return buf;
	}
} // namespace phoenix
