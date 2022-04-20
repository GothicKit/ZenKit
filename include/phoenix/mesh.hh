// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/detail/buffer.hh>
#include <phoenix/material.hh>
#include <phoenix/math/bbox.hh>
#include <phoenix/math/light_map.hh>
#include <phoenix/math/obb.hh>
#include <phoenix/proto_mesh.hh>
#include <phoenix/texture.hh>

namespace phoenix {
	struct date {
		/**
		 * @brief Parses a date from a buffer.
		 * @param buf The buffer to read from
		 * @return The date.
		 */
		static date parse(buffer& buf) {
			auto dt = date {buf.get_uint(),
			                buf.get_ushort(),
			                buf.get_ushort(),
			                buf.get_ushort(),
			                buf.get_ushort(),
			                buf.get_ushort()};
			return dt;
		}

		std::uint32_t year;
		std::uint16_t month;
		std::uint16_t day;
		std::uint16_t hour;
		std::uint16_t minute;
		std::uint16_t second;
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
		std::uint8_t is_portal : 2;
		std::uint8_t is_occluder : 1;
		std::uint8_t is_sector : 1;
		std::uint8_t should_relight : 1;
		std::uint8_t is_outdoor : 1;
		std::uint8_t is_ghost_occluder : 1;
		std::uint8_t is_dynamically_lit : 1;
		std::uint16_t sector_index : 16;

		uint8_t is_lod : 1;
		uint8_t normal_axis : 2;
	};

	struct polygon_list {
		std::vector<uint32_t> material_indices {};
		std::vector<uint32_t> lightmap_indices {};
		std::vector<uint32_t> feature_indices {};
		std::vector<uint32_t> vertex_indices {};
		std::vector<polygon_flags> flags {};
	};

	/**
	 * @brief Represents a basic mesh.
	 *
	 * Parses ZenGin meshes. The reference implementation can be found on GitHub:
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
		 * @brief Parses a mesh from the given reader.
		 * @param in The reader to read from.
		 * @return The mesh parsed.
		 */
		[[nodiscard]] static mesh parse(buffer& in, const std::vector<std::uint32_t>& leaf_polygons);

		/**
		 * @return The creation date of the mesh.
		 */
		[[nodiscard]] inline phoenix::date date() const noexcept {
			return _m_date;
		}

		/**
		 * @return The name of the mesh
		 */
		[[nodiscard]] inline const std::string& name() const noexcept {
			return _m_name;
		}

		/**
		 * @return The bounding box of the mesh as a (min, max) tuple.
		 */
		[[nodiscard]] inline bounding_box bbox() const noexcept {
			return _m_bbox;
		}

		/**
		 * @return A list of materials used by the mesh.
		 */
		[[nodiscard]] inline const std::vector<material>& materials() const noexcept {
			return _m_materials;
		}

		/**
		 * @return A list of vertices of the mesh.
		 */
		[[nodiscard]] inline const std::vector<glm::vec3>& vertices() const noexcept {
			return _m_vertices;
		}

		/**
		 * @return A list of vertex features of the mesh.
		 */
		[[nodiscard]] inline const std::vector<vertex_feature>& features() const noexcept {
			return _m_features;
		}

		/**
		 * @return A list of polygons of the mesh.
		 */
		[[nodiscard]] inline const polygon_list& polygons() const noexcept {
			return _m_polygons;
		}

		[[nodiscard]] inline polygon_list& polygons() noexcept {
			return _m_polygons;
		}

		/**
		 * @return The oriented bbox tree of the mesh.
		 */
		[[nodiscard]] const obb& oriented_bbox() const noexcept {
			return _m_obb;
		};

		/**
		 * @return All shared lightmaps associated with the mesh
		 */
		[[nodiscard]] const std::vector<light_map>& lightmaps() const noexcept {
			return _m_lightmaps;
		}

		static constexpr auto version_g1 = 9;
		static constexpr auto version_g2 = 265;

	private:
		phoenix::date _m_date;
		std::string _m_name;
		bounding_box _m_bbox;
		obb _m_obb;

		std::vector<material> _m_materials;
		std::vector<glm::vec3> _m_vertices;
		std::vector<vertex_feature> _m_features;
		std::vector<light_map> _m_lightmaps;

		polygon_list _m_polygons {};
	};
} // namespace phoenix
