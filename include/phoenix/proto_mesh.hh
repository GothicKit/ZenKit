// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/detail/buffer.hh>
#include <phoenix/material.hh>
#include <phoenix/math/obb.hh>

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

	struct mesh_section {
		std::uint32_t offset;
		std::uint32_t size;
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

		/**
		 * @brief Reads sub-mesh data from a reader.
		 * @param in The reader to read from
		 * @param map A a section map for the sub-mesh.
		 * @return The sub-mesh read.
		 */
		static sub_mesh parse(buffer& in, const sub_mesh_section& map);
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
		static proto_mesh parse(buffer& in);

		/**
		 * @brief Reads a proto mesh directly from a section.
		 * @param in The section to read from.
		 * @return The mesh just read.
		 */
		static proto_mesh parse_from_section(buffer& in);

		/**
		 * @return The vertex positions associated with the mesh.
		 */
		[[nodiscard]] inline const std::vector<glm::vec3>& positions() const noexcept {
			return _m_vertices;
		}

		/**
		 * @return The normal vectors of the mesh.
		 * @note I'm not yet sure why this is here since normals are stored in sub-meshes as well.
		 */
		[[nodiscard]] inline const std::vector<glm::vec3>& normals() const noexcept {
			return _m_normals;
		}

		/**
		 * @return A list of sub-meshes of the mesh.
		 */
		[[nodiscard]] inline const std::vector<sub_mesh>& submeshes() const noexcept {
			return _m_sub_meshes;
		}

		/**
		 * @return A list of all materials used by the mesh.
		 */
		[[nodiscard]] inline const std::vector<material>& materials() const noexcept {
			return _m_materials;
		}

		/**
		 * @return The bounding box of the mesh as a (min, max) tuple.
		 */
		[[nodiscard]] inline bounding_box bbox() const noexcept {
			return _m_bbox;
		}

		/**
		 * @return If alpha testing should be enabled.
		 */
		[[nodiscard]] inline bool alpha_test() const noexcept {
			return _m_has_alpha_test;
		}

		static constexpr auto version_g1 = 0x305;
		static constexpr auto version_g2 = 0x905;

	private:
		std::vector<glm::vec3> _m_vertices;
		std::vector<glm::vec3> _m_normals;
		std::vector<sub_mesh> _m_sub_meshes;
		std::vector<material> _m_materials;
		std::uint8_t _m_has_alpha_test {true};
		bounding_box _m_bbox;
		obb _m_obbox;
	};
} // namespace phoenix
