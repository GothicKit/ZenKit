// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"

#include <glm/vec3.hpp>

#include <vector>

namespace phoenix {
	class buffer;
}

namespace zenkit {
	class Read;
	class Write;

	/// \brief Represents a axis-aligned bounding box (AABB)
	struct AxisAlignedBoundingBox {
		static constexpr AxisAlignedBoundingBox zero() {
			return {glm::vec3 {0}, glm::vec3 {0}};
		}

		/// \brief The coordinates of the minimum corner of the bounding box.
		glm::vec3 min;

		/// \brief The coordinates of the maximum corner of the bounding box.
		glm::vec3 max;

		/// \brief Parses a bounding box from the given buffer.
		/// \param[in,out] in The buffer to parse from.
		/// \return The bounding box parsed.
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static AxisAlignedBoundingBox parse(phoenix::buffer& in);

		ZKAPI void load(Read* r);
		ZKAPI void save(Write* w) const;
	};

	/// \brief Represents an oriented bounding box.
	///
	/// In contrast to regular bounding boxes, [oriented bounding
	/// boxes](https://en.wikipedia.org/wiki/Minimum_bounding_box#Arbitrarily_oriented_minimum_bounding_box) may be
	/// rotated in the coordinate system and don't have to align with its axes.
	struct OrientedBoundingBox {
		glm::vec3 center;
		glm::vec3 axes[3];
		glm::vec3 half_width;

		std::vector<OrientedBoundingBox> children;

		ZKAPI void load(Read* r);

		/// \brief Calculates an axis-aligned bounding box from this oriented bounding box.
		/// \todo Write a test for this.
		/// \return An AABB which contains this OBB.
		[[nodiscard]] ZKAPI AxisAlignedBoundingBox as_bbox() const;

		/// \brief Parses an oriented bounding box from a buffer.
		/// \param[in,out] in The buffer to parse from.
		/// \return The parsed bounding box.
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static OrientedBoundingBox parse(phoenix::buffer& in);
	};
} // namespace zenkit
