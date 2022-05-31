// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/archive.hh>
#include <phoenix/detail/compat.hh>
#include <phoenix/detail/error.hh>
#include <phoenix/world.hh>

#include <fmt/format.h>

namespace phoenix {
	world world::parse(buffer& in, game_version version) {
		world wld;

		auto archive = archive_reader::open(in);

		archive_object chnk {};
		archive->read_object_begin(chnk);

		if (chnk.class_name != "oCWorld:zCWorld") {
			throw parser_error(
			    fmt::format("world: cannot read world; 'oCWorld:zCWorld' chunk expected, got '{}'", chnk.class_name));
		}

		while (!archive->read_object_end()) {
			archive->read_object_begin(chnk);

			if (chnk.object_name == "MeshAndBsp") {
				auto bsp_version = in.get_uint();
				(void) /* size = */ in.get_uint();


				std::uint16_t chunk_type = 0;
				auto mesh_data = in.slice();

				do {
					chunk_type = in.get_ushort();
					in.skip(in.get_uint());
				}  while (chunk_type != 0xB060);


				wld.world_bsp_tree = bsp_tree::parse(in, bsp_version);
				wld.world_mesh = mesh::parse(mesh_data, wld.world_bsp_tree.leaf_polygons());
			} else if (chnk.object_name == "VobTree") {
				auto count = archive->read_int();
				wld.world_vobs.reserve(count);

				for (int i = 0; i < count; ++i) {
					auto child = parse_vob_tree(archive, version);
					if (child == nullptr)
						continue;
					wld.world_vobs.push_back(std::move(child));
				}
			} else if (chnk.object_name == "WayNet") {
				wld.world_way_net = way_net::parse(archive);
			}

			if (!archive->read_object_end()) {
				fmt::print(stderr,
				           "warning: world: not all data consumed of object '[{} {} {} {}]'\n",
				           chnk.object_name,
				           chnk.class_name,
				           chnk.version,
				           chnk.index);
				archive->skip_object(true);
			}
		}

		return wld;
	}
} // namespace phoenix
