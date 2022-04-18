// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/detail/buffer.hh>

#include <glm/vec3.hpp>

namespace phoenix {
	/**
	 * @brief Represents a simple bounding box (BBox)
	 */
	struct bounding_box {
		glm::vec3 min;
		glm::vec3 max;

		static bounding_box parse(buffer& in) {
			bounding_box bbox {};
			bbox.min = in.get_vec3();
			bbox.max = in.get_vec3();
			return bbox;
		}
	};
} // namespace phoenix
