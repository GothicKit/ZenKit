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

	/// \brief Represents a soft-skin mesh.
	class softskin_mesh {
	public:
		/// \brief Parses a soft-skin mesh from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed soft-skin mesh.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] static softskin_mesh parse(buffer& in);

		/// \brief Parses a soft-skin mesh from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed soft-skin mesh.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] inline static softskin_mesh parse(buffer&& in) {
			return softskin_mesh::parse(in);
		}

		/// \return The embedded proto-mesh.
		[[nodiscard]] const proto_mesh& mesh() const noexcept {
			return _m_mesh;
		}

		/// \return The meshes bounding boxes (there is one for each node).
		[[nodiscard]] const std::vector<obb>& bboxes() const noexcept {
			return _m_bboxes;
		}

		/// \return A list of wedge normals.
		[[nodiscard]] const std::vector<wedge_normal>& wedge_normals() const noexcept {
			return _m_wedge_normals;
		}

		/// \return Node weights.
		[[nodiscard]] const std::vector<std::vector<weight_entry>>& weights() const noexcept {
			return _m_weights;
		}

		/// \return Nodes.
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
