// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Boxes.hh"
#include "zenkit/Date.hh"
#include "zenkit/Library.hh"

#include <string>
#include <vector>

namespace zenkit {
	class Read;

	/// \brief A node in the hierarchy tree.
	struct ModelHierarchyNode {
		/// \brief The index of this node's parent node.
		std::int16_t parent_index;

		/// \brief The name of this node.
		std::string name;

		/// \brief The position and rotation of this node in its base state.
		Mat4 transform;
	};

	/// \brief Represents a *ZenGin* model hierarchy.
	///
	/// <p>Model hierarchy files represent the skeletal structure of a mesh. These skeletons are used to animate mostly
	/// animals and humans in the game which is commonly referred to as rigging.</p>
	class ModelHierarchy {
	public:
		ZKAPI void load(Read* r);
		ZKAPI void save(Write* w) const;

		/// \brief The list of nodes this hierarchy consists of.
		std::vector<ModelHierarchyNode> nodes {};

		/// \brief The bounding box of this hierarchy.
		AxisAlignedBoundingBox bbox {};

		/// \brief The collision bounding box of this hierarchy.
		AxisAlignedBoundingBox collision_bbox {};

		/// \brief The translation of the root node of this hierarchy.
		Vec3 root_translation {};

		/// \brief The checksum of this hierarchy.
		std::uint32_t checksum;

		Date source_date;
		std::string source_path;
	};
} // namespace zenkit
