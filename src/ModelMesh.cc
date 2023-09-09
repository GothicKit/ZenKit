// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/ModelMesh.hh"
#include "zenkit/Date.hh"
#include "zenkit/Stream.hh"

namespace zenkit {
	enum class ModelMeshChunkType : std::uint16_t {
		HEADER = 0xD000,
		SOURCE = 0xD010,
		NODES = 0xD020,
		SOFTSKINS = 0xD030,
		END = 0xD040,
		PROTO = 0xB100,
	};

	ModelMesh ModelMesh::parse(phoenix::buffer& in) {
		ModelMesh msh {};

		auto r = Read::from(&in);
		msh.load(r.get());

		return msh;
	}

	void ModelMesh::load(Read* r) {
		std::vector<std::string> attachment_names {};
		proto::read_chunked<ModelMeshChunkType>(
		    r,
		    "ModelMesh",
		    [this, &attachment_names](Read* c, ModelMeshChunkType type, size_t& end) {
			    switch (type) {
			    case ModelMeshChunkType::HEADER:
				    (void) /* version = */ c->read_uint();
				    break;
			    case ModelMeshChunkType::SOURCE: {
				    // supposedly a date? weird values
				    Date date {};
				    date.load(c);

				    (void) /* source file = */ c->read_line(false);
				    break;
			    }
			    case ModelMeshChunkType::NODES: {
				    auto node_count = c->read_ushort();

				    for (std::uint32_t i = 0; i < node_count; ++i) {
					    attachment_names.push_back(c->read_line(true));
				    }
				    break;
			    }
			    case ModelMeshChunkType::PROTO:
				    this->attachments[attachment_names[this->attachments.size()]].load_from_section(c);
				    break;
			    case ModelMeshChunkType::SOFTSKINS: {
				    this->checksum = c->read_uint();

				    // Quirk: the meshes are not embedded within the chunk (as in: the stored length does not contain
				    //        the size of these meshes) so they have to be read directly from `in`.
				    this->meshes.resize(c->read_ushort());
				    for (auto& mesh : this->meshes) {
					    mesh.load(c);
				    }

				    end = c->tell();
				    break;
			    }
			    case ModelMeshChunkType::END:
				    return true;
			    default:
				    break;
			    }

			    return false;
		    });
	}

	ModelMesh ModelMesh::parse(phoenix::buffer&& buf) {
		return ModelMesh::parse(buf);
	}
} // namespace zenkit
