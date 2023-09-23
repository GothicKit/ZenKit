// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include "../Api.hh"
#include <phoenix/vobs/vob.hh>

namespace phoenix {
	enum class message_filter_action : uint32_t {
		none = 0,
		trigger = 1,
		untrigger = 2,
		enable = 3,
		disable = 4,
		toggle = 5,
	};

	enum class mover_message_type : uint32_t {
		fixed_direct = 0,
		fixed_order = 1,
		next = 2,
		previous = 3,
	};

	enum class collision_type : std::uint32_t {
		none = 0,
		box = 1,
		point = 2,
	};

	namespace vobs {
		/// \brief An animated VOb.
		struct animate : public vob {
			bool start_on {false};

			// Save-game only variables
			bool s_is_running {false};

			/// \brief Parses an animated VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(animate& obj, archive_reader& ctx, game_version version);
		};

		/// \brief A VOb representing an in-game item.
		struct item : public vob {
			std::string instance;

			// Save-game only variables
			int s_amount;
			int s_flags;

			/// \brief Parses an item VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(item& obj, archive_reader& ctx, game_version version);
		};

		/// \brief A VOb representing a [lens flare](https://en.wikipedia.org/wiki/Lens_flare).
		struct lens_flare : public vob {
			std::string fx;

			/// \brief Parses a lens flare VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(lens_flare& obj, archive_reader& ctx, game_version version);
		};

		/// \brief A VOb representing a particle system controller.
		struct pfx_controller : public vob {
			std::string pfx_name;
			bool kill_when_done;
			bool initially_running;

			/// \brief Parses a particle system controller VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(pfx_controller& obj, archive_reader& ctx, game_version version);
		};

		struct message_filter : public vob {
			std::string target;
			message_filter_action on_trigger;
			message_filter_action on_untrigger;

			/// \brief Parses a message filter VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(message_filter& obj, archive_reader& ctx, game_version version);
		};

		struct code_master : public vob {
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
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(code_master& obj, archive_reader& ctx, game_version version);
		};

		struct mover_controller : public vob {
			std::string target;
			mover_message_type message;
			std::int32_t key;

			/// \brief Parses a mover controller VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(mover_controller& obj, archive_reader& ctx, game_version version);
		};

		/// \brief A VOb which represents a damage source.
		struct touch_damage : public vob {
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
			collision_type collision;

			/// \brief Parses a touch damage VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(touch_damage& obj, archive_reader& ctx, game_version version);
		};

		/// \brief A VOb which represents an earthquake-like effect.
		struct earthquake : public vob {
			float radius;
			float duration;
			glm::vec3 amplitude;

			/// \brief Parses an earthquake VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(earthquake& obj, archive_reader& ctx, game_version version);
		};

		struct npc : public vob {
			static const std::uint32_t attribute_count = 8;
			static const std::uint32_t hcs_count = 4;
			static const std::uint32_t missions_count = 5;
			static const std::uint32_t aivar_count = 100;
			static const std::uint32_t packed_count = 9;
			static const std::uint32_t protection_count = 8;

			struct talent {
				int talent;
				int value;
				int skill;
			};

			struct slot {
				bool used;
				std::string name;
				int item_index;
				bool in_inventory;
			};

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

			std::vector<talent> talents;

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
			std::vector<std::unique_ptr<item>> items;
			std::vector<slot> slots;

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
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(npc& obj, archive_reader& ctx, game_version version);
		};
	} // namespace vobs
} // namespace phoenix