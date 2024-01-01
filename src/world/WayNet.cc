// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/world/WayNet.hh"
#include "zenkit/Archive.hh"

#include "../Internal.hh"

namespace zenkit {
#ifndef ZK_FUTURE
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
#else
	void WayPoint::load(ReadArchive& r, GameVersion) {
		this->name = r.read_string();      // wpName
		this->water_depth = r.read_int();  // waterDepth
		this->under_water = r.read_bool(); // underWater
		this->position = r.read_vec3();    // position
		this->direction = r.read_vec3();   // direction
	}

	void WayPoint::save(WriteArchive& w, GameVersion version) const {
		Object::save(w, version);
	}

	void WayNet::load(ReadArchive& r, GameVersion version) {
		[[maybe_unused]] auto ver = r.read_int(); // waynetVersion
		auto count_points = r.read_int();         // numWaypoints
		this->points.reserve(count_points);

		for (auto i = 0; i < count_points; ++i) {
			this->points.push_back(r.read_object<WayPoint>(version));
		}

		auto count_edges = r.read_int(); // numWays
		this->edges.reserve(count_edges);

		for (auto i = 0; i < count_edges; ++i) {
			auto wayl = r.read_object<WayPoint>(version);
			auto wayr = r.read_object<WayPoint>(version);

			if (wayl.use_count() < 3) this->points.push_back(wayl);
			if (wayr.use_count() < 3) this->points.push_back(wayr);

			this->edges.emplace_back(wayl, wayr);
		}
	}

	void WayNet::save(WriteArchive& w, GameVersion version) const {
		Object::save(w, version);
	}
#endif

} // namespace zenkit
