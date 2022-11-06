// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/archive.hh>

#include <unordered_map>
#include <vector>

namespace phoenix {
	/// \brief Represents a single waypoint.
	struct way_point {
		std::string name;
		std::int32_t water_depth;
		bool under_water;
		glm::vec3 position;
		glm::vec3 direction;

		bool free_point {false};
	};

	/// \brief Represents a connection between two waypoints.
	struct way_edge {
		/// \brief The index of the first waypoint of the connection.
		std::uint32_t a;

		/// \brief The index of the second waypoint of the connection.
		std::uint32_t b;
	};

	/// \brief Represents a way-net.
	///
	/// Way-nets are used for NPC navigation and path finding.
	class way_net {
	public:
		/// \brief PParses a way-net from the given reader.
		/// \param in The reader to read from.
		/// \return The way-net parsed.
		static way_net parse(archive_reader& in);

		/// \brief Get the waypoint with the given name.
		/// \param name The name of the waypoint to get.
		/// \return A pointer to the waypoint or `nullptr` if the waypoint was not fount.
		[[nodiscard]] const way_point* waypoint(std::string_view name) const;

	public:
		/// \brief All waypoints of this way-net.
		std::vector<way_point> waypoints;

		/// \brief All edges of this way-net.
		std::vector<way_edge> edges;
	};

} // namespace phoenix
