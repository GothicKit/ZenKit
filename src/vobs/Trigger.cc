// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/vobs/Trigger.hh"
#include "zenkit/Archive.hh"

namespace zenkit {
	void VTrigger::parse(VTrigger& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VTrigger::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->target = r.read_string();                   // triggerTarget
		this->flags = r.read_raw(1)->read_ubyte();        // flags
		this->filter_flags = r.read_raw(1)->read_ubyte(); // filterFlags
		this->vob_target = r.read_string();               // respondToVobName
		this->max_activation_count = r.read_int();        // numCanBeActivated
		this->retrigger_delay_sec = r.read_float();       // retriggerWaitSec
		this->damage_threshold = r.read_float();          // damageThreshold
		this->fire_delay_sec = r.read_float();            // fireDelaySec

		this->start_enabled = this->flags & 1;                    // startEnabled
		this->send_untrigger = (this->flags & 4) >> 2;            // sendUntrigger
		this->react_to_on_trigger = this->filter_flags & 1;       // reactToOnTrigger
		this->react_to_on_touch = (this->filter_flags & 2) >> 1;  // reactToOnTouch
		this->react_to_on_damage = (this->filter_flags & 4) >> 2; // reactToOnDamage
		this->respond_to_object = (this->filter_flags & 8) >> 3;  // respondToObject
		this->respond_to_pc = (this->filter_flags & 16) >> 4;     // respondToPC
		this->respond_to_npc = (this->filter_flags & 32) >> 5;    // respondToNPC

		this->s_count_can_be_activated = this->max_activation_count;

		if (r.is_save_game()) {
			// In save-games, triggers contain extra variables
			this->s_next_time_triggerable = r.read_float(); // nextTimeTriggerable
			this->s_other_vob = r.read_object(version);     // [savedOtherVob % 0 0]
			this->s_count_can_be_activated = r.read_int();  // countCanBeActivated

			if (version == GameVersion::GOTHIC_2) {
				this->s_is_enabled = r.read_bool(); // isEnabled
			}
		}
	}

	void VTrigger::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_string("triggerTarget", this->target);
		w.write_raw("flags", reinterpret_cast<std::byte const*>(&this->flags), 1);
		w.write_raw("filterFlags", reinterpret_cast<std::byte const*>(&this->filter_flags), 1);
		w.write_string("respondToVobName", this->vob_target);
		w.write_int("numCanBeActivated", this->max_activation_count);
		w.write_float("retriggerWaitSec", this->retrigger_delay_sec);
		w.write_float("damageThreshold", this->damage_threshold);
		w.write_float("fireDelaySec", this->fire_delay_sec);

		if (w.is_save_game()) {
			// In save-games, triggers contain extra variables
			w.write_float("nextTimeTriggerable", this->s_next_time_triggerable);
			w.write_object("savedOtherVob", this->s_other_vob, version);
			w.write_int("countCanBeActivated", this->s_count_can_be_activated);

			if (version == GameVersion::GOTHIC_2) {
				w.write_bool("isEnabled", this->s_is_enabled);
			}
		}
	}

	uint16_t VTrigger::get_version_identifier(GameVersion) const {
		return 47105;
	}

	uint16_t VCutsceneTrigger::get_version_identifier(GameVersion) const {
		return 24577;
	}

	void VMover::parse(VMover& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VMover::load(ReadArchive& r, GameVersion version) {
		VTrigger::load(r, version);
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

	void VMover::save(WriteArchive& w, GameVersion version) const {
		VTrigger::save(w, version);
		w.write_enum("moverBehavior", static_cast<std::uint32_t>(this->behavior));
		w.write_float("autoLinkEnabled", this->touch_blocker_damage);
		w.write_float("autoLinkEnabled", this->stay_open_time_sec);
		w.write_bool("autoLinkEnabled", this->locked);
		w.write_bool("autoLinkEnabled", this->auto_link);

		if (version == GameVersion::GOTHIC_2) {
			w.write_bool("autoRotate", this->auto_rotate);
		}

		w.write_word("numKeyframes", this->keyframes.size());

		if (!this->keyframes.empty()) {
			w.write_float("moveSpeed", this->speed);
			w.write_enum("posLerpType", static_cast<std::uint32_t>(this->lerp_mode));
			w.write_enum("speedType", static_cast<std::uint32_t>(this->speed_mode));

			std::vector<std::byte> samples;
			auto sample_writer = Write::to(&samples);

			for (auto& sample : this->keyframes) {
				sample_writer->write_vec3(sample.position);
				sample_writer->write_float(sample.rotation.x);
				sample_writer->write_float(sample.rotation.y);
				sample_writer->write_float(sample.rotation.z);
				sample_writer->write_float(sample.rotation.w);
			}

			w.write_raw("keyframes", samples);
		}

		if (w.is_save_game()) {
			// In save-games, movers contain extra variables
			w.write_vec3("actKeyPosDelta", this->s_act_key_pos_delta);
			w.write_float("actKeyframeF", this->s_act_keyframe_f);
			w.write_int("actKeyframe", this->s_act_keyframe);
			w.write_int("nextKeyframe", this->s_next_keyframe);
			w.write_float("moveSpeedUnit", this->s_move_speed_unit);
			w.write_float("advanceDir", this->s_advance_dir);
			w.write_enum("moverState", this->s_mover_state);
			w.write_int("numTriggerEvents", this->s_trigger_event_count);
			w.write_float("stayOpenTimeDest", this->s_stay_open_time_dest);
		}

		w.write_string("sfxOpenStart", this->sfx_open_start);
		w.write_string("sfxOpenEnd", this->sfx_open_end);
		w.write_string("sfxMoving", this->sfx_transitioning);
		w.write_string("sfxCloseStart", this->sfx_close_start);
		w.write_string("sfxCloseEnd", this->sfx_close_end);
		w.write_string("sfxLock", this->sfx_lock);
		w.write_string("sfxUnlock", this->sfx_unlock);
		w.write_string("sfxUseLocked", this->sfx_use_locked);
	}

	uint16_t VMover::get_version_identifier(GameVersion) const {
		return 39936;
	}

	void VTriggerList::parse(VTriggerList& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VTriggerList::load(ReadArchive& r, GameVersion version) {
		VTrigger::load(r, version);
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

	void VTriggerList::save(WriteArchive& w, GameVersion version) const {
		VTrigger::save(w, version);
		w.write_enum("listProcess", static_cast<std::uint32_t>(this->mode));

		w.write_byte("numTargets", this->targets.size());
		for (auto i = 0u; i < this->targets.size(); ++i) {
			w.write_string("triggerTarget" + std::to_string(i), targets[i].name);
			w.write_float("fireDelay" + std::to_string(i), targets[i].delay);
		}

		if (w.is_save_game()) {
			// In save-games, trigger lists contain extra variables
			w.write_byte("actTarget", this->s_act_target);
			w.write_bool("sendOnTrigger", this->s_send_on_trigger);
		}
	}

	uint16_t VTriggerList::get_version_identifier(GameVersion game) const {
		return game == GameVersion::GOTHIC_1 ? 59776 : 5505;
	}

	void VTriggerScript::parse(VTriggerScript& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VTriggerScript::load(ReadArchive& r, GameVersion version) {
		VTrigger::load(r, version);
		this->function = r.read_string(); // scriptFunc
	}

	void VTriggerScript::save(WriteArchive& w, GameVersion version) const {
		VTrigger::save(w, version);
		w.write_string("scriptFunc", this->function);
	}

	uint16_t VTriggerScript::get_version_identifier(GameVersion) const {
		return 24577;
	}

	void VTriggerChangeLevel::parse(VTriggerChangeLevel& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VTriggerChangeLevel::load(ReadArchive& r, GameVersion version) {
		VTrigger::load(r, version);
		this->level_name = r.read_string(); // levelName
		this->start_vob = r.read_string();  // startVobName
	}

	void VTriggerChangeLevel::save(WriteArchive& w, GameVersion version) const {
		VTrigger::save(w, version);
		w.write_string("levelName", this->level_name);
		w.write_string("startVobName", this->start_vob);
	}

	uint16_t VTriggerChangeLevel::get_version_identifier(GameVersion) const {
		return 24577;
	}

	void VTriggerWorldStart::parse(VTriggerWorldStart& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VTriggerWorldStart::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->target = r.read_string();  // triggerTarget
		this->fire_once = r.read_bool(); // fireOnlyFirstTime

		if (r.is_save_game() && version == GameVersion::GOTHIC_2) {
			// In Gothic 2 save-games, world start triggers contain extra variables
			this->s_has_fired = r.read_bool(); // hasFired
		}
	}

	void VTriggerWorldStart::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_string("triggerTarget", this->target);
		w.write_bool("fireOnlyFirstTime", this->fire_once);

		if (w.is_save_game() && version == GameVersion::GOTHIC_2) {
			// In Gothic 2 save-games, world start triggers contain extra variables
			w.write_bool("hasFired", this->s_has_fired);
		}
	}

	uint16_t VTriggerWorldStart::get_version_identifier(GameVersion) const {
		return 52224;
	}

	void VTriggerUntouch::parse(VTriggerUntouch& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VTriggerUntouch::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->target = r.read_string(); // triggerTarget
	}

	void VTriggerUntouch::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_string("triggerTarget", this->target);
	}

	uint16_t VTriggerUntouch::get_version_identifier(GameVersion) const {
		return 193;
	}
} // namespace zenkit
