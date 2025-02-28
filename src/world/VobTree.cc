// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/world/VobTree.hh"
#include "zenkit/Archive.hh"
#include "zenkit/vobs/VirtualObject.hh"

namespace zenkit {
	std::shared_ptr<VirtualObject> parse_vob_tree(ReadArchive& in, GameVersion version) {
		auto obj = in.read_object(version);
		if (obj != nullptr && !is_vobject(obj->get_object_type())) {
			obj = nullptr;
		}

		// NOTE(lmichaelis): The NDK does not seem to support `reinterpret_pointer_cast`.
		std::shared_ptr<VirtualObject> object {obj, reinterpret_cast<VirtualObject*>(obj.get())};

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

	void save_vob_tree(WriteArchive& w, GameVersion version, std::shared_ptr<VirtualObject> const& obj) {
		w.write_object(obj, version);

		// TODO(lmichaelis): replace `0` below with the n of the nth child we're writing
		w.write_int("childs0", obj->children.size());

		for (auto& child : obj->children) {
			save_vob_tree(w, version, child);
		}
	}
} // namespace zenkit
