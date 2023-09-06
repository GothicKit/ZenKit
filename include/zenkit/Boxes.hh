// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <glm/vec3.hpp>

#include <vector>

namespace phoenix {
	class buffer;

	/// \brief Represents a axis-aligned bounding box (AABB)
	struct bounding_box {
		/// \brief The coordinates of the minimum corner of the bounding box.
		glm::vec3 min;

		/// \brief The coordinates of the maximum corner of the bounding box.
		glm::vec3 max;

		/// \brief Parses a bounding box from the given buffer.
		/// \param[in,out] in The buffer to parse from.
		/// \return The bounding box parsed.
		PHOENIX_API static bounding_box parse(buffer& in);
	};

	/// \brief Represents an oriented bounding box.
	///
	/// In contrast to regular bounding boxes, [oriented bounding
	/// boxes](https://en.wikipedia.org/wiki/Minimum_bounding_box#Arbitrarily_oriented_minimum_bounding_box) may be
	/// rotated in the coordinate system and don't have to align with its axes.
	struct obb {
		glm::vec3 center;
		glm::vec3 axes[3];
		glm::vec3 half_width;

		std::vector<obb> children;

		/// \brief Calculates an axis-aligned bounding box from this oriented bounding box.
		/// \todo Write a test for this.
		/// \return An AABB which contains this OBB.
		[[nodiscard]] PHOENIX_API bounding_box as_bbox() const;

		/// \brief Parses an oriented bounding box from a buffer.
		/// \param[in,out] in The buffer to parse from.
		/// \return The parsed bounding box.
		[[nodiscard]] PHOENIX_API static obb parse(buffer& in);
	};
} // namespace phoenix
