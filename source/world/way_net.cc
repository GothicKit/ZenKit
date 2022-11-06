// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/world/way_net.hh>

#include <fmt/format.h>

namespace phoenix {
	static void read_waypoint_data(way_point& wp, archive_reader& in) {
		wp.name = in.read_string();      // wpName
		wp.water_depth = in.read_int();  // waterDepth
		wp.under_water = in.read_bool(); // underWater
		wp.position = in.read_vec3();    // position
		wp.direction = in.read_vec3();   // direction
		wp.free_point = true;
	}

	way_net way_net::parse(archive_reader& in) {
		try {
			way_net net;
			archive_object obj;

			if (!in.read_object_begin(obj)) {
				throw parser_error {"way_net", "root object missing"};
			}

			(void) /* auto version = */ in.read_int(); // waynetVersion
			auto count = in.read_int();                // numWaypoints
			net.waypoints.reserve(count);

			std::unordered_map<std::uint32_t, std::uint32_t> obj_id_to_wp {};

			for (int32_t i = 0; i < count; ++i) {
				if (!in.read_object_begin(obj) || obj.class_name != "zCWaypoint") {
					throw parser_error {"way_net", fmt::format("missing waypoint object #{}", i)};
				}

				auto& wp = net.waypoints.emplace_back();
				read_waypoint_data(wp, in);
				wp.free_point = true;
				net._m_name_to_waypoint[wp.name] = net.waypoints.size() - 1;
				obj_id_to_wp[obj.index] = net.waypoints.size() - 1;

				if (!in.read_object_end()) {
					PX_LOGW("way_net: free point {} not fully parsed", obj.index);
					in.skip_object(true);
				}
			}

			auto edge_count = in.read_int(); // numWays

			for (int32_t i = 0; i < edge_count; ++i) {
				auto& edge = net.edges.emplace_back();

				for (int32_t j = 0; j < 2; ++j) {
					if (!in.read_object_begin(obj)) {
						throw parser_error {"way_net", fmt::format("missing edge object #{}", i)};
					}

					std::uint32_t wp;

					if (obj.class_name == "\xA7" /* zReference */) {
						wp = obj_id_to_wp[obj.index];
					} else if (obj.class_name == "zCWaypoint") {
						auto& new_wp = net.waypoints.emplace_back();
						read_waypoint_data(new_wp, in);
						new_wp.free_point = false;
						net._m_name_to_waypoint[new_wp.name] = net.waypoints.size() - 1;
						obj_id_to_wp[obj.index] = net.waypoints.size() - 1;
						wp = net.waypoints.size() - 1;
					} else {
						throw parser_error {
						    "way_net",
						    fmt::format("failed to parse edge #{}: unknown class name '{}'", i, obj.class_name)};
					}

					if (j == 0) {
						edge.a = wp;
					} else {
						edge.b = wp;
					}

					if (!in.read_object_end()) {
						PX_LOGW("way_net: edge {} at index {} not fully parsed", i * 2 + j, obj.index);
						in.skip_object(true);
					}
				}
			}

			if (!in.read_object_end()) {
				PX_LOGW("way_net: not fully parsed");
				in.skip_object(true);
			}
			return net;
		} catch (const buffer_error& exc) {
			throw parser_error {"way_net", exc, "eof reached"};
		}
	}

	const way_point* way_net::waypoint(const std::string& name) const {
		if (auto it = _m_name_to_waypoint.find(name); it != _m_name_to_waypoint.end())
			return &waypoints[it->second];
		return nullptr;
	}
} // namespace phoenix
