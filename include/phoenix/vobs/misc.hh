// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/vobs/Misc.hh"

namespace phoenix {
	namespace vobs {
		using animate ZKREM("renamed to zenkit::VAnimate") = zenkit::VAnimate;
		using item ZKREM("renamed to zenkit::VItem") = zenkit::VItem;
		using lens_flare ZKREM("renamed to zenkit::VLensFlare") = zenkit::VLensFlare;
		using pfx_controller ZKREM("renamed to zenkit::VParticleEffectController") = zenkit::VParticleEffectController;
		using code_master ZKREM("renamed to zenkit::VCodeMaster") = zenkit::VCodeMaster;
		using message_filter ZKREM("renamed to zenkit::VMessageFilter") = zenkit::VMessageFilter;
		using mover_controller ZKREM("renamed to zenkit::VMoverController") = zenkit::VMoverController;
		using touch_damage ZKREM("renamed to zenkit::VTouchDamage") = zenkit::VTouchDamage;
		using earthquake ZKREM("renamed to zenkit::VEarthquake") = zenkit::VEarthquake;
		using npc ZKREM("renamed to zenkit::VNpc") = zenkit::VNpc;
	} // namespace vobs

	using message_filter_action ZKREM("renamed to zenkit::MessageFilterAction") = zenkit::MessageFilterAction;
	using mover_message_type ZKREM("renamed to zenkit::MoverMessageType") = zenkit::MoverMessageType;
	using collision_type ZKREM("renamed to zenkit::TouchCollisionType") = zenkit::TouchCollisionType;
} // namespace phoenix
