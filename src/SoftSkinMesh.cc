// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/SoftSkinMesh.hh"
#include "zenkit/Stream.hh"

#include "Internal.hh"

namespace zenkit {
	constexpr uint32_t VERSION_G1 = 0x00000004;
	constexpr uint32_t VERSION_G2 = 0x00000004;

	enum class SoftSkinMeshChunkType : std::uint16_t {
		HEADER = 0xE100,
		END = 0xE110,
		PROTO = 0xB100,
		NODES = 0xB1FF,
	};

	SoftSkinMesh SoftSkinMesh::parse(phoenix::buffer& in) {
		SoftSkinMesh msh {};

		auto r = Read::from(&in);
		msh.load(r.get());

		return msh;
	}

	void SoftSkinMesh::load(Read* r) {
		proto::read_chunked<SoftSkinMeshChunkType>(r, "SoftSkinMesh", [this](Read* c, SoftSkinMeshChunkType type) {
			switch (type) {
			case SoftSkinMeshChunkType::HEADER:
				(void) /* version = */ c->read_uint();
				break;
			case SoftSkinMeshChunkType::PROTO:
				mesh.load_from_section(c);
				break;
			case SoftSkinMeshChunkType::NODES: {
				// weights
				auto weight_buffer_size = c->read_uint();
				auto weight_buffer_end = c->tell() + weight_buffer_size;

				this->weights.resize(this->mesh.positions.size());
				for (uint32_t i = 0; i < this->mesh.positions.size(); ++i) {
					auto count = c->read_uint();
					this->weights[i].resize(count);

					for (std::uint32_t j = 0; j < count; ++j) {
						auto& weight = this->weights[i][j];
						weight.weight = c->read_float();
						weight.position = c->read_vec3();
						weight.node_index = c->read_ubyte();
					}
				}

				if (c->tell() != weight_buffer_end) {
					ZKLOGW("SoftSkinMesh", "%zu bytes remaining in weight section", weight_buffer_end - c->tell());
					c->seek(static_cast<ssize_t>(weight_buffer_end), Whence::BEG);
				}

				// wedge normals
				this->wedge_normals.resize(c->read_uint());
				for (auto& normal : this->wedge_normals) {
					normal.normal = c->read_vec3();
					normal.index = c->read_uint();
				}

				// nodes
				this->nodes.resize(c->read_ushort());
				for (auto& node : this->nodes) {
					node = c->read_int();
				}

				// bounding boxes
				this->bboxes.resize(this->nodes.size());
				for (auto& bbox : this->bboxes) {
					bbox.load(c);
				}

				break;
			}
			case SoftSkinMeshChunkType::END:
				return true;
			default:
				break;
			}

			return false;
		});
	}

	void SoftSkinMesh::save(Write* w, GameVersion version) const {
		proto::write_chunk(w, SoftSkinMeshChunkType::HEADER, [version](Write* wr) {
			wr->write_uint(version == GameVersion::GOTHIC_1 ? VERSION_G1 : VERSION_G2);
		});

		proto::write_chunk(w, SoftSkinMeshChunkType::PROTO, [&, version](Write* wr) {
			this->mesh.save_to_section(wr, version);
		});

		proto::write_chunk(w, SoftSkinMeshChunkType::HEADER, [&](Write* wr) {
			auto off_size = static_cast<ssize_t>(wr->tell());
			wr->write_uint(0);

			for (auto& we : this->weights) {
				wr->write_uint(we.size());

				for (auto& i : we) {
					wr->write_float(i.weight);
					wr->write_vec3(i.position);
					wr->write_ubyte(i.node_index);
				}
			}

			auto off_end = static_cast<ssize_t>(wr->tell());
			wr->seek(off_size, Whence::BEG);
			wr->write_uint(off_end - off_size);
			wr->seek(off_end, Whence::BEG);

			wr->write_uint(this->wedge_normals.size());
			for (auto& n : this->wedge_normals) {
				wr->write_vec3(n.normal);
				wr->write_uint(n.index);
			}

			wr->write_ushort(this->nodes.size());
			for (auto n : this->nodes) {
				wr->write_int(n);
			}

			for (auto& bbox : this->bboxes) {
				bbox.save(wr);
			}
		});

		proto::write_chunk(w, SoftSkinMeshChunkType::END, [](Write*) {});
	}

	SoftSkinMesh SoftSkinMesh::parse(phoenix::buffer&& in) {
		return parse(in);
	}
} // namespace zenkit
