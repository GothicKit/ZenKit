// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/buffer.hh>
#include <phoenix/mesh.hh>
#include <phoenix/proto_mesh.hh>

namespace phoenix {
	/// \brief An animation used by morph meshes
	struct morph_animation {
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
		std::vector<glm::vec3> samples;
	};

	/// \brief A reference to a morph mesh source file.
	struct morph_source {
		/// \brief The date of file creation.
		date file_date;

		/// \brief The name of the source file.
		std::string file_name;
	};

	/// \brief Represents a *ZenGin* morph mesh.
	///
	/// <p>Morph meshes represents meshes which can deform using a set of animations. With these meshes, the positions
	/// of the vertices of the underlying phoenix::proto_mesh are actually changed while an animation plays.</p>
	class morph_mesh {
	public:
		/// \brief Parses a morph mesh from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed morph mesh.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] static morph_mesh parse(buffer& buf);

		/// \brief Parses a morph mesh from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed morph mesh.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] inline static morph_mesh parse(buffer&& buf) {
			return morph_mesh::parse(buf);
		}

		/// \return All animations associated with the mesh.
		[[nodiscard]] inline const std::vector<morph_animation>& animations() const noexcept {
			return _m_animations;
		}

		/// \return All morph positions associated with the mesh.
		[[nodiscard]] inline const std::vector<glm::vec3>& positions() const noexcept {
			return _m_morph_positions;
		}

		/// \return The name of the mesh.
		[[nodiscard]] inline const std::string& name() const noexcept {
			return _m_name;
		}

		///\return The underlying mesh.
		[[nodiscard]] inline const proto_mesh& mesh() const noexcept {
			return _m_mesh;
		}

		/// \return A list of source files this morph mesh was compiled from.
		[[nodiscard]] inline const std::vector<morph_source>& sources() const noexcept {
			return _m_sources;
		}

	private:
		std::string _m_name;
		proto_mesh _m_mesh;

		std::vector<glm::vec3> _m_morph_positions;
		std::vector<morph_animation> _m_animations;

		std::vector<morph_source> _m_sources;
	};
} // namespace phoenix
