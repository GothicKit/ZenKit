// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/buffer.hh>
#include <phoenix/math/obb.hh>
#include <phoenix/proto_mesh.hh>

namespace phoenix {

	struct wedge_normal {
		glm::vec3 normal;
		std::uint32_t index;
	};

	struct weight_entry {
		float weight;
		glm::vec3 position;
		std::uint8_t node_index;
	};

	/**
	 * @brief Represents a rigged mesh.
	 *
	 * Parses ZenGin meshes. The reference implementation can be found on GitHub:
	 * https://github.com/Try/ZenLib/blob/9b94299307aebcd99887c5e0b050a13622e41ecb/zenload/zCMeshSoftSkin.cpp and the
	 * original version by Andre Taulien was also referenced:
	 * https://github.com/ataulien/ZenLib/blob/e1a5e1b12e71690a5470f3be2aa3d0d6419f5191/zenload/zCMeshSoftSkin.cpp
	 *
	 * Thanks to the original author, Andre Taulien as well as Try for additional work on their ZenLib fork!
	 *
	 * @see https://github.com/ataulien/ZenLib
	 * @see https://github.com/Try/ZenLib
	 */
	class softskin_mesh {
	public:
		[[nodiscard]] static softskin_mesh parse(buffer& in);

		[[nodiscard]] inline static softskin_mesh parse(buffer&& in) {
			return parse(in);
		}

		/**
		 * @return The embedded proto-mesh.
		 */
		[[nodiscard]] const proto_mesh& mesh() const noexcept {
			return _m_mesh;
		}

		/**
		 * @return The meshes bounding boxes (there is one for each node).
		 */
		[[nodiscard]] const std::vector<obb>& bboxes() const noexcept {
			return _m_bboxes;
		}

		/**
		 * @return Egde normals.
		 */
		[[nodiscard]] const std::vector<wedge_normal>& wedge_normals() const noexcept {
			return _m_wedge_normals;
		}

		/**
		 * @return Node weights.
		 */
		[[nodiscard]] const std::vector<std::vector<weight_entry>>& weights() const noexcept {
			return _m_weights;
		}

		/**
		 * @return Nodes.
		 */
		[[nodiscard]] const std::vector<std::int32_t>& nodes() const noexcept {
			return _m_nodes;
		}

	private:
		proto_mesh _m_mesh;
		std::vector<obb> _m_bboxes;
		std::vector<wedge_normal> _m_wedge_normals;
		std::vector<std::vector<weight_entry>> _m_weights;
		std::vector<std::int32_t> _m_nodes;
	};

} // namespace phoenix
