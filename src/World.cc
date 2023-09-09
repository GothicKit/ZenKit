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

	void World::load(zenkit::Read* r, zenkit::GameVersion version) {
		auto archive = ReadArchive::from(r);

		ArchiveObject chnk {};
		archive->read_object_begin(chnk);

		if (chnk.class_name != "oCWorld:zCWorld") {
			throw zenkit::ParserError {"World", "'oCWorld:zCWorld' chunk expected, got '" + chnk.class_name + "'"};
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
				auto count = archive->read_int();
				this->world_vobs.reserve(count);

				for (int32_t i = 0; i < count; ++i) {
					auto child = parse_vob_tree(*archive, version);
					if (child == nullptr)
						continue;
					this->world_vobs.push_back(std::move(child));
				}
			} else if (chnk.object_name == "WayNet") {
				this->world_way_net.load(*archive);
			} else if (chnk.object_name == "CutscenePlayer") {
				// TODO: only present in save-games

				if (!archive->read_object_begin(chnk)) {
					ZKLOGW("World",
					       "Object [%s %s %u %u] encountered but unable to parse",
					       chnk.object_name.c_str(),
					       chnk.class_name.c_str(),
					       chnk.version,
					       chnk.index);
					archive->skip_object(true);
					continue;
				}

				(void) archive->read_int(); // lastProcessDay
				(void) archive->read_int(); // lastProcessHour
				(void) archive->read_int(); // playListCount

				archive->read_object_end();
			} else if (chnk.object_name == "SkyCtrl") {
				// TODO: only present in save-games

				if (!archive->read_object_begin(chnk)) {
					ZKLOGW("World",
					       "Object [%s %s %u %u] encountered but unable to parse",
					       chnk.object_name.c_str(),
					       chnk.class_name.c_str(),
					       chnk.version,
					       chnk.index);
					archive->skip_object(true);
					continue;
				}

				(void) archive->read_float(); // masterTime
				(void) archive->read_float(); // rainWeight
				(void) archive->read_float(); // rainStart
				(void) archive->read_float(); // rainStop
				(void) archive->read_float(); // rainSctTimer
				(void) archive->read_float(); // rainSndVol
				(void) archive->read_float(); // dayCtr

				if (version == GameVersion::GOTHIC_2) {
					(void) archive->read_float(); // fadeScale
					(void) archive->read_bool();  // renderLightning
					(void) archive->read_bool();  // isRaining
					(void) archive->read_int();   // rainCtr
				}

				archive->read_object_end();
			} else if (chnk.object_name == "EndMarker" && archive->get_header().save) {
				// TODO: save games contain a list of NPCs after the end marker
				// First, Consume the end-maker fully
				archive->read_object_end();

				// Then, read all the NPCs
				auto npc_count = archive->read_int(); // npcCount
				for (auto i = 0; i < npc_count; ++i) {
					archive->read_object_begin(chnk);

					if (chnk.class_name != "\xA7") {
						vobs::Npc npc {};
						npc.load(*archive, version);
					} else {
						ZKLOGE("World",
						       "Cannot load NPC reference [%s %s %d %d]",
						       chnk.object_name.c_str(),
						       chnk.class_name.c_str(),
						       chnk.version,
						       chnk.index);
					}

					if (!archive->read_object_end()) {
						archive->skip_object(true);
					}
				}

				// After that, read all NPC spawn locations
				auto npc_spawn_count = archive->read_int(); // NoOfEntries
				for (auto i = 0; i < npc_spawn_count; ++i) {
					archive->skip_object(false);  // npc zReference
					(void) archive->read_vec3();  // spawnPos
					(void) archive->read_float(); // timer
				}

				(void) archive->read_bool(); // spawningEnabled

				if (version == GameVersion::GOTHIC_2) {
					(void) archive->read_int(); // spawnFlags
				}

				if (!archive->read_object_end()) {
					ZKLOGW("World", "Npc-list not fully parsed");
					archive->skip_object(true);
				}

				// We have fully consumed the world block. From here we should just die.
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
	}
} // namespace zenkit
