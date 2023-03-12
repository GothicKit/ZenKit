// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/vobs/vob.hh>

namespace phoenix {
	/// \brief Describes how a sound should be played when the player enters its trigger volume.
	enum class sound_mode : uint32_t {
		loop = 0,   ///< The sound should be player forever until the player exits the trigger volume.
		once = 1,   ///< The sound should be played once when the player enters the trigger volume.
		random = 2, ///< While the player is in the trigger volume, the should should play randomly.
	};

	/// \brief The trigger volume type for sounds.
	enum class sound_trigger_volume : uint32_t {
		/// \brief The sound is triggered when the player enters a spherical area around the VOb
		///        indicated by its radius setting.
		spherical = 0,

		/// \brief The sound is triggered when the player enters a ellipsoidal area around the VOb
		///        indicated by its radius setting.
		ellipsoidal = 1,
	};

	namespace vobs {
		/// \brief A VOb which emits a sound.
		struct sound : public vob {
			float volume {0};
			sound_mode mode {sound_mode::once};
			float random_delay {0};
			float random_delay_var {0};
			bool initially_playing {false};
			bool ambient3d {false};
			bool obstruction {true};
			float cone_angle {0};
			sound_trigger_volume volume_type {sound_trigger_volume::spherical};
			float radius {0};
			std::string sound_name {};

			// Save-game only variables
			bool s_is_running;
			bool s_is_allowed_to_run;

			/// \brief Parses a sound VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(sound& obj, archive_reader& ctx, game_version version);
		};

		/// \brief A VOb which emits a sound only during certain times of the day.
		struct sound_daytime : public sound {
			float start_time {0};
			float end_time {0};
			std::string sound_name2 {};

			/// \brief Parses a timed sound VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws parser_error if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(sound_daytime& obj, archive_reader& ctx, game_version version);
		};
	} // namespace vobs
} // namespace phoenix