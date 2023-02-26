// Copyright © 2023 Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include <phoenix/buffer.hh>
#include <phoenix/math.hh>

namespace phoenix {
	bounding_box bounding_box::parse(buffer& in) {
		bounding_box bbox {};
		bbox.min = in.get_vec3();
		bbox.max = in.get_vec3();
		return bbox;
	}

	obb obb::parse(buffer& in) {
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

	bounding_box obb::as_bbox() const {
		const float sign[8][3] = {{-1, -1, -1},
		                          {-1, -1, +1},
		                          {-1, +1, -1},
		                          {-1, +1, +1},
		                          {+1, -1, -1},
		                          {+1, -1, +1},
		                          {+1, +1, -1},
		                          {+1, +1, +1}};

		bounding_box box {};
		box.min = {std::numeric_limits<float>::max(),
		           std::numeric_limits<float>::max(),
		           std::numeric_limits<float>::max()};

		box.max = {std::numeric_limits<float>::min(),
		           std::numeric_limits<float>::min(),
		           std::numeric_limits<float>::min()};

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
} // namespace phoenix