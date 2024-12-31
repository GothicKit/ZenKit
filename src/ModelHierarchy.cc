// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/ModelHierarchy.hh"
#include "zenkit/Stream.hh"

#include "Internal.hh"

namespace zenkit {
	enum class ModelHierarchyChunkType : std::uint16_t {
		HIERARCHY = 0xD100,
		SOURCE = 0xD110,
		END = 0xD120,
	};

	void ModelHierarchy::load(Read* r) {
		proto::read_chunked<ModelHierarchyChunkType>( //
		    r,
		    "ModelHierarchy",
		    [this](Read* c, ModelHierarchyChunkType type) {
			    switch (type) {
			    case ModelHierarchyChunkType::HIERARCHY: {
				    if (auto version = c->read_uint(); version != 0x03) {
					    ZKLOGW("ModelHierarchy", "Trying to parse ModelHierarchy with unsupported version %d", version);
				    }

				    this->nodes.resize(c->read_ushort());
				    for (auto& node : this->nodes) {
					    node.name = c->read_line(false);
					    node.parent_index = c->read_short();
					    node.transform = c->read_mat4();
				    }

				    this->bbox.load(c);
				    this->collision_bbox.load(c);
				    this->root_translation = c->read_vec3();
				    this->checksum = c->read_uint();
				    break;
			    }
			    case ModelHierarchyChunkType::SOURCE:
				    this->source_date.load(c);
				    this->source_path = c->read_line(false);
				    break;
			    case ModelHierarchyChunkType::END:
				    return true;
			    default:
				    break;
			    }

			    return false;
		    });
	}

	void ModelHierarchy::save(Write* w) const {
		proto::write_chunk(w, ModelHierarchyChunkType::HIERARCHY, [this](Write* c) {
			c->write_uint(0x03);
			c->write_ushort(static_cast<uint16_t>(this->nodes.size()));

			for (auto& node : this->nodes) {
				c->write_line(node.name);
				c->write_short(node.parent_index);
				c->write_mat4(node.transform);
			}

			this->bbox.save(c);
			this->collision_bbox.save(c);
			c->write_vec3(this->root_translation);
			c->write_uint(this->checksum);
		});

		proto::write_chunk(w, ModelHierarchyChunkType::SOURCE, [this](Write* c) {
			this->source_date.save(c);
			c->write_line(this->source_path);
		});

		proto::write_chunk(w, ModelHierarchyChunkType::END, [](Write*) {});
	}
} // namespace zenkit
