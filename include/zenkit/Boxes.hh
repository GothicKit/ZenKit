// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/Misc.hh"

#include <vector>

namespace zenkit {
	class Read;
	class Write;

	/// \brief Represents a axis-aligned bounding box (AABB)
	struct AxisAlignedBoundingBox {
		static constexpr AxisAlignedBoundingBox zero() {
			return {Vec3 {0}, Vec3 {0}};
		}

		/// \brief The coordinates of the minimum corner of the bounding box.
		Vec3 min;

		/// \brief The coordinates of the maximum corner of the bounding box.
		Vec3 max;

		ZKAPI void load(Read* r);
		ZKAPI void save(Write* w) const;
	};

	/// \brief Represents an oriented bounding box.
	///
	/// In contrast to regular bounding boxes, [oriented bounding
	/// boxes](https://en.wikipedia.org/wiki/Minimum_bounding_box#Arbitrarily_oriented_minimum_bounding_box) may be
	/// rotated in the coordinate system and don't have to align with its axes.
	struct OrientedBoundingBox {
		Vec3 center;
		Vec3 axes[3];
		Vec3 half_width;

		std::vector<OrientedBoundingBox> children;

		ZKAPI void load(Read* r);
		ZKAPI void save(Write* w) const;

		/// \brief Calculates an axis-aligned bounding box from this oriented bounding box.
		/// \todo Write a test for this.
		/// \return An AABB which contains this OBB.
		[[nodiscard]] ZKAPI AxisAlignedBoundingBox as_bbox() const;
	};
} // namespace zenkit
