// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/Misc.hh"
#include "zenkit/vobs/VirtualObject.hh"

#include <glm/mat4x4.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace zenkit {
	class ReadArchive;

	enum class CameraMotion : std::uint32_t {
		UNDEFINED = 0,
		SMOOTH = 1,
		LINEAR = 2,
		STEP = 3,
		SLOW = 4,
		FAST = 5,
		CUSTOM = 6,

		// Deprecated entries.
		undefined ZKREM("renamed to CameraMotion::UNDEFINED") = UNDEFINED,
		smooth ZKREM("renamed to CameraMotion::SMOOTH") = SMOOTH,
		linear ZKREM("renamed to CameraMotion::LINEAR") = LINEAR,
		step ZKREM("renamed to CameraMotion::STEP") = STEP,
		slow ZKREM("renamed to CameraMotion::SLOW") = SLOW,
		fast ZKREM("renamed to CameraMotion::FAST") = FAST,
		custom ZKREM("renamed to CameraMotion::CUSTOM") = CUSTOM,
	};

	enum class CameraTrajectory : std::uint32_t {
		WORLD = 0,
		OBJECT = 1,

		// Deprecated entries.
		world ZKREM("renamed to CameraTrajectory::WORLD") = WORLD,
		object ZKREM("renamed to CameraTrajectory::OBJECT") = OBJECT,
	};

	enum class CameraLerpType : std::uint32_t {
		UNDEFINED = 0,
		PATH = 1,
		PATH_IGNORE_ROLL = 2,
		PATH_ROTATION_SAMPLES = 3,

		// Deprecated entries.
		undefined ZKREM("renamed to CameraLerpType::UNDEFINED") = UNDEFINED,
		path ZKREM("renamed to CameraLerpType::PATH") = PATH,
		path_ignore_roll ZKREM("renamed to CameraLerpType::PATH_IGNORE_ROLL") = PATH_IGNORE_ROLL,
		path_rotation_samples ZKREM("renamed to CameraLerpType::PATH_ROTATION_SAMPLES") = PATH_ROTATION_SAMPLES,
	};

	enum class CameraLoop : std::uint32_t {
		NONE = 0,
		RESTART = 1,
		PINGPONG = 2,

		// Deprecated entries.
		none ZKREM("renamed to CameraLoop::NONE") = NONE,
		restart ZKREM("renamed to CameraLoop::RESTART") = RESTART,
		pingpong ZKREM("renamed to CameraLoop::PINGPONG") = PINGPONG,
	};

	/// \brief A VOb which describes the trajectory of a camera during a cutscene.
	struct VCameraTrajectoryFrame : VirtualObject {
		ZK_OBJECT(ObjectType::zCCamTrj_KeyFrame);

	public:
		float time;
		float roll_angle;
		float fov_scale;
		CameraMotion motion_type;
		CameraMotion motion_type_fov;
		CameraMotion motion_type_roll;
		CameraMotion motion_type_time_scale;
		float tension;
		float cam_bias;
		float continuity;
		float time_scale;
		bool time_fixed;
		glm::mat4 original_pose;

		/// \brief Parses a camera trajectory VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws ParserError if parsing fails.
		/// \see vob::parse
		ZKREM("use ::load()")
		ZKAPI static std::unique_ptr<VCameraTrajectoryFrame> parse(ReadArchive& ctx, GameVersion version);

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
	};

	/// \brief A VOb which defined the movement of the camera during a cutscene.
	struct VCutsceneCamera : VirtualObject {
		ZK_OBJECT(ObjectType::zCCSCamera);

	public:
		CameraTrajectory trajectory_for;
		CameraTrajectory target_trajectory_for;
		CameraLoop loop_mode;
		CameraLerpType lerp_mode;
		bool ignore_for_vob_rotation;
		bool ignore_for_vob_rotation_target;
		bool adapt;
		bool ease_first;
		bool ease_last;
		float total_duration;
		std::string auto_focus_vob;
		bool auto_player_movable;
		bool auto_untrigger_last;
		float auto_untrigger_last_delay;
		std::int32_t position_count;
		std::int32_t target_count;

		std::vector<std::shared_ptr<VCameraTrajectoryFrame>> frames;

		// Save-game only variables
		bool paused {false};
		bool started {false};
		bool goto_time_mode {false};
		float cs_time {0};

		ZKREM("use ::load()") ZKAPI static void parse(VCutsceneCamera& obj, ReadArchive& ctx, GameVersion version);

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};
} // namespace zenkit
