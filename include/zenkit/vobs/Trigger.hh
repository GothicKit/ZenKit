// Copyright © 2022-2024 GothicKit Contributors.
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

	/// \see https://zk.gothickit.dev/engine/objects/zCMover/#moverBehavior
	enum class MoverBehavior : uint32_t {
		/// \brief Receiving either event causes the state of the mover to swap. The direction of the animation is
		///        reversed.
		TOGGLE = 0,

		/// \brief An `OnTrigger` causes the mover to transition to the "open" state while an `OnUntrigger`
		//         event causes the mover to transition to the "closed" state.
		TRIGGER_CONTROL = 1,

		/// \brief An `OnTrigger` event causes the mover to transition to the "open" state. It than transitions
		///        to the "closed" state after a fixed amount of time.
		OPEN_TIME = 2,

		/// \brief The mover opens and closes in a loop indefinitely.
		LOOP = 3,

		/// \brief The mover can transition to each keyframe separately.
		///
		/// Often used in conjunction with a VMoverController`.
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

		/// \brief The name of VObject to send `OnTrigger` and `OnUntrigger` events to after processing.
		/// \see https://zk.gothickit.dev/engine/objects/zCTrigger/#triggerTarget
		std::string target;

		ZKREM("replaced by `start_enabled` and `send_untrigger`") std::uint8_t flags;

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

		ZKREM("replaced by `react_to_*` and `respond_to_*`") std::uint8_t filter_flags;

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
		std::shared_ptr<VirtualObject> s_other_vob {nullptr};
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

	/// \brief A special trigger VObject which can start a cutscene.
	///
	/// The VTrigger#target must be the name of the cutscene (without the extension) to be started. For this to work,
	/// the cutscene must already be loaded. `OnUntrigger` events are ignored by `oCCSTrigger`s.
	///
	/// \see https://zk.gothickit.dev/engine/objects/oCCSTrigger/
	struct VCutsceneTrigger : VTrigger {
		ZK_OBJECT(ObjectType::oCCSTrigger);

		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	/// \brief A VObject which can move along a pre-determined path in response to an event.
	///
	/// Movers can be controlled through a set of keyframes or the animation of their visuals. If the visual has an
	/// attached animation to be used as the mover's animation, it must have the following animations which are run
	/// depending on the mover's state: `S_OPEN`, `S_CLOSED`, `T_CLOSED_2_OPEN`, `T_OPEN_2_CLOSED`, `S_LOCKED`,
	/// `S_UNLOCKED`, `T_UNLOCKED_TO_LOCKED`, `T_LOCKED_TO_UNLOCKED`.
	///
	/// Each mover is in one of three states, "open", "closed" or "moving". Keyframe `0` (or `S_OPEN` when using the
	/// visual's animation) corresponds to the "open" state while the last keyframe (or `S_CLOSED`) corresponds to the
	/// "closed" state. When transitioning between the "open" and "closed" states, the mover is in the "moving" state
	/// during which it sequentially moves between its keyframes (or runs the `T_CLOSED_2_OPEN` or `T_OPEN_TO_CLOSED`
	/// animations). Movers with their #behavior set to MoverBehavior::SINGLE_KEYS are an exception to this rule:
	/// each keyframe can individually be addressed as a state.
	///
	/// Movers are specialized triggers. Before beginning their animation, movers first filter the incoming events as
	/// per the VTrigger's rules and only start the animation if the event passes through the filters. When the the
	/// mover reaches the "open" state after being activated, it emits an `OnTrigger` event according to the fire
	/// behavior set for the VTrigger. Similarly, when it reaches the "closed" state it emits an `OnUntrigger` event.
	///
	/// Movers can be enabled and disabled using the `OnEnable`, `OnDisable` and `OnToggleDisabled` events.
	///
	/// \see https://zk.gothickit.dev/engine/objects/zCMover/
	struct VMover : VTrigger {
		ZK_OBJECT(ObjectType::zCMover);

		/// \brief Controls how the mover behaves in response to events ("activation")
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#moverBehavior
		MoverBehavior behavior {MoverBehavior::TOGGLE};

		/// \brief The amount of damage to deal to objects in the way of the mover.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#touchBlockerDamage
		float touch_blocker_damage {0};

		/// \brief The number of seconds a mover with the MoverBehavior::OPEN_TIME #behavior stays in the "open"
		///        state until transitioning to the "closed" state again.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#stayOpenTimeSec
		float stay_open_time_sec {0};

		/// \brief Unclear.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#moverLocked
		bool locked {true};

		/// \brief Determines whether the position of the object triggering the mover should be tied to the mover's
		///        position.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#autoLinkEnabled
		bool auto_link {false};

		/// \brief Whether to automatically rotate the mover along its movement trajectory.
		/// \note Only available in Gothic II.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#autoRotate
		bool auto_rotate {false};

		/// \brief The movement speed for transitioning between keyframes in units per millisecond.
		/// \note Only relevant if manually specified keyframes are used. Ignored when using the visual for the
		///       animation.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#moveSpeed
		float speed {0};

		/// \brief Defines how the mover should interpolate between the keyframe positions.
		/// \note Only relevant if manually specified keyframes are used. Ignored when using the visual for the
		///       animation.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#posLerpType
		MoverLerpType lerp_mode {MoverLerpType::CURVE};

		/// \brief Controls the acceleration and deceleration behavior of the mover.
		/// \note Only relevant if manually specified keyframes are used. Ignored when using the visual for the
		///       animation.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#speedType
		MoverSpeedType speed_mode {MoverSpeedType::CONSTANT};

		std::vector<AnimationSample> keyframes {};

		/// \brief The name of the sound to play at the beginning of the opening sequence.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#sfxOpenStart
		std::string sfx_open_start {};

		/// \brief The name of the sound to play at the end of the opening sequence.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#sfxOpenEnd
		std::string sfx_open_end {};

		/// \brief The name of the sound to play in a loop while the mover is transitioning between keyframes.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#sfxMoving
		std::string sfx_transitioning {};

		/// \brief The name of the sound to play at the beginning of the closing sequence.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#sfxCloseStart
		std::string sfx_close_start {};

		/// \brief The name of the sound to play at the end of the closing sequence.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#sfxCloseEnd
		std::string sfx_close_end {};

		/// \brief The name of the sound to play when locking a mover.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#sfxLock
		std::string sfx_lock {};

		/// \brief The name of the sound to play when unlocking a mover.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#sfxUnlock
		std::string sfx_unlock {};

		/// \brief The name of the sound to play when using a locked mover.
		/// \see https://zk.gothickit.dev/engine/objects/zCMover/#sfxUseLocked
		std::string sfx_use_locked {};

		// Save-game only variables
		Vec3 s_act_key_pos_delta;
		float s_act_keyframe_f;
		int s_act_keyframe;
		int s_next_keyframe;
		float s_move_speed_unit;
		float s_advance_dir;
		uint32_t s_mover_state;
		int s_trigger_event_count;
		float s_stay_open_time_dest;

		ZKREM("use ::load()") ZKAPI static void parse(VMover& obj, ReadArchive& ctx, GameVersion version);

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

	/// \brief A special trigger which emits the `OnTrigger` events emitted by the VTrigger to multiple targets..
	/// \see https://zk.gothickit.dev/engine/objects/zCTriggerList/
	struct VTriggerList : VTrigger {
		ZK_OBJECT(ObjectType::zCTriggerList);

		struct Target {
			/// \brief The name of the VObject to send events to. Behaves like VTrigger#target`.
			/// \see https://zk.gothickit.dev/engine/objects/zCTriggerList/#triggerTarget
			std::string name {};

			/// \brief The delay after which to fire the event to the target. Behaves like VTrigger#fireDelaySec.
			/// \see https://zk.gothickit.dev/engine/objects/zCTriggerList/#fireDelay
			float delay {};

			[[nodiscard]] bool operator==(Target const& tgt) const noexcept {
				return this->name == tgt.name && this->delay == tgt.delay;
			}
		};

		using target ZKREM("renamed to Target") = Target;

		/// \brief Controls how events are sent to the targets.
		/// \see https://zk.gothickit.dev/engine/objects/zCTriggerList/#listProcess
		TriggerBatchMode mode {};

		std::vector<Target> targets {};

		// Save-game only variables
		uint8_t s_act_target {0};
		bool s_send_on_trigger {false};

		ZKREM("use ::load()") ZKAPI static void parse(VTriggerList& obj, ReadArchive& ctx, GameVersion version);

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

	/// \brief A special VTrigger which in addition to performing all other trigger actions also calls a script
	///        function when the trigger is about to emit an `OnTrigger` message to its target.
	/// \see https://zk.gothickit.dev/engine/objects/oCTriggerScript/
	struct VTriggerScript : VTrigger {
		ZK_OBJECT(ObjectType::oCTriggerScript);

		/// \brief The name script function to call when the trigger successfully processes a message.
		/// \see https://zk.gothickit.dev/engine/objects/oCTriggerScript/#scriptFunc
		std::string function {};

		ZKREM("use ::load()") ZKAPI static void parse(VTriggerScript& obj, ReadArchive& ctx, GameVersion version);

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

	/// \brief A special VTrigger which in addition to performing all other trigger actions also causes the
	//         engine to load another level when activated.
	/// \see https://zk.gothickit.dev/engine/objects/oCTriggerChangeLevel/
	struct VTriggerChangeLevel : VTrigger {
		ZK_OBJECT(ObjectType::oCTriggerChangeLevel);

		/// \brief The name of the level to load including the file extension.
		/// \see https://zk.gothickit.dev/engine/objects/oCTriggerChangeLevel/#levelName
		std::string level_name {};

		/// \brief The name of the VObject in the new level to place the player at.
		/// \see https://zk.gothickit.dev/engine/objects/oCTriggerChangeLevel/#startVobName
		std::string start_vob {};

		ZKREM("use ::load()")
		ZKAPI static void parse(VTriggerChangeLevel& obj, ReadArchive& ctx, GameVersion version);

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

	/// \brief A special trigger which fires an `OnTrigger` event to its #target when the world is loaded and started.
	/// \see https://zk.gothickit.dev/engine/objects/zCTriggerChangeLevel/
	struct VTriggerWorldStart : VirtualObject {
		ZK_OBJECT(ObjectType::zCTriggerWorldStart);

		/// \brief The name of VObject to send an `OnTrigger` event to when the world is loaded and started.
		/// \see https://zk.gothickit.dev/engine/objects/zCTriggerChangeLevel/#triggerTarget
		std::string target;

		/// \brief Determines whether to fire the `OnTrigger` event only the first time the world is loaded.
		/// \see https://zk.gothickit.dev/engine/objects/zCTriggerChangeLevel/#fireOnlyFirstTime
		bool fire_once;

		// Save-game only variables
		bool s_has_fired {false};

		ZKREM("use ::load()") ZKAPI static void parse(VTriggerWorldStart& obj, ReadArchive& ctx, GameVersion version);

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

	/// \brief A trigger which fires an `OnTrigger` event to its #target if any `OnUntouch` event is emitted from
	///        an inanimate object within the object's bounding box.
	/// \see https://zk.gothickit.dev/engine/objects/zCTriggerUntouch/
	struct VTriggerUntouch : VirtualObject {
		ZK_OBJECT(ObjectType::zCTriggerUntouch);

		/// \brief The name of VObject to send the `OnTrigger` events to.
		/// \see https://zk.gothickit.dev/engine/objects/zCTriggerUntouch/#triggerTarget
		std::string target;

		ZKREM("use ::load()") ZKAPI static void parse(VTriggerUntouch& obj, ReadArchive& ctx, GameVersion version);

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
