// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Boxes.hh"
#include "zenkit/Date.hh"
#include "zenkit/Library.hh"

#include "glm/mat4x4.hpp"

#include <string>
#include <vector>

namespace phoenix {
	class buffer;
}

namespace zenkit {
	class Read;

	/// \brief A node in the hierarchy tree.
	struct ModelHierarchyNode {
		/// \brief The index of this node's parent node.
		std::int16_t parent_index;

		/// \brief The name of this node.
		std::string name;

		/// \brief The position and rotation of this node in its base state.
		glm::mat4x4 transform;
	};

	/// \brief Represents a *ZenGin* model hierarchy.
	///
	/// <p>Model hierarchy files represent the skeletal structure of a mesh. These skeletons are used to animate mostly
	/// animals and humans in the game which is commonly referred to as rigging.</p>
	class ModelHierarchy {
	public:
		/// \brief Parses a model hierarchy from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).</p>
		///
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed model hierarchy object.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws zenkit::ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static ModelHierarchy parse(phoenix::buffer& buf);

		/// \brief Parses a model hierarchy from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).</p>
		///
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed model hierarchy object.
		/// \throws zenkit::ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static ModelHierarchy parse(phoenix::buffer&& buf);

		ZKAPI void load(Read* r);
		ZKAPI void save(Write* w) const;

		/// \brief The list of nodes this hierarchy consists of.
		std::vector<ModelHierarchyNode> nodes {};

		/// \brief The bounding box of this hierarchy.
		AxisAlignedBoundingBox bbox {};

		/// \brief The collision bounding box of this hierarchy.
		AxisAlignedBoundingBox collision_bbox {};

		/// \brief The translation of the root node of this hierarchy.
		glm::vec3 root_translation {};

		/// \brief The checksum of this hierarchy.
		std::uint32_t checksum;

		Date source_date;
		std::string source_path;
	};
} // namespace zenkit
