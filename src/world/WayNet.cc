// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/world/WayNet.hh"
#include "zenkit/Archive.hh"
#include "zenkit/Error.hh"

#include "../Internal.hh"

namespace zenkit {
	void WayPoint::load(ReadArchive& r, GameVersion) {
		this->name = r.read_string();      // wpName
		this->water_depth = r.read_int();  // waterDepth
		this->under_water = r.read_bool(); // underWater
		this->position = r.read_vec3();    // position
		this->direction = r.read_vec3();   // direction
	}

	void WayPoint::save(WriteArchive& w, GameVersion) const {
		w.write_string("wpName", this->name);
		w.write_int("waterDepth", this->water_depth);
		w.write_bool("underWater", this->under_water);
		w.write_vec3("position", this->position);
		w.write_vec3("direction", this->direction);
	}

	void WayNet::load(ReadArchive& r, GameVersion version) {
		[[maybe_unused]] auto ver = r.read_int(); // waynetVersion
		auto count_points = r.read_int();         // numWaypoints
		this->points.reserve(count_points);

		for (auto i = 0; i < count_points; ++i) {
			this->points.push_back(r.read_object<WayPoint>(version));
			this->points.back()->free_point = true;
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
		w.write_int("waynetVersion", 1);

		int count_free_points = 0;
		for (auto& point : this->points) {
			if (point->free_point) count_free_points += 1;
		}

		w.write_int("numWaypoints", count_free_points);
		auto i = 0u;
		for (auto& point : this->points) {
			if (point->free_point) {
				w.write_object("waypoint" + std::to_string(i), point, version);
				i += 1;
			}
		}

		w.write_int("numWays", this->edges.size());
		for (i = 0u; i < this->edges.size(); ++i) {
			auto& [wayl, wayr] = this->edges[i];
			w.write_object("wayl" + std::to_string(i), wayl, version);
			w.write_object("wayr" + std::to_string(i), wayr, version);
		}
	}
} // namespace zenkit
