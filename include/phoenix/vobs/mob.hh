// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/vobs/vob.hh>

namespace phoenix {
	/// \brief The different sounds a material can make.
	enum class sound_material : std::uint32_t {
		wood = 0,
		stone = 1,
		metal = 2,
		leather = 3,
		clay = 4,
		glass = 5,
	};

	namespace vobs {
		struct mob : public vob {
			std::string name;
			std::int32_t hp;
			std::int32_t damage;
			bool movable;
			bool takable;
			bool focus_override;
			sound_material material;
			std::string visual_destroyed;
			std::string owner;
			std::string owner_guild;
			bool destroyed;

			/// \brief Parses an interactive VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			static void parse(mob& obj, std::unique_ptr<archive_reader>& ctx, game_version version);
		};

		struct mob_inter : public mob {
			std::int32_t state;
			std::string target;
			std::string item;
			std::string condition_function;
			std::string on_state_change_function;
			bool rewind;

			/// \brief Parses a interactive VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			static void parse(mob_inter& obj, std::unique_ptr<archive_reader>& ctx, game_version version);
		};

		/// \brief A VOb representing a campfire.
		struct mob_fire : public mob_inter {
			std::string slot;
			std::string vob_tree;

			/// \brief Parses a campfire VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			/// \see mob::parse
			static void parse(mob_fire& obj, std::unique_ptr<archive_reader>& ctx, game_version version);
		};

		/// \brief A VOb representing a container.
		struct mob_container : public mob_inter {
			bool locked;
			std::string key;
			std::string pick_string;
			std::string contents;

			/// \brief Parses a container VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			/// \see mob::parse
			/// \see mob_container::parse
			static void parse(mob_container& obj, std::unique_ptr<archive_reader>& ctx, game_version version);
		};

		/// \brief A VOb representing a door.
		struct mob_door : public mob_inter {
			bool locked;
			std::string key;
			std::string pick_string;

			/// \brief Parses a door VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			/// \see mob::parse
			/// \see mob_container::parse
			static void parse(mob_door& obj, std::unique_ptr<archive_reader>& ctx, game_version version);
		};
	} // namespace vobs
} // namespace phoenix
