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

	void VZoneFarPlane::parse(VZoneFarPlane& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VZoneFarPlane::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->vob_far_plane_z = r.read_float();        // vobFarPlaneZ
		this->inner_range_percentage = r.read_float(); // innerRangePerc
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
} // namespace zenkit
