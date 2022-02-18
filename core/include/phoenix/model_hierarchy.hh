// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/detail/stream.hh>

#include <glm/mat4x4.hpp>

#include <tuple>
#include <vector>

namespace phoenix {
	struct model_hierarchy_node {
		s16 parent_index;
		std::string name;
		glm::mat4x4 transform;
	};

	class model_hierachy {
	public:
		static model_hierachy parse(reader& in);

		[[nodiscard]] const std::vector<model_hierarchy_node>& nodes() const noexcept {
			return _m_nodes;
		}

		[[nodiscard]] std::tuple<glm::vec3, glm::vec3> bbox() const noexcept {
			return {_m_bbox[0], _m_bbox[1]};
		}

		[[nodiscard]] std::tuple<glm::vec3, glm::vec3> collision_bbox() const noexcept {
			return {_m_collision_bbox[0], _m_collision_bbox[1]};
		}

		[[nodiscard]] glm::vec3 root_translation() const noexcept {
			return _m_root_translation;
		}

	private:
		std::vector<model_hierarchy_node> _m_nodes;
		glm::vec3 _m_bbox[2];
		glm::vec3 _m_collision_bbox[2];
		glm::vec3 _m_root_translation;
	};

} // namespace phoenix
