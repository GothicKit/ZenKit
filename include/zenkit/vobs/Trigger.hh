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

	/// \brief VObject to process and relay specific filtered events.
	///
	/// When a `zCTrigger` receives an `OnTrigger`, `OnDamage`, or `OnTouch` event it checks whether the event
	/// source (an VNpc, the player or another VObject) should be acknowledged (see #respond_to_npc, #respond_to_pc
	/// and #respond_to_object respectively). It then verifies if it should react to the specific event by checking
	/// the #react_to_on_trigger, #react_to_on_touch and #react_to_on_damage properties. If both checks succeed, an
	/// `OnTrigger` event is sent to the #target. `zCTrigger` objects can be enabled and disabled by firing `OnEnable`,
	/// `OnDisable` and `OnToggleEnabled` events at them.
	///
	/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/
	struct VTrigger : VirtualObject {
		ZK_OBJECT(ObjectType::zCTrigger);

	public:
		/// \brief The name of VObject to send `OnTrigger` and `OnUntrigger` events to after processing.
		/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/#triggerTarget
		std::string target;

		std::uint8_t ZKREM("replaced by `start_enabled` and `send_untrigger`") flags;

		/// \brief Determines whether the trigger is initially enabled.
		///
		/// Enabled triggers will process incoming events and send outgoing events while disabled triggers do not.
		/// Triggers can be activated and deactivated at runtime by sending them `OnEnable`, `OnDisable` or
		/// `OnToggleEnabled` events.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/#startEnabled
		bool start_enabled;

		/// \brief Whether to send and `OnUntrigger` event to the #target after the trigger receives an
		///       `OnUntrigger` or `OnUntouch` event.
		///
		/// Only fires the `OnUntrigger` event if #react_to_on_trigger and #react_to_on_touch are set
		/// to `TRUE` respectively.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/#sendUntrigger
		bool send_untrigger;

		std::uint8_t ZKREM("replaced by `react_to_*` and `respond_to_*`") filter_flags;

		/// \brief Whether this trigger should react to `OnTrigger` events.
		/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/#reactToOnTrigger
		bool react_to_on_trigger;

		/// \brief Whether this trigger should react to `OnTouch` events.
		/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/#reactToOnTouch
		bool react_to_on_touch;

		/// \brief Whether this trigger should react to `OnDamage` events.
		/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/#reactToOnDamage
		bool react_to_on_damage;

		/// \brief Whether this trigger should process events coming from inanimate objects.
		/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/#respondToObject
		bool respond_to_object;

		/// \brief Whether this trigger should process events coming from the player.
		/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/#respondToPC
		bool respond_to_pc;

		/// \brief Whether this trigger should process events coming from NPCs.
		/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/#respondToNPC
		bool respond_to_npc;

		/// \brief Whether this trigger should process events coming from VObjects with this name.
		/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/#respondToVobName
		std::string vob_target;

		/// \brief The number of times the trigger will process incoming events.
		///
		/// If set to `-1` the trigger will process an infinite number of events.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/#numCanBeActivated
		std::int32_t max_activation_count;

		/// \brief The number of seconds that have to elapse after processing an event before the trigger will
		///        process additional events.
		///
		/// All events received by the trigger during that time are ignored.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/#retriggerWaitSec
		float retrigger_delay_sec;

		/// \brief The amount of damage which must be dealt for the trigger to react to an `OnDamage` event.
		/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/#damageThreshold
		float damage_threshold;

		/// \brief The number of seconds to wait before emitting the `OnTrigger` event after processing.
		/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/#fireDelaySec
		float fire_delay_sec;

		// Save-game only variables
		float s_next_time_triggerable {0};
		int s_count_can_be_activated {0};
		std::shared_ptr<Object> s_other_vob {nullptr};
		bool s_is_enabled {true};

		ZKREM("use ::load()") ZKAPI static void parse(VTrigger& obj, ReadArchive& ctx, GameVersion version);

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

	struct VCutsceneTrigger : VTrigger {
		ZK_OBJECT(ObjectType::oCCSTrigger);
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	/// \brief A VOb which can move upon player interaction.
	struct VMover : VTrigger {
		ZK_OBJECT(ObjectType::zCMover);

	public:
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
		ZKREM("use ::load()") ZKAPI static void parse(VMover& obj, ReadArchive& ctx, GameVersion version);
		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	/// \brief A VOb which can call multiple script function upon being triggered.
	struct VTriggerList : VTrigger {
		ZK_OBJECT(ObjectType::zCTriggerList);

	public:
		struct Target {
			std::string name {};
			float delay {};

			[[nodiscard]] bool operator==(Target const& tgt) const noexcept {
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
		ZKREM("use ::load()") ZKAPI static void parse(VTriggerList& obj, ReadArchive& ctx, GameVersion version);
		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	/// \brief A VOb which calls a script function upon being triggered.
	struct VTriggerScript : VTrigger {
		ZK_OBJECT(ObjectType::oCTriggerScript);

	public:
		std::string function {};

		/// \brief Parses a script trigger VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws ParserError if parsing fails.
		/// \see vob::parse
		/// \see trigger::parse
		ZKREM("use ::load()") ZKAPI static void parse(VTriggerScript& obj, ReadArchive& ctx, GameVersion version);
		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	/// \brief A VOb which triggers a level change if the player moves close to it.
	struct VTriggerChangeLevel : VTrigger {
		ZK_OBJECT(ObjectType::oCTriggerChangeLevel);

	public:
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
		ZKAPI static void parse(VTriggerChangeLevel& obj, ReadArchive& ctx, GameVersion version);
		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	/// \brief A VOb which triggers a world start event.
	struct VTriggerWorldStart : VirtualObject {
		ZK_OBJECT(ObjectType::zCTriggerWorldStart);

	public:
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
		ZKAPI static void parse(VTriggerWorldStart& obj, ReadArchive& ctx, GameVersion version);
		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	struct VTriggerUntouch : VirtualObject {
		ZK_OBJECT(ObjectType::zCTriggerUntouch);

	public:
		std::string target;

		/// \brief Parses an untouch trigger VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws ParserError if parsing fails.
		/// \see vob::parse
		ZKREM("use ::load()") ZKAPI static void parse(VTriggerUntouch& obj, ReadArchive& ctx, GameVersion version);
		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};
} // namespace zenkit
