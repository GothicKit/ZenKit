// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/vobs/Trigger.hh"

#include "phoenix/animation.hh"

namespace phoenix {
	namespace vobs {
		using trigger ZKREM("renamed to zenkit::vobs::Trigger") = zenkit::vobs::Trigger;
		using trigger_mover ZKREM("renamed to zenkit::vobs::Mover") = zenkit::vobs::Mover;
		using trigger_list ZKREM("renamed to zenkit::vobs::TriggerList") = zenkit::vobs::TriggerList;
		using trigger_script ZKREM("renamed to zenkit::vobs::TriggerScript") = zenkit::vobs::TriggerScript;
		using trigger_change_level
		    ZKREM("renamed to zenkit::vobs::TriggerChangeLevel") = zenkit::vobs::TriggerChangeLevel;
		using trigger_world_start ZKREM("renamed to zenkit::vobs::TriggerWorldStart") = zenkit::vobs::TriggerWorldStart;
		using trigger_untouch ZKREM("renamed to zenkit::vobs::TriggerUntouch") = zenkit::vobs::TriggerUntouch;
	} // namespace vobs

	using mover_behavior ZKREM("renamed to zenkit::MoverBehavior") = zenkit::MoverBehavior;
	using mover_lerp_mode ZKREM("renamed to zenkit::MoverLerpType") = zenkit::MoverLerpType;
	using mover_speed_mode ZKREM("renamed to zenkit::MoverSpeedType") = zenkit::MoverSpeedType;
	using trigger_batch_mode ZKREM("renamed to zenkit::TriggerBatchMode") = zenkit::TriggerBatchMode;
} // namespace phoenix
