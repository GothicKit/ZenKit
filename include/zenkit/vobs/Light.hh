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
		POINT = 0,     ///< The light source emits a cone-shaped light beam.
		SPOT = 1,      ///< The light source emits light in a sphere.
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

	namespace vobs {
		/// \brief A preset for light sources.
		struct LightPreset {
			std::string preset {};
			LightType light_type {LightType::SPOT};
			float range {};
			glm::u8vec4 color {};
			float cone_angle {0};
			bool is_static {false};
			LightQuality quality {LightQuality::MEDIUM};
			std::string lensflare_fx {};

			bool on {false};
			std::vector<float> range_animation_scale {};
			float range_animation_fps {0};
			bool range_animation_smooth {true};
			std::vector<glm::u8vec4> color_animation_list {};
			float color_animation_fps {0};
			bool color_animation_smooth {true};
			bool can_move {true};

			/// \brief Parses a light preset the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			ZKREM("use ::load()") ZKAPI static void parse(LightPreset& obj, ReadArchive& ctx, GameVersion version);

			/// \brief Parses a light preset the given *ZenGin* archive.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \return The parsed light preset.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()") ZKAPI static LightPreset parse(ReadArchive& in, GameVersion version);

			ZKAPI void load(ReadArchive& r, GameVersion version);
		};

		/// \brief A VOb which acts as a light source.
		struct Light : public VirtualObject, public LightPreset {
			/// \brief Parses a light VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see light_preset::parse
			ZKREM("use ::load()") ZKAPI static void parse(Light& obj, ReadArchive& ctx, GameVersion version);

			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};
	} // namespace vobs
} // namespace zenkit
