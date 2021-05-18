// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/detail/stream.hh>
#include <phoenix/detail/types.hh>
#include <phoenix/material.hh>

namespace phoenix {
	namespace mesh {
		struct triangle {
			u16 wedges[3];
		};

		struct triangle_edge {
			u16 edges[3];
		};

		struct edge {
			u16 edges[2];
		};

		struct wedge {
			glm::vec3 normal;
			glm::vec2 texture;
			u16 index;
		};

		struct plane {
			float distance;
			glm::vec3 normal;
		};
	}// namespace mesh

	struct mesh_section {
		u32 offset;
		u32 size;
	};

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

	/**
	 * @brief Represents a sub-mesh
	 */
	struct sub_mesh {
		material material;
		std::vector<mesh::triangle> triangles;
		std::vector<mesh::wedge> wedges;
		std::vector<float> colors;
		std::vector<u16> triangle_plane_indices;
		std::vector<mesh::plane> triangle_planes;
		std::vector<mesh::triangle_edge> triangle_edges;
		std::vector<mesh::edge> edges;
		std::vector<float> edge_scores;
		std::vector<u16> wedge_map;

		/**
		 * @brief Reads sub-mesh data from a reader.
		 * @param in The reader to read from
		 * @param map A a section map for the sub-mesh.
		 * @return The sub-mesh read.
		 */
		static sub_mesh parse(reader& in, const sub_mesh_section& map);
	};

	/**
	 * @brief Represents a basic mesh (.MRM).
	 *
	 * Parses ZenGin mesh (MRM) files. The reference implementation can be found on GitHub:
	 * https://github.com/Try/ZenLib/blob/732077c82589f5060d1762839293b996c8222c18/zenload/zCProgMeshProto.cpp and the
	 * original version by Andre Taulien was also referenced:
	 * https://github.com/ataulien/ZenLib/blob/e1a5e1b12e71690a5470f3be2aa3d0d6419f5191/zenload/zCProgMeshProto.cpp
	 *
	 * Thanks to the original author, Andre Taulien as well as Try for additional work on their ZenLib fork!
	 *
	 * @see https://github.com/ataulien/ZenLib
	 * @see https://github.com/Try/ZenLib
	 */
	class proto_mesh {
	public:
		/**
		 * @brief Read a mesh from the given reader.
		 * @param in The reader to read from.
		 * @return The mesh read.
		 */
		static proto_mesh parse(reader& in);

		/**
		 * @return The vertex positions associated with the mesh.
		 */
		[[nodiscard]] inline const std::vector<glm::vec3>& positions() const noexcept { return _m_vertices; }

		/**
		 * @return The normal vectors of the mesh.
		 * @note I'm not yet sure why this is here since normals are stored in sub-meshes as well.
		 */
		[[nodiscard]] inline const std::vector<glm::vec3>& normals() const noexcept { return _m_normals; }

		/**
		 * @return A list of sub-meshes of the mesh.
		 */
		[[nodiscard]] inline const std::vector<sub_mesh>& submeshes() const noexcept { return _m_sub_meshes; }

		/**
		 * @return A list of all materials used by the mesh.
		 */
		[[nodiscard]] inline const std::vector<material>& materials() const noexcept { return _m_materials; }

		/**
		 * @return The bounding box of the mesh as a (min, max) tuple.
		 */
		[[nodiscard]] inline std::tuple<glm::vec3, glm::vec3> bbox() const noexcept { return std::make_tuple(_m_bbox[0], _m_bbox[1]); }

		/**
		 * @return If alpha testing should be enabled.
		 */
		[[nodiscard]] inline bool alpha_test() const noexcept { return _m_has_alpha_test; }

	private:
		std::vector<glm::vec3> _m_vertices;
		std::vector<glm::vec3> _m_normals;
		std::vector<sub_mesh> _m_sub_meshes;
		std::vector<material> _m_materials;
		u8 _m_has_alpha_test {true};
		glm::vec3 _m_bbox[2];
	};
}// namespace phoenix