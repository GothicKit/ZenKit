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
		ArchiveObject chnk {};
		auto ar = ReadArchive::from(r);
		ar->read_object_begin(chnk);

		if (chnk.class_name != "oCWorld:zCWorld") {
			throw ParserError {"World", "'oCWorld:zCWorld' chunk expected, got '" + chnk.class_name + "'"};
		}

		this->load(*ar, version);

		if (!ar->read_object_end()) {
			ZKLOGW("World", "Not fully parsed");
			ar->skip_object(true);
		}
	}

	void World::load(ReadArchive& r, GameVersion version) {
		ArchiveObject hdr;

		// Load properties of `zCWorld`
		while (!r.read_object_end()) {
			if (!r.read_object_begin(hdr)) {
				throw ParserError {"World", "Failed to load zCWorld: expected object, got field!"};
			}

			ZKLOGI("World",
			       "Parsing object [%s %s %u %u]",
			       hdr.object_name.c_str(),
			       hdr.class_name.c_str(),
			       hdr.version,
			       hdr.index);

			if (hdr.object_name == "MeshAndBsp") {
				auto* raw = r.get_stream();

				auto bsp_version = raw->read_uint();
				(void) /* size = */ raw->read_uint();

				std::uint16_t chunk_type = 0;
				auto mesh_offset = raw->tell();

				do {
					chunk_type = raw->read_ushort();
					raw->seek(raw->read_uint(), Whence::CUR);
				} while (chunk_type != 0xB060);

				auto is_xzen = r.get_header().user == "XZEN";
				if (is_xzen) {
					ZKLOGI("World", "XZEN world detected, forcing wide vertex indices");
				}

				this->world_bsp_tree.load(raw, bsp_version);
				auto end = raw->tell();

				raw->seek(static_cast<ssize_t>(mesh_offset), Whence::BEG);
				this->world_mesh.load(raw, this->world_bsp_tree.leaf_polygons, is_xzen);

				raw->seek(static_cast<ssize_t>(end), Whence::BEG);
			} else if (hdr.object_name == "VobTree") {
				auto count = r.read_int(); // childs0
				for (auto i = 0; i < count; ++i) {
					auto child = parse_vob_tree(r, version);

					// We failed to parse this root VObject.
					if (child == nullptr) {
						ZKLOGE("World", "Failed to parse root VOb %d!", i);
						continue;
					}

					this->world_vobs.push_back(std::move(child));
				}
			} else if (hdr.object_name == "WayNet") {
#ifndef ZK_FUTURE
				this->world_way_net.load(r);
#else
				this->way_net = r.read_object<WayNet>(version);
#endif
			} else if (hdr.object_name == "CutscenePlayer") {
				this->player = r.read_object<CutscenePlayer>(version);
			} else if (hdr.object_name == "SkyCtrl") {
				this->sky_controller = r.read_object<SkyController>(version);
			} else if (hdr.object_name == "EndMarker") {
				r.read_object_end();
				break;
			}

			if (!r.read_object_end()) {
				ZKLOGW("World",
				       "Object [%s %s %u %u] not fully parsed",
				       hdr.object_name.c_str(),
				       hdr.class_name.c_str(),
				       hdr.version,
				       hdr.index);
				r.skip_object(true);
			}
		}

		if (r.is_save_game()) {
			// Then, read all the NPCs
			auto npc_count = r.read_int(); // npcCount
			this->npcs.resize(npc_count);
			for (auto i = 0; i < npc_count; ++i) {
				this->npcs[i] = r.read_object<VNpc>(version);
			}

			// After that, read all NPC spawn locations
			auto npc_spawn_count = r.read_int(); // NoOfEntries
			this->npc_spawns.resize(npc_spawn_count);

			for (auto& spawn : this->npc_spawns) {
				spawn.npc = r.read_object<VNpc>(version); // npc
				spawn.position = r.read_vec3();           // spawnPos
				spawn.timer = r.read_float();             // timer
			}

			this->npc_spawn_enabled = r.read_bool(); // spawningEnabled

			if (version == GameVersion::GOTHIC_2) {
				this->npc_spawn_flags = r.read_int(); // spawnFlags
			}
		}
	}

	void World::save(WriteArchive& w, GameVersion version) const {
		if (!w.is_save_game()) {
			w.write_object_begin("MeshAndBsp", "", 0);

			Write* raw = w.get_stream();
			raw->write_uint(0); // TODO: version
			raw->write_uint(0); // TODO: size

			this->world_mesh.save(raw, version);
			this->world_bsp_tree.save(raw, version);

			w.write_object_end();
		}

		w.write_object_begin("VobTree", "", 0);
		w.write_int("childs0", this->world_vobs.size());
		for (auto& root : this->world_vobs) {
			save_vob_tree(w, version, root);
		}
		w.write_object_end();

		if (w.is_save_game()) {
			if (this->player) {
				w.write_object_begin("CutscenePlayer", "", 0);
				w.write_object(this->player, version);
				w.write_object_end();
			}

			if (this->sky_controller) {
				w.write_object_begin("SkyCtrl", "", 0);
				w.write_object(this->sky_controller, version);
				w.write_object_end();
			}
		}

#ifdef ZK_FUTURE
		w.write_object_begin("WayNet", "", 0);
		w.write_object(this->way_net, version);
		w.write_object_end();
#endif

		w.write_object_begin("EndMarker", "", 0);
		w.write_object_end();

		if (w.is_save_game()) {
			// Then, read all the NPCs
			w.write_int("npcCount", this->npcs.size());
			for (auto i = 0u; i < this->npcs.size(); ++i) {
				w.write_object("npc" + std::to_string(i), this->npcs[i], version);
			}

			// After that, read all NPC spawn locations
			w.write_int("NoOfEntries", this->npc_spawns.size());

			for (auto& spawn : this->npc_spawns) {
				w.write_object("npc", spawn.npc, version);
				w.write_vec3("spawnPos", spawn.position);
				w.write_float("timer", spawn.timer);
			}

			w.write_bool("spawningEnabled", this->npc_spawn_enabled);

			if (version == GameVersion::GOTHIC_2) {
				w.write_bool("spawnFlags", this->npc_spawn_flags);
			}
		}
	}

	uint16_t World::get_version_identifier(GameVersion) const {
		return 64513;
	}

	void CutscenePlayer::load(ReadArchive& r, GameVersion) {
		this->last_process_day = r.read_int();  // lastProcessDay
		this->last_process_hour = r.read_int(); // lastProcessHour
		this->play_list_count = r.read_int();   // playListCount
	}

	void CutscenePlayer::save(WriteArchive& w, GameVersion) const {
		w.write_int("lastProcessDay", this->last_process_day);
		w.write_int("lastProcessHour", this->last_process_hour);
		w.write_int("playListCount", this->play_list_count);
	}

	void SkyController::load(ReadArchive& r, GameVersion version) {
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

	void SkyController::save(WriteArchive& w, GameVersion version) const {
		w.write_float("masterTime", master_time);
		w.write_float("rainWeight", rain_weight);
		w.write_float("rainStart", rain_start);
		w.write_float("rainStop", rain_stop);
		w.write_float("rainSctTimer", rain_sct_timer);
		w.write_float("rainSndVol", rain_snd_vol);
		w.write_float("dayCtr", day_ctr);

		if (version == GameVersion::GOTHIC_2) {
			w.write_float("", fade_scale);      // fadeScale
			w.write_bool("", render_lightning); // renderLightning
			w.write_bool("", is_raining);       // isRaining
			w.write_int("", rain_ctr);          // rainCtr
		}
	}
} // namespace zenkit
