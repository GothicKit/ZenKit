// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Boxes.hh"
#include "zenkit/Date.hh"
#include "zenkit/Library.hh"
#include "zenkit/Material.hh"
#include "zenkit/Texture.hh"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <string>
#include <vector>

namespace zenkit {
	class Read;
	class Write;

	/// \brief Represents a light map.
	struct LightMap {
		std::shared_ptr<Texture> image;
		glm::vec3 normals[2];
		glm::vec3 origin;
	};

	/// \brief Represents a vertex feature.
	struct VertexFeature {
		/// \brief The texture coordinates of the polygon.
		glm::vec2 texture;

		/// \brief A value indicating the light level of the polygon.
		uint32_t light;

		/// The normal vector of the polygon.
		glm::vec3 normal;
	};

	/// \brief Flags set for a polygon of a mesh.
	struct PolygonFlagSet {
		std::uint8_t is_portal : 2;
		std::uint8_t is_occluder : 1;
		std::uint8_t is_sector : 1;
		std::uint8_t should_relight : 1;
		std::uint8_t is_outdoor : 1;
		std::uint8_t is_ghost_occluder : 1;
		std::uint8_t is_dynamically_lit : 1;
		std::int16_t sector_index : 16;

		uint8_t is_lod : 1;
		uint8_t normal_axis : 2;

		ZKAPI bool operator==(PolygonFlagSet const& b) const;
	};

	/// \brief List of data indices for polygons of meshes.
	struct PolygonList {
		/// \brief The index of the material for each polygon in mesh::materials
		std::vector<uint32_t> material_indices {};

		/// \brief The index of the light map for each polygon in mesh::lightmaps. May be -1.
		std::vector<int32_t> lightmap_indices {};

		/// \brief The index of the vertex features of each polygon in mesh::features.
		std::vector<uint32_t> feature_indices {};

		/// \brief The indices of each vertex for a polygon. Three consecutive values form one polygon.
		std::vector<uint32_t> vertex_indices {};

		/// \brief The flags for each polygon.
		std::vector<PolygonFlagSet> flags {};
	};

	struct Polygon {
		uint32_t material;
		int32_t lightmap;
		PolygonFlagSet flags;

		size_t index_count;
		size_t index_offset;
	};

	/// \brief Represents a *ZenGin* basic mesh.
	///
	/// <p>Found in files with the `MSH` extension and as the world meshes in world archives, these meshes contain a
	/// set of polygons and accompanying data describing a static mesh in three dimensions.</p>
	class Mesh {
	public:
		ZKAPI void load(Read* r, bool force_wide_indices);
		ZKAPI void load(Read* r, std::vector<std::uint32_t> const& leaf_polygons, bool force_wide_indices);
		ZKAPI void save(Write* w, GameVersion version) const;

	private:
		ZKINT void triangulate(std::vector<std::uint32_t> const& leaf_polygons);

	public:
		/// \brief The creation date of this mesh.
		Date date {};

		/// \brief The name of this mesh
		std::string name {};

		/// \brief The bounding box of this mesh.
		AxisAlignedBoundingBox bbox {};

		/// \brief The oriented bbox tree of this mesh.
		OrientedBoundingBox obb {};

		/// \brief A list of materials used by this mesh.
		std::vector<Material> materials {};

		/// \brief A list of vertices of this mesh.
		std::vector<glm::vec3> vertices {};

		/// \brief A list of vertex features of this mesh.
		std::vector<VertexFeature> features {};

		/// \brief All shared light-maps associated with this mesh
		std::vector<LightMap> lightmaps {};

		std::vector<Polygon> geometry {};
		std::vector<uint32_t> polygon_vertex_indices;
		std::vector<uint32_t> polygon_feature_indices;

		/// \brief A list of polygons of this mesh.
		PolygonList polygons {};
	};
} // namespace zenkit
