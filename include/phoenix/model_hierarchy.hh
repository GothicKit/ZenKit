// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/detail/buffer.hh>
#include <phoenix/math/bbox.hh>

#include <glm/mat4x4.hpp>

#include <tuple>
#include <vector>

namespace phoenix {
	struct model_hierarchy_node {
		model_hierarchy_node* parent;
		std::int16_t parent_index;
		std::string name;
		glm::mat4x4 transform;
	};

	class model_hierachy {
	public:
		static model_hierachy parse(buffer& in);

		[[nodiscard]] const std::vector<model_hierarchy_node>& nodes() const noexcept {
			return _m_nodes;
		}

		[[nodiscard]] bounding_box bbox() const noexcept {
			return _m_bbox;
		}

		[[nodiscard]] bounding_box collision_bbox() const noexcept {
			return _m_collision_bbox;
		}

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
