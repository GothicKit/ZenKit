// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/vobs/vob.hh>

namespace phoenix {
	/// \brief The type of a light source.
	enum class light_mode : std::uint32_t {
		point = 0,     ///< The light source emits a cone-shaped light beam.
		spot = 1,      ///< The light source emits light in a sphere.
		reserved0 = 2, ///< Unused.
		reserved1 = 3, ///< Unused.
	};

	/// \brief The quality of a light source.
	enum class light_quality : std::uint32_t {
		high = 0,   ///< The light is a high quality light.
		medium = 1, ///< The light is a medium quality light.
		low = 2,    ///< The light is a low quality light, which should be the fastest option.
	};

	namespace vobs {
		/// \brief A preset for light sources.
		struct light_preset {
			std::string preset {};
			light_mode light_type {light_mode::spot};
			float range {};
			glm::u8vec4 color {};
			float cone_angle {0};
			bool is_static {false};
			light_quality quality {light_quality::medium};
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
			/// \throws parser_error if parsing fails.
			static void parse(light_preset& obj, archive_reader& ctx, game_version version);

			/// \brief Parses a light preset the given *ZenGin* archive.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \return The parsed light preset.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			static light_preset parse(archive_reader& in, game_version version);
		};

		/// \brief A VOb which acts as a light source.
		struct light : public vob, public light_preset {
			/// \brief Parses a light VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			/// \see light_preset::parse
			static void parse(light& obj, archive_reader& ctx, game_version version);
		};
	} // namespace vobs
} // namespace phoenix
