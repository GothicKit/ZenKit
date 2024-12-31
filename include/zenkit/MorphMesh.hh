// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Date.hh"
#include "zenkit/Library.hh"
#include "zenkit/MultiResolutionMesh.hh"

#include <cstdint>
#include <string>
#include <vector>

namespace zenkit {
	class Read;

	/// \brief An animation used by morph meshes
	struct MorphAnimation {
		/// \brief The name of the animation.
		std::string name;

		/// \brief The animation layer the animation is played at.
		std::int32_t layer;
		float blend_in;
		float blend_out;

		/// \brief The duration of the animation in seconds.
		float duration;
		float speed;
		std::uint8_t flags;

		/// \brief The number of frames of the animation.
		std::uint32_t frame_count;

		/// \brief The set of vertices affected by the animation.
		std::vector<std::uint32_t> vertices;

		/// \brief The set of samples of the animation.
		std::vector<Vec3> samples;
	};

	/// \brief A reference to a morph mesh source file.
	struct MorphSource {
		/// \brief The date of file creation.
		Date file_date;

		/// \brief The name of the source file.
		std::string file_name;
	};

	/// \brief Represents a *ZenGin* morph mesh.
	///
	/// <p>Morph meshes represents meshes which can deform using a set of animations. With these meshes, the positions
	/// of the vertices of the underlying phoenix::proto_mesh are actually changed while an animation plays.</p>
	class MorphMesh {
	public:
		ZKAPI void load(Read* r);

		/// \brief The name of the mesh.
		std::string name {};

		/// \brief The underlying mesh.
		MultiResolutionMesh mesh {};

		/// \brief All morph positions associated with the mesh.
		std::vector<Vec3> morph_positions {};

		/// \brief All animations associated with the mesh.
		std::vector<MorphAnimation> animations {};

		/// \brief A list of source files this morph mesh was compiled from.
		std::vector<MorphSource> sources {};
	};
} // namespace zenkit
