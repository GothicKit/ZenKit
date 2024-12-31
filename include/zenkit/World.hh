// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/Mesh.hh"
#include "zenkit/Misc.hh"

#include "zenkit/world/BspTree.hh"
#include "zenkit/world/VobTree.hh"
#include "zenkit/world/WayNet.hh"

#include <memory>
#include <vector>

namespace zenkit {
	struct VNpc;
	struct CutsceneContext;

	struct CutscenePlayer : Object {
		static constexpr ObjectType TYPE = ObjectType::oCCSPlayer;

		int32_t last_process_day;
		int32_t last_process_hour;
		std::vector<std::weak_ptr<CutsceneContext>> playlists;

		[[nodiscard]] ObjectType get_object_type() const override {
			return TYPE;
		}

		void load(ReadArchive& r, GameVersion version) override;
		void save(WriteArchive& w, GameVersion version) const override;
	};

	struct SkyController : Object {
		static constexpr ObjectType TYPE = ObjectType::zCSkyControler_Outdoor;

		float master_time;
		float rain_weight;
		float rain_start;
		float rain_stop;
		float rain_sct_timer;
		float rain_snd_vol;
		float day_ctr;

		// Only relevant for G1:
		float fade_scale;
		bool render_lightning;
		bool is_raining;
		int rain_ctr;

		[[nodiscard]] ObjectType get_object_type() const override {
			return TYPE;
		}

		void load(ReadArchive& r, GameVersion version) override;
		void save(WriteArchive& w, GameVersion version) const override;
	};

	struct SpawnLocation {
		std::shared_ptr<VNpc> npc;
		glm::vec3 position;
		float timer;
	};

	/// \brief Represents a ZenGin world.
	class World : public Object {
		ZK_OBJECT(ObjectType::oCWorld);

		ZKAPI void load(Read* r);
		ZKAPI void load(Read* r, GameVersion version);

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;

		/// \brief The list of VObs defined in this world.
		std::vector<std::shared_ptr<VirtualObject>> world_vobs;

		/// \brief The mesh of the world.
		Mesh world_mesh;

		/// \brief The BSP-tree of this world.
		BspTree world_bsp_tree;

#ifndef ZK_FUTURE
		/// \brief The way-net of this world.
		WayNet world_way_net;
#else
		/// \brief The way-net of this world.
		std::shared_ptr<WayNet> way_net;
#endif

		// \note Only available in save-games, otherwise empty.
		std::vector<std::shared_ptr<VNpc>> npcs {};
		std::vector<SpawnLocation> npc_spawns {};
		bool npc_spawn_enabled = false;
		int npc_spawn_flags = 0;

		// \note Only available in save-games, otherwise null.
		std::shared_ptr<CutscenePlayer> player;

		// \note Only available in save-games, otherwise null.
		std::shared_ptr<SkyController> sky_controller;
	};
} // namespace zenkit
