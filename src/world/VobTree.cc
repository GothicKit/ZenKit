// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/world/VobTree.hh"
#include "zenkit/Archive.hh"
#include "zenkit/vobs/VirtualObject.hh"

namespace zenkit {
	std::shared_ptr<VirtualObject> parse_vob_tree(ReadArchive& in, GameVersion version) {
		// TODO(lmichaelis): Make this dynamic cast faster!
		auto object = std::dynamic_pointer_cast<VirtualObject>(in.read_object(version));

		auto child_count = static_cast<size_t>(in.read_int());
		if (object == nullptr) {
			std::function<void(size_t)> skip;
			skip = [&skip, &in](size_t count) {
				for (auto i = 0u; i < count; ++i) {
					in.skip_object(false);

					auto num_children = static_cast<size_t>(in.read_int());
					skip(num_children);
				}
			};

			skip(child_count);
			return nullptr;
		}

		object->children.reserve(child_count);

		for (auto i = 0u; i < child_count; ++i) {
			auto child = parse_vob_tree(in, version);
			if (child == nullptr) continue;

			object->children.push_back(std::move(child));
		}

		return object;
	}
} // namespace zenkit
