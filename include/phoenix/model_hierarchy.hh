// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/buffer.hh>
#include <phoenix/math/bbox.hh>

#include <glm/mat4x4.hpp>

#include <tuple>
#include <vector>

namespace phoenix {
	/// \brief A node in the hierarchy tree.
	struct model_hierarchy_node {
		/// \brief The index of this node's parent node.
		std::int16_t parent_index;

		/// \brief The name of this node.
		std::string name;

		/// \brief The position and rotation of this node in its base state.
		glm::mat4x4 transform;
	};

	/// \brief Represents a *ZenGin* model hierarchy.
	///
	/// <p>Model hierarchy files represent the skeletal structure of a mesh. These skeletons are used to animate mostly
	/// animals and humans in the game which is commonly referred to as rigging.</p>
	class model_hierarchy {
	public:
		/// \brief Parses a model hierarchy from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).</p>
		///
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed model hierarchy object.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] static model_hierarchy parse(buffer& in);

		/// \brief Parses a model hierarchy from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).</p>
		///
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed model hierarchy object.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] inline static model_hierarchy parse(buffer&& in) {
			return model_hierarchy::parse(in);
		}

		/// \return The list of nodes this hierarchy consists of.
		[[nodiscard]] const std::vector<model_hierarchy_node>& nodes() const noexcept {
			return _m_nodes;
		}

		/// \return The bounding box of this hierarchy.
		[[nodiscard]] bounding_box bbox() const noexcept {
			return _m_bbox;
		}

		/// \return The collision bounding box of this hierarchy.
		[[nodiscard]] bounding_box collision_bbox() const noexcept {
			return _m_collision_bbox;
		}

		/// \return The translation of the root node of this hierarchy.
		[[nodiscard]] glm::vec3 root_translation() const noexcept {
			return _m_root_translation;
		}

	private:
		std::vector<model_hierarchy_node> _m_nodes;
		bounding_box _m_bbox;
		bounding_box _m_collision_bbox;
		glm::vec3 _m_root_translation;
	};
} // namespace phoenix
