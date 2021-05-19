// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/animation.hh>
#include <phoenix/archive.hh>

#include <glm/mat3x3.hpp>

#include <memory>
#include <variant>


namespace phoenix {
	/**
	 * @brief All possible types of VOBs.
	 *
	 * Directly copied from Try's fork of ZenLib:
	 * https://github.com/Try/ZenLib/blob/732077c82589f5060d1762839293b996c8222c18/zenload/zenParser.h#L48
	 */
	enum class vob_type {
		zUnknown = 0,
		zReference,
		zCCSLib,
		zCCSBlock,
		zCCSAtomicBlock,
		oCCSTrigger,
		zCAICamera,
		zCMoverController,
		zCVobScreenFX,
		oCMsgConversation,
		zCDecal,
		zCProgMeshProto,
		zCParticleFX,
		zCMesh,
		zCModel,
		zCMorphMesh,
		zCWorld,
		zCWayNet,
		zCWaypoint,
		zCVob,
		zCVobLevelCompo,
		zCVobStair,
		oCItem,
		zCTrigger,
		oCMOB,
		oCMobInter,
		oCMobBed,
		oCMobFire,
		oCMobLadder,
		oCMobSwitch,
		oCMobWheel,
		oCMobContainer,
		oCMobDoor,
		zCPFXController,
		zCVobAnimate,
		zCVobLensFlare,
		zCVobLight,
		zCVobSpot,
		zCVobStartpoint,
		zCVobSound,
		zCVobSoundDaytime,
		oCZoneMusic,
		oCZoneMusicDefault,
		zCZoneZFog,
		zCZoneZFogDefault,
		zCZoneVobFarPlane,
		zCZoneVobFarPlaneDefault,
		zCMessageFilter,
		zCCodeMaster,
		zCTriggerList,
		oCTriggerScript,
		zCMover,
		oCTriggerChangeLevel,
		zCTriggerWorldStart,
		zCCSCamera,
		zCCamTrj_KeyFrame,
		oCTouchDamage,
		zCTriggerUntouch,
		zCEarthquake,
	};

	namespace vob {
		struct empty {
		};

		struct item {
			std::string instance;
		};

		struct pfx_controller {
			std::string name;
			bool kill_when_done;
			bool initially_running;
		};

		struct light {
			std::string preset;
			u32 type;
			float range;
			color colour;
			float cone_angle;
			bool is_static;
			u32 quality;
			std::string lensflare_fx;

			bool on;
			std::string range_animation_scale;
			float range_animation_fps;
			bool range_animation_smooth;
			std::string color_animation_list;
			float color_animation_fps;
			float color_animation_smooth;
			bool can_move;
		};

		struct sound {
			float volume;
			u32 mode;
			float random_delay;
			float random_delay_var;
			bool initially_playing;
			bool is_ambient3d;
			bool obstruction;
			float cone_angle;
			u32 volume_type;
			float radius;
			std::string name;
		};

		struct sound_daytime : public sound {
			float start_time;
			float end_time;
			std::string name2;
		};

		struct zone_music {
			bool enabled;
			u32 priority;
			bool ellipsoid;
			float reverb;
			float volume;
			bool loop;
		};

		struct message_filter {
			std::string target;
			u8 on_trigger;
			u8 on_untrigger;
		};

		struct code_master {
			std::string target;
			bool ordered;
			bool first_false_is_failure;
			std::string failure_target;
			bool untriggered_cancels;
			std::vector<std::string> slaves;
		};

		struct trigger {
			std::string target;
			u8 flags;
			u8 filter_flags;
			std::string vob_target;
			s32 max_activation_count;
			float retrigger_delay_sec;
			float damage_threshold;
			float fire_delay_sec;
		};

		struct trigger_list : public trigger {
			struct target {
				std::string name;
				float delay;
			};

			s32 list_process;
			std::vector<target> targets;
		};

		struct trigger_script : public trigger {
			std::string function;
		};

		struct trigger_change_level : public trigger {
			std::string level_name;
			std::string start_vob;
		};

		struct trigger_world_start {
			std::string target;
			bool fire_once;
		};

		struct trigger_untouch {
			std::string target;
		};

		struct mover_controller {
			std::string target;
			u8 message;
			s32 key;
		};

		struct touch_damage {
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
			s32 damage_type;
		};

		struct mob {
			std::string name;
			s32 hp;
			s32 damage;
			bool movable;
			bool takable;
			bool focus_override;
			u32 material;
			std::string visual_destroyed;
			std::string owner;
			std::string owner_guild;
			bool destroyed;
		};

		struct mob_inter : public mob {
			s32 state;
			std::string target;
			std::string item;
			std::string condition_function;
			std::string on_state_change_function;
			bool rewind;
		};

		struct mob_fire : public mob_inter {
			std::string slot;
			std::string vob_tree;
		};

		struct mob_container : public mob_inter {
			bool locked;
			std::string key;
			std::string pick_string;
			std::string contents;
		};

		struct mob_door : public mob_inter {
			bool locked;
			std::string key;
			std::string pick_string;
		};

		struct trigger_mover : public trigger {
			u32 behavior;
			float touch_blocker_damage;
			float stay_open_time_sec;
			bool locked;
			bool auto_link;
			bool auto_rotate;

			float speed;
			u32 lerp_mode;
			u32 speed_mode;

			std::vector<animation_sample> keyframes;

			std::string sfx_open_start;
			std::string sfx_open_end;
			std::string sfx_transitioning;
			std::string sfx_close_start;
			std::string sfx_close_end;
			std::string sfx_lock;
			std::string sfx_unlock;
			std::string sfx_use_locked;
		};
	}// namespace vob


	/**
	 * @brief Represents ZenGin's VOB tree.
	 * @note This is not currently fully finished.
	 */
	class vob_tree {
	public:
		/**
		 * @brief Parses a VOB tree from the given reader.
		 * @param in The reader to read from.
		 * @param version The version of Gothic being used.
		 * @return The tree parsed.
		 */
		static vob_tree parse(archive_reader_ref& in, game_version version);

		/**
		 * @brief Retrieves the extra content associated with the VOB.
		 * @tparam T The type of data to get.
		 * @return The data.
		 */
		template <typename T>
		inline const T& get() { return std::get<T>(_m_content); }

		/**
		 * @return A list of child VOBs.
		 */
		[[nodiscard]] inline const std::vector<vob_tree>& children() const noexcept { return _m_children; }

		/**
		 * @return The bounding box of the VOB as a (min, max) tuple.
		 */
		[[nodiscard]] inline std::tuple<glm::vec3, glm::vec3> bbox() const noexcept { return std::make_tuple(_m_bbox[0], _m_bbox[1]); }

		/**
		 * @return The position of the VOB
		 */
		[[nodiscard]] inline glm::vec3 position() const noexcept { return _m_position; }

		/**
		 * @return The rotation of the VOB
		 */
		[[nodiscard]] inline glm::mat3x3 rotation() const noexcept { return _m_rotation; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline bool show_visual() const noexcept { return _m_show_visual; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline u8 camera_alignment() const noexcept { return _m_camera_alignment; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline u8 animation_mode() const noexcept { return _m_animation_mode; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline s32 bias() const noexcept { return _m_bias; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline f32 animation_strength() const noexcept { return _m_animation_strength; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline f32 far_clip_scale() const noexcept { return _m_far_clip_scale; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline u8 dynamic_shadows() const noexcept { return _m_dynamic_shadows; }

		/**
		 * @return The name of the preset being used
		 */
		[[nodiscard]] inline const std::string& preset_name() const noexcept { return _m_preset_name; }

		/**
		 * @return The name of the VOB
		 */
		[[nodiscard]] inline const std::string& vob_name() const noexcept { return _m_vob_name; }

		/**
		 * @return The name of the attached visual
		 */
		[[nodiscard]] inline const std::string& visual_name() const noexcept { return _m_visual_name; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline bool is_cd_static() const noexcept { return _m_cd_static; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline bool is_cd_dynamic() const noexcept { return _m_cd_dynamic; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline bool is_vob_static() const noexcept { return _m_vob_static; }

		/**
		 * @return `true` if physics should be activated for the VOB
		 */
		[[nodiscard]] inline bool is_physics_enabled() const noexcept { return _m_physics_enabled; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline bool is_ambient() const noexcept { return _m_ambient; }

		/**
		 * @return The type of the VOB.
		 */
		[[nodiscard]] inline vob_type type() const noexcept { return _m_type; }

	private:
		std::vector<vob_tree> _m_children;
		std::variant<
				vob::empty,
				vob::item,
				vob::pfx_controller,
				vob::light,
				vob::sound,
				vob::sound_daytime,
				vob::zone_music,
				vob::message_filter,
				vob::code_master,
				vob::trigger,
				vob::trigger_list,
				vob::trigger_script,
				vob::trigger_change_level,
				vob::trigger_world_start,
				vob::trigger_untouch,
				vob::mover_controller,
				vob::touch_damage,
				vob::mob,
				vob::mob_inter,
				vob::mob_fire,
				vob::mob_container,
				vob::mob_door,
				vob::trigger_mover>
				_m_content;

		vob_type _m_type;
		glm::vec3 _m_bbox[2];
		glm::vec3 _m_position;
		glm::mat3x3 _m_rotation;
		bool _m_show_visual;
		u8 _m_camera_alignment;
		bool _m_cd_static;
		bool _m_cd_dynamic;
		bool _m_vob_static;
		bool _m_dynamic_shadows;
		bool _m_physics_enabled;
		u8 _m_animation_mode;
		s32 _m_bias;
		bool _m_ambient;
		float _m_animation_strength;
		float _m_far_clip_scale;

		std::string _m_preset_name;
		std::string _m_vob_name;
		std::string _m_visual_name;
	};


}// namespace phoenix
