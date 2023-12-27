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

	/// \brief A VOb which defines the background music in a certain zone.
	struct VZoneMusic : VirtualObject {
		ZK_OBJECT(ObjectType::oCZoneMusic);

	public:
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
		ZKREM("use ::load()") ZKAPI static void parse(VZoneMusic& obj, ReadArchive& ctx, GameVersion version);
		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	struct VZoneMusicDefault : VZoneMusic {
		ZK_OBJECT(ObjectType::oCZoneMusicDefault);
	};

	/// \brief A VOb which defines the far plane settings in a certain zone.
	struct VZoneFarPlane : VirtualObject {
		ZK_OBJECT(ObjectType::zCZoneVobFarPlane);

	public:
		float vob_far_plane_z;
		float inner_range_percentage;

		/// \brief Parses a zone far plane VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws ParserError if parsing fails.
		/// \see vob::parse
		ZKREM("use ::load()") ZKAPI static void parse(VZoneFarPlane& obj, ReadArchive& ctx, GameVersion version);
		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
	};

	struct VZoneFarPlaneDefault : VZoneFarPlane {
		ZK_OBJECT(ObjectType::zCZoneVobFarPlaneDefault);
	};

	/// \brief A VOb which defines the fog in a certain zone.
	struct VZoneFog : VirtualObject {
		ZK_OBJECT(ObjectType::zCZoneZFog);

	public:
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
		ZKREM("use ::load()") ZKAPI static void parse(VZoneFog& obj, ReadArchive& ctx, GameVersion version);
		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	struct VZoneFogDefault : VZoneFog {
		ZK_OBJECT(ObjectType::zCZoneZFogDefault);
	};
} // namespace zenkit
