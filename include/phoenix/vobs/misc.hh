// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/vobs/Misc.hh"

namespace phoenix {
	namespace vobs {
		using animate ZKREM("renamed to zenkit::vobs::Animate") = zenkit::vobs::Animate;
		using item ZKREM("renamed to zenkit::vobs::Item") = zenkit::vobs::Item;
		using lens_flare ZKREM("renamed to zenkit::vobs::LensFlare") = zenkit::vobs::LensFlare;
		using pfx_controller
		    ZKREM("renamed to zenkit::vobs::ParticleEffectController") = zenkit::vobs::ParticleEffectController;
		using code_master ZKREM("renamed to zenkit::vobs::CodeMaster") = zenkit::vobs::CodeMaster;
		using message_filter ZKREM("renamed to zenkit::vobs::MessageFilter") = zenkit::vobs::MessageFilter;
		using mover_controller ZKREM("renamed to zenkit::vobs::MoverController") = zenkit::vobs::MoverController;
		using touch_damage ZKREM("renamed to zenkit::vobs::TouchDamage") = zenkit::vobs::TouchDamage;
		using earthquake ZKREM("renamed to zenkit::vobs::Earthquake") = zenkit::vobs::Earthquake;
		using npc ZKREM("renamed to zenkit::vobs::Npc") = zenkit::vobs::Npc;
	} // namespace vobs

	using message_filter_action ZKREM("renamed to zenkit::MessageFilterAction") = zenkit::MessageFilterAction;
	using mover_message_type ZKREM("renamed to zenkit::MoverMessageType") = zenkit::MoverMessageType;
	using collision_type ZKREM("renamed to zenkit::TouchCollisionType") = zenkit::TouchCollisionType;
} // namespace phoenix
