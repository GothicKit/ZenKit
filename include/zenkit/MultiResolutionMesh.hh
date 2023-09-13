// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Boxes.hh"
#include "zenkit/Library.hh"
#include "zenkit/Material.hh"

#include <glm/vec3.hpp>

#include <cstdint>
#include <vector>

namespace phoenix {
	class buffer;
}

namespace zenkit {
	class Read;

	struct MeshTriangle {
		std::uint16_t wedges[3];
	};

	struct MeshTriangleEdge {
		std::uint16_t edges[3];
	};

	struct MeshEdge {
		std::uint16_t edges[2];
	};

	struct MeshWedge {
		glm::vec3 normal;
		glm::vec2 texture;
		std::uint16_t index;
	};

	struct MeshPlane {
		float distance;
		glm::vec3 normal;
	};

	/// \brief An offset and size tuple for mesh sections.
	struct MeshSection {
		std::size_t offset;
		std::uint32_t size;
	};

	/// \brief Offsets and sizes of binary data sections containing sub-mesh data.
	/// \note This is only of use ZenKit-internally.
	struct SubMeshSection {
		MeshSection triangles;
		MeshSection wedges;
		MeshSection colors;
		MeshSection triangle_plane_indices;
		MeshSection triangle_planes;
		MeshSection wedge_map;
		MeshSection vertex_updates;
		MeshSection triangle_edges;
		MeshSection edges;
		MeshSection edge_scores;
	};

	/// \brief Represents a sub-mesh.
	struct SubMesh {
		/// \brief The material of this sub mesh.
		Material mat;

		std::vector<MeshTriangle> triangles;
		std::vector<MeshWedge> wedges;
		std::vector<float> colors;
		std::vector<std::uint16_t> triangle_plane_indices;
		std::vector<MeshPlane> triangle_planes;
		std::vector<MeshTriangleEdge> triangle_edges;
		std::vector<MeshEdge> edges;
		std::vector<float> edge_scores;
		std::vector<std::uint16_t> wedge_map;

		ZKINT void load(Read* r, SubMeshSection const& map);
	};

	/// \brief Represents a *ZenGin* proto mesh.
	///
	/// <p>A proto mesh is a mesh which is made up of multiple sub-meshes. Each sub-mesh has its own material and
	/// related values.</p>
	class MultiResolutionMesh {
	public:
		/// \brief Parses a proto mesh from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed proto mesh.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws zenkit::ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static MultiResolutionMesh parse(phoenix::buffer& in);

		/// \brief Parses a proto mesh from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed proto mesh.
		/// \throws zenkit::ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static MultiResolutionMesh parse(phoenix::buffer&& in);

		ZKAPI void load(Read* r);
		ZKINT void load_from_section(Read* r);

	public:
		/// \brief The vertex positions associated with the mesh.
		std::vector<glm::vec3> positions;

		/// \brief The normal vectors of the mesh.
		std::vector<glm::vec3> normals;

		/// \brief A list of sub-meshes of the mesh.
		std::vector<SubMesh> sub_meshes;

		/// \brief A list of all materials used by the mesh.
		std::vector<Material> materials;

		/// \brief If alpha testing should be enabled.
		std::uint8_t alpha_test {true};

		/// \brief The bounding box of the mesh.
		AxisAlignedBoundingBox bbox;

		OrientedBoundingBox obbox;
	};
} // namespace zenkit
