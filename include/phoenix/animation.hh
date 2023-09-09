// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/ModelAnimation.hh"

namespace phoenix {
	using animation ZKREM("renamed to zenkit::Animation") = zenkit::ModelAnimation;
	using animation_sample ZKREM("renamed to zenkit::AnimationSample") = zenkit::AnimationSample;
	using animation_event ZKREM("renamed to zenkit::AnimationEvent") = zenkit::AnimationEvent;
	using animation_event_type ZKREM("renamed to zenkit::AnimationEventType") = zenkit::AnimationEventType;
} // namespace phoenix
