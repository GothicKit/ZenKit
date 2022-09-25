// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
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

			/// \brief Parses an animated VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			static void parse(animate& obj, archive_reader& ctx, game_version version);
		};

		/// \brief A VOb representing an in-game item.
		struct item : public vob {
			std::string instance;

			/// \brief Parses an item VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			static void parse(item& obj, archive_reader& ctx, game_version version);
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
			static void parse(lens_flare& obj, archive_reader& ctx, game_version version);
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
			static void parse(pfx_controller& obj, archive_reader& ctx, game_version version);
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
			static void parse(message_filter& obj, archive_reader& ctx, game_version version);
		};

		struct code_master : public vob {
			std::string target;
			bool ordered;
			bool first_false_is_failure;
			std::string failure_target;
			bool untriggered_cancels;
			std::vector<std::string> slaves;

			/// \brief Parses a code master VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			static void parse(code_master& obj, archive_reader& ctx, game_version version);
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
			static void parse(mover_controller& obj, archive_reader& ctx, game_version version);
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
			static void parse(touch_damage& obj, archive_reader& ctx, game_version version);
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
			static void parse(earthquake& obj, archive_reader& ctx, game_version version);
		};
	} // namespace vobs
} // namespace phoenix