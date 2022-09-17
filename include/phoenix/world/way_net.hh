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
		static way_net parse(std::unique_ptr<archive_reader>& in);

		/// \return All waypoints of this way-net.
		[[nodiscard]] inline const std::vector<way_point>& waypoints() const noexcept {
			return _m_waypoints;
		}

		/// \return All edges of this way-net.
		[[nodiscard]] inline const std::vector<way_edge>& edges() const noexcept {
			return _m_edges;
		}

		/// \brief Get the waypoint with the given name.
		/// \param name The name of the waypoint to get.
		/// \return A pointer to the waypoint or `nullptr` if the waypoint was not fount.
		[[nodiscard]] const way_point* waypoint(const std::string& name) const;

	private:
		std::vector<way_point> _m_waypoints;
		std::vector<way_edge> _m_edges;
		std::unordered_map<std::string, std::uint32_t> _m_name_to_waypoint;
	};

} // namespace phoenix
