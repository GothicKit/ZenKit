// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/detail/stream.hh>
#include <phoenix/material.hh>
#include <phoenix/math/light_map.hh>
#include <phoenix/math/obb.hh>
#include <phoenix/proto_mesh.hh>
#include <phoenix/texture.hh>

namespace phoenix {
	struct date {
		u32 year;
		u16 month;
		u16 day;
		u16 hour;
		u16 minute;
		u16 second;
	};

	/**
	 * @brief Represents a vertex feature.
	 */
	struct vertex_feature {
		glm::vec2 texture;
		uint32_t light;
		glm::vec3 normal;
	};

	struct polygon_flags {
		u8 is_portal : 2;
		u8 is_occluder : 1;
		u8 is_sector : 1;
		u8 should_relight : 1;
		u8 is_outdoor : 1;
		u8 is_ghost_occluder : 1;
		u8 is_dynamically_lit : 1;
		u16 sector_index : 16;

		uint8_t is_lod : 1;
		uint8_t normal_axis : 2;
	};

	struct polygon_index {
		uint32_t vertex;
		uint32_t feature;
	};

	/**
	 * @brief Represents a polygon.
	 */
	struct polygon {
		u16 material_index;
		u16 lightmap_index;
		plane polygon_plane;

		polygon_flags flags;

		u8 vertex_count;
		polygon_index indices[255];// ZenLib uses 255 here. not sure why
	};

	/**
	 * @brief Represents a world mesh.
	 *
	 * Parses ZenGin world meshes. The reference implementation can be found on GitHub:
	 * https://github.com/Try/ZenLib/blob/master/zenload/zCMesh.cpp and the
	 * original version by Andre Taulien was also referenced:
	 * https://github.com/ataulien/ZenLib/blob/e1a5e1b12e71690a5470f3be2aa3d0d6419f5191/zenload/zCMesh.cpp
	 *
	 * Thanks to the original author, Andre Taulien, CommanderJax and ousnius as well
	 * as Try for additional work on their ZenLib fork!
	 *
	 * @see https://github.com/ataulien/ZenLib
	 * @see https://github.com/Try/ZenLib
	 */
	class mesh {
	public:
		/**
		 * @brief Parses a world mesh from the given reader.
		 * @param in The reader to read from.
		 * @return The mesh parsed.
		 */
		[[nodiscard]] static mesh parse(reader& in);

		/**
		 * @return The creation date of the mesh.
		 */
		[[nodiscard]] inline phoenix::date date() const noexcept { return _m_date; }

		/**
		 * @return The name of the mesh
		 */
		[[nodiscard]] inline const std::string& name() const noexcept { return _m_name; }

		/**
		 * @return The bounding box of the mesh as a (min, max) tuple.
		 */
		[[nodiscard]] inline std::tuple<glm::vec3, glm::vec3> bbox() const noexcept { return std::make_tuple(_m_bbox[0], _m_bbox[1]); }

		/**
		 * @return A list of materials used by the mesh.
		 */
		[[nodiscard]] inline const std::vector<material>& materials() const noexcept { return _m_materials; }

		/**
		 * @return A list of vertices of the mesh.
		 */
		[[nodiscard]] inline const std::vector<glm::vec3>& vertices() const noexcept { return _m_vertices; }

		/**
		 * @return A list of vertex features of the mesh.
		 */
		[[nodiscard]] inline const std::vector<vertex_feature>& features() const noexcept { return _m_features; }

		/**
		 * @return A list of polygons of the mesh.
		 */
		[[nodiscard]] inline const std::vector<polygon>& polygons() const noexcept { return _m_polygons; }

		/**
		 * @return The oriented bbox tree of the mesh.
		 */
		[[nodiscard]] const obb& oriented_bbox() const noexcept { return _m_obb; };

		/**
		 * @return All shared lightmaps associated with the mesh
		 */
		[[nodiscard]] const std::vector<light_map>& lightmaps() const noexcept { return _m_lightmaps; }

	private:
		phoenix::date _m_date;
		std::string _m_name;
		glm::vec3 _m_bbox[2];
		obb _m_obb;
		std::vector<material> _m_materials;
		std::vector<glm::vec3> _m_vertices;
		std::vector<vertex_feature> _m_features;
		std::vector<polygon> _m_polygons;
		std::vector<light_map> _m_lightmaps;
	};
}// namespace phoenix