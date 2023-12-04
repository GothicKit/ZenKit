// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/world/WayNet.hh"
#include "zenkit/Archive.hh"

#include "../Internal.hh"

namespace zenkit {
	static void read_waypoint_data(WayPoint& wp, ReadArchive& in) {
		wp.name = in.read_string();      // wpName
		wp.water_depth = in.read_int();  // waterDepth
		wp.under_water = in.read_bool(); // underWater
		wp.position = in.read_vec3();    // position
		wp.direction = in.read_vec3();   // direction
		wp.free_point = true;
	}

	void WayNet::load(ReadArchive& in) {
		ArchiveObject obj;

		if (!in.read_object_begin(obj)) {
			throw ParserError {"WayNet", "root object missing"};
		}

		(void) /* auto version = */ in.read_int();            // waynetVersion
		auto count = static_cast<std::size_t>(in.read_int()); // numWaypoints
		this->waypoints.reserve(count);

		std::unordered_map<std::uint32_t, std::uint32_t> obj_id_to_wp {};

		for (auto i = 0u; i < count; ++i) {
			if (!in.read_object_begin(obj) || obj.class_name != "zCWaypoint") {
				throw ParserError {"WayNet", "missing waypoint object #" + std::to_string(i)};
			}

			auto& wp = this->waypoints.emplace_back();
			read_waypoint_data(wp, in);
			wp.free_point = true;
			obj_id_to_wp[obj.index] = static_cast<uint32_t>(this->waypoints.size() - 1);

			if (!in.read_object_end()) {
				ZKLOGE("WayNet", "free point %u not fully parsed", obj.index);
				in.skip_object(true);
			}
		}

		auto edge_count = in.read_int(); // numWays

		for (int32_t i = 0; i < edge_count; ++i) {
			auto& edge = this->edges.emplace_back();

			for (int32_t j = 0; j < 2; ++j) {
				if (!in.read_object_begin(obj)) {
					throw ParserError {"WayNet", "missing edge object #" + std::to_string(i)};
				}

				std::uint32_t wp;

				if (obj.class_name == "\xA7" /* zReference */) {
					wp = obj_id_to_wp[obj.index];
				} else if (obj.class_name == "zCWaypoint") {
					auto& new_wp = this->waypoints.emplace_back();
					read_waypoint_data(new_wp, in);
					new_wp.free_point = false;
					obj_id_to_wp[obj.index] = static_cast<uint32_t>(this->waypoints.size() - 1);
					wp = static_cast<uint32_t>(this->waypoints.size() - 1);
				} else {
					throw ParserError {"WayNet",
					                   "failed to parse edge #" + std::to_string(i) + ": unknown class name '" +
					                       obj.class_name + "'"};
				}

				if (j == 0) {
					edge.a = wp;
				} else {
					edge.b = wp;
				}

				if (!in.read_object_end()) {
					ZKLOGW("WayNet", "WayEdge %u at index %u not fully parsed", i * 2 + j, obj.index);
					in.skip_object(true);
				}
			}
		}

		if (!in.read_object_end()) {
			ZKLOGW("WayNet", "Not fully parsed");
			in.skip_object(true);
		}
	}
} // namespace zenkit
