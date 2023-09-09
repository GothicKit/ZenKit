// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Model.hh"
#include "zenkit/Stream.hh"

namespace zenkit {
	Model Model::parse(phoenix::buffer& buf) {
		Model tmp {};

		auto r = Read::from(&buf);
		tmp.load(r.get());

		return tmp;
	}

	void Model::load(Read* r) {
		this->hierarchy.load(r);
		this->mesh.load(r);
	}

	Model Model::parse(phoenix::buffer&& buf) {
		return Model::parse(buf);
	}
} // namespace zenkit
