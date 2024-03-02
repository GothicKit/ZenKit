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

		/// \brief The movement method is calculated taking into account nearby frames.
		SMOOTH = 1,

		/// \brief Constant speed in the current frame.
		LINEAR = 2,

		/// \brief Step by step. Movement is jerky.
		STEP = 3,

		/// \brief The camera movement slows down in the current frame.
		SLOW = 4,

		/// \brief The camera movement accelerates in the current frame.
		FAST = 5,

		/// \brief Not implemented.
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

	enum class CameraCoordinateReference : std::uint32_t {
		/// \brief Positions are relative to the global world coordinate system.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#targettrjfor
		WORLD = 0,

		/// \brief Positions are relative to the position of the camera object in the previous frame.
		///
		/// Essentially, when the camera travels along the keyframes each subsequent keyframe's position is relative
		/// to the position of the previous keyframe. The position of the first keyframe is relative to the initial
		/// position of the camera.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#targettrjfor
		OBJECT = 1,

		// Deprecated entries.
		world ZKREM("renamed to CameraCoordinateReference::WORLD") = WORLD,
		object ZKREM("renamed to CameraCoordinateReference::OBJECT") = OBJECT,
	};

	enum class CameraLerpType : std::uint32_t {
		/// \brief Unset.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#spllerpmode
		UNDEFINED = 0,

		/// \brief The camera's orientation is specified by the slope of the target path spline.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#spllerpmode
		PATH = 1,

		/// \brief The camera's orientation is similar to PATH with the only difference being that the camera's XZ
		///        plane is always parallel to the world's XZ plane.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#spllerpmode
		PATH_IGNORE_ROLL = 2,

		/// \brief The camera's orientation is based on the keyframes.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#spllerpmode
		PATH_ROTATION_SAMPLES = 3,

		// Deprecated entries.
		undefined ZKREM("renamed to CameraLerpType::UNDEFINED") = UNDEFINED,
		path ZKREM("renamed to CameraLerpType::PATH") = PATH,
		path_ignore_roll ZKREM("renamed to CameraLerpType::PATH_IGNORE_ROLL") = PATH_IGNORE_ROLL,
		path_rotation_samples ZKREM("renamed to CameraLerpType::PATH_ROTATION_SAMPLES") = PATH_ROTATION_SAMPLES,
	};

	enum class CameraLoop : std::uint32_t {
		/// \brief The camera animation stops after the last frame is reached.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#loopmode
		NONE = 0,

		/// \brief The camera animation starts from the beginning after the last frame is reached.
		///
		/// The ModKit states, that this is only useful if the animation is to be interrupted by scripts
		/// or triggers.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#loopmode
		RESTART = 1,

		/// \brief The camera animation re-plays the keyframes in reverse order after reaching the last frame.
		///
		/// This causes the camera to infinitely move back and forth along the keyframes. The ModKit states, that
		/// like CameraLoop::RESTART, this is only useful if the animation will be interrupted by scripts or triggers.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#loopmode
		PINGPONG = 2,

		// Deprecated entries.
		none ZKREM("renamed to CameraLoop::NONE") = NONE,
		restart ZKREM("renamed to CameraLoop::RESTART") = RESTART,
		pingpong ZKREM("renamed to CameraLoop::PINGPONG") = PINGPONG,
	};

	/// \brief A VOb which describes the trajectory of a camera during a cutscene.
	/// \see https://zk.gothickit.dev/engine/objects/zCCamTrj_KeyFrame/
	struct VCameraTrajectoryFrame : VirtualObject {
		ZK_OBJECT(ObjectType::zCCamTrj_KeyFrame);

		/// \brief Unclear.
		/// \see https://zk.gothickit.dev/engine/objects/zCCamTrj_KeyFrame/#time
		float time;

		/// \brief The camera rotation angle of the frame along the Z axis.
		/// \see https://zk.gothickit.dev/engine/objects/zCCamTrj_KeyFrame/#anglerolldeg
		float roll_angle;

		/// \brief Focal zoom of the camera's field of view (FOV) of the frame. Use reasonable values to avoid dizzying
		///        effects.
		/// \see https://zk.gothickit.dev/engine/objects/zCCamTrj_KeyFrame/#camfovscale
		float fov_scale;

		/// \brief The type of camera movement for the frame.
		/// \see https://zk.gothickit.dev/engine/objects/zCCamTrj_KeyFrame/#motiontype
		CameraMotion motion_type;

		/// \brief The type of focus change for the frame.
		/// \see https://zk.gothickit.dev/engine/objects/zCCamTrj_KeyFrame/#motiontypefov
		CameraMotion motion_type_fov;

		/// \brief The type of rotation for the frame.
		/// \see https://zk.gothickit.dev/engine/objects/zCCamTrj_KeyFrame/#motiontyperoll
		CameraMotion motion_type_roll;

		/// \brief Unclear.
		/// \see https://zk.gothickit.dev/engine/objects/zCCamTrj_KeyFrame/#motiontypetimescale
		CameraMotion motion_type_time_scale;

		/// \brief Defines the bend of the camera motion curve at the specified key. Larger values result in a sharper
		///       bend.
		/// \see https://zk.gothickit.dev/engine/objects/zCCamTrj_KeyFrame/#tension
		float tension;

		/// \brief Determines the slope of a line in one direction. The direction of the slope is determined by the
		///       sign, and the steepness by the value.
		/// \see https://zk.gothickit.dev/engine/objects/zCCamTrj_KeyFrame/#bias
		float cam_bias;

		/// \brief Determines the amount of curve discontinuity at this animation key.
		/// \see https://zk.gothickit.dev/engine/objects/zCCamTrj_KeyFrame/#continuity
		float continuity;

		/// \brief Unknown.
		/// \see https://zk.gothickit.dev/engine/objects/zCCamTrj_KeyFrame/#timescale
		float time_scale;

		/// \brief An auxiliary flag indicating the time of the key frame, which cannot be changed automatically.
		/// \see https://zk.gothickit.dev/engine/objects/zCCamTrj_KeyFrame/#timeisfixed
		bool time_fixed;

		/// \brief Unknown.
		/// \see https://zk.gothickit.dev/engine/objects/zCCamTrj_KeyFrame/#originalpose
		glm::mat4 original_pose;

		ZKREM("use ::load()")
		ZKAPI static std::unique_ptr<VCameraTrajectoryFrame> parse(ReadArchive& r, GameVersion version);

		/// \brief Load this object from the given archive.
		/// \param r The archive to read from;
		/// \param version The version of the game the object was made for.
		ZKAPI void load(ReadArchive& r, GameVersion version) override;

		/// \brief Save this object to the given archive.
		/// \param w The archive to save to.
		/// \param version The version of the game to save for.
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
	};

	/// \brief A VOb which defined the movement of the camera during a cutscene.
	/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/
	struct VCutsceneCamera : VirtualObject {
		ZK_OBJECT(ObjectType::zCCSCamera);

		/// \brief The reference coordinate system of camera trajectory keyframes.
		/// \note FOR stands for "frame of reference".
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#camtrjfor
		CameraCoordinateReference trajectory_for;

		/// \brief The reference coordinate system of camera target keyframes.
		/// \note FOR stands for "frame of reference".
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#targettrjfor
		CameraCoordinateReference target_trajectory_for;

		/// \brief The repetition mode of the camera animation.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#loopmode
		CameraLoop loop_mode;

		/// \brief Adjust camera orientation while moving.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#spllerpmode
		CameraLerpType lerp_mode;

		/// \brief Determines whether the orientation to reference VOB objects should be taken into account when
		///        passing through key frames.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#ignoreforvobrotcam
		bool ignore_for_vob_rotation;

		/// \brief Determines the need to take into account the orientation of target key frames to reference VOB
		///       objects.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#ignoreforvobrottarget
		bool ignore_for_vob_rotation_target;

		/// \brief Indicates the need for dynamic adaptation to the environment.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#adapttosurroundings
		bool adapt;

		/// \brief Gives a smooth transition of camera position from the first keyframe to the last, if possible.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#easetofirstkey
		bool ease_first;

		/// \brief Gives a smooth transition of camera position from the last keyframe to the first, if possible.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#easefromlastkey
		bool ease_last;

		/// \brief The duration of the camera animation in seconds.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#totaltime
		float total_duration;

		/// \brief The name of the VOB object which the camera will automatically focus.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#autocamfocusvobname
		std::string auto_focus_vob;

		/// \brief Indicates whether the player will be able to move when moving the camera animation.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#autocamplayermovable
		bool auto_player_movable;

		/// \brief Emit an `OnUntrigger` event after reaching the last animation frame.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#autocamuntriggeronlastkey
		bool auto_untrigger_last;

		/// \brief Unclear.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#autocamuntriggeronlastkeydelay
		float auto_untrigger_last_delay;

		/// \brief The number of position key frames for the camera animation.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#numpos
		std::int32_t position_count;

		/// \brief The number of target key frames for the camera animation.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#numtargets
		std::int32_t target_count;

		/// \brief The list of all position keyframes.
		std::vector<std::shared_ptr<VCameraTrajectoryFrame>> trajectory_frames;

		/// \brief The list of all target keyframes.
		std::vector<std::shared_ptr<VCameraTrajectoryFrame>> target_frames;

		// Save-game only variables

		/// \brief Unknown.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#paused
		bool paused {false};

		/// \brief Unknown.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#started
		bool started {false};

		/// \brief Unknown.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#gototimemode
		bool goto_time_mode {false};

		/// \brief Unknown.
		/// \see https://zk.gothickit.dev/engine/objects/zCCSCamera/#cstime
		float cs_time {0};

		ZKREM("use ::load()") ZKAPI static void parse(VCutsceneCamera& obj, ReadArchive& r, GameVersion version);

		/// \brief Load this object from the given archive.
		/// \param r The archive to read from;
		/// \param version The version of the game the object was made for.
		ZKAPI void load(ReadArchive& r, GameVersion version) override;

		/// \brief Save this object to the given archive.
		/// \param w The archive to save to.
		/// \param version The version of the game to save for.
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;

		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};
} // namespace zenkit
