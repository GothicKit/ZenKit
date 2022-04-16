// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/animation.hh>
#include <phoenix/archive.hh>
#include <phoenix/detail/compat.hh>

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
		oCCSTrigger,
		zCMoverController,
		zCVobScreenFX,
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
		unknown,
	};

	namespace vob {
		struct base {
			glm::vec3 bbox[2];
			glm::vec3 position;
			glm::mat3x3 rotation;
			bool show_visual;
			std::uint8_t camera_alignment;
			bool cd_static;
			bool cd_dynamic;
			bool vob_static;
			bool dynamic_shadows;
			bool physics_enabled;
			std::uint8_t animation_mode;
			std::int32_t bias;
			bool ambient;
			float animation_strength;
			float far_clip_scale;

			std::string preset_name;
			std::string vob_name;
			std::string visual_name;

			static void parse(base& vob, archive_reader_ref& in, game_version version);
		};

		struct animate : public base {
			bool field1;

			static void parse(animate& vob, archive_reader_ref& in, game_version version) {
				base::parse(vob, in, version);
				vob.field1 = in->read_bool();
			}
		};

		struct item : public base {
			std::string instance;

			static void parse(item& vob, archive_reader_ref& in, game_version version) {
				base::parse(vob, in, version);
				vob.instance = in->read_string();
			}
		};

		struct pfx_controller : public base {
			std::string name;
			bool kill_when_done;
			bool initially_running;

			static void parse(pfx_controller& vob, archive_reader_ref& in, game_version version) {
				base::parse(vob, in, version);
				vob.name = in->read_string();
				vob.kill_when_done = in->read_bool();
				vob.initially_running = in->read_bool();
			}
		};

		struct light : public base {
			std::string preset;
			std::uint32_t type;
			float range;
			glm::u8vec4 colour;
			float cone_angle;
			bool is_static;
			std::uint32_t quality;
			std::string lensflare_fx;

			bool on;
			std::string range_animation_scale;
			float range_animation_fps;
			bool range_animation_smooth;
			std::string color_animation_list;
			float color_animation_fps;
			bool color_animation_smooth;
			bool can_move;

			static void parse(light& vob, archive_reader_ref& in, game_version version);
		};

		struct sound : public base {
			float volume;
			std::uint32_t mode;
			float random_delay;
			float random_delay_var;
			bool initially_playing;
			bool ambient3d;
			bool obstruction;
			float cone_angle;
			std::uint32_t volume_type;
			float radius;
			std::string name;

			static void parse(sound& vob, archive_reader_ref& in, game_version version);
		};

		struct sound_daytime : public sound {
			float start_time;
			float end_time;
			std::string name2;

			static void parse(sound_daytime& vob, archive_reader_ref& in, game_version version) {
				sound::parse(vob, in, version);
				vob.start_time = in->read_float();
				vob.end_time = in->read_float();
				vob.name2 = in->read_string();
			}
		};

		struct zone_music : public base {
			bool enabled;
			std::int32_t priority;
			bool ellipsoid;
			float reverb;
			float volume;
			bool loop;

			static void parse(zone_music& vob, archive_reader_ref& in, game_version version);
		};

		struct zone_far_plane : public base {
			float field1;
			float field2;

			static void parse(zone_far_plane& vob, archive_reader_ref& in, game_version version) {
				base::parse(vob, in, version);
				vob.field1 = in->read_float();
				vob.field2 = in->read_float();
			}
		};

		struct zone_fog : public base {
			float field1;
			float field2;
			glm::u8vec4 field3;
			bool field4 {false};
			bool field5 {false};

			static void parse(zone_fog& vob, archive_reader_ref& in, game_version version) {
				base::parse(vob, in, version);
				vob.field1 = in->read_float();
				vob.field2 = in->read_float();
				vob.field3 = in->read_color();

				if (version == game_version::gothic_2) {
					vob.field4 = in->read_bool();
					vob.field5 = in->read_bool();
				}
			}
		};

		struct message_filter : public base {
			std::string target;
			std::uint32_t on_trigger;
			std::uint32_t on_untrigger;

			static void parse(message_filter& vob, archive_reader_ref& in, game_version version) {
				base::parse(vob, in, version);
				vob.target = in->read_string();
				vob.on_trigger = in->read_enum();
				vob.on_untrigger = in->read_enum();
			}
		};

		struct code_master : public base {
			std::string target;
			bool ordered;
			bool first_false_is_failure;
			std::string failure_target;
			bool untriggered_cancels;
			std::vector<std::string> slaves;

			static void parse(code_master& vob, archive_reader_ref& in, game_version version);
		};

		struct trigger : public base {
			std::string target;
			std::uint8_t flags;
			std::uint8_t filter_flags;
			std::string vob_target;
			std::int32_t max_activation_count;
			float retrigger_delay_sec;
			float damage_threshold;
			float fire_delay_sec;

			static void parse(trigger& vob, archive_reader_ref& in, game_version version);
		};

		struct trigger_list : public trigger {
			struct target {
				std::string name;
				float delay;
			};

			std::uint32_t list_process;
			std::vector<target> targets;

			static void parse(trigger_list& vob, archive_reader_ref& in, game_version version) {
				trigger::parse(vob, in, version);
				vob.list_process = in->read_enum();

				auto target_count = in->read_byte();
				for (int i = 0; i < target_count; ++i) {
					vob.targets.emplace_back(target {in->read_string(), in->read_float()});
				}
			}
		};

		struct trigger_script : public trigger {
			std::string function;

			static void parse(trigger_script& vob, archive_reader_ref& in, game_version version) {
				trigger::parse(vob, in, version);
				vob.function = in->read_string();
			}
		};

		struct trigger_change_level : public trigger {
			std::string level_name;
			std::string start_vob;

			static void parse(trigger_change_level& vob, archive_reader_ref& in, game_version version) {
				trigger::parse(vob, in, version);
				vob.level_name = in->read_string();
				vob.start_vob = in->read_string();
			}
		};

		struct trigger_world_start : public base {
			std::string target;
			bool fire_once;

			static void parse(trigger_world_start& vob, archive_reader_ref& in, game_version version) {
				base::parse(vob, in, version);
				vob.target = in->read_string();
				vob.fire_once = in->read_bool();
			}
		};

		struct trigger_untouch : public base {
			std::string target;

			static void parse(trigger_untouch& vob, archive_reader_ref& in, game_version version) {
				base::parse(vob, in, version);
				vob.target = in->read_string();
			}
		};

		struct mover_controller : public base {
			std::string target;
			std::uint8_t message;
			std::int32_t key;

			static void parse(mover_controller& vob, archive_reader_ref& in, game_version version) {
				base::parse(vob, in, version);
				vob.target = in->read_string();
				vob.message = in->read_byte();
				vob.key = in->read_int();
			}
		};

		struct touch_damage : public base {
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
			std::uint32_t damage_type;

			static void parse(touch_damage& vob, archive_reader_ref& in, game_version version);
		};

		struct mob : public base {
			std::string name;
			std::int32_t hp;
			std::int32_t damage;
			bool movable;
			bool takable;
			bool focus_override;
			std::uint32_t material;
			std::string visual_destroyed;
			std::string owner;
			std::string owner_guild;
			bool destroyed;

			static void parse(mob& vob, archive_reader_ref& in, game_version version);
		};

		struct mob_inter : public mob {
			std::int32_t state;
			std::string target;
			std::string item;
			std::string condition_function;
			std::string on_state_change_function;
			bool rewind;

			static void parse(mob_inter& vob, archive_reader_ref& in, game_version version);
		};

		struct mob_fire : public mob_inter {
			std::string slot;
			std::string vob_tree;

			static void parse(mob_fire& vob, archive_reader_ref& in, game_version version) {
				mob_inter::parse(vob, in, version);
				vob.slot = in->read_string();
				vob.vob_tree = in->read_string();
			}
		};

		struct mob_container : public mob_inter {
			bool locked;
			std::string key;
			std::string pick_string;
			std::string contents;

			static void parse(mob_container& vob, archive_reader_ref& in, game_version version);
		};

		struct mob_door : public mob_inter {
			bool locked;
			std::string key;
			std::string pick_string;

			static void parse(mob_door& vob, archive_reader_ref& in, game_version version) {
				mob_inter::parse(vob, in, version);
				vob.locked = in->read_bool();
				vob.key = in->read_string();
				vob.pick_string = in->read_string();
			}
		};

		struct trigger_mover : public trigger {
			std::uint32_t behavior;
			float touch_blocker_damage;
			float stay_open_time_sec;
			bool locked;
			bool auto_link;
			bool auto_rotate;

			float speed;
			std::uint32_t lerp_mode;
			std::uint32_t speed_mode;

			std::vector<animation_sample> keyframes;

			std::string sfx_open_start;
			std::string sfx_open_end;
			std::string sfx_transitioning;
			std::string sfx_close_start;
			std::string sfx_close_end;
			std::string sfx_lock;
			std::string sfx_unlock;
			std::string sfx_use_locked;

			static void parse(trigger_mover& vob, archive_reader_ref& in, game_version version);
		};

		struct earthquake : public base {
			float field1;
			float field2;
			glm::vec3 field3;

			static void parse(earthquake& vob, archive_reader_ref& in, game_version version) {
				base::parse(vob, in, version);
				vob.field1 = in->read_float();
				vob.field2 = in->read_float();
				vob.field3 = in->read_vec3();
			}
		};

		struct camera_trj_frame : public base {
			float field1;
			float field2;
			float field3;
			std::uint32_t field4;
			std::uint32_t field5;
			std::uint32_t field6;
			std::uint32_t field7;
			float field8;
			float field9;
			float field10;
			float field11;
			bool field12;

			std::vector<std::uint8_t> blob;

			static void parse(camera_trj_frame& vob, archive_reader_ref& in, game_version version);
		};

		struct cs_camera : public base {
			std::uint32_t field1;
			std::uint32_t field2;
			std::uint32_t field3;
			std::uint32_t field4;
			bool field5;
			bool field6;
			bool field7;
			bool field8;
			bool field9;
			float field10;
			std::string field11;
			bool field12;
			bool field13;
			float field14;
			std::int32_t field15;
			std::int32_t field16;

			std::vector<camera_trj_frame> frames;

			static void parse(cs_camera& vob, archive_reader_ref& in, game_version version);
		};
	} // namespace vob

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
		inline const T& get() const {
			return std::get<T>(_m_content);
		}

		/**
		 * @return A list of child VOBs.
		 */
		[[nodiscard]] inline const std::vector<vob_tree>& children() const noexcept {
			return _m_children;
		}

	private:
		std::vector<vob_tree> _m_children;
		std::variant<vob::base,
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
		             vob::trigger_mover,
		             vob::animate,
		             vob::zone_fog,
		             vob::zone_far_plane,
		             vob::earthquake,
		             vob::cs_camera>
		    _m_content;

		vob_type _m_type;
	};

} // namespace phoenix
