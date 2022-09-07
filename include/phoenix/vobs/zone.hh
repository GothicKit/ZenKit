// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/vobs/vob.hh>

namespace phoenix::vobs {
	/// \brief A VOb which defines the background music in a certain zone.
	struct zone_music : public vob {
		bool enabled {false};
		std::int32_t priority {0};
		bool ellipsoid {false};
		float reverb {0};
		float volume {0};
		bool loop {false};

		/// \brief Parses a zone music VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws parser_error if parsing fails.
		/// \see vob::parse
		static void parse(zone_music& obj, std::unique_ptr<archive_reader>& ctx, game_version version);
	};

	/// \brief A VOb which defines the far plane settings in a certain zone.
	struct zone_far_plane : public vob {
		float vob_far_plane_z;
		float inner_range_percentage;

		/// \brief Parses a zone far plane VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws parser_error if parsing fails.
		/// \see vob::parse
		static void parse(zone_far_plane& obj, std::unique_ptr<archive_reader>& ctx, game_version version);
	};

	/// \brief A VOb which defines the fog in a certain zone.
	struct zone_fog : public vob {
		float range_center {0};
		float inner_range_percentage {0};
		glm::u8vec4 color {};
		bool fade_out_sky {false};
		bool override_color {false};

		/// \brief Parses a zone fog VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws parser_error if parsing fails.
		/// \see vob::parse
		static void parse(zone_fog& obj, std::unique_ptr<archive_reader>& ctx, game_version version);
	};
} // namespace phoenix::vobs