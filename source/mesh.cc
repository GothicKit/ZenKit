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

	mesh mesh::parse(buffer& in) {
		mesh msh;

		std::uint16_t version {};
		bool finished = false;
		world_mesh_chunk chunk = world_mesh_chunk::unknown;
		std::uint32_t end = 0;

		do {
			chunk = static_cast<world_mesh_chunk>(in.get_ushort());

			auto length = in.get_uint();
			end = length + in.position();

			switch (chunk) {
			case world_mesh_chunk::mesh:
				version = in.get_uint();
				msh._m_date = {
				    in.get_uint(),
				    in.get_ushort(),
				    in.get_ushort(),
				    in.get_ushort(),
				    in.get_ushort(),
				    in.get_ushort(),
				};

				msh._m_name = in.get_line(false);
				break;
			case world_mesh_chunk::bbox:
				// first, we find a basic AABB bounding box
				msh._m_bbox[0] = in.get_vec3();
				msh._m_bbox[1] = in.get_vec3();

				// but second, we find a list of OOBBs with one acting as a parent
				msh._m_obb = obb::parse(in);
				break;
			case world_mesh_chunk::material: {
				auto matreader = archive_reader::open(in);

				std::uint32_t material_count = in.get_uint();
				msh._m_materials.reserve(material_count);

				for (std::uint32_t i = 0; i < material_count; ++i) {
					msh._m_materials.emplace_back(material::parse(matreader));
				}

				break;
			}
			case world_mesh_chunk::vertices:
				msh._m_vertices.resize(in.get_uint());

				for (auto& vertex : msh._m_vertices) {
					vertex = in.get_vec3();
				}
				break;
			case world_mesh_chunk::features:
				msh._m_features.resize(in.get_uint());

				for (auto& feature : msh._m_features) {
					feature.texture = in.get_vec2();
					feature.light = in.get_uint();
					feature.normal = in.get_vec3();
				}

				break;
			case world_mesh_chunk::polygons: {
				auto poly_count = in.get_uint();
				msh._m_polygons.resize(poly_count);

				for (std::uint32_t i = 0; i < poly_count; ++i) {
					polygon& p = msh._m_polygons[i];

					p.material_index = in.get_ushort();
					p.lightmap_index = in.get_ushort();
					p.polygon_plane = {in.get_float(), in.get_vec3()};

					if (version == 265 /* G26fix */) {
						std::uint8_t flags = in.get();
						p.flags.is_portal = (flags & 0b00000011) >> 0;
						p.flags.is_occluder = (flags & 0b00000100) >> 2;
						p.flags.is_sector = (flags & 0b00001000) >> 3;
						p.flags.should_relight = (flags & 0b00010000) >> 4;
						p.flags.is_outdoor = (flags & 0b00100000) >> 5;
						p.flags.is_ghost_occluder = (flags & 0b01000000) >> 6;
						p.flags.is_dynamically_lit = (flags & 0b10000000) >> 7;
						p.flags.sector_index = in.get_ushort();
					} else {
						std::uint8_t flags1 = in.get();
						std::uint8_t flags2 = in.get();

						p.flags.is_portal = (flags1 & 0b00000011) >> 0;
						p.flags.is_occluder = (flags1 & 0b00000100) >> 2;
						p.flags.is_sector = (flags1 & 0b00001000) >> 3;
						p.flags.is_lod = (flags1 & 0b00010000) >> 4;
						p.flags.is_outdoor = (flags1 & 0b00100000) >> 5;
						p.flags.is_ghost_occluder = (flags1 & 0b01000000) >> 6;
						p.flags.normal_axis = ((flags1 & 0b10000000) >> 7) | (flags2 & 0b00000001);
						p.flags.sector_index = in.get_ushort();
					}

					p.vertex_count = in.get();

					for (int j = 0; j < p.vertex_count; ++j) {
						p.indices[j] = {version == 265 /* G26fix */ ? in.get_uint() : in.get_ushort(), in.get_uint()};
					}
				}

				break;
			}
			case world_mesh_chunk::shared_lightmaps: {
				auto texture_count = in.get_uint();

				std::vector<std::shared_ptr<texture>> lightmap_textures {};
				lightmap_textures.resize(texture_count);

				for (std::uint32_t i = 0; i < texture_count; ++i) {
					lightmap_textures[i] = std::make_shared<texture>(texture::parse(in));
				}

				auto lightmap_count = in.get_uint();
				for (std::uint32_t i = 0; i < lightmap_count; ++i) {
					auto origin = in.get_vec3();
					auto normal_a = in.get_vec3();
					auto normal_b = in.get_vec3();
					std::uint32_t texture_index = in.get_uint();

					msh._m_lightmaps.emplace_back(
					    light_map {lightmap_textures[texture_index], {normal_a, normal_b}, origin});
				}

				break;
			}
			case world_mesh_chunk::lightmaps: {
				auto lightmap_count = in.get_uint();

				for (std::uint32_t i = 0; i < lightmap_count; ++i) {
					auto origin = in.get_vec3();
					auto normal_a = in.get_vec3();
					auto normal_b = in.get_vec3();
					auto lightmap_texture = texture::parse(in);

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

			if (in.position() != end) {
				fmt::print(stderr,
				           "warning: world mesh: not all data or too much data consumed from section 0x{:X}\n",
				           chunk);
			}

			in.position(end);
		} while (!finished);

		return msh;
	}
} // namespace phoenix
