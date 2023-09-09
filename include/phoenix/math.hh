// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Boxes.hh"

namespace phoenix {
	using bounding_box ZKREM("renamed to zenkit::AxisAlignedBoundingBox") = zenkit::AxisAlignedBoundingBox;
	using obb ZKREM("renamed to zenkit::OrientedBoundingBox") = zenkit::OrientedBoundingBox;
} // namespace phoenix
