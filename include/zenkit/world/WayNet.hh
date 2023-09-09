// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"

#include <glm/vec3.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace zenkit {
	class ReadArchive;

	/// \brief Represents a single waypoint.
	struct WayPoint {
		std::string name;
		std::int32_t water_depth;
		bool under_water;
		glm::vec3 position;
		glm::vec3 direction;
		bool free_point {false};
	};

	/// \brief Represents a connection between two waypoints.
	struct WayEdge {
		/// \brief The index of the first waypoint of the connection.
		std::uint32_t a;

		/// \brief The index of the second waypoint of the connection.
		std::uint32_t b;
	};

	/// \brief Represents a way-net.
	///
	/// Way-nets are used for NPC navigation and path finding.
	class WayNet {
	public:
		/// \brief PParses a way-net from the given reader.
		/// \param in The reader to read from.
		/// \return The way-net parsed.
		ZKINT void load(ReadArchive& in);

	public:
		/// \brief All waypoints of this way-net.
		std::vector<WayPoint> waypoints;

		/// \brief All edges of this way-net.
		std::vector<WayEdge> edges;
	};
} // namespace zenkit
