// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/Misc.hh"
#include "zenkit/vobs/VirtualObject.hh"

#include <glm/vec3.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace zenkit {
	class ReadArchive;

	enum class MessageFilterAction : uint32_t {
		NONE = 0,
		TRIGGER = 1,
		UNTRIGGER = 2,
		ENABLE = 3,
		DISABLE = 4,
		TOGGLE = 5,

		// Deprecated entries.
		none ZKREM("renamed to MessageFilterAction::NONE") = NONE,
		trigger ZKREM("renamed to MessageFilterAction::TRIGGER") = TRIGGER,
		untrigger ZKREM("renamed to MessageFilterAction::UNTRIGGER") = UNTRIGGER,
		enable ZKREM("renamed to MessageFilterAction::ENABLE") = ENABLE,
		disable ZKREM("renamed to MessageFilterAction::DISABLE") = DISABLE,
		toggle ZKREM("renamed to MessageFilterAction::TOGGLE") = TOGGLE,
	};

	enum class MoverMessageType : uint32_t {
		FIXED_DIRECT = 0,
		FIXED_ORDER = 1,
		NEXT = 2,
		PREVIOUS = 3,

		// Deprecated entries.
		fixed_direct ZKREM("renamed to MoverMessageType::FIXED_DIRECT") = FIXED_DIRECT,
		fixed_order ZKREM("renamed to MoverMessageType::FIXED_ORDER") = FIXED_ORDER,
		next ZKREM("renamed to MoverMessageType::NEXT") = NEXT,
		previous ZKREM("renamed to MoverMessageType::PREVIOUS") = PREVIOUS,
	};

	enum class TouchCollisionType : std::uint32_t {
		NONE = 0,
		BOX = 1,
		POINT = 2,

		// Deprecated entries.
		none ZKREM("renamed to TouchCollisionType::NONE") = NONE,
		box ZKREM("renamed to TouchCollisionType::BOX") = BOX,
		point ZKREM("renamed to TouchCollisionType::POINT") = POINT,
	};

	namespace vobs {
		/// \brief An animated VOb.
		struct Animate : public VirtualObject {
			bool start_on {false};

			// Save-game only variables
			bool s_is_running {false};

			/// \brief Parses an animated VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()") ZKAPI static void parse(Animate& obj, ReadArchive& ctx, GameVersion version);

			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		/// \brief A VOb representing an in-game item.
		struct Item : public VirtualObject {
			std::string instance;

			// Save-game only variables
			int s_amount;
			int s_flags;

			/// \brief Parses an item VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()") ZKAPI static void parse(Item& obj, ReadArchive& ctx, GameVersion version);

			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		/// \brief A VOb representing a [lens flare](https://en.wikipedia.org/wiki/Lens_flare).
		struct LensFlare : public VirtualObject {
			std::string fx;

			/// \brief Parses a lens flare VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()") ZKAPI static void parse(LensFlare& obj, ReadArchive& ctx, GameVersion version);

			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		/// \brief A VOb representing a particle system controller.
		struct ParticleEffectController : public VirtualObject {
			std::string pfx_name;
			bool kill_when_done;
			bool initially_running;

			/// \brief Parses a particle system controller VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()")
			ZKAPI static void parse(ParticleEffectController& obj, ReadArchive& ctx, GameVersion version);

			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		struct MessageFilter : public VirtualObject {
			std::string target;
			MessageFilterAction on_trigger;
			MessageFilterAction on_untrigger;

			/// \brief Parses a message filter VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()") ZKAPI static void parse(MessageFilter& obj, ReadArchive& ctx, GameVersion version);

			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		struct CodeMaster : public VirtualObject {
			std::string target;
			bool ordered;
			bool first_false_is_failure;
			std::string failure_target;
			bool untriggered_cancels;
			std::vector<std::string> slaves;

			// Save-game only variables
			uint8_t s_num_triggered_slaves;

			/// \brief Parses a code master VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()") ZKAPI static void parse(CodeMaster& obj, ReadArchive& ctx, GameVersion version);

			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		struct MoverController : public VirtualObject {
			std::string target;
			MoverMessageType message;
			std::int32_t key;

			/// \brief Parses a mover controller VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()") ZKAPI static void parse(MoverController& obj, ReadArchive& ctx, GameVersion version);

			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		/// \brief A VOb which represents a damage source.
		struct TouchDamage : public VirtualObject {
			float damage;

			bool barrier;
			bool blunt;
			bool edge;
			bool fire;
			bool fly;
			bool magic;
			bool point;
			bool fall;

			float repeat_delay_sec;
			float volume_scale;
			TouchCollisionType collision;

			/// \brief Parses a touch damage VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()") ZKAPI static void parse(TouchDamage& obj, ReadArchive& ctx, GameVersion version);

			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		/// \brief A VOb which represents an earthquake-like effect.
		struct Earthquake : public VirtualObject {
			float radius;
			float duration;
			glm::vec3 amplitude;

			/// \brief Parses an earthquake VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()") ZKAPI static void parse(Earthquake& obj, ReadArchive& ctx, GameVersion version);

			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		struct Npc : public VirtualObject {
			static const std::uint32_t attribute_count = 8;
			static const std::uint32_t hcs_count = 4;
			static const std::uint32_t missions_count = 5;
			static const std::uint32_t aivar_count = 100;
			static const std::uint32_t packed_count = 9;
			static const std::uint32_t protection_count = 8;

			struct Talent {
				int talent;
				int value;
				int skill;
			};

			using talent ZKREM("renamed to Talent") = Talent;

			struct Slot {
				bool used;
				std::string name;
				Item* item {};
				bool in_inventory;
			};

			using slot ZKREM("renamed to Slot") = Slot;

			std::string npc_instance;
			glm::vec3 model_scale;
			float model_fatness;

			std::vector<std::string> overlays;

			int flags;
			int guild;
			int guild_true;
			int level;
			int xp;
			int xp_next_level;
			int lp;

			std::vector<Talent> talents;

			int fight_tactic;
			int fight_mode;
			bool wounded;
			bool mad;
			int mad_time;
			bool player;

			int attributes[attribute_count];
			int hcs[hcs_count];
			int missions[missions_count];

			std::string start_ai_state;
			int aivar[aivar_count];
			std::string script_waypoint;
			int attitude;
			int attitude_temp;
			int name_nr;
			bool move_lock;

			std::string packed[packed_count];
			std::vector<std::unique_ptr<Item>> items;
			std::vector<Slot> slots;

			bool current_state_valid;
			std::string current_state_name;
			int current_state_index;
			bool current_state_is_routine;

			bool next_state_valid;
			std::string next_state_name;
			int next_state_index;
			bool next_state_is_routine;

			int last_ai_state;
			bool has_routine;
			bool routine_changed;
			bool routine_overlay;
			int routine_overlay_count;
			int walkmode_routine;
			bool weaponmode_routine;
			bool start_new_routine;
			int ai_state_driven;
			glm::vec3 ai_state_pos;
			std::string current_routine;
			bool respawn;
			int respawn_time;

			int protection[protection_count];

			int bs_interruptable_override {0};
			int npc_type {0};
			int spell_mana {0};

			/// \brief Parses an NPC VOb from the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			ZKREM("use ::load()") ZKAPI static void parse(Npc& obj, ReadArchive& ctx, GameVersion version);

			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		struct ScreenEffect : public VirtualObject {
			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};
	} // namespace vobs
} // namespace zenkit
