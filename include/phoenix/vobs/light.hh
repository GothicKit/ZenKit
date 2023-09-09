// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/vobs/Light.hh"

namespace phoenix {
	namespace vobs {
		using light_preset ZKREM("renamed to zenkit::vobs::LightPreset") = zenkit::vobs::LightPreset;
		using light ZKREM("renamed to zenkit::vobs::Light") = zenkit::vobs::Light;
	} // namespace vobs

	using light_mode ZKREM("renamed to zenkit::LightType") = zenkit::LightType;
	using light_quality ZKREM("renamed to zenkit::LightQuality") = zenkit::LightQuality;
} // namespace phoenix
