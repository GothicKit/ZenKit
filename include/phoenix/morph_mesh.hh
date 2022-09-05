// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/buffer.hh>
#include <phoenix/mesh.hh>
#include <phoenix/proto_mesh.hh>

namespace phoenix {
	/**
	 * @brief An animation used by morph meshes
	 */
	struct morph_animation {
		std::string name;
		std::int32_t layer;
		float blend_in;
		float blend_out;
		float duration;
		float speed;
		std::uint8_t flags;

		std::uint32_t frame_count;

		std::vector<std::uint32_t> vertices;
		std::vector<glm::vec3> samples;
	};

	struct morph_source {
		date file_date;
		std::string file_name;
	};

	/**
	 * @brief Represents a morph mesh.
	 *
	 * Parses ZenGin morph meshes. The reference implementation can be found on GitHub:
	 * https://github.com/Try/ZenLib/blob/9b94299307aebcd99887c5e0b050a13622e41ecb/zenload/zCMorphMesh.cpp and the
	 * original version by Andre Taulien was also referenced:
	 * https://github.com/ataulien/ZenLib/blob/e1a5e1b12e71690a5470f3be2aa3d0d6419f5191/zenload/zCMorphMesh.cpp
	 *
	 * Thanks to the original author, Andre Taulien as Try for additional work on their ZenLib fork!
	 *
	 * @see https://github.com/ataulien/ZenLib
	 * @see https://github.com/Try/ZenLib
	 */
	class morph_mesh {
	public:
		/**
		 * @brief Parses a morph mesh from the given reader.
		 * @param in The reader to read from.
		 * @return The mesh parsed.
		 */
		[[nodiscard]] static morph_mesh parse(buffer& in);

		/**
		 * @brief Parses a morph mesh from the given reader.
		 * @param in The reader to read from.
		 * @return The mesh parsed.
		 */
		[[nodiscard]] inline static morph_mesh parse(buffer&& in) {
			return parse(in);
		}

		/**
		 * @return All animations associated with the mesh
		 */
		inline const std::vector<morph_animation>& animations() const noexcept {
			return _m_animations;
		}

		/**
		 * @return All "morph positions" associated with the mesh
		 */
		inline const std::vector<glm::vec3>& positions() const noexcept {
			return _m_morph_positions;
		}

		/**
		 * @return The name of the mesh
		 */
		inline const std::string& name() const noexcept {
			return _m_name;
		}

		/**
		 * @return The underlying mesh data
		 */
		inline const proto_mesh& mesh() const noexcept {
			return _m_mesh;
		}

		/**
		 * @return The underlying mesh data
		 */
		inline const std::vector<morph_source>& sources() const noexcept {
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
