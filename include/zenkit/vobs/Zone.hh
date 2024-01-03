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

	/// \brief A VObject which defines the music to be played within its bounding box.
	///
	/// Music zones may overlap; if they do, their #priority property determines which music zone should be used. The
	/// camera object is used to test which music zone applies. If no music zone applies the music defined in the
	/// optional VZoneMusicDefault is used.
	///
	/// \see https://zk.gothickit.dev/engine/objects/oCZoneMusic/
	struct VZoneMusic : VirtualObject {
		ZK_OBJECT(ObjectType::oCZoneMusic);

	public:
		/// \brief Whether the music zone is enabled when loading the level.
		/// \see https://zk.gothickit.dev/engine/objects/oCZoneMusic/#enabled
		bool enabled {false};

		/// \brief The priority of the music zone if two zones overlap.
		///
		/// A higher number means a higher priority. `0` is the lowest possible priority.
		///
		/// \note Only relevant if two `zCZoneMusic` zones overlap.
		/// \see https://zk.gothickit.dev/engine/objects/oCZoneMusic/#priority
		std::int32_t priority {0};

		/// \brief Determines the shape of the zone.
		/// \see https://zk.gothickit.dev/engine/objects/oCZoneMusic/#ellipsoid
		bool ellipsoid {false};

		/// \brief Unclear.
		/// \see https://zk.gothickit.dev/engine/objects/oCZoneMusic/#reverbLevel
		float reverb {0};

		/// \brief Unclear.
		/// \see https://zk.gothickit.dev/engine/objects/oCZoneMusic/#volumeLevel
		float volume {0};

		/// \brief Unclear.
		/// \see https://zk.gothickit.dev/engine/objects/oCZoneMusic/#loop
		bool loop {false};

		// Save-game only variables
		bool s_local_enabled {true};
		bool s_day_entrance_done {false};
		bool s_night_entrance_done {false};

		ZKREM("use ::load()") ZKAPI static void parse(VZoneMusic& obj, ReadArchive& ctx, GameVersion version);

		/// \brief Load this object from the given archive.
		/// \param r The archive to read from;
		/// \param version The version of the game the object was made for.
		ZKAPI void load(ReadArchive& r, GameVersion version) override;

		/// \brief Save this object to the given archive.
		/// \param w The archive to save to.
		/// \param version The version of the game to save for.
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;

		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	/// \brief Determines the default music to be played when the camera is outside any VZoneMusic zones.
	/// \see https://zk.gothickit.dev/engine/objects/oCZoneMusicDefault/
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
