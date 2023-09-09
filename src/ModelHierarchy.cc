// Copyright © 2021-2023 GothicKit Contributors.
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

	ModelHierarchy ModelHierarchy::parse(phoenix::buffer& in) {
		ModelHierarchy hierarchy {};

		auto r = Read::from(&in);
		hierarchy.load(r.get());

		return hierarchy;
	}

	zenkit::ModelHierarchy zenkit::ModelHierarchy::parse(phoenix::buffer&& in) {
		return ModelHierarchy::parse(in);
	}

	void ModelHierarchy::load(Read* r) {
		proto::read_chunked<ModelHierarchyChunkType>( //
		    r,
		    "ModelHierarchy",
		    [this](Read* c, ModelHierarchyChunkType type) {
			    switch (type) {
			    case ModelHierarchyChunkType::HIERARCHY: {
				    auto version = c->read_uint();
				    if (version != 0x03) {
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
} // namespace zenkit
