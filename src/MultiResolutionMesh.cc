// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/MultiResolutionMesh.hh"
#include "zenkit/Archive.hh"
#include "zenkit/Stream.hh"

namespace zenkit {
	[[maybe_unused]] static constexpr auto version_g1 = 0x305;
	static constexpr auto version_g2 = 0x905;

	enum class MrmChunkType : std::uint16_t { MESH = 0xB100, END = 0xB1FF };

	MultiResolutionMesh MultiResolutionMesh::parse(phoenix::buffer& in) {
		MultiResolutionMesh msh {};

		auto r = Read::from(&in);
		msh.load(r.get());

		return msh;
	}

	MultiResolutionMesh MultiResolutionMesh::parse(phoenix::buffer&& in) {
		return MultiResolutionMesh::parse(in);
	}

	void MultiResolutionMesh::load(zenkit::Read* r) {
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

	void MultiResolutionMesh::load_from_section(zenkit::Read* r) {
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

		if (version == version_g2) {
			this->alpha_test = r->read_byte() != 0;
		}

		this->bbox.load(r);
		this->obbox.load(r);

		// TODO: this might be a vec4 though the values don't make any sense.
		r->seek(0x10, Whence::CUR);
		auto end = r->tell();

		// read positions
		this->positions.resize(vertices_size);
		r->seek((ssize_t) vertices_offset, Whence::BEG);
		for (auto& pos : this->positions) {
			pos = r->read_vec3();
		}

		// read normals
		this->normals.resize(normals_size);
		r->seek((ssize_t) normals_offset, Whence::BEG);
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

	void SubMesh::load(Read* r, SubMeshSection const& map) {
		// triangles
		r->seek((ssize_t) map.triangles.offset, Whence::BEG);
		this->triangles.resize(map.triangles.size);
		for (std::uint32_t i = 0; i < map.triangles.size; ++i) {
			this->triangles[i] = {{r->read_ushort(), r->read_ushort(), r->read_ushort()}};
		}

		// wedges
		r->seek((ssize_t) map.wedges.offset, Whence::BEG);
		this->wedges.resize(map.wedges.size);

		for (std::uint32_t i = 0; i < map.wedges.size; ++i) {
			this->wedges[i] = {r->read_vec3(), r->read_vec2(), r->read_ushort()};

			// and this is why you don't just dump raw binary data
			(void) r->read_ushort();
		}

		// colors
		r->seek((ssize_t) map.colors.offset, Whence::BEG);
		this->colors.resize(map.colors.size);

		for (std::uint32_t i = 0; i < map.colors.size; ++i) {
			this->colors[i] = r->read_float();
		}

		// triangle_plane_indices
		r->seek((ssize_t) map.triangle_plane_indices.offset, Whence::BEG);
		this->triangle_plane_indices.resize(map.triangle_plane_indices.size);

		for (std::uint32_t i = 0; i < map.triangle_plane_indices.size; ++i) {
			this->triangle_plane_indices[i] = r->read_ushort();
		}

		// triangle_planes
		r->seek((ssize_t) map.triangle_planes.offset, Whence::BEG);
		this->triangle_planes.resize(map.triangle_planes.size);

		for (std::uint32_t i = 0; i < map.triangle_planes.size; ++i) {
			this->triangle_planes[i] = {r->read_float(), r->read_vec3()};
		}

		// triangle_edges
		r->seek((ssize_t) map.triangle_edges.offset, Whence::BEG);
		this->triangle_edges.resize(map.triangle_edges.size);

		for (std::uint32_t i = 0; i < map.triangle_edges.size; ++i) {
			this->triangle_edges[i] = {{r->read_ushort(), r->read_ushort(), r->read_ushort()}};
		}

		// edges
		r->seek((ssize_t) map.edges.offset, Whence::BEG);
		this->edges.resize(map.edges.size);

		for (std::uint32_t i = 0; i < map.edges.size; ++i) {
			this->edges[i] = {{r->read_ushort(), r->read_ushort()}};
		}

		// edge_scores
		r->seek((ssize_t) map.edge_scores.offset, Whence::BEG);
		this->edge_scores.resize(map.edge_scores.size);

		for (std::uint32_t i = 0; i < map.edge_scores.size; ++i) {
			this->edge_scores[i] = r->read_float();
		}

		// wedge_map
		r->seek((ssize_t) map.wedge_map.offset, Whence::BEG);
		this->wedge_map.resize(map.wedge_map.size);

		for (std::uint32_t i = 0; i < map.wedge_map.size; ++i) {
			this->wedge_map[i] = r->read_ushort();
		}
	}
} // namespace zenkit
