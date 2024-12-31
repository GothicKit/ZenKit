// Copyright © 2022-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/ModelHierarchy.hh"
#include "zenkit/ModelMesh.hh"

namespace zenkit {
	class Read;

	/// \brief Represents a *ZenGin* model.
	///
	/// <p>*ZenGin* models contain a zenkit::ModelMesh and a zenkit::ModelHierarchy bundled into one file. Try are
	/// typically found in files with the `MDL` extension.</p>
	class Model {
	public:
		ZKAPI void load(Read* r);
		ZKAPI void save(Write* w, GameVersion version) const;

		/// \brief The zenkit::ModelHierarchy associated with this model.
		ModelHierarchy hierarchy {};

		/// \brief The zenkit::ModelMesh associated with this model.
		ModelMesh mesh {};
	};
} // namespace zenkit
