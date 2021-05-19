// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/world/world_mesh.hh>

#include <fmt/format.h>

namespace phoenix {
	enum class world_mesh_chunk : u16 {
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


	world_mesh world_mesh::parse(reader& in) {
		world_mesh msh;

		u16 version {};
		bool finished = false;
		world_mesh_chunk chunk = world_mesh_chunk::unknown;
		u32 end = 0;

		do {
			chunk = static_cast<world_mesh_chunk>(in.read_u16());

			auto length = in.read_u32();
			end = length + in.tell();

			switch (chunk) {
				case world_mesh_chunk::mesh:
					version = in.read_u32();
					msh._m_date = {
							in.read_u32(),
							in.read_u16(),
							in.read_u16(),
							in.read_u16(),
							in.read_u16(),
							in.read_u16(),
					};

					msh._m_name = in.read_line(false);
					break;
				case world_mesh_chunk::bbox:
					msh._m_bbox[0] = in.read_vec4();
					msh._m_bbox[1] = in.read_vec4();
					break;
				case world_mesh_chunk::material: {
					auto matreader = archive_reader::open(in);

					u32 material_count = in.read_u32();
					msh._m_materials.reserve(material_count);

					for (u32 i = 0; i < material_count; ++i) {
						msh._m_materials.emplace_back(material::parse(matreader));
					}

					break;
				}
				case world_mesh_chunk::vertices:
					msh._m_vertices.resize(in.read_u32());

					for (auto& vertex : msh._m_vertices) {
						vertex = in.read_vec3();
					}
					break;
				case world_mesh_chunk::features:
					msh._m_features.resize(in.read_u32());

					for (auto& feature : msh._m_features) {
						feature.texture = in.read_vec2();
						feature.light = in.read_u32();
						feature.normal = in.read_vec3();
					}

					break;
				case world_mesh_chunk::polygons: {
					auto poly_count = in.read_u32();
					msh._m_polygons.resize(poly_count);

					for (u32 i = 0; i < poly_count; ++i) {
						polygon& p = msh._m_polygons[i];

						p.material_index = in.read_u16();
						p.lightmap_index = in.read_u16();
						p.polygon_plane = {in.read_f32(), in.read_vec3()};

						if (version == 265 /* G26fix */) {
							u8 flags = in.read_u8();
							p.flags.is_portal = (flags & 0b00000011) >> 0;
							p.flags.is_occluder = (flags & 0b00000100) >> 2;
							p.flags.is_sector = (flags & 0b00001000) >> 3;
							p.flags.should_relight = (flags & 0b00010000) >> 4;
							p.flags.is_outdoor = (flags & 0b00100000) >> 5;
							p.flags.is_ghost_occluder = (flags & 0b01000000) >> 6;
							p.flags.is_dynamically_lit = (flags & 0b10000000) >> 7;
							p.flags.sector_index = in.read_u16();
						} else {
							u8 flags1 = in.read_u8();
							u8 flags2 = in.read_u8();

							p.flags.is_portal = (flags1 & 0b00000011) >> 0;
							p.flags.is_occluder = (flags1 & 0b00000100) >> 2;
							p.flags.is_sector = (flags1 & 0b00001000) >> 3;
							p.flags.is_lod = (flags1 & 0b00010000) >> 4;
							p.flags.is_outdoor = (flags1 & 0b00100000) >> 5;
							p.flags.is_ghost_occluder = (flags1 & 0b01000000) >> 6;
							p.flags.normal_axis = ((flags1 & 0b10000000) >> 7) | (flags2 & 0b00000001);
							p.flags.sector_index = in.read_u16();
						}

						p.vertex_count = in.read_u8();

						for (int j = 0; j < p.vertex_count; ++j) {
							p.indices[j] = {
									version == 265 /* G26fix */ ? in.read_u32() : in.read_u16(),
									in.read_u32()};
						}
					}

					break;
				}
				case world_mesh_chunk::end:
					finished = true;
					break;
				default:
					fmt::print(stderr, "warning: world mesh: chunk not implemented: 0x{:X}\n", chunk);
					break;
			}

			if (in.tell() != end) {
				fmt::print(stderr, "warning: world mesh: not all data consumed from section 0x{:X}\n", chunk);
				in.seek(end);
			}
		} while (!finished);

		return msh;
	}
}// namespace phoenix