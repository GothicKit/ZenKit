// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"
#include <phoenix/buffer.hh>
#include <phoenix/material.hh>
#include <phoenix/math.hh>

namespace phoenix {
	struct triangle {
		std::uint16_t wedges[3];
	};

	struct triangle_edge {
		std::uint16_t edges[3];
	};

	struct edge {
		std::uint16_t edges[2];
	};

	struct wedge {
		glm::vec3 normal;
		glm::vec2 texture;
		std::uint16_t index;
	};

	struct plane {
		float distance;
		glm::vec3 normal;
	};

	/// \brief An offset and size tuple for mesh sections.
	struct mesh_section {
		std::uint32_t offset;
		std::uint32_t size;
	};

	/// \brief Offsets and sizes of binary data sections containing sub-mesh data.
	/// \note This is only of use phoenix-internally.
	struct sub_mesh_section {
		mesh_section triangles;
		mesh_section wedges;
		mesh_section colors;
		mesh_section triangle_plane_indices;
		mesh_section triangle_planes;
		mesh_section wedge_map;
		mesh_section vertex_updates;
		mesh_section triangle_edges;
		mesh_section edges;
		mesh_section edge_scores;
	};

	/// \brief Represents a sub-mesh.
	struct sub_mesh {
		/// \brief The material of this sub mesh.
		material mat;

		std::vector<triangle> triangles;
		std::vector<wedge> wedges;
		std::vector<float> colors;
		std::vector<std::uint16_t> triangle_plane_indices;
		std::vector<plane> triangle_planes;
		std::vector<triangle_edge> triangle_edges;
		std::vector<edge> edges;
		std::vector<float> edge_scores;
		std::vector<std::uint16_t> wedge_map;

		/// \brief Reads sub-mesh data from a reader.
		/// \param in The reader to read from
		/// \param map A a section map for the sub-mesh.
		/// \return The sub-mesh read.
		[[nodiscard]] PHOENIX_INTERNAL static sub_mesh parse(buffer& in, const sub_mesh_section& map);
	};

	/// \brief Represents a *ZenGin* proto mesh.
	///
	/// <p>A proto mesh is a mesh which is made up of multiple sub-meshes. Each sub-mesh has its own material and
	/// related values.</p>
	class proto_mesh {
	public:
		/// \brief Parses a proto mesh from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed proto mesh.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] PHOENIX_API static proto_mesh parse(buffer& in);

		/// \brief Parses a proto mesh from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed proto mesh.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] PHOENIX_API static proto_mesh parse(buffer&& in) {
			return proto_mesh::parse(in);
		}

		/// \brief Parses a proto mesh directly from the given buffer.
		///
		/// This function assumes that the caller has already parsed part of the file and should only be used if you
		/// know what you're doing. If you just want to parse a basic proto mesh, please use #parse.
		///
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed proto mesh.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws parser_error if parsing fails.
		[[nodiscard]] PHOENIX_INTERNAL static proto_mesh parse_from_section(buffer& in);

	public:
		/// \brief The vertex positions associated with the mesh.
		std::vector<glm::vec3> positions;

		/// \brief The normal vectors of the mesh.
		std::vector<glm::vec3> normals;

		/// \brief A list of sub-meshes of the mesh.
		std::vector<sub_mesh> sub_meshes;

		/// \brief A list of all materials used by the mesh.
		std::vector<material> materials;

		/// \brief If alpha testing should be enabled.
		std::uint8_t alpha_test {true};

		/// \brief The bounding box of the mesh.
		bounding_box bbox;

		obb obbox;
	};
} // namespace phoenix
