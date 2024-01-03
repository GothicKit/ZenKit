// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/Misc.hh"
#include "zenkit/vobs/VirtualObject.hh"

#include <cstdint>
#include <string>

namespace zenkit {
	/// \brief Describes how a sound should be played when the player enters its trigger volume.
	enum class SoundMode : uint32_t {
		LOOP = 0,   ///< The sound should be player forever until the player exits the trigger volume.
		ONCE = 1,   ///< The sound should be played once when the player enters the trigger volume.
		RANDOM = 2, ///< While the player is in the trigger volume, the should should play randomly.

		// Deprecated entries.
		loop ZKREM("renamed to SoundMode::LOOP") = LOOP,
		once ZKREM("renamed to SoundMode::ONCE") = ONCE,
		random ZKREM("renamed to SoundMode::RANDOM") = RANDOM,
	};

	/// \brief The trigger volume type for sounds.
	enum class SoundTriggerVolumeType : uint32_t {
		/// \brief The sound is triggered when the player enters a spherical area around the VOb
		///        indicated by its radius setting.
		SPHERICAL = 0,

		/// \brief The sound is triggered when the player enters a ellipsoidal area around the VOb
		///        indicated by its radius setting.
		ELLIPSOIDAL = 1,

		// Deprecated entries.
		spherical ZKREM("renamed to SoundTriggerVolume::SPHERICAL") = SPHERICAL,
		ellipsoidal ZKREM("renamed to SoundTriggerVolume::ELLIPSOIDAL") = ELLIPSOIDAL,
	};

	/// \brief Emits a sound defined by a script instance in a given radius.
	///
	/// The sound playback can be activated by sending an `OnTrigger` event to the VObject and
	/// sending an `OnUntrigger` event disables the sound playback.
	///
	/// \see https://zk.gothickit.dev/engine/objects/zCVobSound/
	struct VSound : VirtualObject {
		ZK_OBJECT(ObjectType::zCVobSound);

	public:
		/// \brief The volume of the sound effect in percent (0-100).
		/// \see https://zk.gothickit.dev/engine/objects/zCVobSound/#sndVolume
		float volume {0};

		/// \brief Determines how the sound effect is played.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobSound/#sndMode
		SoundMode mode {SoundMode::ONCE};

		/// \brief The average delay between repetitions in seconds if #mode is SoundMode::RANDOM.
		/// \note Only relevant if #mode is SoundMode::RANDOM.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobSound/#sndRandDelay
		float random_delay {0};

		/// \brief The maximum deviation of the random delay between repetitions if #mode is SoundMode::RANDOM.
		///
		/// The resulting delay will be a value between #random_delay ± #random_delay_var..
		///
		/// \note Only relevant if #mode is SoundMode::RANDOM.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobSound/#sndRandDelayVar
		float random_delay_var {0};

		/// \brief Whether to start the sound when loading the world.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobSound/#sndStartOn
		bool initially_playing {false};

		/// \brief Determines whether the sound is an omnidirectional ambient sound.
		///
		/// It will change volume depending on the distance to this VObject, but it won't sound as if this
		/// VObject is emitting the effect.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCVobSound/#sndAmbient3D
		bool ambient3d {false};

		/// \brief Whether to dampen the sound when there is no direct path from the VObject to the listener.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobSound/#sndObstruction
		bool obstruction {true};

		/// \brief When set to a value other than `0`, determines a cone in front of the VObject in which the sound can
		/// be heard. \see https://zk.gothickit.dev/engine/objects/zCVobSound/#sndConeAngle
		float cone_angle {0};

		/// \brief Determines the type of volume used to check whether the sound can be heard.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobSound/#sndVolType
		SoundTriggerVolumeType volume_type {SoundTriggerVolumeType::SPHERICAL};

		/// \brief The radius in which the sound can be heard.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobSound/#sndRadius
		float radius {0};

		/// \brief The name of the script instance describing the sound.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobSound/#sndName
		std::string sound_name {};

		// Save-game only variables
		bool s_is_running;
		bool s_is_allowed_to_run;

		ZKREM("use ::load()") ZKAPI static void parse(VSound& obj, ReadArchive& ctx, GameVersion version);

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

	/// \brief A VOb which emits a sound only during certain times of the day.
	struct VSoundDaytime : VSound {
		ZK_OBJECT(ObjectType::zCVobSoundDaytime);

	public:
		float start_time {0};
		float end_time {0};
		std::string sound_name2 {};

		/// \brief Parses a timed sound VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws ParserError if parsing fails.
		/// \see vob::parse
		ZKREM("use ::load()") ZKAPI static void parse(VSoundDaytime& obj, ReadArchive& ctx, GameVersion version);
		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
	};
} // namespace zenkit
