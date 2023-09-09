// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/vobs/MovableObject.hh"

namespace phoenix {
	namespace vobs {
		using mob ZKREM("renamed to zenkit::vobs::MovableObject") = zenkit::vobs::MovableObject;
		using mob_inter ZKREM("renamed to zenkit::vobs::InteractiveObject") = zenkit::vobs::InteractiveObject;
		using mob_fire ZKREM("renamed to zenkit::vobs::Fire") = zenkit::vobs::Fire;
		using mob_container ZKREM("renamed to zenkit::vobs::Container") = zenkit::vobs::Container;
		using mob_door ZKREM("renamed to zenkit::vobs::Door") = zenkit::vobs::Door;
	} // namespace vobs

	using sound_material ZKREM("renamed to zenkit::SoundMaterialType") = zenkit::SoundMaterialType;
} // namespace phoenix
