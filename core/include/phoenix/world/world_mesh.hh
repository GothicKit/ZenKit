// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/detail/stream.hh>
#include <phoenix/material.hh>
#include <phoenix/mesh.hh>

namespace phoenix {
	struct date {
		u32 year;
		u16 month;
		u16 day;
		u16 hour;
		u16 minute;
		u16 second;
	};

	struct mesh_feature {
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

	struct polygon {
		u16 material_index;
		u16 lightmap_index;
		mesh::plane polygon_plane;

		polygon_flags flags;

		u8 vertex_count;
		polygon_index indices[255];// ZenLib uses 255 here. not sure why
	};

	class world_mesh {
	public:
		static world_mesh read(reader& in);

		inline phoenix::date date() const noexcept { return _m_date; }
		inline const std::string& name() const noexcept { return _m_name; }
		inline std::tuple<glm::vec4, glm::vec4> bbox() const noexcept { return std::make_tuple(_m_bbox[0], _m_bbox[1]); }
		inline const std::vector<material>& materials() const noexcept { return _m_materials; }
		inline const std::vector<glm::vec3>& vertices() const noexcept { return _m_vertices; }
		inline const std::vector<mesh_feature>& features() const noexcept { return _m_features; }
		inline const std::vector<polygon>& polygons() const noexcept { return _m_polygons; }

	private:
		phoenix::date _m_date;
		std::string _m_name;
		glm::vec4 _m_bbox[2];
		std::vector<material> _m_materials;
		std::vector<glm::vec3> _m_vertices;
		std::vector<mesh_feature> _m_features;
		std::vector<polygon> _m_polygons;
	};
}// namespace phoenix