// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/world/bsp_tree.hh>

#include <fmt/format.h>

namespace phoenix {
	enum class bsp_chunk : u16 {
		unknown,
		header = 0xC000,
		polygons = 0xC010,
		tree = 0xC040,
		outdoors = 0xC050,
		end = 0xC0FF
	};

	bsp_tree bsp_tree::read(reader& in) {
		bsp_tree bsp;

		bool finished = false;
		bsp_chunk chunk = bsp_chunk::unknown;
		u32 end = 0;

		do {
			chunk = static_cast<bsp_chunk>(in.read_u16());

			auto length = in.read_u32();
			end = length + in.tell();

			switch (chunk) {
				case bsp_chunk::header:
					bsp._m_version = in.read_u16();
					bsp._m_mode = static_cast<bsp_tree_mode>(in.read_u32());
					break;
				case bsp_chunk::polygons:
					bsp._m_polygon_indices.resize(in.read_u32());

					for (unsigned int& index : bsp._m_polygon_indices) {
						index = in.read_u32();
					}
					break;
				case bsp_chunk::end:
					finished = true;
					break;
				default:
					fmt::print(stderr, "warning: bsp tree: chunk not implemented: 0x{:X}\n", chunk);
					break;
			}

			if (in.tell() != end) {
				fmt::print(stderr, "warning: bsp tree: not all data consumed from section 0x{:X}\n", chunk);
				in.seek(end);
			}
		} while (!finished);

		return bsp;
	}
}// namespace phoenix