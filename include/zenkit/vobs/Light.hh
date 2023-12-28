// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/Misc.hh"
#include "zenkit/vobs/VirtualObject.hh"

#include <glm/vec4.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace zenkit {
	class ReadArchive;

	/// \brief The type of a light source.
	enum class LightType : std::uint32_t {
		POINT = 0,     ///< The light source emits light in a sphere.
		SPOT = 1,      ///< The light source emits a cone-shaped light beam.
		RESERVED0 = 2, ///< Unused.
		RESERVED1 = 3, ///< Unused.

		// Deprecated entries.
		point ZKREM("renamed to LightType::POINT") = POINT,
		spot ZKREM("renamed to LightType::SPOT") = SPOT,
		reserved0 ZKREM("renamed to LightType::RESERVED0") = RESERVED0,
		reserved1 ZKREM("renamed to LightType::RESERVED1") = RESERVED1,
	};

	/// \brief The quality of a light source.
	enum class LightQuality : std::uint32_t {
		HIGH = 0,   ///< The light is a high quality light.
		MEDIUM = 1, ///< The light is a medium quality light.
		LOW = 2,    ///< The light is a low quality light, which should be the fastest option.

		// Deprecated entries.
		high ZKREM("renamed to LightQuality::HIGH") = HIGH,
		medium ZKREM("renamed to LightQuality::MEDIUM") = MEDIUM,
		low ZKREM("renamed to LightQuality::LOW") = LOW,
	};

	/// \brief A preset for light sources.
	struct LightPreset {
		/// \brief The name of the selected light template.
		///
		/// Templates can be found in `_work/data/Presets/LightPresets.zen`. This property is irrelevant
		/// for most use-cases.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#lightPresetInUse
		std::string preset {};

		/// \brief The type of the light source.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#lightType
		LightType light_type {LightType::SPOT};

		/// \brief The range in which this light source illuminates objects.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#range
		float range {};

		/// \brief The color of this light source in RGBA form.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#color
		glm::u8vec4 color {};

		/// \brief The angle of the beam cone for spotlights.
		///
		/// Only relevant if #light_type is LightType::SPOT, ignored otherwise.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#spotConeAngle
		float cone_angle {0};

		/// \brief Determines whether this is a static light or not.
		///
		/// Static lights are baked into the world when its lighting is compiled. This makes static lights faster
		/// at runtime, but it also means, that they cannot change in any way after the world is compiled.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#lightStatic
		bool is_static {false};

		/// \brief Determines the quality of the light source.
		///
		/// What exactly changes with these values is unknown.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#lightQuality
		LightQuality quality {LightQuality::MEDIUM};

		/// \brief The name of the lensflare effect associated with this light.
		///
		/// Empty if there is no associated lensflare effect.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#lensflareFX
		std::string lensflare_fx {};

		/// \brief Whether this dynamic light is turned on.
		/// \note This value is only valid for dynamic lights. Check #is_static before use.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#turnedOn
		bool on {false};

		/// \brief Frames for the light's range animation.
		///
		/// Contains a list of floats where each value is a percentage of the initial #range. A value of `1.0`
		/// corresponds to the initial #range of the light source, a value of `2.0` corresponds to double the
		/// initial #range and a value of `0.5` means half the initial #range. The range animation is further
		/// controlled by #range_animation_fps and #range_animation_smooth.
		///
		/// \note This value is only valid for dynamic lights. Check #is_static before use.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#rangeAniScale
		std::vector<float> range_animation_scale {};

		/// \brief The speed of the range animation in frames per second.
		/// \note This value is only valid for dynamic lights. Check #is_static before use.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#rangeAniFPS
		float range_animation_fps {0};

		/// \brief Whether the range animation should smoothly interpolate between keyframes.
		/// \note This value is only valid for dynamic lights. Check #is_static before use.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#rangeAniSmooth
		bool range_animation_smooth {true};

		/// \brief Frames for the light's color animation.
		///
		/// Contains a list of colors where each value acts as a keyframe for the animation. The color animation is
		/// further controlled by #color_animation_fps and #color_animation_smooth.
		///
		/// \note This value is only valid for dynamic lights. Check #is_static before use.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#colorAniList
		std::vector<glm::u8vec4> color_animation_list {};

		/// \brief The speed of the color animation in frames per second.
		/// \note This value is only valid for dynamic lights. Check #is_static before use.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#colorAniFPS
		float color_animation_fps {0};

		/// \brief Whether the color animation should smoothly interpolate between keyframes.
		/// \note This value is only valid for dynamic lights. Check #is_static before use.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#colorAniSmooth
		bool color_animation_smooth {true};

		/// \brief Unknown.
		/// \note This value is only valid for dynamic lights. Check #is_static before use.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/#canMove
		bool can_move {true};

		ZKREM("use ::load()") ZKAPI static void parse(LightPreset& obj, ReadArchive& ctx, GameVersion version);
		ZKREM("use ::load()") ZKAPI static LightPreset parse(ReadArchive& in, GameVersion version);

		ZKAPI void load(ReadArchive& r, GameVersion version);
		ZKAPI void save(WriteArchive& w, GameVersion version) const;
	};

	/// \brief Represents a virtual light source which lights up nearby game objects.
	///
	/// Point and spot light sources are supported and may be animated with different colors and ranges.
	///
	/// \see https://zk.gothickit.dev/engine/objects/zCVobLight/
	struct VLight : VirtualObject, LightPreset {
		ZK_OBJECT(ObjectType::zCVobLight);

	public:
		ZKREM("use ::load()") ZKAPI static void parse(VLight& obj, ReadArchive& ctx, GameVersion version);

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};
} // namespace zenkit
