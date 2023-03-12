// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/animation.hh>
#include <phoenix/vobs/vob.hh>

namespace phoenix {
	enum class mover_behavior : uint32_t {
		toggle = 0,
		trigger_control = 1,
		open_timed = 2,
		loop = 3,
		single_keys = 4,
	};

	enum class mover_lerp_mode : uint32_t {
		curve = 0,
		linear = 1,
	};

	enum class mover_speed_mode : uint32_t {
		seg_constant = 0,
		slow_start_end = 1,
		slow_start = 2,
		slow_end = 3,
		seg_slow_start_end = 4,
		seg_slow_start = 5,
		seg_slow_end = 6,
	};

	enum class trigger_batch_mode {
		all = 0,
		next = 1,
		random = 2,
	};

	namespace vobs {
		/// \brief A basic trigger VOb which does something upon the player interacting with it.
		struct trigger : public vob {
			std::string target;
			std::uint8_t flags;
			std::uint8_t filter_flags;
			std::string vob_target;
			std::int32_t max_activation_count;
			float retrigger_delay_sec;
			float damage_threshold;
			float fire_delay_sec;

			// Save-game only variables
			float s_next_time_triggerable {0};
			int s_count_can_be_activated {0};
			bool s_is_enabled {true};

			/// \brief Parses a trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			/// \see trigger::parse
			PHOENIX_API static void parse(trigger& obj, archive_reader& ctx, game_version version);
		};

		/// \brief A VOb which can move upon player interaction.
		struct trigger_mover : public trigger {
			mover_behavior behavior {mover_behavior::toggle};
			float touch_blocker_damage {0};
			float stay_open_time_sec {0};
			bool locked {true};
			bool auto_link {false};
			bool auto_rotate {false};

			float speed {0};
			mover_lerp_mode lerp_mode {mover_lerp_mode::curve};
			mover_speed_mode speed_mode {mover_speed_mode::seg_constant};

			std::vector<animation_sample> keyframes {};

			std::string sfx_open_start {};
			std::string sfx_open_end {};
			std::string sfx_transitioning {};
			std::string sfx_close_start {};
			std::string sfx_close_end {};
			std::string sfx_lock {};
			std::string sfx_unlock {};
			std::string sfx_use_locked {};

			// Save-game only variables
			glm::vec3 s_act_key_pos_delta;
			float s_act_keyframe_f;
			int s_act_keyframe;
			int s_next_keyframe;
			float s_move_speed_unit;
			float s_advance_dir;
			uint32_t s_mover_state;
			int s_trigger_event_count;
			float s_stay_open_time_dest;

			/// \brief Parses a mover trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			/// \see trigger::parse
			PHOENIX_API static void parse(trigger_mover& obj, archive_reader& ctx, game_version version);
		};

		/// \brief A VOb which can call multiple script function upon being triggered.
		struct trigger_list : public trigger {
			struct target {
				std::string name {};
				float delay {};

				[[nodiscard]] inline bool operator==(const target& tgt) const noexcept {
					return this->name == tgt.name && this->delay == tgt.delay;
				}
			};

			trigger_batch_mode mode {};
			std::vector<target> targets {};

			// Save-game only variables
			uint8_t s_act_target {0};
			bool s_send_on_trigger {false};

			/// \brief Parses a trigger list VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			/// \see trigger::parse
			PHOENIX_API static void parse(trigger_list& obj, archive_reader& ctx, game_version version);
		};

		/// \brief A VOb which calls a script function upon being triggered.
		struct trigger_script : public trigger {
			std::string function {};

			/// \brief Parses a script trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			/// \see trigger::parse
			PHOENIX_API static void parse(trigger_script& obj, archive_reader& ctx, game_version version);
		};

		/// \brief A VOb which triggers a level change if the player moves close to it.
		struct trigger_change_level : public trigger {
			std::string level_name {};
			std::string start_vob {};

			/// \brief Parses a change level trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			/// \see trigger::parse
			PHOENIX_API static void parse(trigger_change_level& obj, archive_reader& ctx, game_version version);
		};

		/// \brief A VOb which triggers a world start event.
		struct trigger_world_start : public vob {
			std::string target;
			bool fire_once;

			// Save-game only variables
			bool s_has_fired {false};

			/// \brief Parses a world load trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(trigger_world_start& obj, archive_reader& ctx, game_version version);
		};

		struct trigger_untouch : public vob {
			std::string target;

			/// \brief Parses an untouch trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(trigger_untouch& obj, archive_reader& ctx, game_version version);
		};
	} // namespace vobs
} // namespace phoenix
