// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/mesh.hh>

#include <fmt/format.h>

namespace phoenix {
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

	mesh mesh::parse(buffer& buf) {
		mesh msh;

		std::uint16_t version {};
		bool finished = false;
		world_mesh_chunk type = world_mesh_chunk::unknown;

		do {
			type = static_cast<world_mesh_chunk>(buf.get_ushort());

			auto length = buf.get_uint();
			auto chunk = buf.extract(length);

			switch (type) {
			case world_mesh_chunk::mesh:
				version = chunk.get_uint();
				msh._m_date = date::parse(chunk);
				msh._m_name = chunk.get_line(false);
				break;
			case world_mesh_chunk::bbox:
				// first, we find a basic AABB bounding box
				msh._m_bbox = bounding_box::parse(chunk);

				// but second, we find a list of OOBBs with one acting as a parent
				msh._m_obb = obb::parse(chunk);
				break;
			case world_mesh_chunk::material: {
				auto matreader = archive_reader::open(chunk);

				std::uint32_t material_count = chunk.get_uint();
				msh._m_materials.reserve(material_count);

				for (std::uint32_t i = 0; i < material_count; ++i) {
					msh._m_materials.emplace_back(material::parse(matreader));
				}

				break;
			}
			case world_mesh_chunk::vertices:
				msh._m_vertices.resize(chunk.get_uint());

				for (auto& vertex : msh._m_vertices) {
					vertex = chunk.get_vec3();
				}

				break;
			case world_mesh_chunk::features:
				msh._m_features.resize(chunk.get_uint());

				for (auto& feature : msh._m_features) {
					feature.texture = chunk.get_vec2();
					feature.light = chunk.get_uint();
					feature.normal = chunk.get_vec3();
				}

				break;
			case world_mesh_chunk::polygons: {
				auto poly_count = chunk.get_uint();
				msh._m_polygons.resize(poly_count);

				for (std::uint32_t i = 0; i < poly_count; ++i) {
					polygon& p = msh._m_polygons[i];

					p.material_index = chunk.get_ushort();
					p.lightmap_index = chunk.get_ushort();
					p.polygon_plane = {chunk.get_float(), chunk.get_vec3()};

					if (version == version_g2) {
						std::uint8_t flags = chunk.get();
						p.flags.is_portal = (flags & 0b00000011) >> 0;
						p.flags.is_occluder = (flags & 0b00000100) >> 2;
						p.flags.is_sector = (flags & 0b00001000) >> 3;
						p.flags.should_relight = (flags & 0b00010000) >> 4;
						p.flags.is_outdoor = (flags & 0b00100000) >> 5;
						p.flags.is_ghost_occluder = (flags & 0b01000000) >> 6;
						p.flags.is_dynamically_lit = (flags & 0b10000000) >> 7;
						p.flags.sector_index = chunk.get_ushort();
					} else {
						std::uint8_t flags1 = chunk.get();
						std::uint8_t flags2 = chunk.get();

						p.flags.is_portal = (flags1 & 0b00000011) >> 0;
						p.flags.is_occluder = (flags1 & 0b00000100) >> 2;
						p.flags.is_sector = (flags1 & 0b00001000) >> 3;
						p.flags.is_lod = (flags1 & 0b00010000) >> 4;
						p.flags.is_outdoor = (flags1 & 0b00100000) >> 5;
						p.flags.is_ghost_occluder = (flags1 & 0b01000000) >> 6;
						p.flags.normal_axis = ((flags1 & 0b10000000) >> 7) | (flags2 & 0b00000001);
						p.flags.sector_index = chunk.get_ushort();
					}

					p.vertex_count = chunk.get();

					for (int j = 0; j < p.vertex_count; ++j) {
						p.indices[j] = {version == version_g2 ? chunk.get_uint() : chunk.get_ushort(),
						                chunk.get_uint()};
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

					msh._m_lightmaps.emplace_back(
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

					msh._m_lightmaps.emplace_back(light_map {std::make_shared<texture>(std::move(lightmap_texture)),
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

			if (chunk.remaining() != 0)
				fmt::print(stderr, "warning: world mesh: not all data consumed from section 0x{:X}\n", type);
		} while (!finished);

		return msh;
	}
} // namespace phoenix
