// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/vobs/Trigger.hh"

#include "phoenix/animation.hh"

namespace phoenix {
	namespace vobs {
		using trigger ZKREM("renamed to zenkit::VTrigger") = zenkit::VTrigger;
		using trigger_mover ZKREM("renamed to zenkit::VMover") = zenkit::VMover;
		using trigger_list ZKREM("renamed to zenkit::VTriggerList") = zenkit::VTriggerList;
		using trigger_script ZKREM("renamed to zenkit::VTriggerScript") = zenkit::VTriggerScript;
		using trigger_change_level ZKREM("renamed to zenkit::VTriggerChangeLevel") = zenkit::VTriggerChangeLevel;
		using trigger_world_start ZKREM("renamed to zenkit::VTriggerWorldStart") = zenkit::VTriggerWorldStart;
		using trigger_untouch ZKREM("renamed to zenkit::VTriggerUntouch") = zenkit::VTriggerUntouch;
	} // namespace vobs

	using mover_behavior ZKREM("renamed to zenkit::MoverBehavior") = zenkit::MoverBehavior;
	using mover_lerp_mode ZKREM("renamed to zenkit::MoverLerpType") = zenkit::MoverLerpType;
	using mover_speed_mode ZKREM("renamed to zenkit::MoverSpeedType") = zenkit::MoverSpeedType;
	using trigger_batch_mode ZKREM("renamed to zenkit::TriggerBatchMode") = zenkit::TriggerBatchMode;
} // namespace phoenix
