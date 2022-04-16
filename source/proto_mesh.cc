// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/proto_mesh.hh>

#include <fmt/format.h>

namespace phoenix {
	enum class proto_chunk { unknown, mesh = 0xB100, end = 0xB1FF };

	proto_mesh proto_mesh::parse(buffer& in) {
		proto_mesh msh {};

		proto_chunk chunk = proto_chunk::unknown;
		std::uint32_t end = 0;

		bool end_mesh = false;

		do {
			chunk = static_cast<proto_chunk>(in.get_ushort());

			auto length = in.get_uint();
			end = length + in.position();

			switch (chunk) {
			case proto_chunk::mesh: {
				auto version = in.get_ushort();
				auto content_size = in.get_uint();
				auto content = in.slice(in.position(), content_size);
				in.position(in.position() + content_size);

				auto submesh_count = in.get();
				auto vertices_index = in.get_uint();
				auto vertices_size = in.get_uint();
				auto normals_index = in.get_uint();
				auto normals_size = in.get_uint();

				std::vector<sub_mesh_section> submesh_sections;
				submesh_sections.resize(submesh_count);

				for (int i = 0; i < submesh_count; ++i) {
					submesh_sections[i] = {
					    {in.get_uint(), in.get_uint()},
					    {in.get_uint(), in.get_uint()},
					    {in.get_uint(), in.get_uint()},
					    {in.get_uint(), in.get_uint()},
					    {in.get_uint(), in.get_uint()},
					    {in.get_uint(), in.get_uint()},
					    {in.get_uint(), in.get_uint()},
					    {in.get_uint(), in.get_uint()},
					    {in.get_uint(), in.get_uint()},
					    {in.get_uint(), in.get_uint()},
					};
				}

				// read all materials
				auto mats = archive_reader::open(in);
				for (int i = 0; i < submesh_count; ++i) {
					msh._m_materials.emplace_back(material::parse(mats));
				}

				if (version == 0x0905 /*G2*/) {
					msh._m_has_alpha_test = in.get() != 0;
				}

				msh._m_bbox[0] = in.get_vec3();
				msh._m_bbox[1] = in.get_vec3();

				// read positions and normals
				msh._m_vertices.resize(vertices_size);
				auto vertices = content.slice(vertices_index, vertices_size * sizeof(float) * 3);

				for (std::uint32_t i = 0; i < vertices_size; ++i) {
					msh._m_vertices[i] = vertices.get_vec3();
				}

				msh._m_normals.resize(normals_size);
				auto normals = content.slice(normals_index, normals_size * sizeof(float) * 3);

				for (std::uint32_t i = 0; i < normals_size; ++i) {
					msh._m_normals[i] = normals.get_vec3();
				}

				// read submeshes
				msh._m_sub_meshes.reserve(submesh_count);

				for (int i = 0; i < submesh_count; ++i) {
					auto& mesh = msh._m_sub_meshes.emplace_back(sub_mesh::parse(content, submesh_sections[i]));
					mesh.mat = msh._m_materials[i];
				}

				msh._m_obbox = obb::parse(in);

				// TODO: this might be a vec4 though the values don't make any sense.
				in.position(in.position() + 0x10);
				break;
			}
			case proto_chunk::end:
				end_mesh = true;
				break;
			default:
				break;
			}

			// quirk: we don't skip to the end if this is the last section
			if (!end_mesh) {
				if (in.position() != end) {
					fmt::print(stderr,
					           "warning: proto mesh: not all data or too much data consumed from section 0x{:X}\n",
					           chunk);
				}

				in.position(end);
			}
		} while (!end_mesh);

		return msh;
	}

	sub_mesh sub_mesh::parse(buffer& in, const sub_mesh_section& map) {
		sub_mesh subm {};

		// triangles
		in.position(map.triangles.offset);
		subm.triangles.resize(map.triangles.size);

		for (std::uint32_t i = 0; i < map.triangles.size; ++i) {
			subm.triangles[i] = {{in.get_ushort(), in.get_ushort(), in.get_ushort()}};
		}

		// wedges
		in.position(map.wedges.offset);
		subm.wedges.resize(map.wedges.size);

		for (std::uint32_t i = 0; i < map.wedges.size; ++i) {
			subm.wedges[i] = {in.get_vec3(), in.get_vec2(), in.get_ushort()};

			// and this is why you don't just dump raw binary data
			(void) in.get_ushort();
		}

		// colors
		in.position(map.colors.offset);
		subm.colors.resize(map.colors.size);

		for (std::uint32_t i = 0; i < map.colors.size; ++i) {
			subm.colors[i] = in.get_float();
		}

		// triangle_plane_indices
		in.position(map.triangle_plane_indices.offset);
		subm.triangle_plane_indices.resize(map.triangle_plane_indices.size);

		for (std::uint32_t i = 0; i < map.triangle_plane_indices.size; ++i) {
			subm.triangle_plane_indices[i] = in.get_ushort();
		}

		// triangle_planes
		in.position(map.triangle_planes.offset);
		subm.triangle_planes.resize(map.triangle_planes.size);

		for (std::uint32_t i = 0; i < map.triangle_planes.size; ++i) {
			subm.triangle_planes[i] = {in.get_float(), in.get_vec3()};
		}

		// triangle_edges
		in.position(map.triangle_edges.offset);
		subm.triangle_edges.resize(map.triangle_edges.size);

		for (std::uint32_t i = 0; i < map.triangle_edges.size; ++i) {
			subm.triangle_edges[i] = {{in.get_ushort(), in.get_ushort(), in.get_ushort()}};
		}

		// edges
		in.position(map.edges.offset);
		subm.edges.resize(map.edges.size);

		for (std::uint32_t i = 0; i < map.edges.size; ++i) {
			subm.edges[i] = {{in.get_ushort(), in.get_ushort()}};
		}

		// edge_scores
		in.position(map.edge_scores.offset);
		subm.edge_scores.resize(map.edge_scores.size);

		for (std::uint32_t i = 0; i < map.edge_scores.size; ++i) {
			subm.edge_scores[i] = in.get_float();
		}

		// wedge_map
		in.position(map.wedge_map.offset);
		subm.wedge_map.resize(map.wedge_map.size);

		for (std::uint32_t i = 0; i < map.wedge_map.size; ++i) {
			subm.wedge_map[i] = in.get_ushort();
		}

		return subm;
	}
} // namespace phoenix
