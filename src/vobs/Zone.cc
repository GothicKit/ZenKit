// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/vobs/Zone.hh"
#include "zenkit/Archive.hh"

namespace zenkit {
	void VZoneMusic::parse(VZoneMusic& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VZoneMusic::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->enabled = r.read_bool();   // enabled
		this->priority = r.read_int();   // priority
		this->ellipsoid = r.read_bool(); // ellipsoid
		this->reverb = r.read_float();   // reverbLevel
		this->volume = r.read_float();   // volumeLevel
		this->loop = r.read_bool();      // loop

		if (r.is_save_game()) {
			// In save-games, zones contain extra variables
			this->s_local_enabled = r.read_bool();       // local_enabled
			this->s_day_entrance_done = r.read_bool();   // dayEntranceDone
			this->s_night_entrance_done = r.read_bool(); // nightEntranceDone
		}
	}

	void VZoneMusic::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_bool("enabled", this->enabled);
		w.write_int("priority", this->priority);
		w.write_bool("ellipsoid", this->ellipsoid);
		w.write_float("reverbLevel", this->reverb);
		w.write_float("volumeLevel", this->volume);
		w.write_bool("loop", this->loop);

		if (w.is_save_game()) {
			// In save-games, zones contain extra variables
			w.write_bool("local_enabled", this->s_local_enabled);
			w.write_bool("dayEntranceDone", this->s_day_entrance_done);
			w.write_bool("nightEntranceDone", this->s_night_entrance_done);
		}
	}

	uint16_t VZoneMusic::get_version_identifier(GameVersion) const {
		return 0;
	}

	void VZoneFarPlane::parse(VZoneFarPlane& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VZoneFarPlane::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->vob_far_plane_z = r.read_float();        // vobFarPlaneZ
		this->inner_range_percentage = r.read_float(); // innerRangePerc
	}

	void VZoneFarPlane::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_float("vobFarPlaneZ", this->vob_far_plane_z);
		w.write_float("innerRangePerc", this->inner_range_percentage);
	}

	void VZoneFog::parse(VZoneFog& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VZoneFog::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->range_center = r.read_float();           // fogRangeCenter
		this->inner_range_percentage = r.read_float(); // innerRangePerc
		this->color = r.read_color();                  // fogColor

		if (version == GameVersion::GOTHIC_2) {
			this->fade_out_sky = r.read_bool();   // fadeOutSky
			this->override_color = r.read_bool(); // overrideColor
		}
	}

	void VZoneFog::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_float("fogRangeCenter", this->range_center);
		w.write_float("innerRangePerc", this->inner_range_percentage);
		w.write_color("fogColor", this->color);

		if (version == GameVersion::GOTHIC_2) {
			w.write_bool("fadeOutSky", this->fade_out_sky);
			w.write_bool("overrideColor", this->override_color);
		}
	}

	uint16_t VZoneFog::get_version_identifier(GameVersion game) const {
		return game == GameVersion::GOTHIC_1 ? 64704 : 5505;
	}
} // namespace zenkit
