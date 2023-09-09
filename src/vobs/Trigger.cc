// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/vobs/Trigger.hh"
#include "zenkit/Archive.hh"

namespace zenkit::vobs {
	void Trigger::parse(Trigger& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void Trigger::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		VirtualObject::load(r, version);
		this->target = r.read_string();                   // triggerTarget
		this->flags = r.read_raw(1)->read_ubyte();        // flags
		this->filter_flags = r.read_raw(1)->read_ubyte(); // filterFlags
		this->vob_target = r.read_string();               // respondToVobName
		this->max_activation_count = r.read_int();        // numCanBeActivated
		this->retrigger_delay_sec = r.read_float();       // retriggerWaitSec
		this->damage_threshold = r.read_float();          // damageThreshold
		this->fire_delay_sec = r.read_float();            // fireDelaySec

		this->s_count_can_be_activated = this->max_activation_count;

		if (r.is_save_game()) {
			// In save-games, triggers contain extra variables
			this->s_next_time_triggerable = r.read_float(); // nextTimeTriggerable
			r.skip_object(false);                           // [savedOtherVob % 0 0]
			this->s_count_can_be_activated = r.read_int();  // countCanBeActivated

			if (version == GameVersion::GOTHIC_2) {
				this->s_is_enabled = r.read_bool(); // isEnabled
			}
		}
	}

	void Mover::parse(Mover& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void Mover::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		Trigger::load(r, version);
		this->behavior = static_cast<MoverBehavior>(r.read_enum()); // moverBehavior
		this->touch_blocker_damage = r.read_float();                // touchBlockerDamage
		this->stay_open_time_sec = r.read_float();                  // stayOpenTimeSec
		this->locked = r.read_bool();                               // moverLocked
		this->auto_link = r.read_bool();                            // autoLinkEnabled

		if (version == GameVersion::GOTHIC_2) {
			this->auto_rotate = r.read_bool(); // autoRotate
		}

		auto keyframe_count = r.read_word(); // numKeyframes
		if (keyframe_count > 0) {
			this->speed = r.read_float();                                  // moveSpeed
			this->lerp_mode = static_cast<MoverLerpType>(r.read_enum());   // posLerpType
			this->speed_mode = static_cast<MoverSpeedType>(r.read_enum()); // speedType

			auto sample_reader = r.read_raw(keyframe_count * sizeof(float) * 7); // keyframes

			for (int32_t i = 0; i < keyframe_count; ++i) {
				auto pos = sample_reader->read_vec3();

				auto x = sample_reader->read_float();
				auto y = sample_reader->read_float();
				auto z = sample_reader->read_float();
				auto w = sample_reader->read_float();

				this->keyframes.push_back(AnimationSample {pos, glm::quat {w, x, y, z}});
			}
		}

		if (r.is_save_game()) {
			// In save-games, movers contain extra variables
			this->s_act_key_pos_delta = r.read_vec3();    // actKeyPosDelta
			this->s_act_keyframe_f = r.read_float();      // actKeyframeF
			this->s_act_keyframe = r.read_int();          // actKeyframe
			this->s_next_keyframe = r.read_int();         // nextKeyframe
			this->s_move_speed_unit = r.read_float();     // moveSpeedUnit
			this->s_advance_dir = r.read_float();         // advanceDir
			this->s_mover_state = r.read_enum();          // moverState
			this->s_trigger_event_count = r.read_int();   // numTriggerEvents
			this->s_stay_open_time_dest = r.read_float(); // stayOpenTimeDest
		}

		this->sfx_open_start = r.read_string();    // sfxOpenStart
		this->sfx_open_end = r.read_string();      // sfxOpenEnd
		this->sfx_transitioning = r.read_string(); // sfxMoving
		this->sfx_close_start = r.read_string();   // sfxCloseStart
		this->sfx_close_end = r.read_string();     // sfxCloseEnd
		this->sfx_lock = r.read_string();          // sfxLock
		this->sfx_unlock = r.read_string();        // sfxUnlock
		this->sfx_use_locked = r.read_string();    // sfxUseLocked
	}

	void TriggerList::parse(TriggerList& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void TriggerList::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		Trigger::load(r, version);
		this->mode = static_cast<TriggerBatchMode>(r.read_enum()); // listProcess

		auto target_count = r.read_byte(); // numTarget
		for (int32_t i = 0; i < target_count; ++i) {
			this->targets.emplace_back(Target {
			    r.read_string(), // triggerTarget[i]
			    r.read_float()   // fireDelay[i]
			});
		}

		if (r.is_save_game()) {
			// In save-games, trigger lists contain extra variables
			this->s_act_target = r.read_byte();      // actTarget
			this->s_send_on_trigger = r.read_bool(); // sendOnTrigger
		}
	}

	void TriggerScript::parse(TriggerScript& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void TriggerScript::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		Trigger::load(r, version);
		this->function = r.read_string(); // scriptFunc
	}

	void TriggerChangeLevel::parse(TriggerChangeLevel& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void TriggerChangeLevel::load(zenkit::ReadArchive& r, GameVersion version) {
		Trigger::load(r, version);
		this->level_name = r.read_string(); // levelName
		this->start_vob = r.read_string();  // startVobName
	}

	void TriggerWorldStart::parse(TriggerWorldStart& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void TriggerWorldStart::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->target = r.read_string();  // triggerTarget
		this->fire_once = r.read_bool(); // fireOnlyFirstTime

		if (r.is_save_game() && version == GameVersion::GOTHIC_2) {
			// In Gothic 2 save-games, world start triggers contain extra variables
			this->s_has_fired = r.read_bool(); // hasFired
		}
	}

	void TriggerUntouch::parse(TriggerUntouch& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void TriggerUntouch::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		VirtualObject::load(r, version);
		this->target = r.read_string(); // triggerTarget
	}
} // namespace zenkit::vobs
