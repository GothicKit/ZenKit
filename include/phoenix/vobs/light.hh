// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/vobs/Light.hh"

namespace phoenix {
	namespace vobs {
		using light_preset ZKREM("renamed to zenkit::VLightPreset") = zenkit::VLightPreset;
		using light ZKREM("renamed to zenkit::VLight") = zenkit::VLight;
	} // namespace vobs

	using light_mode ZKREM("renamed to zenkit::LightType") = zenkit::LightType;
	using light_quality ZKREM("renamed to zenkit::LightQuality") = zenkit::LightQuality;
} // namespace phoenix
