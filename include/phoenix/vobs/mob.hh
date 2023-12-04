// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/vobs/MovableObject.hh"

namespace phoenix {
	namespace vobs {
		using mob ZKREM("renamed to zenkit::VMovableObject") = zenkit::VMovableObject;
		using mob_inter ZKREM("renamed to zenkit::VInteractiveObject") = zenkit::VInteractiveObject;
		using mob_fire ZKREM("renamed to zenkit::VFire") = zenkit::VFire;
		using mob_container ZKREM("renamed to zenkit::VContainer") = zenkit::VContainer;
		using mob_door ZKREM("renamed to zenkit::VDoor") = zenkit::VDoor;
	} // namespace vobs

	using sound_material ZKREM("renamed to zenkit::SoundMaterialType") = zenkit::SoundMaterialType;
} // namespace phoenix
