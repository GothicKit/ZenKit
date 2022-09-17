// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/model_script.hh>

#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>

#include <optional>

namespace phoenix::mds {
	namespace dsl = lexy::dsl;

	namespace sinks {
		struct dsl_script_body_sink {
			using return_type = model_script;

			struct _sink {
				using return_type = model_script;
				model_script _result;

				void operator()(skeleton&& skel) {
					_result.skeleton.name = std::move(skel.name);
					_result.skeleton.disable_mesh = skel.disable_mesh;
				}

				void operator()(std::string&& mesh) {
					_result.meshes.push_back(mesh);
				}

				void operator()(model_script&& mesh) {
					_result.animations = std::move(mesh.animations);
					_result.blends = std::move(mesh.blends);
					_result.aliases = std::move(mesh.aliases);
					_result.combinations = std::move(mesh.combinations);
					_result.disabled_animations = std::move(mesh.disabled_animations);
					_result.model_tags = std::move(mesh.model_tags);
				}

				void operator()(lexy::nullopt) {}

				void operator()(model_script&& mesh, lexy::nullopt) {
					this->operator()(std::move(mesh));
				}

				model_script&& finish() && {
					return LEXY_MOV(_result);
				}
			};

			model_script operator()(lexy::nullopt&&) const {
				return model_script {};
			}

			model_script operator()(lexy::nullopt&&, lexy::nullopt&&) const {
				return model_script {};
			}

			model_script operator()(model_script&& s, lexy::nullopt&&) const {
				return s;
			}

			[[nodiscard]] auto sink() const {
				return _sink {model_script {}};
			}
		};

		struct dsl_ani_enum_sink {
			using return_type = model_script;

			struct _sink {
				using return_type = model_script;
				model_script _result;

				void operator()(std::string&& disable) {
					_result.disabled_animations.push_back(disable);
				}
				void operator()(animation&& a) {
					_result.animations.push_back(a);
				}

				void operator()(animation_alias&& a) {
					_result.aliases.push_back(a);
				}

				void operator()(animation_blending&& a) {
					_result.blends.push_back(a);
				}

				void operator()(animation_combination&& a) {
					_result.combinations.push_back(a);
				}

				void operator()(model_tag&& a) {
					_result.model_tags.push_back(a);
				}

				void operator()(lexy::nullopt&&) {}

				model_script&& finish() && {
					return LEXY_MOV(_result);
				}
			};

			model_script operator()(lexy::nullopt&&) const {
				return model_script {};
			}

			model_script operator()(lexy::nullopt&&, lexy::nullopt&&) const {
				return model_script {};
			}

			model_script operator()(model_script&& s, lexy::nullopt&&) const {
				return s;
			}

			[[nodiscard]] auto sink() const {
				return _sink {model_script {}};
			}
		};

		struct dsl_ani_event_sink {
			using return_type = animation;

			struct _sink {
				using return_type = animation;
				animation _result;

				void operator()(lexy::nullopt&&) {}

				void operator()(event_tag&& a) {
					_result.events.push_back(a);
				}

				void operator()(event_sfx&& a) {
					_result.sfx.push_back(a);
				}

				void operator()(event_sfx_ground&& a) {
					_result.sfx_ground.push_back(a);
				}

				void operator()(event_pfx&& a) {
					_result.pfx.push_back(a);
				}

				void operator()(event_pfx_stop&& a) {
					_result.pfx_stop.push_back(a);
				}

				void operator()(event_morph_animate&& a) {
					_result.morph.push_back(a);
				}

				void operator()(event_camera_tremor&& a) {
					_result.tremors.push_back(a);
				}

				animation&& finish() && {
					return LEXY_MOV(_result);
				}
			};

			animation operator()(lexy::nullopt&&) const {
				return animation {};
			}

			[[nodiscard]] auto sink() const {
				return _sink {animation {}};
			}
		};
	} // namespace sinks

	static constexpr auto ws = LEXY_LIT("//") >> dsl::until(dsl::newline) | dsl::ascii::space;

	struct dsl_string {
		static constexpr auto rule = dsl::quoted(-dsl::lit_c<'"'>);
		static constexpr auto value = lexy::as_string<std::string>;
	};

	struct dsl_integer {
		static constexpr auto rule = dsl::integer<int32_t>;
		static constexpr auto value = lexy::forward<int32_t>;
	};

	struct dsl_float {
		static constexpr auto rule =
		    dsl::capture(dsl::token(dsl::digits<> + dsl::opt(dsl::lit_c<'.'> >> dsl::digits<>)));
		static constexpr auto value =
		    lexy::as_string<std::string> | lexy::callback<float>([](std::string&& v) { return std::stof(v); });
	};

	struct dsl_ani_flags {
		static constexpr auto rule =
		    dsl::capture(dsl::token(dsl::list(dsl::lit_c<'M'> / dsl::lit_c<'R'> / dsl::lit_c<'E'> / dsl::lit_c<'F'> /
		                                      dsl::lit_c<'I'> / dsl::lit_c<'.'>)));
		static constexpr auto value = lexy::as_string<std::string> |
		    lexy::callback<animation_flags>([](std::string&& v) { return animation_flags_from_string(v); });
	};

	event_tag make_event_tag(int32_t, std::string&&, std::optional<std::string>&&, std::optional<std::string>&&, bool);

	struct dsl_event_morph {
		static constexpr auto whitespace = ws;
		static constexpr auto rule =
		    dsl::round_bracketed(dsl::p<dsl_integer> + dsl::p<dsl_string> + dsl::opt(dsl::p<dsl_string>));
		static constexpr auto value = lexy::callback<event_morph_animate>([](int32_t frame,
		                                                                     std::string&& animation,
		                                                                     std::optional<std::string>&& node) {
			return event_morph_animate {.frame = frame, .animation = std::move(animation), .node = node.value_or("")};
		});
	};

	struct dsl_event_camera_tremor {
		static constexpr auto whitespace = ws;
		static constexpr auto rule =
		    dsl::round_bracketed(dsl::p<dsl_integer> + dsl::p<dsl_integer> + dsl::p<dsl_integer> + dsl::p<dsl_integer> +
		                         dsl::p<dsl_integer>);
		static constexpr auto value = lexy::callback<event_camera_tremor>(
		    [](int32_t frame, int32_t field1, int32_t field2, int32_t field3, int32_t field4) {
			    return event_camera_tremor {.frame = frame,
			                                .field1 = field1,
			                                .field2 = field2,
			                                .field3 = field3,
			                                .field4 = field4};
		    });
	};

	struct dsl_event_pfx_stop {
		static constexpr auto whitespace = ws;
		static constexpr auto rule = dsl::round_bracketed(dsl::p<dsl_integer> + dsl::p<dsl_integer>);
		static constexpr auto value = lexy::callback<event_pfx_stop>(
		    [](int32_t frame, int32_t index) { return event_pfx_stop {.frame = frame, .index = index}; });
	};

	struct dsl_event_pfx {
		static constexpr auto whitespace = ws;
		static constexpr auto rule =
		    dsl::round_bracketed(dsl::p<dsl_integer> + dsl::opt(dsl::p<dsl_integer>) + dsl::p<dsl_string> +
		                         dsl::p<dsl_string> + dsl::opt(LEXY_LIT("ATTACH")));
		static constexpr auto value = lexy::callback<event_pfx>(
		    [](int32_t frame, std::optional<int32_t> index, std::string&& name, std::string&& position) {
			    return event_pfx {.frame = frame,
			                      .index = index.value_or(0),
			                      .name = std::move(name),
			                      .position = std::move(position),
			                      .attached = true};
		    },
		    [](int32_t frame, std::optional<int32_t> index, std::string&& name, std::string&& position, lexy::nullopt) {
			    return event_pfx {.frame = frame,
			                      .index = index.value_or(0),
			                      .name = std::move(name),
			                      .position = std::move(position),
			                      .attached = false};
		    });
	};

	struct dsl_event_sfx_grnd {
		static constexpr auto whitespace = ws;
		static constexpr auto rule = dsl::round_bracketed(dsl::p<dsl_integer> + dsl::p<dsl_string>);
		static constexpr auto value = lexy::callback<event_sfx_ground>(
		    [](int32_t frame, std::string&& name) { return event_sfx_ground {.frame = frame, .name = name}; });
	};

	struct dsl_event_sfx {
		static constexpr auto whitespace = ws;
		static constexpr auto rule =
		    dsl::round_bracketed(dsl::p<dsl_integer> + dsl::p<dsl_string> +
		                         dsl::opt(LEXY_LIT("R:") >> dsl::p<dsl_float>) + dsl::opt(LEXY_LIT("EMPTY_SLOT")));
		static constexpr auto value = lexy::callback<event_sfx>(
		    [](int32_t frame, std::string&& name, std::optional<float> range) {
			    return event_sfx {.frame = frame, .name = name, .range = range.value_or(1000), .empty_slot = true};
		    },
		    [](int32_t frame, std::string&& name, std::optional<float> range, lexy::nullopt) {
			    return event_sfx {.frame = frame, .name = name, .range = range.value_or(1000), .empty_slot = false};
		    });
	};

	struct dsl_event_tag {
		static constexpr auto whitespace = ws;
		static constexpr auto rule =
		    dsl::round_bracketed(dsl::p<dsl_integer> + dsl::p<dsl_string> + dsl::opt(dsl::p<dsl_string>) +
		                         dsl::opt(dsl::p<dsl_string>) + dsl::opt(LEXY_LIT("ATTACH")));
		static constexpr auto value = lexy::callback<event_tag>(
		    [](int32_t frame, std::string&& a, std::optional<std::string>&& b, std::optional<std::string>&& c) {
			    return make_event_tag(frame, std::move(a), std::move(b), std::move(c), true);
		    },
		    [](int32_t frame,
		       std::string&& a,
		       std::optional<std::string>&& b,
		       std::optional<std::string>&& c,
		       lexy::nullopt) { return make_event_tag(frame, std::move(a), std::move(b), std::move(c), false); });
	};

	struct dsl_ani_events {
		static constexpr auto whitespace = ws;
		static constexpr auto rule = dsl::opt(dsl::list(
		    LEXY_LIT("*eventTag") >> dsl::p<dsl_event_tag> | LEXY_LIT("*eventSFXGrnd") >> dsl::p<dsl_event_sfx_grnd> |
		    LEXY_LIT("*eventSFX") >> dsl::p<dsl_event_sfx> | LEXY_LIT("*eventPFXStop") >> dsl::p<dsl_event_pfx_stop> |
		    LEXY_LIT("*eventPFX") >> dsl::p<dsl_event_pfx> | LEXY_LIT("*eventMMStartAni") >> dsl::p<dsl_event_morph> |
		    LEXY_LIT("*eventCamTremor") >> dsl::p<dsl_event_camera_tremor>));
		static constexpr auto value = sinks::dsl_ani_event_sink {};
	};

	struct dsl_ani {
		static constexpr auto whitespace = ws;
		static constexpr auto rule =
		    dsl::round_bracketed(dsl::p<dsl_string> + dsl::p<dsl_integer> + dsl::p<dsl_string> + dsl::p<dsl_float> +
		                         dsl::p<dsl_float> + dsl::p<dsl_ani_flags> + dsl::p<dsl_string> +
		                         dsl::capture(dsl::token(dsl::lit_c<'F'> | dsl::lit_c<'R'>)) + dsl::p<dsl_integer> +
		                         dsl::p<dsl_integer> + dsl::opt(LEXY_LIT("FPS:") >> dsl::p<dsl_float>) +
		                         dsl::opt(LEXY_LIT("CVS:") >> dsl::p<dsl_float>)) +
		    dsl::opt(dsl::curly_bracketed(dsl::p<dsl_ani_events>));
		static constexpr auto value = lexy::callback<animation>(
		    [](std::string&& name,
		       int32_t layer,
		       std::string&& next,
		       float blend_in,
		       float blend_out,
		       animation_flags flags,
		       std::string&& model,
		       auto direction,
		       int32_t first_frame,
		       int32_t last_frame,
		       std::optional<float> fps,
		       std::optional<float> cvs,
		       animation&& _enum) {
			    return animation {.name = std::move(name),
			                      .layer = static_cast<uint32_t>(layer),
			                      .next = std::move(next),
			                      .blend_in = blend_in,
			                      .blend_out = blend_out,
			                      .flags = flags,
			                      .model = std::move(model),
			                      .direction = *direction.begin() == 'R' ? animation_direction::backward
			                                                             : animation_direction::forward,
			                      .first_frame = first_frame,
			                      .last_frame = last_frame,
			                      .fps = fps.value_or(25.0f),
			                      .speed = 0,
			                      .collision_volume_scale = cvs.value_or(1),
			                      .events = std::move(_enum.events),
			                      .pfx = std::move(_enum.pfx),
			                      .pfx_stop = std::move(_enum.pfx_stop),
			                      .sfx = std::move(_enum.sfx),
			                      .sfx_ground = std::move(_enum.sfx_ground),
			                      .morph = std::move(_enum.morph),
			                      .tremors = std::move(_enum.tremors)};
		    },
		    [](std::string&& name,
		       int32_t layer,
		       std::string&& next,
		       float blend_in,
		       float blend_out,
		       animation_flags flags,
		       std::string&& model,
		       auto direction,
		       int32_t first_frame,
		       int32_t last_frame,
		       std::optional<float> fps,
		       std::optional<float> cvs,
		       lexy::nullopt) {
			    return animation {
			        .name = std::move(name),
			        .layer = static_cast<uint32_t>(layer),
			        .next = std::move(next),
			        .blend_in = blend_in,
			        .blend_out = blend_out,
			        .flags = flags,
			        .model = std::move(model),
			        .direction =
			            *direction.begin() == 'R' ? animation_direction::backward : animation_direction::forward,
			        .first_frame = first_frame,
			        .last_frame = last_frame,
			        .fps = fps.value_or(25.0f),
			        .speed = 0,
			        .collision_volume_scale = cvs.value_or(1),
			    };
		    });
	};

	struct dsl_ani_blend {
		static constexpr auto whitespace = ws;
		static constexpr auto rule =
		    dsl::round_bracketed(dsl::p<dsl_string> + dsl::opt(dsl::p<dsl_integer>) + dsl::p<dsl_string> +
		                         dsl::opt(dsl::p<dsl_float>) + dsl::opt(dsl::p<dsl_float>));
		static constexpr auto value = lexy::callback<animation_blending>([](std::string&& name,
		                                                                    std::optional<int32_t>,
		                                                                    std::string&& next,
		                                                                    std::optional<float> blend_in,
		                                                                    std::optional<float> blend_out) {
			return animation_blending {
			    .name = std::move(name),
			    .next = std::move(next),
			    .blend_in = blend_in.value_or(0),
			    .blend_out = blend_out.value_or(0),
			};
		});
	};

	struct dsl_ani_alias {
		static constexpr auto whitespace = ws;
		static constexpr auto rule =
		    dsl::round_bracketed(dsl::p<dsl_string> + dsl::p<dsl_integer> + dsl::p<dsl_string> + dsl::p<dsl_float> +
		                         dsl::p<dsl_float> + dsl::p<dsl_ani_flags> + dsl::p<dsl_string> +
		                         dsl::opt(dsl::capture(dsl::token(dsl::lit_c<'F'> | dsl::lit_c<'R'>))));
		static constexpr auto value = lexy::callback<animation_alias>(
		    [](std::string&& name,
		       int32_t layer,
		       std::string&& next,
		       float blend_in,
		       float blend_out,
		       animation_flags flags,
		       std::string&& alias,
		       auto direction) {
			    return animation_alias {.name = std::move(name),
			                            .layer = static_cast<uint32_t>(layer),
			                            .next = std::move(next),
			                            .blend_in = blend_in,
			                            .blend_out = blend_out,
			                            .flags = flags,
			                            .alias = std::move(alias),
			                            .direction = *direction.begin() == 'R' ? animation_direction::backward
			                                                                   : animation_direction::forward};
		    },
		    [](std::string&& name,
		       int32_t layer,
		       std::string&& next,
		       float blend_in,
		       float blend_out,
		       animation_flags flags,
		       std::string&& alias,
		       lexy::nullopt) {
			    return animation_alias {.name = std::move(name),
			                            .layer = static_cast<uint32_t>(layer),
			                            .next = std::move(next),
			                            .blend_in = blend_in,
			                            .blend_out = blend_out,
			                            .flags = flags,
			                            .alias = std::move(alias),
			                            .direction = animation_direction::forward};
		    });
	};

	struct dsl_ani_comb {
		static constexpr auto whitespace = ws;
		static constexpr auto rule =
		    dsl::round_bracketed(dsl::p<dsl_string> + dsl::p<dsl_integer> + dsl::p<dsl_string> + dsl::p<dsl_float> +
		                         dsl::p<dsl_float> + dsl::p<dsl_ani_flags> + dsl::p<dsl_string> + dsl::p<dsl_integer>);
		static constexpr auto value = lexy::callback<animation_combination>([](std::string&& name,
		                                                                       int32_t layer,
		                                                                       std::string&& next,
		                                                                       float blend_in,
		                                                                       float blend_out,
		                                                                       animation_flags flags,
		                                                                       std::string&& model,
		                                                                       int32_t last_frame) {
			return animation_combination {
			    .name = std::move(name),
			    .layer = static_cast<uint32_t>(layer),
			    .next = std::move(next),
			    .blend_in = blend_in,
			    .blend_out = blend_out,
			    .flags = flags,
			    .model = std::move(model),
			    .last_frame = last_frame,
			};
		});
	};

	struct dsl_ani_disable {
		static constexpr auto whitespace = ws;
		static constexpr auto rule = dsl::round_bracketed(dsl::p<dsl_string>);
		static constexpr auto value = lexy::forward<std::string>;
	};

	struct dsl_model_tag {
		static constexpr auto whitespace = ws;
		static constexpr auto rule = dsl::round_bracketed(LEXY_LIT("\"DEF_HIT_LIMB\"") + dsl::p<dsl_string>);
		static constexpr auto value =
		    lexy::callback<model_tag>([](std::string&& name) { return model_tag {.bone = name}; });
	};

	struct dsl_ani_enum {
		static constexpr auto whitespace = ws;
		static constexpr auto rule = dsl::lit_c<'{'> +
		    dsl::opt(dsl::list(
		        LEXY_LIT("aniBlend") >> dsl::p<dsl_ani_blend> | LEXY_LIT("aniAlias") >> dsl::p<dsl_ani_alias> |
		        LEXY_LIT("aniComb") >> dsl::p<dsl_ani_comb> | LEXY_LIT("aniDisable") >> dsl::p<dsl_ani_disable> |
		        LEXY_LIT("ani") >> dsl::p<dsl_ani> | LEXY_LIT("modelTag") >> dsl::p<dsl_model_tag>)) +
		    dsl::opt(dsl::lit_c<'}'>);
		static constexpr auto value = sinks::dsl_ani_enum_sink {};
	};

	struct dsl_skeleton {
		static constexpr auto whitespace = ws;
		static constexpr auto rule = dsl::round_bracketed(dsl::p<dsl_string> + dsl::opt(LEXY_LIT("DONT_USE_MESH")));
		static constexpr auto value = lexy::callback<skeleton>(
		    [](std::string&& name) {
			    skeleton sk {};
			    sk.name = std::forward<std::string>(name);
			    sk.disable_mesh = true;
			    return sk;
		    },
		    [](std::string&& name, lexy::nullopt) {
			    skeleton sk {};
			    sk.name = std::forward<std::string>(name);
			    sk.disable_mesh = false;
			    return sk;
		    });
	};

	struct dsl_mesh {
		static constexpr auto whitespace = ws;
		static constexpr auto rule = dsl::round_bracketed(dsl::p<dsl_string>);
		static constexpr auto value = lexy::as_string<std::string>;
	};

	struct dsl_script_body {
		static constexpr auto whitespace = ws;
		static constexpr auto rule = dsl::lit_c<'{'> +
		    dsl::opt(dsl::list(
		        LEXY_LIT("meshAndTree") >> dsl::p<dsl_skeleton> | LEXY_LIT("MeshAndTree") >> dsl::p<dsl_skeleton> |
		        LEXY_LIT("registerMesh") >> dsl::p<dsl_mesh> | LEXY_LIT("RegisterMesh") >> dsl::p<dsl_mesh> |
		        LEXY_LIT("aniEnum") >> dsl::p<dsl_ani_enum>)) +
		    dsl::opt(dsl::lit_c<'}'>);
		static constexpr auto value = sinks::dsl_script_body_sink {};
	};

	struct d_script {
		static constexpr auto whitespace = ws;
		static constexpr auto rule =
		    LEXY_LIT("Model") + dsl::round_bracketed(dsl::p<dsl_string>) + dsl::p<dsl_script_body>;

		static constexpr auto value =
		    lexy::callback<model_script>([](std::string&&, model_script&& script) { return script; });
	};
} // namespace phoenix::mds
