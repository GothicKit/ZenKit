// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Boxes.hh"
#include "zenkit/Library.hh"
#include "zenkit/MultiResolutionMesh.hh"

#include <glm/vec3.hpp>

#include <cstdint>
#include <vector>

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
		ZKAPI void load(Read* r);
		ZKAPI void save(Write* w, GameVersion version) const;

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
