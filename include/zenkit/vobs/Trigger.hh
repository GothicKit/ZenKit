// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/Misc.hh"
#include "zenkit/ModelAnimation.hh"
#include "zenkit/vobs/VirtualObject.hh"

#include <cstdint>
#include <string>
#include <vector>

namespace zenkit {
	class ReadArchive;

	enum class MoverBehavior : uint32_t {
		TOGGLE = 0,
		TRIGGER_CONTROL = 1,
		OPEN_TIME = 2,
		LOOP = 3,
		SINGLE_KEYS = 4,

		// Deprecated entries.
		toggle ZKREM("renamed to MoverBehavior::TOGGLE") = TOGGLE,
		trigger_control ZKREM("renamed to MoverBehavior::TRIGGER_CONTROL") = TRIGGER_CONTROL,
		open_timed ZKREM("renamed to MoverBehavior::OPEN_TIME") = OPEN_TIME,
		loop ZKREM("renamed to MoverBehavior::LOOP") = LOOP,
		single_keys ZKREM("renamed to MoverBehavior::SINGLE_KEYS") = SINGLE_KEYS,
	};

	enum class MoverLerpType : uint32_t {
		CURVE = 0,
		LINEAR = 1,

		// Deprecated entries.
		curve ZKREM("renamed to MoverLerpType::CURVE") = CURVE,
		linear ZKREM("renamed to MoverLerpType::LINEAR") = LINEAR,
	};

	enum class MoverSpeedType : uint32_t {
		CONSTANT = 0,
		SLOW_START_END = 1,
		SLOW_START = 2,
		SLOW_END = 3,
		SEGMENT_SLOW_START_END = 4,
		SEGMENT_SLOW_START = 5,
		SEGMENT_SLOW_END = 6,

		// Deprecated entries.
		seg_constant ZKREM("renamed to MoverSpeedType::CONSTANT") = CONSTANT,
		slow_start_end ZKREM("renamed to MoverSpeedType::SLOW_START_END") = SLOW_START_END,
		slow_start ZKREM("renamed to MoverSpeedType::SLOW_START") = SLOW_START,
		slow_end ZKREM("renamed to MoverSpeedType::SLOW_END") = SLOW_END,
		seg_slow_start_end ZKREM("renamed to MoverSpeedType::SEGMENT_SLOW_START_END") = SEGMENT_SLOW_START_END,
		seg_slow_start ZKREM("renamed to MoverSpeedType::SEGMENT_SLOW_START") = SEGMENT_SLOW_START,
		seg_slow_end ZKREM("renamed to MoverSpeedType::SEGMENT_SLOW_END") = SEGMENT_SLOW_END,
	};

	enum class TriggerBatchMode {
		ALL = 0,
		NEXT = 1,
		RANDOM = 2,

		// Deprecated entries.
		all ZKREM("renamed to TriggerBatchMode::ALL") = ALL,
		next ZKREM("renamed to TriggerBatchMode::NEXT") = NEXT,
		random ZKREM("renamed to TriggerBatchMode::RANDOM") = RANDOM,
	};

	namespace vobs {
		/// \brief A basic trigger VOb which does something upon the player interacting with it.
		struct Trigger : public VirtualObject {
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
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see trigger::parse
			ZKREM("use ::load()") ZKAPI static void parse(Trigger& obj, ReadArchive& ctx, GameVersion version);

			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		/// \brief A VOb which can move upon player interaction.
		struct Mover : public Trigger {
			MoverBehavior behavior {MoverBehavior::TOGGLE};
			float touch_blocker_damage {0};
			float stay_open_time_sec {0};
			bool locked {true};
			bool auto_link {false};
			bool auto_rotate {false};

			float speed {0};
			MoverLerpType lerp_mode {MoverLerpType::CURVE};
			MoverSpeedType speed_mode {MoverSpeedType::CONSTANT};

			std::vector<AnimationSample> keyframes {};

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
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see trigger::parse
			ZKREM("use ::load()") ZKAPI static void parse(Mover& obj, ReadArchive& ctx, GameVersion version);
			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		/// \brief A VOb which can call multiple script function upon being triggered.
		struct TriggerList : public Trigger {
			struct Target {
				std::string name {};
				float delay {};

				[[nodiscard]] inline bool operator==(Target const& tgt) const noexcept {
					return this->name == tgt.name && this->delay == tgt.delay;
				}
			};

			using target ZKREM("renamed to Target") = Target;

			TriggerBatchMode mode {};
			std::vector<Target> targets {};

			// Save-game only variables
			uint8_t s_act_target {0};
			bool s_send_on_trigger {false};

			/// \brief Parses a trigger list VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see trigger::parse
			ZKREM("use ::load()") ZKAPI static void parse(TriggerList& obj, ReadArchive& ctx, GameVersion version);
			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		/// \brief A VOb which calls a script function upon being triggered.
		struct TriggerScript : public Trigger {
			std::string function {};

			/// \brief Parses a script trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see trigger::parse
			ZKREM("use ::load()") ZKAPI static void parse(TriggerScript& obj, ReadArchive& ctx, GameVersion version);
			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		/// \brief A VOb which triggers a level change if the player moves close to it.
		struct TriggerChangeLevel : public Trigger {
			std::string level_name {};
			std::string start_vob {};

			/// \brief Parses a change level trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see trigger::parse
			ZKREM("use ::load()")
			ZKAPI static void parse(TriggerChangeLevel& obj, ReadArchive& ctx, GameVersion version);
			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		/// \brief A VOb which triggers a world start event.
		struct TriggerWorldStart : public VirtualObject {
			std::string target;
			bool fire_once;

			// Save-game only variables
			bool s_has_fired {false};

			/// \brief Parses a world load trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()")
			ZKAPI static void parse(TriggerWorldStart& obj, ReadArchive& ctx, GameVersion version);
			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		struct TriggerUntouch : public VirtualObject {
			std::string target;

			/// \brief Parses an untouch trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()") ZKAPI static void parse(TriggerUntouch& obj, ReadArchive& ctx, GameVersion version);
			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};
	} // namespace vobs
} // namespace zenkit
