// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/Misc.hh"
#include "zenkit/vobs/VirtualObject.hh"

#include <glm/vec4.hpp>

#include <cstdint>

namespace zenkit {
	class ReadArchive;

	namespace vobs {
		/// \brief A VOb which defines the background music in a certain zone.
		struct ZoneMusic : public VirtualObject {
			bool enabled {false};
			std::int32_t priority {0};
			bool ellipsoid {false};
			float reverb {0};
			float volume {0};
			bool loop {false};

			// Save-game only variables
			bool s_local_enabled {true};
			bool s_day_entrance_done {false};
			bool s_night_entrance_done {false};

			/// \brief Parses a zone music VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()") ZKAPI static void parse(ZoneMusic& obj, ReadArchive& ctx, GameVersion version);
			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		/// \brief A VOb which defines the far plane settings in a certain zone.
		struct ZoneFarPlane : public VirtualObject {
			float vob_far_plane_z;
			float inner_range_percentage;

			/// \brief Parses a zone far plane VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()") ZKAPI static void parse(ZoneFarPlane& obj, ReadArchive& ctx, GameVersion version);
			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		/// \brief A VOb which defines the fog in a certain zone.
		struct ZoneFog : public VirtualObject {
			float range_center {0};
			float inner_range_percentage {0};
			glm::u8vec4 color {};
			bool fade_out_sky {false};
			bool override_color {false};

			/// \brief Parses a zone fog VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()") ZKAPI static void parse(ZoneFog& obj, ReadArchive& ctx, GameVersion version);
			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};
	} // namespace vobs
} // namespace zenkit
