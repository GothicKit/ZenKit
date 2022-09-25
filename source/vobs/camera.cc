// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vobs/camera.hh>

#include <fmt/format.h>

#include <fmt/format.h>

namespace phoenix::vobs {
	std::unique_ptr<camera_trj_frame> camera_trj_frame::parse(archive_reader& ctx, game_version version) {
		auto obj = std::make_unique<camera_trj_frame>();

		vob::parse(*obj, ctx, version);
		obj->time = ctx.read_float();                                              // time
		obj->roll_angle = ctx.read_float();                                        // angleRollDeg
		obj->fov_scale = ctx.read_float();                                         // camFOVScale
		obj->motion_type = static_cast<camera_motion>(ctx.read_enum());            // motionType
		obj->motion_type_fov = static_cast<camera_motion>(ctx.read_enum());        // motionTypeFOV
		obj->motion_type_roll = static_cast<camera_motion>(ctx.read_enum());       // motionTypeRoll
		obj->motion_type_time_scale = static_cast<camera_motion>(ctx.read_enum()); // motionTypeTimeScale
		obj->tension = ctx.read_float();                                           // tension
		obj->cam_bias = ctx.read_float();                                          // bias
		obj->continuity = ctx.read_float();                                        // continuity
		obj->time_scale = ctx.read_float();                                        // timeScale
		obj->time_fixed = ctx.read_bool();                                         // timeIsFixed

		auto buf = ctx.read_raw_bytes();
		obj->original_pose = glm::transpose(buf.get_mat4x4()); // originalPose
		return obj;
	}

	void cs_camera::parse(cs_camera& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.trajectory_for = static_cast<camera_trajectory>(ctx.read_enum());        // camTrjFOR
		obj.target_trajectory_for = static_cast<camera_trajectory>(ctx.read_enum()); // targetTrjFOR
		obj.loop_mode = static_cast<camera_loop>(ctx.read_enum());                   // loopMode
		obj.lerp_mode = static_cast<camera_lerp_mode>(ctx.read_enum());              // splLerpMode
		obj.ignore_for_vob_rotation = ctx.read_bool();                               // ignoreFORVobRotCam
		obj.ignore_for_vob_rotation_target = ctx.read_bool();                        // ignoreFORVobRotTarget
		obj.adapt = ctx.read_bool();                                                 // adaptToSurroundings
		obj.ease_first = ctx.read_bool();                                            // easeToFirstKey
		obj.ease_last = ctx.read_bool();                                             // easeFromLastKey
		obj.total_duration = ctx.read_float();                                       // totalTime
		obj.auto_focus_vob = ctx.read_string();                                      // autoCamFocusVobName
		obj.auto_player_movable = ctx.read_bool();                                   // autoCamPlayerMovable
		obj.auto_untrigger_last = ctx.read_bool();                                   // autoCamUntriggerOnLastKey
		obj.auto_untrigger_last_delay = ctx.read_float();                            // autoCamUntriggerOnLastKeyDelay
		obj.position_count = ctx.read_int();                                         // numPos
		obj.target_count = ctx.read_int();                                           // numTargets

		archive_object frame_obj {};
		while (ctx.read_object_begin(frame_obj)) {
			if (frame_obj.class_name != "zCCamTrj_KeyFrame:zCVob") {
				PX_LOGW("cs_camera: unexpected \"{}\" in \"zCCSCamera:zCVob\"", frame_obj.class_name);
				ctx.skip_object(true);
				continue;
			}

			obj.frames.emplace_back(camera_trj_frame::parse(ctx, version));

			if (!ctx.read_object_end()) {
				PX_LOGW("cs_camera: \"zCCamTrj_KeyFrame\" not fully parsed");
				ctx.skip_object(true);
			}
		}
	}
} // namespace phoenix::vobs