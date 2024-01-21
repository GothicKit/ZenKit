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

	/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#damageCollType
	enum class TouchCollisionType : std::uint32_t {
		/// \brief Disable collision detection and thus damage application.
		/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#damageCollType
		NONE = 0,

		/// \brief Scale the bounding box of the VTouchDamage object by VTouchDamage#volume_scale and
		///        use this new bounding box to calculate collisions with other VObjects.
		/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#damageCollType
		BOX = 1,

		/// \brief Scale the bounding box of the colliding object by VTouchDamage#volume_scale and only deal
		///        damage if it contains the center point of the `zCTouchDamage` object.
		///
		/// If the VirtualObject#visual of the `zCTouchDamage` object is a particle effect, test against
		/// every particle instead.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#damageCollType
		POINT = 2,

		// Deprecated entries.
		none ZKREM("renamed to TouchCollisionType::NONE") = NONE,
		box ZKREM("renamed to TouchCollisionType::BOX") = BOX,
		point ZKREM("renamed to TouchCollisionType::POINT") = POINT,
	};

	/// \brief A VObject used to create animated models.
	///
	/// The visual of such objects can either be a morph mesh or a model with a skeletal animation. The animation
	/// of these objects can be started and stopped using `OnTrigger` and `OnUntrigger` events.
	///
	/// \see https://zk.gothickit.dev/engine/objects/zCVobAnimate/
	struct VAnimate : VirtualObject {
		ZK_OBJECT(ObjectType::zCVobAnimate);

		/// \brief Whether to start this animation when loading the level.
		/// \see https://zk.gothickit.dev/engine/objects/zCVobAnimate/#startOn
		bool start_on {false};

		// Save-game only variables
		bool s_is_running {false};

		ZKREM("use ::load()") ZKAPI static void parse(VAnimate& obj, ReadArchive& ctx, GameVersion version);

		/// \brief Load this object from the given archive.
		/// \param r The archive to read from;
		/// \param version The version of the game the object was made for.
		ZKAPI void load(ReadArchive& r, GameVersion version) override;

		/// \brief Save this object to the given archive.
		/// \param w The archive to save to.
		/// \param version The version of the game to save for.
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
	};

	/// \brief Represents an item in the game world.
	///
	/// Items are special VObjects which are tied to a script instance which defines their attributes. They
	/// have physics enabled and can be targeted and picked up by the player.
	///
	/// \see https://zk.gothickit.dev/engine/objects/oCItem/
	struct VItem : VirtualObject {
		ZK_OBJECT(ObjectType::oCItem);

		/// \brief The name of the script instance representing the item.
		/// \see https://zk.gothickit.dev/engine/objects/oCItem/#itemInstance
		std::string instance;

		// Save-game only variables
		int s_amount;
		int s_flags;

		ZKREM("use ::load()") ZKAPI static void parse(VItem& obj, ReadArchive& ctx, GameVersion version);

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

	/// \brief Represents a lens flare effect.
	/// \see https://zk.gothickit.dev/engine/objects/zCVobLensFlare/
	struct VLensFlare : VirtualObject {
		ZK_OBJECT(ObjectType::zCVobLensFlare);

		/// \brief The name of the lens flare effect.
		///
		/// Must be one of the effects listed in `/_work/data/Presets/Lensflare.zen`.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCVobLensFlare/#lensflareFX
		std::string fx;

		ZKREM("use ::load()") ZKAPI static void parse(VLensFlare& obj, ReadArchive& ctx, GameVersion version);

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

	/// \brief A particle system in virtual space.
	///
	/// Its effect can be activated and deactivated using the `OnTrigger` and `OnUntrigger` events.
	///
	/// \see https://zk.gothickit.dev/engine/objects/zCPFXController/
	struct VParticleEffectController : VirtualObject {
		ZK_OBJECT(ObjectType::zCPFXController);

		/// \brief The name of the particle effect.
		///
		/// Corresponds to a `.ZEN` file containing the particle system definition.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCPFXController/#pfxName
		std::string pfx_name;

		/// \brief Disable the particle effect after it runs once.
		/// \see https://zk.gothickit.dev/engine/objects/zCPFXController/#killVobWhenDone
		bool kill_when_done;

		/// \brief Whether to start the particle effect when the level is loaded.
		/// \see https://zk.gothickit.dev/engine/objects/zCPFXController/#pfxStartOn
		bool initially_running;

		ZKREM("use ::load()")
		ZKAPI static void parse(VParticleEffectController& obj, ReadArchive& ctx, GameVersion version);

		/// \brief Load this object from the given archive.
		/// \param r The archive to read from;
		/// \param version The version of the game the object was made for.
		ZKAPI void load(ReadArchive& r, GameVersion version) override;

		/// \brief Save this object to the given archive.
		/// \param w The archive to save to.
		/// \param version The version of the game to save for.
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
	};

	/// \brief Transforms any incoming `OnTrigger` and `OnUntrigger` events before passing
	///        them on to the trigger target.
	/// \see https://zk.gothickit.dev/engine/objects/zCMessageFilter/
	struct VMessageFilter : VirtualObject {
		ZK_OBJECT(ObjectType::zCMessageFilter);

		/// \brief The name of the target VObject.
		///
		/// All `OnTrigger` and `OnUntrigger` messages received by this `zCMessageFilter` VObject are transformed
		/// according to #on_trigger and #on_untrigger and then passed on to the VObject with the name specified in
		/// this field.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCMessageFilter/#triggerTarget
		std::string target;

		/// \brief The type of event to emit to the #target if this VObject receives an `OnTrigger` message.
		/// \see https://zk.gothickit.dev/engine/objects/zCMessageFilter/#onTrigger
		MessageFilterAction on_trigger;

		/// \brief The type of event to emit to the #target if this VObject receives an `OnUntrigger` message.
		/// \see https://zk.gothickit.dev/engine/objects/zCMessageFilter/#onUntrigger
		MessageFilterAction on_untrigger;

		ZKREM("use ::load()") ZKAPI static void parse(VMessageFilter& obj, ReadArchive& ctx, GameVersion version);

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

	/// \brief `zCCodeMaster` VObjects keep a list of 'slave' VObjects and keep track of events received by them.
	///
	/// If the master receives an `OnTrigger` event from a slave, it remembers that it did. After it has receives an
	/// `OnTrigger` message from all slaves, it emits an `OnTrigger` event to the #target. Receiving or sending an
	/// `OnTrigger` event is also referred to as an 'activation' for this purpose.
	///
	/// \see https://zk.gothickit.dev/engine/objects/zCCodeMaster/
	struct VCodeMaster : VirtualObject {
		ZK_OBJECT(ObjectType::zCCodeMaster);

		/// \brief The name of the VObject to send an `OnTrigger` event to after all slaves have
		///        fired in the correct order.
		/// \see https://zk.gothickit.dev/engine/objects/zCCodeMaster/#triggerTarget
		std::string target;

		/// \brief Controls whether the master should keep track of the order it receives messages from its slaves.
		/// \see https://zk.gothickit.dev/engine/objects/zCCodeMaster/#orderRelevant
		bool ordered;

		/// \brief Controls when the slave activation sequence is considered to be incorrect.
		///
		/// If set to `true`, the sequence is considered to be incorrect as soon as a single out-of-order slave is
		/// activated. Otherwise, it is only considered incorrect after all slaves have fired at least once and they
		/// have been activated out-of-order.
		///
		/// \note Only relevant if #ordered is set to `true`.
		/// \see https://zk.gothickit.dev/engine/objects/zCCodeMaster/#firstFalseIsFailure
		bool first_false_is_failure;

		/// \brief The name of the VObject to emit an `OnTrigger` event to if the activation sequence fails.
		/// \note Only relevant if #ordered is set to `true`.
		/// \see https://zk.gothickit.dev/engine/objects/zCCodeMaster/#triggerTargetFailure
		std::string failure_target;

		/// \brief Controls whether slaves can emit `OnUntrigger` events to mark themselves as deactivated.
		/// \note Only relevant if #ordered is set to `false`.
		/// \see https://zk.gothickit.dev/engine/objects/zCCodeMaster/#untriggerCancels
		bool untriggered_cancels;

		/// \brief The names of the slave VObjects.
		/// \see https://zk.gothickit.dev/engine/objects/zCCodeMaster/#slaveVobName
		std::vector<std::string> slaves;

		// Save-game only variables
		uint8_t s_num_triggered_slaves;

		ZKREM("use ::load()") ZKAPI static void parse(VCodeMaster& obj, ReadArchive& ctx, GameVersion version);

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

	/// \brief A VObject used to control VMover objects of type MoverBehavior::SINGLE_KEYS specifically.
	///
	/// VMoverController objects react to incoming `OnTrigger` events to send a pre-configured event to the target
	/// mover object, controlling its animation. Mover controllers can make movers go to a specific keyframe in
	/// their animation or just step one keyframe back and forth at a time.
	///
	/// \see https://zk.gothickit.dev/engine/objects/zCMoverController/
	struct VMoverController : VirtualObject {
		ZK_OBJECT(ObjectType::zCMoverController);

		/// \brief The name of the VMover VOject that is controlled by this VMoverController.
		/// \see https://zk.gothickit.dev/engine/objects/zCMoverController/#triggerTarget
		std::string target;

		/// \brief The type of event to emit to the #target when this controller receives an `OnTrigger` event.
		/// \see https://zk.gothickit.dev/engine/objects/zCMoverController/#moverMessage
		MoverMessageType message;

		/// \brief The keyframe of the mover referred to by #target to move to.
		/// \note Only relevant if #message is MoverMessageType::FIXED_DIRECT or MoverMessageType::FIXED_ORDER.
		/// \see https://zk.gothickit.dev/engine/objects/zCMoverController/#gotoFixedKey
		std::int32_t key;

		ZKREM("use ::load()") ZKAPI static void parse(VMoverController& obj, ReadArchive& ctx, GameVersion version);

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

	/// \brief A VObject which damages other VObjects colliding with it.
	/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/
	struct VTouchDamage : VirtualObject {
		ZK_OBJECT(ObjectType::oCTouchDamage);

		/// \brief The amount of damage being dealt.
		/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#damage
		float damage;

		/// \brief `TRUE` — Deal barrier damage. `FALSE` — Do not deal barrier damage.
		/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#Barrier
		bool barrier;

		/// \brief `TRUE` — Deal blunt damage (blunt weapons). `FALSE` — Do not deal blunt damage.
		/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#Blunt
		bool blunt;

		/// \brief `TRUE` — Deal edge damage (sharp weapons). `FALSE` — Do not deal edge damage.
		/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#Edge
		bool edge;

		/// \brief TRUE` — Deal fire damage. `FALSE` — Do not deal fire damage.
		/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#Fire
		bool fire;

		/// \brief `TRUE` — Deal knockout damage. `FALSE` — Do not deal knockout damage.
		/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#Fly
		bool fly;

		/// \brief `TRUE` — Deal magic damage. `FALSE` — Do not deal magic damage.
		/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#Magic
		bool magic;

		/// \brief `TRUE` — Deal point damage (bows and crossbows). `FALSE` — Do not deal point damage.
		/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#Point
		bool point;

		/// \brief Deal fall damage. `FALSE` — Do not deal fall damage.
		/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#Fall
		bool fall;

		/// \brief The delay between damage ticks when applying continuous damage.
		///
		/// If set to `0`, only deals the damage once per collision. If set to a value greater than `0`, deals
		/// damage every `damageRepeatDelaySec` seconds.
		///
		/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#damageRepeatDelaySec
		float repeat_delay_sec;

		/// \brief A scale value used in conjunction with #collision.
		/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#damageVolDownScale
		float volume_scale;

		/// \brief The type of collision detection to use for damage calculations.
		/// \see https://zk.gothickit.dev/engine/objects/zCTouchDamage/#damageCollType
		TouchCollisionType collision;

		ZKREM("use ::load()") ZKAPI static void parse(VTouchDamage& obj, ReadArchive& ctx, GameVersion version);

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

	/// \brief A VObject used to create an earthquake effect.
	///
	/// An earthquake is represented by a shaking camera.
	///
	/// \see https://zk.gothickit.dev/engine/objects/zCEarthquake/
	struct VEarthquake : VirtualObject {
		ZK_OBJECT(ObjectType::zCEarthquake);

		/// \brief The radius of the earthquake effect
		/// \see https://zk.gothickit.dev/engine/objects/zCEarthquake/#radius
		float radius;

		/// \brief The duration of the earthquake effect in seconds.
		/// \see https://zk.gothickit.dev/engine/objects/zCEarthquake/#timeSec
		float duration;

		/// \brief The amplitude of the earthquake effect.
		/// \see https://zk.gothickit.dev/engine/objects/zCEarthquake/#amplitudeCM
		glm::vec3 amplitude;

		ZKREM("use ::load()") ZKAPI static void parse(VEarthquake& obj, ReadArchive& ctx, GameVersion version);

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

	struct VNpc : VirtualObject {
		ZK_OBJECT(ObjectType::oCNpc);

		static constexpr std::uint32_t attribute_count = 8;
		static constexpr std::uint32_t hcs_count = 4;
		static constexpr std::uint32_t missions_count = 5;
		static constexpr std::uint32_t aivar_count = 100;
		static constexpr std::uint32_t packed_count = 9;
		static constexpr std::uint32_t protection_count = 8;

		struct Talent : Object {
			static constexpr ObjectType TYPE = ObjectType::oCNpcTalent;

			int talent;
			int value;
			int skill;

			[[nodiscard]] ObjectType get_object_type() const override {
				return TYPE;
			}

			void load(ReadArchive& r, GameVersion version) override;
			void save(WriteArchive& w, GameVersion version) const override;
		};

		using talent ZKREM("renamed to Talent") = Talent;

		struct Slot {
			bool used;
			std::string name;
			std::shared_ptr<VItem> item {};
			bool in_inventory;
		};

		using slot ZKREM("renamed to Slot") = Slot;

		enum class NewsId {
			MURDER = 200,
			ATTACK = 195,
			THEFT = 190,
			DEFEAT = 185,
			NERVE = 180,
			INTERFERE = 175,
			HAS_DEFEATED = 170,
		};

		enum class NewsSpread {
			DONT_SPREAD = 0,
			FRIENDLY_TOWARDS_VICTIM = 1,
			FRIENDLY_TOWARDS_WITNESS = 2,
			FRIENDLY_TOWARDS_OFFENDER = 3,
			SAME_GUILD_VICTIM = 4,
		};

		struct News {
			bool told;
			float spread_time;
			NewsSpread spread_type;
			NewsId news_id;
			bool gossip;
			bool guild_victim;
			std::string witness_name;
			std::string offender_name;
			std::string victim_name;
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

		std::vector<std::shared_ptr<Talent>> talents;

		int fight_tactic;
		int fight_mode;
		bool wounded;
		bool mad;
		int mad_time;
		bool player;

		int attributes[attribute_count];
		int hit_chance[hcs_count];
		int missions[missions_count];

		std::string start_ai_state;
		int aivar[aivar_count];
		std::string script_waypoint;
		int attitude;
		int attitude_temp;
		int name_nr;
		bool move_lock;

		std::string packed[packed_count];
		std::vector<std::unique_ptr<News>> news;
		std::vector<std::shared_ptr<VItem>> items;
		std::vector<std::unique_ptr<Slot>> slots;

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

		std::shared_ptr<VirtualObject> carry_vob;
		std::shared_ptr<VirtualObject> enemy;

		/// \brief Parses an NPC VOb from the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws ParserError if parsing fails.
		/// \see vob::parse
		ZKREM("use ::load()") ZKAPI static void parse(VNpc& obj, ReadArchive& ctx, GameVersion version);

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

	/// \brief Screen effect VObjects are used to create special effects.
	///
	/// They are used for field-of-view changes, adding black bars for a cinematic feel to the
	/// game and other post-processing effects.
	///
	/// \see https://zk.gothickit.dev/engine/objects/zCVobScreenFX/
	struct VScreenEffect : VirtualObject {
		ZK_OBJECT(ObjectType::zCVobScreenFX)

		/// \brief Load this object from the given archive.
		/// \param r The archive to read from;
		/// \param version The version of the game the object was made for.
		ZKAPI void load(ReadArchive& r, GameVersion version) override;

		/// \brief Save this object to the given archive.
		/// \param w The archive to save to.
		/// \param version The version of the game to save for.
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
	};
} // namespace zenkit
