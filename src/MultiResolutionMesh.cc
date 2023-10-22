// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/proto_mesh.hh>

namespace phoenix {
	[[maybe_unused]] static constexpr auto version_g1 = 0x305;
	static constexpr auto version_g2 = 0x905;

	enum class proto_chunk { unknown, mesh = 0xB100, end = 0xB1FF };

	proto_mesh proto_mesh::parse(buffer& in) {
		proto_mesh msh {};
		proto_chunk type = proto_chunk::unknown;
		bool end_mesh = false;

		do {
			type = static_cast<proto_chunk>(in.get_ushort());

			auto length = in.get_uint();
			auto chunk = in.extract(length);

			switch (type) {
			case proto_chunk::mesh:
				msh = parse_from_section(chunk);
				break;
			case proto_chunk::end:
				end_mesh = true;
				break;
			default:
				break;
			}

			if (chunk.remaining() != 0) {
				PX_LOGW("proto_mesh: ",
				        chunk.remaining(),
				        " bytes remaining in section ",
				        std::hex,
				        std::uint16_t(type));
			}
		} while (!end_mesh);

		return msh;
	}

	proto_mesh proto_mesh::parse_from_section(buffer& chunk) {
		proto_mesh msh {};

		auto version = chunk.get_ushort();
		auto content_size = chunk.get_uint();
		auto content = chunk.extract(content_size);

		auto submesh_count = chunk.get();
		auto vertices_index = chunk.get_uint();
		auto vertices_size = chunk.get_uint();
		auto normals_index = chunk.get_uint();
		auto normals_size = chunk.get_uint();

		std::vector<sub_mesh_section> submesh_sections;
		submesh_sections.resize(submesh_count);

		for (int32_t i = 0; i < submesh_count; ++i) {
			submesh_sections[i] = {
			    {chunk.get_uint(), chunk.get_uint()},
			    {chunk.get_uint(), chunk.get_uint()},
			    {chunk.get_uint(), chunk.get_uint()},
			    {chunk.get_uint(), chunk.get_uint()},
			    {chunk.get_uint(), chunk.get_uint()},
			    {chunk.get_uint(), chunk.get_uint()},
			    {chunk.get_uint(), chunk.get_uint()},
			    {chunk.get_uint(), chunk.get_uint()},
			    {chunk.get_uint(), chunk.get_uint()},
			    {chunk.get_uint(), chunk.get_uint()},
			};
		}

		// read all materials
		auto mats = archive_reader::open(chunk);
		for (int32_t i = 0; i < submesh_count; ++i) {
			msh.materials.emplace_back(material::parse(*mats));
		}

		if (version == version_g2) {
			msh.alpha_test = chunk.get() != 0;
		}

		msh.bbox = bounding_box::parse(chunk);

		// read positions and normals
		msh.positions.resize(vertices_size);
		auto vertices = content.slice(vertices_index, vertices_size * sizeof(float) * 3);

		for (std::uint32_t i = 0; i < vertices_size; ++i) {
			msh.positions[i] = vertices.get_vec3();
		}

		msh.normals.resize(normals_size);
		auto normals = content.slice(normals_index, normals_size * sizeof(float) * 3);

		for (std::uint32_t i = 0; i < normals_size; ++i) {
			msh.normals[i] = normals.get_vec3();
		}

		// read submeshes
		msh.sub_meshes.reserve(submesh_count);

		for (int32_t i = 0; i < submesh_count; ++i) {
			auto& mesh = msh.sub_meshes.emplace_back(sub_mesh::parse(content, submesh_sections[i]));
			mesh.mat = msh.materials[i];
		}

		msh.obbox = obb::parse(chunk);

		// TODO: this might be a vec4 though the values don't make any sense.
		chunk.skip(0x10);
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
