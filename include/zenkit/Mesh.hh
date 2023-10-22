// Copyright © 2021-2023 GothicKit Contributors.
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
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace phoenix {
	class buffer;
}

namespace zenkit {
	class Read;

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

	/// \brief Represents a *ZenGin* basic mesh.
	///
	/// <p>Found in files with the `MSH` extension and as the world meshes in world archives, these meshes contain a
	/// set of polygons and accompanying data describing a static mesh in three dimensions.</p>
	class Mesh {
	public:
		/// \brief Parses a mesh from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).</p>
		///
		/// \param[in,out] buf The buffer to read from.
		/// \param include_polygons A list of polygon indices to include in the final mesh. All other polygons are
		///                         discarded. This is mainly used for world meshes which include level-of-detail
		///                         polygons.
		/// \param force_wide_indices Set to true to force 32-bit vertex indices. Useful for parsing world meshes
		///                           with the XZEN extension (see
		///                           https://github.com/ThielHater/GRMFixes_Union/blob/master/GRMFixes/XZenFileFormat/Plugin_Source.hpp#L86).
		/// \return The parsed mesh object.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws zenkit::ParserError if parsing fails.
		/// \see #parse(buffer&&, const std::vector<std::uint32_t>&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static Mesh parse(phoenix::buffer& buf,
		                                                            std::vector<uint32_t> const& include_polygons = {},
		                                                            bool force_wide_indices = false);

		/// \brief Parses a mesh from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).</p>
		///
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \param include_polygons A list of polygon indices to include in the final mesh. All other polygons are
		///                         discarded. This is mainly used for world meshes which include level-of-detail
		///                         polygons.
		/// \return The parsed mesh object.
		/// \throws zenkit::ParserError if parsing fails.
		/// \see #parse(buffer&, const std::vector<std::uint32_t>&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static Mesh
		    parse(phoenix::buffer&& buf, std::vector<std::uint32_t> const& include_polygons = {});

		ZKAPI void load(Read* r, std::vector<std::uint32_t> const& leaf_polygons, bool force_wide_indices);

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

		/// \brief A list of polygons of this mesh.
		PolygonList polygons {};
	};
} // namespace zenkit
