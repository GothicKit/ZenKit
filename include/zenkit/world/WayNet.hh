// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/Object.hh"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace zenkit {
	class ReadArchive;

	/// \brief Represents a single waypoint.
	struct WayPoint : Object {
		ZK_OBJECT(ObjectType::zCWaypoint);

		std::string name;
		std::int32_t water_depth;
		bool under_water;
		Vec3 position;
		Vec3 direction;
		bool free_point {false};

		ZKINT void load(ReadArchive& r, GameVersion version) override;
		ZKINT void save(WriteArchive& w, GameVersion version) const override;
	};

	/// \brief Represents a way-net.
	///
	/// Way-nets are used for NPC navigation and path finding.
	class WayNet : public Object {
		ZK_OBJECT(ObjectType::zCWayNet);

		ZKINT void load(ReadArchive& r, GameVersion version) override;
		ZKINT void save(WriteArchive& w, GameVersion version) const override;

		/// \brief All waypoints of this way-net.
		std::vector<std::shared_ptr<WayPoint>> points;

		/// \brief All edges of this way-net.
		std::vector<std::pair<std::shared_ptr<WayPoint>, std::shared_ptr<WayPoint>>> edges;
	};
} // namespace zenkit
