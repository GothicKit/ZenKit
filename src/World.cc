// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/World.hh"
#include "zenkit/Archive.hh"
#include "zenkit/Stream.hh"
#include "zenkit/vobs/Misc.hh"

#include "Internal.hh"

namespace zenkit {
	[[maybe_unused]] static constexpr uint32_t BSP_VERSION_G1 = 0x2090000;
	static constexpr uint32_t BSP_VERSION_G2 = 0x4090000;

	/// \brief Tries to determine the serialization version of a game world.
	///
	/// This function might be very slow. If the VOb tree or way-net or both come before the mesh section in the
	/// archive, they have to be skipped since only the `MeshAndBsp` section of the world can be used to reliably
	/// determine the version being used.
	///
	/// \param buf A buffer containing the world's data.
	/// \return The game version associated with that world.
	static GameVersion determine_world_version(Read* buf) {
		auto archive = ReadArchive::from(buf);

		if (archive->is_save_game()) {
			throw zenkit::ParserError {"World", "cannot automatically detect world version for save-games!s"};
		}

		ArchiveObject chnk {};
		archive->read_object_begin(chnk);

		while (!archive->read_object_end()) {
			archive->read_object_begin(chnk);

			if (chnk.object_name == "MeshAndBsp") {
				auto bsp_version = buf->read_uint();
				return bsp_version == BSP_VERSION_G2 ? GameVersion::GOTHIC_2 : GameVersion::GOTHIC_1;
			}

			archive->skip_object(true);
		}

		ZKLOGE("World", "Failed to determine world version. Assuming Gothic 1.");
		return GameVersion::GOTHIC_1;
	}

	World World::parse(phoenix::buffer& buf, GameVersion version) {
		World wld {};

		auto r = Read::from(&buf);
		wld.load(r.get(), version);

		return wld;
	}

	World World::parse(phoenix::buffer&& buf, GameVersion version) {
		return World::parse(buf, version);
	}

	World World::parse(phoenix::buffer& buf) {
		World wld {};

		auto r = Read::from(&buf);
		wld.load(r.get());

		return wld;
	}

	World World::parse(phoenix::buffer&& buf) {
		return World::parse(buf);
	}

	void World::load(Read* r) {
		auto begin = r->tell();
		auto version = determine_world_version(r);
		r->seek(static_cast<ssize_t>(begin), Whence::BEG);
		this->load(r, version);
	}

	void World::load(Read* r, GameVersion version) {
		auto archive = ReadArchive::from(r);

		ArchiveObject chnk {};
		archive->read_object_begin(chnk);

		if (chnk.class_name != "oCWorld:zCWorld") {
			throw ParserError {"World", "'oCWorld:zCWorld' chunk expected, got '" + chnk.class_name + "'"};
		}

		while (!archive->read_object_end()) {
			archive->read_object_begin(chnk);
			ZKLOGI("World",
			       "Parsing object [%s %s %u %u]",
			       chnk.object_name.c_str(),
			       chnk.class_name.c_str(),
			       chnk.version,
			       chnk.index);

			if (chnk.object_name == "MeshAndBsp") {
				auto bsp_version = r->read_uint();
				(void) /* size = */ r->read_uint();

				std::uint16_t chunk_type = 0;
				auto mesh_offset = r->tell();

				do {
					chunk_type = r->read_ushort();
					r->seek(r->read_uint(), Whence::CUR);
				} while (chunk_type != 0xB060);

				auto is_xzen = archive->get_header().user == "XZEN";
				if (is_xzen) {
					ZKLOGI("World", "XZEN world detected, forcing wide vertex indices");
				}

				this->world_bsp_tree.load(r, bsp_version);
				auto end = r->tell();

				r->seek(static_cast<ssize_t>(mesh_offset), Whence::BEG);
				this->world_mesh.load(r, this->world_bsp_tree.leaf_polygons, is_xzen);

				r->seek(static_cast<ssize_t>(end), Whence::BEG);
			} else if (chnk.object_name == "VobTree") {
				auto count = static_cast<size_t>(archive->read_int());
				this->world_vobs.reserve(count);

				for (auto i = 0u; i < count; ++i) {
					auto child = parse_vob_tree(*archive, version);
					if (child == nullptr) continue;
					this->world_vobs.push_back(std::move(child));
				}
			} else if (chnk.object_name == "WayNet") {
				this->world_way_net.load(*archive);
			} else if (chnk.object_name == "CutscenePlayer") {
				this->player = archive->read_object<CutscenePlayer>(version);
			} else if (chnk.object_name == "SkyCtrl") {
				this->sky_controller = archive->read_object<SkyController>(version);
			} else if (chnk.object_name == "EndMarker") {
				archive->read_object_end();
				break;
			}

			if (!archive->read_object_end()) {
				ZKLOGW("World",
				       "Object [%s %s %u %u] not fully parsed",
				       chnk.object_name.c_str(),
				       chnk.class_name.c_str(),
				       chnk.version,
				       chnk.index);
				archive->skip_object(true);
			}
		}

		// TODO: save games contain a list of NPCs after the end marker
		if (archive->get_header().save) {
			// Then, read all the NPCs
			auto npc_count = archive->read_int(); // npcCount
			this->npcs.resize(npc_count);
			for (auto i = 0; i < npc_count; ++i) {
				this->npcs[i] = archive->read_object<vobs::Npc>(version);
			}

			// After that, read all NPC spawn locations
			auto npc_spawn_count = archive->read_int(); // NoOfEntries
			this->npc_spawns.resize(npc_spawn_count);

			for (auto& spawn : this->npc_spawns) {
				spawn.npc = archive->read_object<vobs::Npc>(version); // npc
				spawn.position = archive->read_vec3();                // spawnPos
				spawn.timer = archive->read_float();                  // timer
			}

			this->npc_spawn_enabled = archive->read_bool(); // spawningEnabled

			if (version == GameVersion::GOTHIC_2) {
				this->npc_spawn_flags = archive->read_int(); // spawnFlags
			}
		}

		if (!archive->read_object_end()) {
			ZKLOGW("World", "Not fully parsed");
			archive->skip_object(true);
		}
	}

	void CutscenePlayer::load(ReadArchive& r, GameVersion) {
		this->last_process_day = r.read_int();  // lastProcessDay
		this->last_process_hour = r.read_int(); // lastProcessHour
		this->play_list_count = r.read_int();   // playListCount
	}

	void SkyController::load(ReadArchive& r, GameVersion version) {
		// TODO
		master_time = r.read_float();    // masterTime
		rain_weight = r.read_float();    // rainWeight
		rain_start = r.read_float();     // rainStart
		rain_stop = r.read_float();      // rainStop
		rain_sct_timer = r.read_float(); // rainSctTimer
		rain_snd_vol = r.read_float();   // rainSndVol
		day_ctr = r.read_float();        // dayCtr

		if (version == GameVersion::GOTHIC_2) {
			fade_scale = r.read_float();      // fadeScale
			render_lightning = r.read_bool(); // renderLightning
			is_raining = r.read_bool();       // isRaining
			rain_ctr = r.read_int();          // rainCtr
		}
	}
} // namespace zenkit
