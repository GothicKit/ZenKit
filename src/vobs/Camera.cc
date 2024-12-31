// Copyright © 2022-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/vobs/Camera.hh"
#include "zenkit/Archive.hh"

namespace zenkit {
	std::unique_ptr<VCameraTrajectoryFrame> VCameraTrajectoryFrame::parse(ReadArchive& r, GameVersion version) {
		auto obj = std::make_unique<VCameraTrajectoryFrame>();
		obj->load(r, version);
		return obj;
	}

	void VCameraTrajectoryFrame::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->time = r.read_float();                                             // time
		this->roll_angle = r.read_float();                                       // angleRollDeg
		this->fov_scale = r.read_float();                                        // camFOVScale
		this->motion_type = static_cast<CameraMotion>(r.read_enum());            // motionType
		this->motion_type_fov = static_cast<CameraMotion>(r.read_enum());        // motionTypeFOV
		this->motion_type_roll = static_cast<CameraMotion>(r.read_enum());       // motionTypeRoll
		this->motion_type_time_scale = static_cast<CameraMotion>(r.read_enum()); // motionTypeTimeScale
		this->tension = r.read_float();                                          // tension
		this->cam_bias = r.read_float();                                         // bias
		this->continuity = r.read_float();                                       // continuity
		this->time_scale = r.read_float();                                       // timeScale
		this->time_fixed = r.read_bool();                                        // timeIsFixed

		auto buf = r.read_raw(sizeof(float) * 4 * 4); // originalPose
		this->original_pose = buf->read_mat4();
	}

	void VCameraTrajectoryFrame::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);

		w.write_float("time", this->time);
		w.write_float("angleRollDeg", this->roll_angle);
		w.write_float("camFOVScale", this->fov_scale);
		w.write_enum("motionType", static_cast<uint32_t>(this->motion_type));
		w.write_enum("motionTypeFOV", static_cast<uint32_t>(this->motion_type_fov));
		w.write_enum("motionTypeRoll", static_cast<uint32_t>(this->motion_type_roll));
		w.write_enum("motionTypeTimeScale", static_cast<uint32_t>(this->motion_type_time_scale));
		w.write_float("tension", this->tension);
		w.write_float("bias", this->cam_bias);
		w.write_float("continuity", this->continuity);
		w.write_float("timeScale", this->time_scale);
		w.write_bool("timeIsFixed", this->time_fixed);
		w.write_raw_float("originalPose", this->original_pose.pointer(), 4 * 4);
	}

	void VCutsceneCamera::parse(VCutsceneCamera& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VCutsceneCamera::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->trajectory_for = static_cast<CameraCoordinateReference>(r.read_enum());        // camTrjFOR
		this->target_trajectory_for = static_cast<CameraCoordinateReference>(r.read_enum()); // targetTrjFOR
		this->loop_mode = static_cast<CameraLoop>(r.read_enum());                            // loopMode
		this->lerp_mode = static_cast<CameraLerpType>(r.read_enum());                        // splLerpMode
		this->ignore_for_vob_rotation = r.read_bool();                                       // ignoreFORVobRotCam
		this->ignore_for_vob_rotation_target = r.read_bool();                                // ignoreFORVobRotTarget
		this->adapt = r.read_bool();                                                         // adaptToSurroundings
		this->ease_first = r.read_bool();                                                    // easeToFirstKey
		this->ease_last = r.read_bool();                                                     // easeFromLastKey
		this->total_duration = r.read_float();                                               // totalTime
		this->auto_focus_vob = r.read_string();                                              // autoCamFocusVobName
		this->auto_player_movable = r.read_bool();                                           // autoCamPlayerMovable
		this->auto_untrigger_last = r.read_bool();        // autoCamUntriggerOnLastKey
		this->auto_untrigger_last_delay = r.read_float(); // autoCamUntriggerOnLastKeyDelay
		this->position_count = r.read_int();              // numPos
		this->target_count = r.read_int();                // numTargets

		for (auto i = 0; i < this->position_count; ++i) {
			auto obj = r.read_object<VCameraTrajectoryFrame>(version);
			this->trajectory_frames.push_back(obj);
		}

		for (auto i = 0; i < this->target_count; ++i) {
			auto obj = r.read_object<VCameraTrajectoryFrame>(version);
			this->target_frames.push_back(obj);
		}

		if (r.is_save_game() && version == GameVersion::GOTHIC_2) {
			// In save-games, cutscene cameras contain extra variables
			this->paused = r.read_bool();         // paused
			this->started = r.read_bool();        // started
			this->goto_time_mode = r.read_bool(); // gotoTimeMode
			this->cs_time = r.read_float();       // csTime
		}
	}

	void VCutsceneCamera::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);

		w.write_enum("camTrjFOR", static_cast<uint32_t>(this->trajectory_for));
		w.write_enum("targetTrjFOR", static_cast<uint32_t>(this->target_trajectory_for));
		w.write_enum("loopMode", static_cast<uint32_t>(this->loop_mode));
		w.write_enum("splLerpMode", static_cast<uint32_t>(this->lerp_mode));
		w.write_bool("ignoreFORVobRotCam", this->ignore_for_vob_rotation);
		w.write_bool("ignoreFORVobRotTarget", this->ignore_for_vob_rotation_target);
		w.write_bool("adaptToSurroundings", this->adapt);
		w.write_bool("easeToFirstKey", this->ease_first);
		w.write_bool("easeFromLastKey", this->ease_last);
		w.write_float("totalTime", this->total_duration);
		w.write_string("autoCamFocusVobName", this->auto_focus_vob);
		w.write_bool("autoCamPlayerMovable", this->auto_player_movable);
		w.write_bool("autoCamUntriggerOnLastKey", this->auto_untrigger_last);
		w.write_float("autoCamUntriggerOnLastKeyDelay", this->auto_untrigger_last_delay);
		w.write_int("numPos", this->position_count);
		w.write_int("numTargets", this->target_count);

		for (auto i = 0; i < this->position_count; ++i) {
			w.write_object(this->trajectory_frames[static_cast<size_t>(i)], version);
		}

		for (auto i = 0; i < this->target_count; ++i) {
			w.write_object(this->target_frames[static_cast<size_t>(i)], version);
		}

		if (w.is_save_game() && version == GameVersion::GOTHIC_2) {
			// In save-games, cutscene cameras contain extra variables
			w.write_bool("paused", this->paused);
			w.write_bool("started", this->started);
			w.write_bool("gotoTimeMode", this->goto_time_mode);
			w.write_float("csTime", this->cs_time);
		}
	}

	uint16_t VCutsceneCamera::get_version_identifier(GameVersion game) const {
		return game == GameVersion::GOTHIC_1 ? 30720 : 33793;
	}
} // namespace zenkit
