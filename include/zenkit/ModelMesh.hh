// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/MultiResolutionMesh.hh"
#include "zenkit/SoftSkinMesh.hh"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace zenkit {
	class Read;

	/// \brief Represents a *ZenGin* model mesh.
	///
	/// <p>Model meshes contain multiple phoenix::softskin_mesh instances as well as a set of phoenix::proto_mesh
	/// attachments. They can be found within `MDM` files and are embedded within phoenix::model objects.</p>
	class ModelMesh {
	public:
		ZKAPI void load(Read* r);
		ZKAPI void save(Write* w, GameVersion version) const;

		/// \brief A list of soft-skin meshes associated with this model mesh.
		std::vector<SoftSkinMesh> meshes {};

		/// \brief A map of attachment names to attachment meshes of this model mesh.
		std::unordered_map<std::string, MultiResolutionMesh> attachments {};

		/// \brief The checksum of the model hierarchy this model was made for.
		std::uint32_t checksum;
	};
} // namespace zenkit
