// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/detail/error.hh>
#include <phoenix/world/way_net.hh>

#include <fmt/format.h>

namespace phoenix {

	static void read_waypoint_data(way_point& wp, archive_reader_ref& in) {
		wp.name = in->read_string();
		wp.water_depth = in->read_int();
		wp.under_water = in->read_bool();
		wp.position = in->read_vec3();
		wp.direction = in->read_vec3();
		wp.free_point = true;
	}

	way_net way_net::parse(archive_reader_ref& in) {
		way_net net;
		archive_object obj;

		if (!in->read_object_begin(obj)) {
			throw parser_error("way net: header object missing => invalid or corrupted file");
		}

		(void) in->read_int();
		auto count = in->read_int();
		net._m_waypoints.reserve(count);

		std::unordered_map<std::uint32_t, std::uint32_t> obj_id_to_wp {};

		for (int i = 0; i < count; ++i) {
			if (!in->read_object_begin(obj) || obj.class_name != "zCWaypoint") {
				throw parser_error(fmt::format("way net: failed to read waypoint #{}", i));
			}

			auto& wp = net._m_waypoints.emplace_back();
			read_waypoint_data(wp, in);
			wp.free_point = true;
			net._m_name_to_waypoint[wp.name] = &wp;
			obj_id_to_wp[obj.index] = net._m_waypoints.size() - 1;

			if (!in->read_object_end()) {
				fmt::print(stderr,
				           "warning: way net: not all entries consumed from free point (index == {})",
				           obj.index);
				in->skip_object(true);
			}
		}

		auto edge_count = in->read_int();

		for (int i = 0; i < edge_count; ++i) {
			auto& edge = net._m_edges.emplace_back();

			for (int j = 0; j < 2; ++j) {
				if (!in->read_object_begin(obj)) {
					throw parser_error(fmt::format("way net: failed to read edge #{}", i));
				}

				std::uint32_t wp;

				if (obj.class_name == "\xA7" /* zReference */) {
					wp = obj_id_to_wp[obj.index];
				} else if (obj.class_name == "zCWaypoint") {
					auto& new_wp = net._m_waypoints.emplace_back();
					read_waypoint_data(new_wp, in);
					new_wp.free_point = false;

					obj_id_to_wp[obj.index] = net._m_waypoints.size() - 1;
					wp = net._m_waypoints.size() - 1;
				} else {
					throw parser_error(
					    fmt::format("way net: failed to read edge #{}: illegal class name '{}'", i, obj.class_name));
				}

				if (j == 0) {
					edge.a = wp;
				} else {
					edge.b = wp;
				}

				if (!in->read_object_end()) {
					fmt::print(stderr,
					           "warning: way net: not all entries consumed from edge #{} (index == {})",
					           i * 2 + j,
					           obj.index);
					in->skip_object(true);
				}
			}
		}

		if (!in->read_object_end()) {
			fmt::print(stderr, "warning: way net: not all entries consumed from way net");
			in->skip_object(true);
		}
		return net;
	}

	const way_point* way_net::waypoint(const std::string& name) const {
		if (auto it = _m_name_to_waypoint.find(name); it != _m_name_to_waypoint.end())
			return it->second;
		return nullptr;
	}
} // namespace phoenix
