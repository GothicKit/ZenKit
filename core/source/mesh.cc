// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/mesh.hh>

#include <fmt/format.h>

namespace phoenix {
	enum class proto_chunk {
		unknown,
		mesh = 0xB100,
		end = 0xB1FF
	};

	proto_mesh proto_mesh::read(reader& in) {
		proto_mesh msh {};

		proto_chunk chunk = proto_chunk::unknown;
		u32 end = 0;

		bool end_mesh = false;

		do {
			chunk = static_cast<proto_chunk>(in.read_u16());

			auto length = in.read_u32();
			end = length + in.tell();

			switch (chunk) {
				case proto_chunk::mesh: {
					auto version = in.read_u16();
					auto content = in.fork(in.read_u32());

					auto submesh_count = in.read_u8();
					auto vertices_index = in.read_u32();
					auto vertices_size = in.read_u32();
					auto normals_index = in.read_u32();
					auto normals_size = in.read_u32();

					std::vector<sub_mesh_section> submesh_sections;
					submesh_sections.resize(submesh_count);

					for (int i = 0; i < submesh_count; ++i) {
						submesh_sections[i] = {
								{in.read_u32(), in.read_u32()},
								{in.read_u32(), in.read_u32()},
								{in.read_u32(), in.read_u32()},
								{in.read_u32(), in.read_u32()},
								{in.read_u32(), in.read_u32()},
								{in.read_u32(), in.read_u32()},
								{in.read_u32(), in.read_u32()},
								{in.read_u32(), in.read_u32()},
								{in.read_u32(), in.read_u32()},
								{in.read_u32(), in.read_u32()},
						};
					}

					// read all materials
					auto mats = archive_reader::open(in);
					for (int i = 0; i < submesh_count; ++i) {
						msh._m_materials.emplace_back(material::parse(mats));
					}

					if (version == 0x0905 /*G2*/) {
						msh._m_has_alpha_test = in.read_u8() != 0;
					}

					msh._m_bbox[0] = in.read_vec3();
					msh._m_bbox[1] = in.read_vec3();

					// read positions and normals
					msh._m_vertices.resize(vertices_size);
					auto vertices = content.fork(vertices_size * sizeof(float) * 3, vertices_index);

					for (u32 i = 0; i < vertices_size; ++i) {
						msh._m_vertices[i] = vertices.read_vec3();
					}

					msh._m_normals.resize(normals_size);
					auto normals = content.fork(normals_size * sizeof(float) * 3, normals_index);

					for (u32 i = 0; i < normals_size; ++i) {
						msh._m_normals[i] = normals.read_vec3();
					}

					// read submeshes
					msh._m_sub_meshes.resize(submesh_count);

					for (int i = 0; i < submesh_count; ++i) {
						msh._m_sub_meshes[i] = sub_mesh::read(content, submesh_sections[i]);
						msh._m_sub_meshes[i].material = msh._m_materials[i];
					}

					break;
				}
				case proto_chunk::end:
					end_mesh = true;
					break;
				default:
					fmt::print(stderr, "warning: proto mesh: chunk not implemented: 0x{:X}\n", chunk);
					break;
			}

			if (in.tell() != end) {
				fmt::print(stderr, "warning: proto mesh: not all data consumed from section 0x{:X}\n", chunk);
				in.seek(end);
			}
		} while (in.tell() < in.size() - 4 || !end_mesh);

		return msh;
	}

	sub_mesh sub_mesh::read(reader& in, const sub_mesh_section& map) {
		sub_mesh subm {};

		// triangles
		in.seek(map.triangles.offset);
		subm.triangles.resize(map.triangles.size);

		for (u32 i = 0; i < map.triangles.size; ++i) {
			subm.triangles[i] = {{in.read_u16(), in.read_u16(), in.read_u16()}};
		}

		// wedges
		in.seek(map.wedges.offset);
		subm.wedges.resize(map.wedges.size);

		for (u32 i = 0; i < map.wedges.size; ++i) {
			subm.wedges[i] = {in.read_vec3(), in.read_vec2(), in.read_u16()};

			// and this is why you don't just dump raw binary data
			(void) in.read_u16();
		}

		// colors
		in.seek(map.colors.offset);
		subm.colors.resize(map.colors.size);

		for (u32 i = 0; i < map.colors.size; ++i) {
			subm.colors[i] = in.read_f32();
		}

		// triangle_plane_indices
		in.seek(map.triangle_plane_indices.offset);
		subm.triangle_plane_indices.resize(map.triangle_plane_indices.size);

		for (u32 i = 0; i < map.triangle_plane_indices.size; ++i) {
			subm.triangle_plane_indices[i] = in.read_u16();
		}

		// triangle_planes
		in.seek(map.triangle_planes.offset);
		subm.triangle_planes.resize(map.triangle_planes.size);

		for (u32 i = 0; i < map.triangle_planes.size; ++i) {
			subm.triangle_planes[i] = {in.read_f32(), in.read_vec3()};
		}

		// triangle_edges
		in.seek(map.triangle_edges.offset);
		subm.triangle_edges.resize(map.triangle_edges.size);

		for (u32 i = 0; i < map.triangle_edges.size; ++i) {
			subm.triangle_edges[i] = {{in.read_u16(), in.read_u16(), in.read_u16()}};
		}

		// edges
		in.seek(map.edges.offset);
		subm.edges.resize(map.edges.size);

		for (u32 i = 0; i < map.edges.size; ++i) {
			subm.edges[i] = {{in.read_u16(), in.read_u16()}};
		}

		// edge_scores
		in.seek(map.edge_scores.offset);
		subm.edge_scores.resize(map.edge_scores.size);

		for (u32 i = 0; i < map.edge_scores.size; ++i) {
			subm.edge_scores[i] = in.read_f32();
		}

		// wedge_map
		in.seek(map.wedge_map.offset);
		subm.wedge_map.resize(map.wedge_map.size);

		for (u32 i = 0; i < map.wedge_map.size; ++i) {
			subm.wedge_map[i] = in.read_u16();
		}

		return subm;
	}
}// namespace phoenix