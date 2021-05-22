// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/archive.hh>

#include <vector>

namespace phoenix {
	/**
	 * @brief Represents a waypoint
	 */
	struct way_point {
		std::string name;
		s32 water_depth;
		bool under_water;
		glm::vec3 position;
		glm::vec3 direction;

		bool free_point {false};
	};

	/**
	 * @brief Represents an edge between two waypoints.
	 */
	struct way_edge {
		u32 a;
		u32 b;
	};

	/**
	 * @brief Represents a world's way-net.
	 *
	 * Parses ZenGin way-nets. The reference implementation can be found on GitHub:
	 * https://github.com/Try/ZenLib/blob/9b94299307aebcd99887c5e0b050a13622e41ecb/zenload/zenParser.cpp#L487 and the
	 * original version by Andre Taulien was also referenced:
	 * https://github.com/ataulien/ZenLib/blob/e1a5e1b12e71690a5470f3be2aa3d0d6419f5191/zenload/zCWayNet.h
	 *
	 * Thanks to the original author, Andre Taulien as well as Try for additional work on their ZenLib fork!
	 *
	 * @see https://github.com/ataulien/ZenLib
	 * @see https://github.com/Try/ZenLib
	 */
	class way_net {
	public:
		/**
		 * @brief Parses a way net from the given reader.
		 * @param in The reader to read from.
		 * @return The way net parsed.
		 */
		static way_net parse(archive_reader_ref& in);

		/**
		 * @return All waypoints associated with the way-net.
		 */
		[[nodiscard]] inline const std::vector<way_point>& waypoints() const noexcept { return _m_waypoints; }

		/**
		 * @return All edges associated with the way-net.
		 */
		[[nodiscard]] inline const std::vector<way_edge>& edges() const noexcept { return _m_edges; }

	private:
		std::vector<way_point> _m_waypoints;
		std::vector<way_edge> _m_edges;
	};

}// namespace phoenix