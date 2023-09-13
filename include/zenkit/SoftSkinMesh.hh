// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Boxes.hh"
#include "zenkit/Library.hh"
#include "zenkit/MultiResolutionMesh.hh"

#include <glm/vec3.hpp>

#include <cstdint>
#include <vector>

namespace phoenix {
	class buffer;
}

namespace zenkit {
	class Read;

	struct SoftSkinWedgeNormal {
		glm::vec3 normal;
		std::uint32_t index;
	};

	struct SoftSkinWeightEntry {
		float weight;
		glm::vec3 position;
		std::uint8_t node_index;
	};

	/// \brief Represents a soft-skin mesh.
	class SoftSkinMesh {
	public:
		/// \brief Parses a soft-skin mesh from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed soft-skin mesh.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws zenkit::ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] ZKREM(":: load()") ZKAPI static SoftSkinMesh parse(phoenix::buffer& in);

		/// \brief Parses a soft-skin mesh from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed soft-skin mesh.
		/// \throws zenkit::ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] ZKREM(":: load()") ZKAPI static SoftSkinMesh parse(phoenix::buffer&& in);

		ZKAPI void load(Read* r);

	public:
		/// \brief The embedded proto-mesh.
		MultiResolutionMesh mesh;

		/// \brief The meshes bounding boxes (there is one for each node).
		std::vector<OrientedBoundingBox> bboxes;

		/// \brief A list of wedge normals.
		std::vector<SoftSkinWedgeNormal> wedge_normals;

		/// \brief Node weights.
		std::vector<std::vector<SoftSkinWeightEntry>> weights;

		/// \brief Nodes.
		std::vector<std::int32_t> nodes;
	};
} // namespace zenkit
