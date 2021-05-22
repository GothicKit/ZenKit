// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/detail/stream.hh>

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

#include <vector>

namespace phoenix {

	/**
	 * @brief Represents an oriented bounding box (OBB)
	 */
	struct obb {
		glm::vec3 center;
		glm::mat3x3 rotation;
		glm::vec3 half_width;

		std::vector<obb> children;

		static obb parse(reader& in) {
			obb bbox {};
			bbox.center = in.read_vec3();
			bbox.rotation = glm::mat3x3 {
					in.read_vec3(),
					in.read_vec3(),
					in.read_vec3()};
			bbox.half_width = in.read_vec3();

			auto child_count = in.read_u16();
			for (int i = 0; i < child_count; ++i) {
				bbox.children.push_back(parse(in));
			}

			return bbox;
		}
	};

}// namespace phoenix