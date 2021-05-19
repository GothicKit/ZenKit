// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/archive.hh>
#include <phoenix/detail/error.hh>
#include <phoenix/world.hh>

#include <fmt/format.h>

namespace phoenix {
	world world::parse(reader& in, game_version version) {
		world wld;

		auto archive = archive_reader::open(in);

		archive_object chnk {};
		archive->read_object_begin(chnk);

		if (chnk.class_name != "oCWorld:zCWorld") {
			throw parser_error(fmt::format("world: cannot read world; 'oCWorld:zCWorld' chunk expected, got '{}'", chnk.class_name));
		}

		while (!archive->read_object_end()) {
			archive->read_object_begin(chnk);

			if (chnk.object_name == "MeshAndBsp") {
				/* auto version = */ (void) in.read_u32();
				auto size = in.read_u32();

				auto content = in.fork(size);
				wld._m_mesh = world_mesh::parse(content);
				wld._m_tree = bsp_tree::parse(content);

				archive->read_object_end();// TODO: Require end read!
			} else if (chnk.object_name == "VobTree") {
				auto count = archive->read_int();
				wld._m_root_vobs.reserve(count);

				for (int i = 0; i < count; ++i) {
					wld._m_root_vobs.emplace_back(vob_tree::parse(archive, version));
				}

				archive->read_object_end();// TODO: Require end read!
			} else if (chnk.object_name == "WayNet") {
				fmt::print(stderr, "warning: world: WayNet stub\n");
				archive->skip_object(true);
			} else {
				fmt::print(stderr, "warning: world: unknown world object '[{} {} {} {}]'\n", chnk.object_name,
						   chnk.class_name, chnk.version, chnk.index);
				archive->skip_object(true);
			}
		}

		return wld;
	}
}// namespace phoenix