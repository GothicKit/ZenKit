// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Boxes.hh"
#include "zenkit/Stream.hh"

#include "phoenix/buffer.hh"

#include <limits>

namespace zenkit {
	AxisAlignedBoundingBox AxisAlignedBoundingBox::parse(phoenix::buffer& in) {
		AxisAlignedBoundingBox bbox {};

		auto r = Read::from(&in);
		bbox.load(r.get());

		return bbox;
	}

	void AxisAlignedBoundingBox::load(Read* r) {
		this->min = r->read_vec3();
		this->max = r->read_vec3();
	}

	void AxisAlignedBoundingBox::save(Write* w) const {
		w->write_vec3(this->min);
		w->write_vec3(this->max);
	}

	OrientedBoundingBox OrientedBoundingBox::parse(phoenix::buffer& in) {
		OrientedBoundingBox bbox {};

		auto r = Read::from(&in);
		bbox.load(r.get());

		return bbox;
	}

	void OrientedBoundingBox::load(zenkit::Read* r) {
		center = r->read_vec3();
		axes[0] = r->read_vec3();
		axes[1] = r->read_vec3();
		axes[2] = r->read_vec3();
		half_width = r->read_vec3();

		children.resize(r->read_ushort());
		for (auto& child : children) {
			child.load(r);
		}
	}

	AxisAlignedBoundingBox OrientedBoundingBox::as_bbox() const {
		float const sign[8][3] = {{-1, -1, -1},
		                          {-1, -1, +1},
		                          {-1, +1, -1},
		                          {-1, +1, +1},
		                          {+1, -1, -1},
		                          {+1, -1, +1},
		                          {+1, +1, -1},
		                          {+1, +1, +1}};

		AxisAlignedBoundingBox box {};
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
} // namespace zenkit
