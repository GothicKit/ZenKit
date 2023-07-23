// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/mesh.hh>

namespace phoenix {
	[[maybe_unused]] static constexpr auto mesh_version_g1 = 9;
	static constexpr auto mesh_version_g2 = 265;

	enum class world_mesh_chunk : std::uint16_t {
		unknown,
		mesh = 0xB000,
		bbox = 0xB010,
		material = 0xB020,
		lightmaps = 0xB025,
		shared_lightmaps = 0xB026,
		vertices = 0xB030,
		features = 0xB040,
		polygons = 0xB050,
		end = 0xB060
	};

	mesh mesh::parse(buffer& buf, std::vector<std::uint32_t> const& leaf_polygons) {
		mesh msh {};

		std::uint16_t version {};
		bool finished = false;
		world_mesh_chunk type = world_mesh_chunk::unknown;

		do {
			type = static_cast<world_mesh_chunk>(buf.get_ushort());

			auto length = buf.get_uint();
			auto chunk = buf.extract(length);

			switch (type) {
			case world_mesh_chunk::mesh:
				version = chunk.get_ushort();
				msh.date = date::parse(chunk);
				msh.name = chunk.get_line(false);
				break;
			case world_mesh_chunk::bbox:
				// first, we find a basic AABB bounding box
				msh.bbox = bounding_box::parse(chunk);

				// but second, we find a list of OOBBs with one acting as a parent
				msh.obb = obb::parse(chunk);
				break;
			case world_mesh_chunk::material: {
				auto matreader = archive_reader::open(chunk);

				std::uint32_t material_count = chunk.get_uint();
				msh.materials.reserve(material_count);

				for (std::uint32_t i = 0; i < material_count; ++i) {
					msh.materials.emplace_back(material::parse(*matreader));
				}

				break;
			}
			case world_mesh_chunk::vertices:
				msh.vertices.resize(chunk.get_uint());

				for (auto& vertex : msh.vertices) {
					vertex = chunk.get_vec3();
				}

				break;
			case world_mesh_chunk::features:
				msh.features.resize(chunk.get_uint());

				for (auto& feature : msh.features) {
					feature.texture = chunk.get_vec2();
					feature.light = chunk.get_uint();
					feature.normal = chunk.get_vec3();
				}

				break;
			case world_mesh_chunk::polygons: {
				auto poly_count = chunk.get_uint();

				msh.polygons.material_indices.reserve(poly_count);
				msh.polygons.lightmap_indices.reserve(poly_count);
				msh.polygons.feature_indices.reserve(poly_count * 3);
				msh.polygons.vertex_indices.reserve(poly_count * 3);
				msh.polygons.flags.reserve(poly_count);

				for (std::uint32_t i = 0; i < poly_count; ++i) {

					auto material_index = chunk.get_short();
					auto lightmap_index = chunk.get_short();

					[[maybe_unused]] plane polygon_plane = {chunk.get_float(), chunk.get_vec3()};
					polygon_flags pflags {};

					if (version == mesh_version_g2) {
						std::uint8_t flags = chunk.get();
						pflags.is_portal = (flags & 0b00000011) >> 0;
						pflags.is_occluder = (flags & 0b00000100) >> 2;
						pflags.is_sector = (flags & 0b00001000) >> 3;
						pflags.should_relight = (flags & 0b00010000) >> 4;
						pflags.is_outdoor = (flags & 0b00100000) >> 5;
						pflags.is_ghost_occluder = (flags & 0b01000000) >> 6;
						pflags.is_dynamically_lit = (flags & 0b10000000) >> 7;
						pflags.sector_index = chunk.get_ushort();
					} else {
						std::uint8_t flags1 = chunk.get();
						std::uint8_t flags2 = chunk.get();

						pflags.is_portal = (flags1 & 0b00000011) >> 0;
						pflags.is_occluder = (flags1 & 0b00000100) >> 2;
						pflags.is_sector = (flags1 & 0b00001000) >> 3;
						pflags.is_lod = (flags1 & 0b00010000) >> 4;
						pflags.is_outdoor = (flags1 & 0b00100000) >> 5;
						pflags.is_ghost_occluder = (flags1 & 0b01000000) >> 6;
						pflags.normal_axis = ((flags1 & 0b10000000) >> 7) | (flags2 & 0b00000001);
						pflags.sector_index = chunk.get_ushort();
					}

					auto vertex_count = chunk.get();

					// TODO: For meshes built for Gothic II, the `is_lod` flag can be used to determine whether a
					//       polygon is a leaf-polygon or not. Gothic I does not have this luxury, so the leaf polygons
					//       have to be taken from the BSP tree.
					//
					//       This presents a problem: Taking the leaf polygons as a parameter makes creating a unified
					//       parsing function for world meshes impossible. Instead, there should be a function to remove
					//       this extra data which would grant the user more freedom in how they use _phoenix_.
					if (!leaf_polygons.empty() && !std::binary_search(leaf_polygons.begin(), leaf_polygons.end(), i)) {
						// If the current polygon is not a leaf polygon, skip it.
						chunk.skip((version == mesh_version_g2 ? 8 : 6) * vertex_count);
						continue;
					} else if (vertex_count == 0 || pflags.is_portal || pflags.is_ghost_occluder || pflags.is_outdoor) {
						// There is no actual geometry associated with this vertex; ignore it.
						chunk.skip((version == mesh_version_g2 ? 8 : 6) * vertex_count);
					} else if (vertex_count == 3) {
						// If we have 3 vertices, we are sure that this is already a triangle,
						// so we can just read it in
						for (int32_t j = 0; j < vertex_count; ++j) {
							msh.polygons.vertex_indices.push_back(version == mesh_version_g2 ? chunk.get_uint()
							                                                                 : chunk.get_ushort());

							msh.polygons.feature_indices.push_back(chunk.get_uint());
						}

						msh.polygons.material_indices.push_back(material_index);
						msh.polygons.lightmap_indices.push_back(lightmap_index);
						msh.polygons.flags.push_back(pflags);
					} else {
						// If we don't have 3 vertices, we need to calculate a triangle fan.

						auto vertex_index_root = version == mesh_version_g2 ? chunk.get_uint() : chunk.get_ushort();
						auto feature_index_root = chunk.get_uint();

						auto vertex_index_a = version == mesh_version_g2 ? chunk.get_uint() : chunk.get_ushort();
						auto feature_index_a = chunk.get_uint();

						for (int32_t j = 0; j < vertex_count - 2; ++j) {
							auto vertex_index_b = version == mesh_version_g2 ? chunk.get_uint() : chunk.get_ushort();
							auto feature_index_b = chunk.get_uint();

							msh.polygons.vertex_indices.push_back(vertex_index_root);
							msh.polygons.vertex_indices.push_back(vertex_index_a);
							msh.polygons.vertex_indices.push_back(vertex_index_b);
							msh.polygons.feature_indices.push_back(feature_index_root);
							msh.polygons.feature_indices.push_back(feature_index_a);
							msh.polygons.feature_indices.push_back(feature_index_b);

							msh.polygons.material_indices.push_back(material_index);
							msh.polygons.lightmap_indices.push_back(lightmap_index);
							msh.polygons.flags.push_back(pflags);

							vertex_index_a = vertex_index_b;
							feature_index_a = feature_index_b;
						}
					}
				}

				break;
			}
			case world_mesh_chunk::shared_lightmaps: {
				auto texture_count = chunk.get_uint();

				std::vector<std::shared_ptr<texture>> lightmap_textures {};
				lightmap_textures.resize(texture_count);

				for (std::uint32_t i = 0; i < texture_count; ++i) {
					lightmap_textures[i] = std::make_shared<texture>(texture::parse(chunk));
				}

				auto lightmap_count = chunk.get_uint();
				for (std::uint32_t i = 0; i < lightmap_count; ++i) {
					auto origin = chunk.get_vec3();
					auto normal_a = chunk.get_vec3();
					auto normal_b = chunk.get_vec3();
					std::uint32_t texture_index = chunk.get_uint();

					msh.lightmaps.emplace_back(
					    light_map {lightmap_textures[texture_index], {normal_a, normal_b}, origin});
				}

				break;
			}
			case world_mesh_chunk::lightmaps: {
				auto lightmap_count = chunk.get_uint();

				for (std::uint32_t i = 0; i < lightmap_count; ++i) {
					auto origin = chunk.get_vec3();
					auto normal_a = chunk.get_vec3();
					auto normal_b = chunk.get_vec3();
					auto lightmap_texture = texture::parse(chunk);

					msh.lightmaps.emplace_back(light_map {std::make_shared<texture>(std::move(lightmap_texture)),
					                                      {normal_a, normal_b},
					                                      origin});
				}

				break;
			}
			case world_mesh_chunk::end:
				finished = true;
				break;
			default:
				break;
			}

			if (chunk.remaining() != 0) {
				PX_LOGW("mesh: ", chunk.remaining(), " bytes remaining in section ", std::hex, std::uint16_t(type));
			}
		} while (!finished);

		return msh;
	}

	bool polygon_flags::operator==(const polygon_flags& b) const {
		return is_portal == b.is_portal && is_occluder == b.is_occluder && is_sector == b.is_sector &&
		    should_relight == b.should_relight && is_outdoor == b.is_outdoor &&
		    is_ghost_occluder == b.is_ghost_occluder && is_dynamically_lit == b.is_dynamically_lit &&
		    sector_index == b.sector_index && is_lod == b.is_lod && normal_axis == b.normal_axis;
	}
} // namespace phoenix
