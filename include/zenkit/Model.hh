// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/ModelHierarchy.hh"
#include "zenkit/ModelMesh.hh"

namespace phoenix {
	class buffer;
}

namespace zenkit {
	class Read;

	/// \brief Represents a *ZenGin* model.
	///
	/// <p>*ZenGin* models contain a zenkit::ModelMesh and a zenkit::ModelHierarchy bundled into one file. Try are
	/// typically found in files with the `MDL` extension.</p>
	class Model {
	public:
		/// \brief Parses a model from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed model object.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static Model parse(phoenix::buffer& buf);

		/// \brief Parses a model from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed model object.
		/// \throws zenkit::ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static Model parse(phoenix::buffer&& buf);

		ZKAPI void load(Read* r);
		ZKAPI void save(Write* w, GameVersion version) const;

		/// \brief The zenkit::ModelHierarchy associated with this model.
		ModelHierarchy hierarchy {};

		/// \brief The zenkit::ModelMesh associated with this model.
		ModelMesh mesh {};
	};
} // namespace zenkit
