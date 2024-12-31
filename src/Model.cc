// Copyright © 2022-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Model.hh"
#include "zenkit/Stream.hh"

namespace zenkit {
	void Model::load(Read* r) {
		this->hierarchy.load(r);
		this->mesh.load(r);
	}

	void Model::save(Write* w, GameVersion version) const {
		this->hierarchy.save(w);
		this->mesh.save(w, version);
	}
} // namespace zenkit
