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

	void VSoundDaytime::parse(VSoundDaytime& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VSoundDaytime::load(ReadArchive& r, GameVersion version) {
		VSound::load(r, version);
		this->start_time = r.read_float();   // sndStartTime
		this->end_time = r.read_float();     // sndEndTime
		this->sound_name2 = r.read_string(); // sndName2
	}
} // namespace zenkit
