// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/detail/buffer.hh>
#include <phoenix/math/bbox.hh>

#include <glm/gtx/euler_angles.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

#include <vector>

namespace phoenix {

	/**
	 * @brief Represents an oriented bounding box (OBB)
	 */
	struct obb {
		glm::vec3 center;
		glm::vec3 axes[3];
		glm::vec3 half_width;

		std::vector<obb> children;

		/**
		 * @brief Calculates an AABB from this OBB. This code has been directly taken from ZenLib an
		 *        has not been vetted.
		 * @todo Validate this calculation as well as the `axes` member!
		 * @return An AABB which contains this OBB.
		 */
		[[nodiscard]] bounding_box as_bbox() const {
			const float sign[8][3] = {{-1, -1, -1},
			                          {-1, -1, +1},
			                          {-1, +1, -1},
			                          {-1, +1, +1},
			                          {+1, -1, -1},
			                          {+1, -1, +1},
			                          {+1, +1, -1},
			                          {+1, +1, +1}};

			bounding_box box {};
			box.min = {FLT_MAX, FLT_MAX, FLT_MAX};
			box.max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

			for (auto i : sign) {
				auto point = center;
				auto axis0 = axes[0] * half_width.x * i[0];
				auto axis1 = axes[1] * half_width.y * i[1];
				auto axis2 = axes[2] * half_width.z * i[2];

				point.x += axis0.x + axis1.x + axis2.x;
				point.y += axis0.y + axis1.y + axis2.y;
				point.z += axis0.z + axis1.z + axis2.z;

				box.min.x = std::min(box.min.x, point.x);
				box.min.y = std::min(box.min.y, point.y);
				box.min.z = std::min(box.min.z, point.z);

				box.max.x = std::max(box.max.x, point.x);
				box.max.y = std::max(box.max.y, point.y);
				box.max.z = std::max(box.max.z, point.z);
			}

			return box;
		}

		static obb parse(buffer& in) {
			obb bbox {};
			bbox.center = in.get_vec3();
			bbox.axes[0] = in.get_vec3();
			bbox.axes[1] = in.get_vec3();
			bbox.axes[2] = in.get_vec3();
			bbox.half_width = in.get_vec3();

			auto child_count = in.get_ushort();
			for (int i = 0; i < child_count; ++i) {
				bbox.children.push_back(parse(in));
			}

			return bbox;
		}
	};

} // namespace phoenix
