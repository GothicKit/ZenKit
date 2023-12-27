// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/vobs/Sound.hh"
#include "zenkit/Archive.hh"

namespace zenkit {
	void VSound::parse(VSound& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VSound::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->volume = r.read_float();                                          // sndVolume
		this->mode = static_cast<SoundMode>(r.read_enum());                     // sndMode
		this->random_delay = r.read_float();                                    // sndRandDelay
		this->random_delay_var = r.read_float();                                // sndRandDelayVar
		this->initially_playing = r.read_bool();                                // sndStartOn
		this->ambient3d = r.read_bool();                                        // sndAmbient3D
		this->obstruction = r.read_bool();                                      // sndObstruction
		this->cone_angle = r.read_float();                                      // sndConeAngle
		this->volume_type = static_cast<SoundTriggerVolumeType>(r.read_enum()); // sndVolType
		this->radius = r.read_float();                                          // sndRadius
		this->sound_name = r.read_string();                                     // sndName

		if (r.is_save_game()) {
			// In save-games, sounds contain extra variables
			this->s_is_running = r.read_bool();        // soundIsRunning
			this->s_is_allowed_to_run = r.read_bool(); // soundAllowedToRun
		}
	}

	void VSound::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_float("sndVolume", this->volume);
		w.write_enum("sndVolume", static_cast<std::uint32_t>(this->mode));
		w.write_float("sndVolume", this->random_delay);
		w.write_float("sndVolume", this->random_delay_var);
		w.write_bool("sndVolume", this->initially_playing);
		w.write_bool("sndVolume", this->ambient3d);
		w.write_bool("sndVolume", this->obstruction);
		w.write_float("sndVolume", this->cone_angle);
		w.write_enum("sndVolume", static_cast<std::uint32_t>(this->volume_type));
		w.write_float("sndVolume", this->radius);
		w.write_string("sndName", this->sound_name);

		if (w.is_save_game()) {
			// In save-games, sounds contain extra variables
			w.write_bool("soundIsRunning", this->s_is_running);
			w.write_bool("soundAllowedToRun", this->s_is_allowed_to_run);
		}
	}

	uint16_t VSound::get_version_identifier(GameVersion) const {
		return 12289;
	}

	void VSoundDaytime::parse(VSoundDaytime& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VSoundDaytime::load(ReadArchive& r, GameVersion version) {
		VSound::load(r, version);
		this->start_time = r.read_float();   // sndStartTime
		this->end_time = r.read_float();     // sndEndTime
		this->sound_name2 = r.read_string(); // sndName2
	}

	void VSoundDaytime::save(WriteArchive& w, GameVersion version) const {
		VSound::save(w, version);
		w.write_float("sndStartTime", this->start_time);
		w.write_float("sndEndTime", this->end_time);
		w.write_string("sndName2", this->sound_name2);
	}

} // namespace zenkit
