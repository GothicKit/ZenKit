// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/MultiResolutionMesh.hh"
#include "zenkit/Archive.hh"
#include "zenkit/Stream.hh"

namespace zenkit {
	[[maybe_unused]] static constexpr auto VERSION_G1 = 0x305;
	static constexpr auto VERSION_G2 = 0x905;

	enum class MrmChunkType : std::uint16_t { MESH = 0xB100, END = 0xB1FF };

	MultiResolutionMesh MultiResolutionMesh::parse(phoenix::buffer& in) {
		MultiResolutionMesh msh {};

		auto r = Read::from(&in);
		msh.load(r.get());

		return msh;
	}

	MultiResolutionMesh MultiResolutionMesh::parse(phoenix::buffer&& in) {
		return parse(in);
	}

	void MultiResolutionMesh::load(Read* r) {
		proto::read_chunked<MrmChunkType>(r, "MultiResolutionMesh", [this](Read* c, MrmChunkType type) {
			switch (type) {
			case MrmChunkType::MESH:
				this->load_from_section(c);
				break;
			case MrmChunkType::END:
				return true;
			default:
				break;
			}

			return false;
		});
	}

	void MultiResolutionMesh::load_from_section(Read* r) {
		auto version = r->read_ushort();
		auto content_size = r->read_uint();
		auto content_offset = r->tell();

		r->seek(content_size, Whence::CUR);

		auto submesh_count = r->read_ubyte();
		auto vertices_offset = r->read_uint() + content_offset;
		auto vertices_size = r->read_uint();
		auto normals_offset = r->read_uint() + content_offset;
		auto normals_size = r->read_uint();

		std::vector<SubMeshSection> submesh_sections;
		submesh_sections.resize(submesh_count);

		for (auto i = 0u; i < submesh_count; ++i) {
			submesh_sections[i] = {
			    {r->read_uint() + content_offset, r->read_uint()},
			    {r->read_uint() + content_offset, r->read_uint()},
			    {r->read_uint() + content_offset, r->read_uint()},
			    {r->read_uint() + content_offset, r->read_uint()},
			    {r->read_uint() + content_offset, r->read_uint()},
			    {r->read_uint() + content_offset, r->read_uint()},
			    {r->read_uint() + content_offset, r->read_uint()},
			    {r->read_uint() + content_offset, r->read_uint()},
			    {r->read_uint() + content_offset, r->read_uint()},
			    {r->read_uint() + content_offset, r->read_uint()},
			};
		}

		// read all materials
		auto mats = ReadArchive::from(r);
		this->materials.resize(submesh_count);
		for (auto& material : this->materials) {
			material.load(*mats);
		}

		if (version == VERSION_G2) {
			this->alpha_test = r->read_byte() != 0;
		}

		this->bbox.load(r);
		this->obbox.load(r);

		// TODO: this might be a vec4 though the values don't make any sense.
		r->seek(0x10, Whence::CUR);
		auto end = r->tell();

		// read positions
		this->positions.resize(vertices_size);
		r->seek(static_cast<ssize_t>(vertices_offset), Whence::BEG);
		for (auto& pos : this->positions) {
			pos = r->read_vec3();
		}

		// read normals
		this->normals.resize(normals_size);
		r->seek(static_cast<ssize_t>(normals_offset), Whence::BEG);
		for (auto& normal : this->normals) {
			normal = r->read_vec3();
		}

		// read submeshes
		this->sub_meshes.resize(submesh_count);
		for (auto i = 0u; i < submesh_count; ++i) {
			this->sub_meshes[i].load(r, submesh_sections[i]);
			this->sub_meshes[i].mat = this->materials[i];
		}

		r->seek(static_cast<uint32_t>(end), Whence::BEG);
	}

	void MultiResolutionMesh::save(Write* w, GameVersion version) const {
		proto::write_chunk(w, MrmChunkType::MESH, [&](Write* wr) { this->save_to_section(wr, version); });
		proto::write_chunk(w, MrmChunkType::END, [&](Write*) {});
	}

	void MultiResolutionMesh::save_to_section(Write* w, GameVersion version) const {
		w->write_ushort(version == GameVersion::GOTHIC_1 ? VERSION_G1 : VERSION_G2);

		auto off_size = static_cast<ssize_t>(w->tell());
		w->write_uint(0);

		auto off_content = w->tell();
		auto off_positions = w->tell();
		for (auto& v : this->positions) {
			w->write_vec3(v);
		}

		auto off_normals = w->tell();
		for (auto& v : this->normals) {
			w->write_vec3(v);
		}

		std::vector<SubMeshSection> sections;
		for (auto& mesh : this->sub_meshes) {
			sections.push_back(mesh.save(w));
		}

		auto off_end = static_cast<ssize_t>(w->tell());
		w->seek(off_size, Whence::BEG);
		w->write_uint(off_end - off_size);
		w->seek(off_end, Whence::BEG);

		w->write_ubyte(this->sub_meshes.size());    // submeshCount
		w->write_uint(off_positions - off_content); // positionOffset
		w->write_uint(this->positions.size());      // positionCount
		w->write_uint(off_normals - off_content);   // normalsOffset
		w->write_uint(this->normals.size());        // normalsCount

		for (auto& s : sections) {
			w->write_uint(s.triangles.offset - off_content);
			w->write_uint(s.triangles.size);

			w->write_uint(s.wedges.offset - off_content);
			w->write_uint(s.wedges.size);

			w->write_uint(s.colors.offset - off_content);
			w->write_uint(s.colors.size);

			w->write_uint(s.triangle_plane_indices.offset - off_content);
			w->write_uint(s.triangle_plane_indices.size);

			w->write_uint(s.triangle_planes.offset - off_content);
			w->write_uint(s.triangle_planes.size);

			w->write_uint(s.wedge_map.offset - off_content);
			w->write_uint(s.wedge_map.size);

			w->write_uint(s.vertex_updates.offset - off_content);
			w->write_uint(s.vertex_updates.size);

			w->write_uint(s.triangle_edges.offset - off_content);
			w->write_uint(s.triangle_edges.size);

			w->write_uint(s.edges.offset - off_content);
			w->write_uint(s.edges.size);

			w->write_uint(s.edge_scores.offset - off_content);
			w->write_uint(s.edge_scores.size);
		}

		auto ar = WriteArchive::to(w, ArchiveFormat::BINARY);
		for (auto& mesh : this->sub_meshes) {
			ar->write_string("", mesh.mat.name);
			ar->write_object("%", &mesh.mat, version);
		}
		ar->write_header();

		if (version == GameVersion::GOTHIC_2) {
			w->write_byte(this->alpha_test ? 1 : 0);
		}

		this->bbox.save(w);
		this->obbox.save(w);

		char padding[10] {0};
		w->write(padding, 10);
	}

	void SubMesh::load(Read* r, SubMeshSection const& map) {
		// triangles
		r->seek(static_cast<ssize_t>(map.triangles.offset), Whence::BEG);
		this->triangles.resize(map.triangles.size);
		for (auto i = 0u; i < map.triangles.size; ++i) {
			this->triangles[i] = {{r->read_ushort(), r->read_ushort(), r->read_ushort()}};
		}

		// wedges
		r->seek(static_cast<ssize_t>(map.wedges.offset), Whence::BEG);
		this->wedges.resize(map.wedges.size);

		for (auto i = 0u; i < map.wedges.size; ++i) {
			this->wedges[i] = {r->read_vec3(), r->read_vec2(), r->read_ushort()};

			// and this is why you don't just dump raw binary data
			(void) r->read_ushort();
		}

		// colors
		r->seek(static_cast<ssize_t>(map.colors.offset), Whence::BEG);
		this->colors.resize(map.colors.size);

		for (auto i = 0u; i < map.colors.size; ++i) {
			this->colors[i] = r->read_float();
		}

		// triangle_plane_indices
		r->seek(static_cast<ssize_t>(map.triangle_plane_indices.offset), Whence::BEG);
		this->triangle_plane_indices.resize(map.triangle_plane_indices.size);

		for (auto i = 0u; i < map.triangle_plane_indices.size; ++i) {
			this->triangle_plane_indices[i] = r->read_ushort();
		}

		// triangle_planes
		r->seek(static_cast<ssize_t>(map.triangle_planes.offset), Whence::BEG);
		this->triangle_planes.resize(map.triangle_planes.size);

		for (auto i = 0u; i < map.triangle_planes.size; ++i) {
			this->triangle_planes[i] = {r->read_float(), r->read_vec3()};
		}

		// triangle_edges
		r->seek(static_cast<ssize_t>(map.triangle_edges.offset), Whence::BEG);
		this->triangle_edges.resize(map.triangle_edges.size);

		for (auto i = 0u; i < map.triangle_edges.size; ++i) {
			this->triangle_edges[i] = {{r->read_ushort(), r->read_ushort(), r->read_ushort()}};
		}

		// edges
		r->seek(static_cast<ssize_t>(map.edges.offset), Whence::BEG);
		this->edges.resize(map.edges.size);

		for (auto i = 0u; i < map.edges.size; ++i) {
			this->edges[i] = {{r->read_ushort(), r->read_ushort()}};
		}

		// edge_scores
		r->seek(static_cast<ssize_t>(map.edge_scores.offset), Whence::BEG);
		this->edge_scores.resize(map.edge_scores.size);

		for (auto i = 0u; i < map.edge_scores.size; ++i) {
			this->edge_scores[i] = r->read_float();
		}

		// wedge_map
		r->seek(static_cast<ssize_t>(map.wedge_map.offset), Whence::BEG);
		this->wedge_map.resize(map.wedge_map.size);

		for (auto i = 0u; i < map.wedge_map.size; ++i) {
			this->wedge_map[i] = r->read_ushort();
		}
	}

	SubMeshSection SubMesh::save(Write* w) const {
		SubMeshSection section {};

		// triangles
		section.triangles.offset = w->tell();
		for (auto& tri : this->triangles) {
			w->write_ushort(tri.wedges[0]);
			w->write_ushort(tri.wedges[1]);
			w->write_ushort(tri.wedges[2]);
		}
		section.triangles.size = w->tell() - section.triangles.offset;

		// wedges
		section.wedges.offset = w->tell();
		for (auto& wedge : wedges) {
			w->write_vec3(wedge.normal);
			w->write_vec2(wedge.texture);
			w->write_ushort(wedge.index);
			w->write_ushort(0); // padding
		}
		section.wedges.size = w->tell() - section.wedges.offset;

		// colors
		section.colors.offset = w->tell();
		for (auto color : this->colors) {
			w->write_float(color);
		}
		section.colors.size = w->tell() - section.colors.offset;

		// triangle_plane_indices
		section.triangle_plane_indices.offset = w->tell();
		for (auto i : this->triangle_plane_indices) {
			w->write_ushort(i);
		}
		section.triangle_plane_indices.size = w->tell() - section.triangle_plane_indices.offset;

		// triangle_planes
		section.triangle_planes.offset = w->tell();
		for (auto& plane : this->triangle_planes) {
			w->write_float(plane.distance);
			w->write_vec3(plane.normal);
		}
		section.triangle_planes.size = w->tell() - section.triangle_planes.offset;

		// triangle_edges
		section.triangle_edges.offset = w->tell();
		for (auto& edge : this->triangle_edges) {
			w->write_ushort(edge.edges[0]);
			w->write_ushort(edge.edges[1]);
			w->write_ushort(edge.edges[2]);
		}
		section.triangle_edges.size = w->tell() - section.triangle_edges.offset;

		// edges
		section.edges.offset = w->tell();
		for (auto edge : edges) {
			w->write_ushort(edge.edges[0]);
			w->write_ushort(edge.edges[1]);
		}
		section.edges.size = w->tell() - section.edges.offset;

		// edge_scores
		section.edge_scores.offset = w->tell();
		for (auto score : edge_scores) {
			w->write_float(score);
		}
		section.edge_scores.size = w->tell() - section.edge_scores.offset;

		// wedge_map
		section.wedge_map.offset = w->tell();
		for (auto wedge : this->wedge_map) {
			w->write_ushort(wedge);
		}
		section.wedge_map.size = w->tell() - section.wedge_map.offset;

		return section;
	}
} // namespace zenkit
