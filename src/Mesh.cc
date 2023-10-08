// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Mesh.hh"
#include "zenkit/Archive.hh"
#include "zenkit/Stream.hh"

namespace zenkit {
	[[maybe_unused]] static constexpr auto MESH_VERSION_G1 = 9;
	static constexpr auto MESH_VERSION_G2 = 265;

	enum class MeshChunkType : std::uint16_t {
		MARKER = 0xB000,
		BBOX = 0xB010,
		MATERIAL = 0xB020,
		LIGHTMAPS = 0xB025,
		LIGHTMAPS_SHARED = 0xB026,
		VERTICES = 0xB030,
		FEATURES = 0xB040,
		POLYGONS = 0xB050,
		END = 0xB060
	};

	Mesh Mesh::parse(phoenix::buffer& buf, std::vector<std::uint32_t> const& leaf_polygons, bool force_wide_indices) {
		Mesh msh {};

		auto r = Read::from(&buf);
		msh.load(r.get(), leaf_polygons, force_wide_indices);

		return msh;
	}

	bool PolygonFlagSet::operator==(PolygonFlagSet const& b) const {
		return is_portal == b.is_portal && is_occluder == b.is_occluder && is_sector == b.is_sector &&
		    should_relight == b.should_relight && is_outdoor == b.is_outdoor &&
		    is_ghost_occluder == b.is_ghost_occluder && is_dynamically_lit == b.is_dynamically_lit &&
		    sector_index == b.sector_index && is_lod == b.is_lod && normal_axis == b.normal_axis;
	}

	Mesh Mesh::parse(phoenix::buffer&& buf, std::vector<std::uint32_t> const& include_polygons) {
		return Mesh::parse(buf, include_polygons);
	}

	void Mesh::load(Read* r, std::vector<std::uint32_t> const& leaf_polygons, bool force_wide_indices) {
		std::uint16_t version {};

		proto::read_chunked<MeshChunkType>(
		    r,
		    "Mesh",
		    [this, &version, &leaf_polygons, force_wide_indices](Read* c, MeshChunkType type) {
			    switch (type) {
			    case MeshChunkType::MARKER:
				    version = c->read_ushort();
				    this->date.load(c);
				    this->name = c->read_line(false);
				    break;
			    case MeshChunkType::BBOX:
				    // first, we find a basic AABB bounding box
				    this->bbox.load(c);

				    // but second, we find a list of OOBBs with one acting as a parent
				    this->obb.load(c);
				    break;
			    case MeshChunkType::MATERIAL: {
				    auto matreader = ReadArchive::from(c);

				    this->materials.resize(c->read_uint());
				    for (auto& material : this->materials) {
					    material.load(*matreader);
				    }

				    break;
			    }
			    case MeshChunkType::VERTICES:
				    this->vertices.resize(c->read_uint());

				    for (auto& vertex : this->vertices) {
					    vertex = c->read_vec3();
				    }

				    break;
			    case MeshChunkType::FEATURES:
				    this->features.resize(c->read_uint());

				    for (auto& feature : this->features) {
					    feature.texture = c->read_vec2();
					    feature.light = c->read_uint();
					    feature.normal = c->read_vec3();
				    }

				    break;
			    case MeshChunkType::POLYGONS: {
				    auto poly_count = c->read_uint();

				    this->polygons.material_indices.reserve(poly_count);
				    this->polygons.lightmap_indices.reserve(poly_count);
				    this->polygons.feature_indices.reserve(poly_count * 3);
				    this->polygons.vertex_indices.reserve(poly_count * 3);
				    this->polygons.flags.reserve(poly_count);

				    for (std::uint32_t i = 0; i < poly_count; ++i) {
					    auto material_index = c->read_ushort();
					    auto lightmap_index = c->read_short();

					    (void) c->read_float();
					    (void) c->read_vec3();

					    PolygonFlagSet pflags {};
					    if (version == MESH_VERSION_G2) {
						    std::uint8_t flags = c->read_ubyte();
						    pflags.is_portal = (flags & 0b00000011) >> 0;
						    pflags.is_occluder = (flags & 0b00000100) >> 2;
						    pflags.is_sector = (flags & 0b00001000) >> 3;
						    pflags.should_relight = (flags & 0b00010000) >> 4;
						    pflags.is_outdoor = (flags & 0b00100000) >> 5;
						    pflags.is_ghost_occluder = (flags & 0b01000000) >> 6;
						    pflags.is_dynamically_lit = (flags & 0b10000000) >> 7;
						    pflags.sector_index = c->read_short();
					    } else {
						    std::uint8_t flags1 = c->read_ubyte();
						    std::uint8_t flags2 = c->read_ubyte();

						    pflags.is_portal = (flags1 & 0b00000011) >> 0;
						    pflags.is_occluder = (flags1 & 0b00000100) >> 2;
						    pflags.is_sector = (flags1 & 0b00001000) >> 3;
						    pflags.is_lod = (flags1 & 0b00010000) >> 4;
						    pflags.is_outdoor = (flags1 & 0b00100000) >> 5;
						    pflags.is_ghost_occluder = (flags1 & 0b01000000) >> 6;
						    pflags.normal_axis = ((flags1 & 0b10000000) >> 7) | (flags2 & 0b00000001);
						    pflags.sector_index = c->read_short();
					    }

					    auto vertex_count = c->read_ubyte();
					    auto has_wide_indices = (version == MESH_VERSION_G2) || force_wide_indices;

					    // TODO: For meshes built for Gothic II, the `is_lod` flag can be used to determine whether a
					    //       polygon is a leaf-polygon or not. Gothic I does not have this luxury, so the leaf
					    //       polygons have to be taken from the BSP tree.
					    //
					    //       This presents a problem: Taking the leaf polygons as a parameter makes creating a
					    //       unified parsing function for world meshes impossible. Instead, there should be a
					    //       function to remove this extra data which would grant the user more freedom in how they
					    //       use _ZenKit_.
					    if (!leaf_polygons.empty() &&
					        !std::binary_search(leaf_polygons.begin(), leaf_polygons.end(), i)) {
						    // If the current polygon is not a leaf polygon, skip it.
						    c->seek((has_wide_indices ? 8 : 6) * vertex_count, Whence::CUR);
						    continue;
					    } else if (vertex_count == 0 || pflags.is_portal || pflags.is_ghost_occluder ||
					               pflags.is_outdoor) {
						    // There is no actual geometry associated with this vertex; ignore it.
						    c->seek((has_wide_indices ? 8 : 6) * vertex_count, Whence::CUR);
					    } else if (vertex_count == 3) {
						    // If we have 3 vertices, we are sure that this is already a triangle,
						    // so we can just read it in
						    for (int32_t j = 0; j < vertex_count; ++j) {
							    this->polygons.vertex_indices.push_back(has_wide_indices ? c->read_uint()
							                                                             : c->read_ushort());
							    this->polygons.feature_indices.push_back(c->read_uint());
						    }

						    this->polygons.material_indices.push_back(material_index);
						    this->polygons.lightmap_indices.push_back(lightmap_index);
						    this->polygons.flags.push_back(pflags);
					    } else {
						    // If we don't have 3 vertices, we need to calculate a triangle fan.

						    auto vertex_index_root = has_wide_indices ? c->read_uint() : c->read_ushort();
						    auto feature_index_root = c->read_uint();

						    auto vertex_index_a = has_wide_indices ? c->read_uint() : c->read_ushort();
						    auto feature_index_a = c->read_uint();

						    for (int32_t j = 0; j < vertex_count - 2; ++j) {
							    auto vertex_index_b = has_wide_indices ? c->read_uint() : c->read_ushort();
							    auto feature_index_b = c->read_uint();

							    this->polygons.vertex_indices.push_back(vertex_index_root);
							    this->polygons.vertex_indices.push_back(vertex_index_a);
							    this->polygons.vertex_indices.push_back(vertex_index_b);
							    this->polygons.feature_indices.push_back(feature_index_root);
							    this->polygons.feature_indices.push_back(feature_index_a);
							    this->polygons.feature_indices.push_back(feature_index_b);

							    this->polygons.material_indices.push_back(material_index);
							    this->polygons.lightmap_indices.push_back(lightmap_index);
							    this->polygons.flags.push_back(pflags);

							    vertex_index_a = vertex_index_b;
							    feature_index_a = feature_index_b;
						    }
					    }
				    }

				    break;
			    }
			    case MeshChunkType::LIGHTMAPS_SHARED: {
				    auto texture_count = c->read_uint();

				    std::vector<std::shared_ptr<Texture>> lightmap_textures {};
				    lightmap_textures.resize(texture_count);

				    for (std::uint32_t i = 0; i < texture_count; ++i) {
					    lightmap_textures[i] = std::make_shared<Texture>();
					    lightmap_textures[i]->load(c);
				    }

				    auto lightmap_count = c->read_uint();
				    for (std::uint32_t i = 0; i < lightmap_count; ++i) {
					    auto origin = c->read_vec3();
					    auto normal_a = c->read_vec3();
					    auto normal_b = c->read_vec3();
					    std::uint32_t texture_index = c->read_uint();

					    this->lightmaps.emplace_back(
					        LightMap {lightmap_textures[texture_index], {normal_a, normal_b}, origin});
				    }

				    break;
			    }
			    case MeshChunkType::LIGHTMAPS: {
				    auto lightmap_count = c->read_uint();

				    for (std::uint32_t i = 0; i < lightmap_count; ++i) {
					    auto origin = c->read_vec3();
					    auto normal_a = c->read_vec3();
					    auto normal_b = c->read_vec3();

					    Texture lightmap_texture {};
					    lightmap_texture.load(c);

					    this->lightmaps.emplace_back(LightMap {std::make_shared<Texture>(std::move(lightmap_texture)),
					                                           {normal_a, normal_b},
					                                           origin});
				    }

				    break;
			    }
			    case MeshChunkType::END:
				    return true;
			    default:
				    break;
			    }

			    return false;
		    });
	}
} // namespace zenkit
