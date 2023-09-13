// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/MultiResolutionMesh.hh"
#include "zenkit/SoftSkinMesh.hh"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace phoenix {
	class buffer;
}

namespace zenkit {
	class Read;

	/// \brief Represents a *ZenGin* model mesh.
	///
	/// <p>Model meshes contain multiple phoenix::softskin_mesh instances as well as a set of phoenix::proto_mesh
	/// attachments. They can be found within `MDM` files and are embedded within phoenix::model objects.</p>
	class ModelMesh {
	public:
		/// \brief Parses a model mesh from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).</p>
		///
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed model mesh object.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws zenkit::ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static ModelMesh parse(phoenix::buffer& buf);

		/// \brief Parses a model mesh from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed model mesh object.
		/// \throws zenkit::ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static ModelMesh parse(phoenix::buffer&& buf);

		ZKAPI void load(Read* r);

	public:
		/// \brief A list of soft-skin meshes associated with this model mesh.
		std::vector<SoftSkinMesh> meshes {};

		/// \brief A map of attachment names to attachment meshes of this model mesh.
		std::unordered_map<std::string, MultiResolutionMesh> attachments {};

		/// \brief The checksum of the model hierarchy this model was made for.
		std::uint32_t checksum;
	};
} // namespace zenkit
