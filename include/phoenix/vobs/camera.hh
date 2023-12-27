// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/vobs/Camera.hh"

namespace phoenix {
	namespace vobs {
		using camera_trj_frame ZKREM("renamed to zenkit::VCameraTrajectoryFrame") = zenkit::VCameraTrajectoryFrame;
		using cs_camera ZKREM("renamed to zenkit::VCutsceneCamera") = zenkit::VCutsceneCamera;
	} // namespace vobs

	using camera_motion ZKREM("renamed to zenkit::CameraMotion") = zenkit::CameraMotion;
	using camera_trajectory ZKREM("renamed to zenkit::CameraCoordinateReference") = zenkit::CameraCoordinateReference;
	using camera_lerp_mode ZKREM("renamed to zenkit::CameraLerpType") = zenkit::CameraLerpType;
	using camera_loop ZKREM("renamed to zenkit::CameraLoop") = zenkit::CameraLoop;
} // namespace phoenix
