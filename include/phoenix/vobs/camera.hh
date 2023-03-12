// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include "../Api.hh"
#include <phoenix/vobs/vob.hh>

namespace phoenix {
	enum class camera_motion : std::uint32_t {
		undefined = 0,
		smooth = 1,
		linear = 2,
		step = 3,
		slow = 4,
		fast = 5,
		custom = 6
	};

	enum class camera_trajectory : std::uint32_t {
		world = 0,
		object = 0,
	};

	enum class camera_lerp_mode : std::uint32_t {
		undefined = 0,
		path = 1,
		path_ignore_roll = 2,
		path_rotation_samples = 3,
	};

	enum class camera_loop : std::uint32_t {
		none = 0,
		restart = 1,
		pingpong = 2,
	};

	namespace vobs {
		/// \brief A VOb which describes the trajectory of a camera during a cutscene.
		struct camera_trj_frame : public vob {
			float time;
			float roll_angle;
			float fov_scale;
			camera_motion motion_type;
			camera_motion motion_type_fov;
			camera_motion motion_type_roll;
			camera_motion motion_type_time_scale;
			float tension;
			float cam_bias;
			float continuity;
			float time_scale;
			bool time_fixed;
			glm::mat4x4 original_pose;

			/// \brief Parses a camera trajectory VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static std::unique_ptr<camera_trj_frame> parse(archive_reader& ctx, game_version version);
		};

		/// \brief A VOb which defined the movement of the camera during a cutscene.
		struct cs_camera : public vob {
			camera_trajectory trajectory_for;
			camera_trajectory target_trajectory_for;
			camera_loop loop_mode;
			camera_lerp_mode lerp_mode;
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

			std::vector<std::unique_ptr<camera_trj_frame>> frames;

			// Save-game only variables
			bool s_paused {false};
			bool s_started {false};
			bool s_goto_time_mode {false};
			float s_cs_time {0};

			/// \brief Parses a cutscene camera VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(cs_camera& obj, archive_reader& ctx, game_version version);
		};
	} // namespace vobs
} // namespace phoenix