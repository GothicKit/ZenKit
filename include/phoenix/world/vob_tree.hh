// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
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

	enum class visual_type {
		zCDecal,
		zCMesh,
		zCProgMeshProto,
		zCParticleFX,
		zCAICamera,
		zCModel,
		zCMorphMesh,
		unknown
	};

	enum class animation_mode : uint8_t {
		none = 0,
		wind = 1,
		wind2 = 2,
	};

	enum class sound_mode : uint32_t {
		loop = 0,
		once = 1,
		random = 2,
	};

	enum class sound_volume_type : uint32_t {
		spherical = 0,
		ellipsoidal = 1,
	};

	enum class message_filter_action : uint32_t {
		none = 0,
		trigger = 1,
		untrigger = 2,
		enable = 3,
		disable = 4,
		toggle = 5,
	};

	enum class mover_behavior : uint32_t {
		toggle = 0,
		trigger_control = 1,
		open_timed = 2,
		loop = 3,
		single_keys = 4,
	};

	enum class mover_lerp_mode : uint32_t {
		curve = 0,
		linear = 1,
	};

	enum class mover_speed_mode : uint32_t {
		seg_constant = 0,
		slow_start_end = 1,
		slow_start = 2,
		slow_end = 3,
		seg_slow_start_end = 4,
		seg_slow_start = 5,
		seg_slow_end = 6,
	};

	enum class mover_message_type : uint32_t {
		fixed_direct = 0,
		fixed_order = 1,
		next = 2,
		previous = 3,
	};

	enum class trigger_list_process_type {
		all = 0,
		next = 1,
		random = 2,
	};

	namespace vobs {
		struct decal {
			std::string name;
			glm::vec2 dimension;
			glm::vec2 offset;
			bool two_sided;
			alpha_function alpha_func;
			float texture_anim_fps;
			std::uint8_t alpha_weight;
			bool ignore_daylight;

			static std::unique_ptr<decal> parse(archive_reader_ref& in, game_version version) {
				auto vob = std::make_unique<decal>();
				vob->name = in->read_string();                                  // name
				vob->dimension = in->read_vec2();                               // decalDim
				vob->offset = in->read_vec2();                                  // decalOffset
				vob->two_sided = in->read_bool();                               // decal2Sided
				vob->alpha_func = static_cast<alpha_function>(in->read_enum()); // decalAlphaFunc
				vob->texture_anim_fps = in->read_float();                       // decalTexAniFPS

				if (version == game_version::gothic_2) {
					vob->alpha_weight = in->read_byte();    // decalAlphaWeight
					vob->ignore_daylight = in->read_bool(); // ignoreDayLight
				}

				return vob;
			}
		};

		struct light_preset {
			std::string preset;
			std::uint32_t type;
			float range;
			glm::u8vec4 color;
			float cone_angle;
			bool is_static;
			std::uint32_t quality;
			std::string lensflare_fx;

			bool on;
			std::vector<float> range_animation_scale;
			float range_animation_fps;
			bool range_animation_smooth;
			std::vector<glm::u8vec4> color_animation_list;
			float color_animation_fps;
			bool color_animation_smooth;
			bool can_move;

			static std::unique_ptr<light_preset> parse(archive_reader_ref& in, game_version version);
		};

		struct vob {
			vob_type type;
			uint32_t id;

			bounding_box bbox {};
			glm::vec3 position {};
			glm::mat3x3 rotation {};
			bool show_visual {};
			std::uint8_t camera_alignment {};
			bool cd_static {};
			bool cd_dynamic {};
			bool vob_static {};
			bool dynamic_shadows {};
			bool physics_enabled {};
			animation_mode anim_mode {};
			std::int32_t bias {};
			bool ambient {};
			float anim_strength {};
			float far_clip_scale {};

			std::string preset_name {};
			std::string vob_name {};
			std::string visual_name {};

			visual_type associated_visual_type {};
			std::unique_ptr<decal> visual_decal {};

			std::vector<std::unique_ptr<vob>> children {};

			virtual ~vob() = default;
			static void parse(vob* vob, archive_reader_ref& in, game_version version);
		};

		struct animate : public vob {
			bool start_on;

			static void parse(animate* vob, archive_reader_ref& in, game_version version) {
				vob::parse(vob, in, version);
				vob->start_on = in->read_bool(); // startOn
			}
		};

		struct item : public vob {
			std::string instance;

			static void parse(item* vob, archive_reader_ref& in, game_version version) {
				vob::parse(vob, in, version);
				vob->instance = in->read_string(); // itemInstance
			}
		};

		struct lens_flare : public vob {
			std::string fx;

			static void parse(lens_flare* vob, archive_reader_ref& in, game_version version) {
				vob::parse(vob, in, version);
				vob->fx = in->read_string(); // lensflareFX
			}
		};

		struct pfx_controller : public vob {
			std::string pfx_name;
			bool kill_when_done;
			bool initially_running;

			static void parse(pfx_controller* vob, archive_reader_ref& in, game_version version) {
				vob::parse(vob, in, version);
				vob->pfx_name = in->read_string();        // pfxName
				vob->kill_when_done = in->read_bool();    // killVobWhenDone
				vob->initially_running = in->read_bool(); // pfxStartOn
			}
		};

		struct light : public vob {
			std::string preset;
			std::uint32_t light_type;
			float range;
			glm::u8vec4 color;
			float cone_angle;
			bool is_static;
			std::uint32_t quality;
			std::string lensflare_fx;

			bool on {false};
			std::vector<float> range_animation_scale;
			float range_animation_fps {0};
			bool range_animation_smooth {true};
			std::vector<glm::u8vec4> color_animation_list;
			float color_animation_fps {0};
			bool color_animation_smooth {true};
			bool can_move {true};

			static void parse(light* vob, archive_reader_ref& in, game_version version);
		};

		struct sound : public vob {
			float volume;
			sound_mode mode;
			float random_delay;
			float random_delay_var;
			bool initially_playing;
			bool ambient3d;
			bool obstruction;
			float cone_angle;
			sound_volume_type volume_type;
			float radius;
			std::string sound_name;

			static void parse(sound* vob, archive_reader_ref& in, game_version version);
		};

		struct sound_daytime : public sound {
			float start_time;
			float end_time;
			std::string sound_name2;

			static void parse(sound_daytime* vob, archive_reader_ref& in, game_version version) {
				sound::parse(vob, in, version);
				vob->start_time = in->read_float();   // sndStartTime
				vob->end_time = in->read_float();     // sndEndTime
				vob->sound_name2 = in->read_string(); // sndName2
			}
		};

		struct zone_music : public vob {
			bool enabled;
			std::int32_t priority;
			bool ellipsoid;
			float reverb;
			float volume;
			bool loop;

			static void parse(zone_music* vob, archive_reader_ref& in, game_version version);
		};

		struct zone_far_plane : public vob {
			float vob_far_plane_z;
			float inner_range_percentage;

			static void parse(zone_far_plane* vob, archive_reader_ref& in, game_version version) {
				vob::parse(vob, in, version);
				vob->vob_far_plane_z = in->read_float();        // vobFarPlaneZ
				vob->inner_range_percentage = in->read_float(); // innerRangePerc
			}
		};

		struct zone_fog : public vob {
			float range_center;
			float inner_range_percentage;
			glm::u8vec4 color;
			bool fade_out_sky {false};
			bool override_color {false};

			static void parse(zone_fog* vob, archive_reader_ref& in, game_version version) {
				vob::parse(vob, in, version);
				vob->range_center = in->read_float();           // fogRangeCenter
				vob->inner_range_percentage = in->read_float(); // innerRangePerc
				vob->color = in->read_color();                  // fogColor

				if (version == game_version::gothic_2) {
					vob->fade_out_sky = in->read_bool();   // fadeOutSky
					vob->override_color = in->read_bool(); // overrideColor
				}
			}
		};

		struct message_filter : public vob {
			std::string target;
			message_filter_action on_trigger;
			message_filter_action on_untrigger;

			static void parse(message_filter* vob, archive_reader_ref& in, game_version version) {
				vob::parse(vob, in, version);
				vob->target = in->read_string();                                         // triggerTarget
				vob->on_trigger = static_cast<message_filter_action>(in->read_enum());   // onTrigger
				vob->on_untrigger = static_cast<message_filter_action>(in->read_enum()); // onUntrigger
			}
		};

		struct code_master : public vob {
			std::string target;
			bool ordered;
			bool first_false_is_failure;
			std::string failure_target;
			bool untriggered_cancels;
			std::vector<std::string> slaves;

			static void parse(code_master* vob, archive_reader_ref& in, game_version version);
		};

		struct trigger : public vob {
			std::string target;
			std::uint8_t flags;
			std::uint8_t filter_flags;
			std::string vob_target;
			std::int32_t max_activation_count;
			float retrigger_delay_sec;
			float damage_threshold;
			float fire_delay_sec;

			static void parse(trigger* vob, archive_reader_ref& in, game_version version);
		};

		struct trigger_list : public trigger {
			struct target {
				std::string name;
				float delay;
			};

			trigger_list_process_type list_process;
			std::vector<target> targets;

			static void parse(trigger_list* vob, archive_reader_ref& in, game_version version) {
				trigger::parse(vob, in, version);
				vob->list_process = static_cast<trigger_list_process_type>(in->read_enum()); // listProcess

				auto target_count = in->read_byte(); // numTarget
				for (int i = 0; i < target_count; ++i) {
					vob->targets.emplace_back(target {
					    in->read_string(), // triggerTarget[i]
					    in->read_float()   // fireDelay[i]
					});
				}
			}
		};

		struct trigger_script : public trigger {
			std::string function;

			static void parse(trigger_script* vob, archive_reader_ref& in, game_version version) {
				trigger::parse(vob, in, version);
				vob->function = in->read_string(); // scriptFunc
			}
		};

		struct trigger_change_level : public trigger {
			std::string level_name;
			std::string start_vob;

			static void parse(trigger_change_level* vob, archive_reader_ref& in, game_version version) {
				trigger::parse(vob, in, version);
				vob->level_name = in->read_string(); // levelName
				vob->start_vob = in->read_string();  // startVobName
			}
		};

		struct trigger_world_start : public vob {
			std::string target;
			bool fire_once;

			static void parse(trigger_world_start* vob, archive_reader_ref& in, game_version version) {
				vob::parse(vob, in, version);
				vob->target = in->read_string();  // triggerTarget
				vob->fire_once = in->read_bool(); // fireOnlyFirstTime
			}
		};

		struct trigger_untouch : public vob {
			std::string target;

			static void parse(trigger_untouch* vob, archive_reader_ref& in, game_version version) {
				vob::parse(vob, in, version);
				vob->target = in->read_string(); // triggerTarget
			}
		};

		struct mover_controller : public vob {
			std::string target;
			mover_message_type message;
			std::int32_t key;

			static void parse(mover_controller* vob, archive_reader_ref& in, game_version version) {
				vob::parse(vob, in, version);
				vob->target = in->read_string(); // triggerTarget

				if (version == game_version::gothic_1) {
					vob->message = static_cast<mover_message_type>(in->read_enum()); // moverMessage
				} else {
					vob->message = static_cast<mover_message_type>(in->read_byte()); // moverMessage
				}

				vob->key = in->read_int(); // gotoFixedKey
			}
		};

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
			std::uint32_t damage_type;

			static void parse(touch_damage* vob, archive_reader_ref& in, game_version version);
		};

		struct mob : public vob {
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

			static void parse(mob* vob, archive_reader_ref& in, game_version version);
		};

		struct mob_inter : public mob {
			std::int32_t state;
			std::string target;
			std::string item;
			std::string condition_function;
			std::string on_state_change_function;
			bool rewind;

			static void parse(mob_inter* vob, archive_reader_ref& in, game_version version);
		};

		struct mob_fire : public mob_inter {
			std::string slot;
			std::string vob_tree;

			static void parse(mob_fire* vob, archive_reader_ref& in, game_version version) {
				mob_inter::parse(vob, in, version);
				vob->slot = in->read_string();     // fireSlot
				vob->vob_tree = in->read_string(); // fireVobtreeName
			}
		};

		struct mob_container : public mob_inter {
			bool locked;
			std::string key;
			std::string pick_string;
			std::string contents;

			static void parse(mob_container* vob, archive_reader_ref& in, game_version version);
		};

		struct mob_door : public mob_inter {
			bool locked;
			std::string key;
			std::string pick_string;

			static void parse(mob_door* vob, archive_reader_ref& in, game_version version) {
				mob_inter::parse(vob, in, version);
				vob->locked = in->read_bool();        // locked
				vob->key = in->read_string();         // keyInstance
				vob->pick_string = in->read_string(); // pickLockStr
			}
		};

		struct trigger_mover : public trigger {
			mover_behavior behavior;
			float touch_blocker_damage;
			float stay_open_time_sec;
			bool locked;
			bool auto_link;
			bool auto_rotate;

			float speed;
			mover_lerp_mode lerp_mode;
			mover_speed_mode speed_mode;

			std::vector<animation_sample> keyframes;

			std::string sfx_open_start;
			std::string sfx_open_end;
			std::string sfx_transitioning;
			std::string sfx_close_start;
			std::string sfx_close_end;
			std::string sfx_lock;
			std::string sfx_unlock;
			std::string sfx_use_locked;

			static void parse(trigger_mover* vob, archive_reader_ref& in, game_version version);
		};

		struct earthquake : public vob {
			float radius;
			float duration;
			glm::vec3 amplitude;

			static void parse(earthquake* vob, archive_reader_ref& in, game_version version) {
				vob::parse(vob, in, version);
				vob->radius = in->read_float();   // radius
				vob->duration = in->read_float(); // timeSec
				vob->amplitude = in->read_vec3(); // amplitudeCM
			}
		};

		struct camera_trj_frame : public vob {
			float time;
			float roll_angle;
			float fov_scale;
			std::uint32_t motion_type;
			std::uint32_t motion_type_fov;
			std::uint32_t motion_type_roll;
			std::uint32_t motion_type_time_scale;
			float tension;
			float cam_bias;
			float continuity;
			float time_scale;
			bool time_fixed;
			glm::mat4x4 original_pose;

			static std::unique_ptr<camera_trj_frame> parse(archive_reader_ref& in, game_version version);
		};

		struct cs_camera : public vob {
			std::uint32_t trajectory_for;
			std::uint32_t target_trajectory_for;
			std::uint32_t loop_mode;
			std::uint32_t lerp_mode;
			bool ignore_for_vob_rotation;
			bool ignore_for_vob_rotation_target;
			bool adapt;
			bool ease_first;
			bool ease_last;
			float total_duration;
			std::string auto_focus_vob;
			bool auto_player_movable;
			bool auto_untrigger_last;
			float auto_untrigger_last_delay;
			std::int32_t position_count;
			std::int32_t target_count;

			std::vector<std::unique_ptr<camera_trj_frame>> frames;

			static void parse(cs_camera* vob, archive_reader_ref& in, game_version version);
		};
	} // namespace vobs

	/**
	 * @brief Parses a VOB tree from the given reader.
	 * @param in The reader to read from.
	 * @param version The version of Gothic being used.
	 * @return The tree parsed.
	 */
	std::unique_ptr<vobs::vob> parse_vob_tree(archive_reader_ref& in, game_version version);
} // namespace phoenix
