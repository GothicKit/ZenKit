// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <glm/gtc/type_ptr.hpp>
#include <ranges>
#include <span>

#include "dump.hh"
#include "util/base64.hh"

static const char* EVENT_TYPE_NAMES[] = {
    "tag",
    "sfx",
    "sfxGround",
    "batchAnimation",
    "swapMesh",
    "heading",
    "pfx",
    "pfxGround",
    "pfxStop",
    "setMesh",
    "startAnimation",
    "tremor",
};

static constexpr const char* TEXTURE_FORMAT_NAMES[] = {
    "B8G8R8A8",
    "R8G8B8A8",
    "A8B8G8R8",
    "A8R8G8B8",
    "B8G8R8",
    "R8G8B8",
    "A4R4G4B4",
    "A1R5G5B5",
    "R5G6B5",
    "PAL8",
    "DXT1",
    "DXT2",
    "DXT3",
    "DXT4",
    "DXT5",
};

static constexpr const char* MATERIAL_GROUP_NAMES[] = {
    "unset",
    "metal",
    "stone",
    "wood",
    "earth",
    "water",
    "snow",
    "none",
};

static constexpr const char* ALPHA_FUNC_NAMES[] = {
    "test",
    "transparent",
    "additive",
    "multiply",
};

static constexpr const char* EVENT_TAG_TYPE_NAMES[] = {
    "unknown",      "createItem", "insertItem",     "removeItem",     "destroyItem", "placeItem",
    "exchangeItem", "fightMode",  "placeMunition",  "removeMunition", "drawSound",   "undrawSound",
    "swapMesh",     "drawTorch",  "inventoryTorch", "dropTorch",      "hitLimb",     "hitDirection",
    "damMultiply",  "parFrame",   "optFrame",       "hitEnd",         "window",
};

static constexpr const char* FIGHT_MODE_NAMES[] = {
    "fist",
    "oneHanded",
    "twoHanded",
    "bow",
    "crossbow",
    "magic",
    "none",
};

template <>
void dump_json<px::font>(const px::font& fnt) {
	uint8_t chr = 0;
	fmt::print(
	    R"({{"type": "font", "name": "{}", "glyphHeight": {}, "glyphs": [{}]}})",
	    fnt.name(),
	    fnt.height(),
	    fmt::join(fnt.glyphs() | std::views::transform([&chr](const px::glyph& glyph) {
		              return fmt::format(
		                  R"({{"char": "{}", "width": "{}", "uv": [{{"x": {}, "y": {}}}, {{"x": {}, "y": {}}}]}})",
		                  escape(static_cast<char>(chr++)),
		                  glyph.width,
		                  glyph.uv[0].x,
		                  glyph.uv[0].y,
		                  glyph.uv[1].x,
		                  glyph.uv[1].y);
	              }),
	              ","));
}

template <>
void dump_json<px::messages>(const px::messages& messages) {
	fmt::print(R"({{"type": "dialogs", "blocks": [{}]}})",
	           fmt::join(messages.blocks() | std::views::transform([](const px::message_block& block) {
		                     return fmt::format(
		                         R"({{"name": "{}", "atomicMessage": {{"name": "{}", "type": {}, "text": "{}"}}}})",
		                         block.name,
		                         block.message.name,
		                         block.message.type,
		                         block.message.text);
	                     }),
	                     ","));
}

template <>
void dump_json<px::animation>(const px::animation& ani) {
	fmt::print(
	    R"({{"name": "{}", "next": "{}", "layer": {}, "frames": {}, "framesPerSecond": {}, "framesPerSecondAlt": {}, )"
	    R"("bbox": [{{"x": {}, "y": {}, "z": {}}}, {{"x": {}, "y": {}, "z": {}}}], "checksum": {}, "samples": [{}], )"
	    R"("events": [{}], "nodeIndices": [{}], "sourcePath": "{}", "sourceScript": "{}"}})",
	    ani.name(),
	    ani.next(),
	    ani.layer(),
	    ani.frames(),
	    ani.frames_per_second(),
	    ani.frames_per_second_alt(),
	    ani.bbox().min.x,
	    ani.bbox().min.y,
	    ani.bbox().min.z,
	    ani.bbox().max.x,
	    ani.bbox().max.y,
	    ani.bbox().max.z,
	    ani.checksum(),
	    fmt::join(
	        ani.samples() | std::views::transform([](const px::animation_sample& sample) {
		        return fmt::format(
		            R"({{"position": {{"x": {}, "y": {}, "z": {}}}, "rotation": {{"x": {}, "y": {}, "z": {}, "w": {}}}}})",
		            sample.position.x,
		            sample.position.y,
		            sample.position.z,
		            sample.rotation.x,
		            sample.rotation.y,
		            sample.rotation.z,
		            sample.rotation.w);
	        }),
	        ","),
	    fmt::join(
	        ani.events() | std::views::transform([](const px::animation_event& evt) {
		        auto evt_type = static_cast<int>(evt.type);
		        return fmt::format(
		            R"({{"type": "{}", "no": {}, "tag": "{}", "content": [{}], "values": [{}], "probability": {}}})",
		            evt_type < 12 ? EVENT_TYPE_NAMES[evt_type] : std::to_string(evt_type),
		            evt.no,
		            evt.tag,
		            fmt::join(evt.content |
		                          std::views::transform([](const std::string& v) { return fmt::format("\"{}\"", v); }),
		                      ","),
		            fmt::join(evt.values, ","),
		            evt.probability);
	        }),
	        ","),
	    fmt::join(ani.node_indices(), ","),
	    escape(ani.source_path()),
	    escape(ani.source_script()));
}

template <>
void dump_json<px::model_hierachy>(const px::model_hierachy& obj) {
	fmt::print(R"({{"type": "skeleton", "rootTranslation": {{"x": {}, "y": {}, "z": {}}}, )"
	           R"("bbox": [{{"x": {}, "y": {}, "z": {}}}, {{"x": {}, "y": {}, "z": {}}}], )"
	           R"("collisionBbox": [{{"x": {}, "y": {}, "z": {}}}, {{"x": {}, "y": {}, "z": {}}}], "nodes": [{}]}})",
	           obj.root_translation().x,
	           obj.root_translation().y,
	           obj.root_translation().z,
	           obj.bbox().min.x,
	           obj.bbox().min.y,
	           obj.bbox().min.z,
	           obj.bbox().max.x,
	           obj.bbox().max.y,
	           obj.collision_bbox().max.z,
	           obj.collision_bbox().min.x,
	           obj.collision_bbox().min.y,
	           obj.collision_bbox().min.z,
	           obj.collision_bbox().max.x,
	           obj.collision_bbox().max.y,
	           obj.collision_bbox().max.z,
	           fmt::join(obj.nodes() | std::views::transform([](const px::model_hierarchy_node& node) {
		                     return fmt::format(R"({{"name": "{}", "parentIndex": {}, "transform": [{}]}})",
		                                        node.name,
		                                        node.parent_index,
		                                        fmt::join(std::span {glm::value_ptr(node.transform), 16}, ","));
	                     }),
	                     ","));
}

std::string format_texture_json(const px::texture& tex) {
	auto fmt = static_cast<int>(tex.format());
	return fmt::format(
	    R"({{"type": "texture", "format": {}, "width": {}, "height": {}, "refWidth": {}, "refHeight": {}, )"
	    R"("averageColor": {}, "mipmapCount": {}, "mipmaps": [{}]}})",
	    fmt < 15 ? TEXTURE_FORMAT_NAMES[fmt] : std::to_string(fmt),
	    tex.width(),
	    tex.height(),
	    tex.ref_width(),
	    tex.ref_height(),
	    tex.average_color(),
	    tex.mipmaps(),
	    fmt::join(std::views::iota(0u, tex.mipmaps()) | std::views::transform([&tex](int i) {
		              auto data = tex.as_rgba8(i);
		              return fmt::format(R"({{"width": {}, "height": {}, "data": "{}"}})",
		                                 tex.mipmap_height(i),
		                                 tex.mipmap_width(i),
		                                 base64_encode(data.data(), data.size()));
	              }),
	              ","));
}

template <>
void dump_json<px::texture>(const px::texture& tex) {
	fmt::print("{}", format_texture_json(tex));
}

std::string format_obb_json(const px::obb& bb) {
	return fmt::format(
	    R"({{"center": {{"x": {}, "y": {}, "z": {}}}, "axes": [{{"x": {}, "y": {}, "z": {}}}, )"
	    R"({{"x": {}, "y": {}, "z": {}}}, {{"x": {}, "y": {}, "z": {}}}], "halfWidth": {{"x": {}, "y": {}, "z": {}}}, )"
	    R"("children": [{}]}})",
	    bb.center.x,
	    bb.center.y,
	    bb.center.z,
	    bb.axes[0].x,
	    bb.axes[0].y,
	    bb.axes[0].z,
	    bb.axes[1].x,
	    bb.axes[1].y,
	    bb.axes[1].z,
	    bb.axes[2].x,
	    bb.axes[2].y,
	    bb.axes[2].z,
	    bb.half_width.x,
	    bb.half_width.y,
	    bb.half_width.z,
	    fmt::join(bb.children | std::views::transform(format_obb_json), ","));
}

template <>
void dump_json<px::mesh>(const px::mesh& msh) {
	struct tm ctime {};
	ctime.tm_year = msh.date().year;
	ctime.tm_mon = msh.date().month;
	ctime.tm_mday = msh.date().day;
	ctime.tm_hour = msh.date().hour;
	ctime.tm_min = msh.date().minute;
	ctime.tm_sec = msh.date().second;
	std::time_t timestamp = std::mktime(&ctime);
	auto& polys = msh.polygons();

	fmt::print(
	    R"({{"type": "mesh", "name": "{}", "timestamp": {}, "aabb": [{{"x": {}, "y": {}, "z": {}}}, )"
	    R"({{"x": {}, "y": {}, "z": {}}}], "obb": [{}], "materials": [{}], "vertices": [{}], "features": [{}], )"
	    R"("polygons": [{}], "lightmaps": [{}]}})",
	    msh.name(),
	    timestamp,
	    msh.bbox().min.x,
	    msh.bbox().min.y,
	    msh.bbox().min.z,
	    msh.bbox().max.x,
	    msh.bbox().max.y,
	    msh.bbox().max.z,
	    format_obb_json(msh.oriented_bbox()),
	    fmt::join(
	        msh.materials() | std::views::transform([](const px::material& mat) {
		        auto mat_group = static_cast<int>(mat.group);
		        auto mat_alpha_func = static_cast<int>(mat.alpha_func);

		        return fmt::format(
		            R"({{"name": "{}", "group": {}, "color": {}, "smoothAngle": {}, "texture": "{}", )"
		            R"("textureScale": {{"x": {}, "y": {}}}, "textureAnimFps": {}, "textureAnimMapMode": {}, )"
		            R"("textureAnimMapDir": {{"x": {}, "y": {}}}, "disableCollision": {}, "disableLightmap": {}, )"
		            R"("dontCollapse": {}, "detailObject": "{}", "detailTextureScale": {}, "forceOccluder": {}, )"
		            R"("environmentMapping": {}, "environmentMappingStrength": {}, "waveMode": {}, "waveSpeed": {}, )"
		            R"("waveMaxAmplitude": {}, "waveGridSize": {}, "ignoreSun": {}, "alphaFunction": {}, )"
		            R"("defaultMapping": {{"x": {}, "y": {}}}}})",
		            mat.name,
		            mat_group < 8 ? MATERIAL_GROUP_NAMES[mat_group] : std::to_string(mat_group),
		            static_cast<uint32_t>(mat.color.a << 24 | mat.color.r << 16 | mat.color.g << 8 | mat.color.b),
		            mat.smooth_angle,
		            mat.texture,
		            mat.texture_scale.x,
		            mat.texture_scale.y,
		            mat.texture_anim_fps,
		            static_cast<int>(mat.texture_anim_map_mode),
		            mat.texture_anim_map_dir.x,
		            mat.texture_anim_map_dir.y,
		            mat.disable_collision,
		            mat.disable_lightmap,
		            mat.dont_collapse,
		            mat.detail_object,
		            mat.detail_texture_scale,
		            mat.force_occluder,
		            static_cast<int>(mat.environment_mapping),
		            mat.environment_mapping_strength,
		            static_cast<int>(mat.wave_mode),
		            static_cast<int>(mat.wave_speed),
		            mat.wave_max_amplitude,
		            mat.wave_grid_size,
		            mat.ignore_sun,
		            mat_alpha_func < 4 ? ALPHA_FUNC_NAMES[mat_alpha_func] : std::to_string(mat_alpha_func),
		            mat.default_mapping.x,
		            mat.default_mapping.y);
	        }),
	        ","),
	    fmt::join(msh.vertices() | std::views::transform([](const glm::vec3& v) {
		              return fmt::format("[{}, {}, {}]", v.x, v.y, v.z);
	              }),
	              ","),
	    fmt::join(msh.features() | std::views::transform([](const px::vertex_feature& feat) {
		              return fmt::format(R"({{"uv":[{},{}],"light":{},"normal":[{},{},{}]}})",
		                                 feat.texture.x,
		                                 feat.texture.y,
		                                 feat.light,
		                                 feat.normal.x,
		                                 feat.normal.y,
		                                 feat.normal.z);
	              }),
	              ","),
	    fmt::join(
	        std::views::iota(0u, msh.vertices().size() / 3) | std::views::transform([&polys](int i) {
		        return fmt::format(
		            R"({{"vertices": [{},{},{}], "features": [{},{},{}], "material": {}, "flags": {{"isPortal": {}, )"
		            R"("isOccluder": {}, "is_sector": {}, "shouldRelight": {}, "isOutdoor": {}, "isGhostOccluder": {}, )"
		            R"("isDynamicallyLit": {}, "sectorIndex": {}, "isLod": {}, "normalAxis": {}}}, "lightmap": {}}})",
		            polys.vertex_indices[i * 3],
		            polys.vertex_indices[i * 3 + 1],
		            polys.vertex_indices[i * 3 + 2],
		            polys.feature_indices[i * 3],
		            polys.feature_indices[i * 3 + 1],
		            polys.feature_indices[i * 3 + 2],
		            polys.material_indices[i],
		            polys.flags[i].is_portal,
		            polys.flags[i].is_occluder,
		            polys.flags[i].is_sector,
		            polys.flags[i].should_relight,
		            polys.flags[i].is_outdoor,
		            polys.flags[i].is_ghost_occluder,
		            polys.flags[i].is_dynamically_lit,
		            polys.flags[i].sector_index,
		            polys.flags[i].is_lod,
		            polys.flags[i].normal_axis,
		            polys.lightmap_indices[i]);
	        }),
	        ","),
	    fmt::join(msh.lightmaps() | std::views::transform([](const px::light_map& lm) {
		              return fmt::format(
		                  R"({{"texture": {}, "origin": {{"x": {}, "y": {}, "z": {}}}, )"
		                  R"("normals": [{{"x": {}, "y": {}, "z": {}}}, {{"x": {}, "y": {}, "z": {}}}]}})",
		                  format_texture_json(*lm.image),
		                  lm.origin.x,
		                  lm.origin.y,
		                  lm.origin.z,
		                  lm.normals[0].x,
		                  lm.normals[0].y,
		                  lm.normals[0].z,
		                  lm.normals[1].x,
		                  lm.normals[1].y,
		                  lm.normals[1].z);
	              }),
	              ","));
}

template <>
void dump_json<px::model_script>(const px::model_script& obj) {
	auto get_ani_flags = [](px::mds::animation_flags flag) {
		std::string flags {};

		if (flag & px::mds::af_move)
			flags += "\"move\"";
		if (flag & px::mds::af_rotate)
			flags += flags.empty() ? "\"rotate\"" : ", \"rotate\"";
		if (flag & px::mds::af_queue)
			flags += flags.empty() ? "\"queue\"" : ", \"queue\"";
		if (flag & px::mds::af_fly)
			flags += flags.empty() ? "\"fly\"" : ", \"fly\"";
		if (flag & px::mds::af_idle)
			flags += flags.empty() ? "\"idle\"" : ", \"idle\"";

		return flags;
	};

	fmt::print(
	    R"({{"type": "model_script", "skeleton": {{"name": "{}", "disableMesh": {}}}, "meshes": [{}], )"
	    R"("disabledAnimations": [{}], "combinations": [{}], "blends": [{}], "aliases": [{}], "modelTags": [{}], )"
	    R"("animations": [{}]}})",
	    obj.skeleton.name,
	    obj.skeleton.disable_mesh,
	    fmt::join(obj.meshes | std::views::transform([](const std::string& s) { return fmt::format("\"{}\"", s); }),
	              ","),
	    fmt::join(obj.disabled_animations |
	                  std::views::transform([](const std::string& s) { return fmt::format("\"{}\"", s); }),
	              ","),
	    fmt::join(obj.combinations | std::views::transform([&get_ani_flags](const px::mds::animation_combination& cmb) {
		              return fmt::format(
		                  R"({{"name": "{}", "layer": {}, "next": "{}", "blendIn": {}, "blendOut": {}, "flags": [{}], )"
		                  R"("model": "{}", "lastFrame": {}}})",
		                  cmb.name,
		                  cmb.layer,
		                  cmb.next,
		                  cmb.blend_in,
		                  cmb.blend_out,
		                  get_ani_flags(cmb.flags),
		                  cmb.model,
		                  cmb.last_frame);
	              }),
	              ","),
	    fmt::join(obj.blends | std::views::transform([](const px::mds::animation_blending& cmb) {
		              return fmt::format(R"({{"name": "{}", "next": "{}", "blendIn": {}, "blendOut": {}}})",
		                                 cmb.name,
		                                 cmb.next,
		                                 cmb.blend_in,
		                                 cmb.blend_out);
	              }),
	              ","),
	    fmt::join(obj.aliases | std::views::transform([&get_ani_flags](const px::mds::animation_alias& cmb) {
		              return fmt::format(
		                  R"({{"name": "{}", "layer": {}, "next": "{}", "blendIn": {}, "blendOut": {}, "flags": [{}], )"
		                  R"("alias": "{}", "direction": "{}"}})",
		                  cmb.name,
		                  cmb.layer,
		                  cmb.next,
		                  cmb.blend_in,
		                  cmb.blend_out,
		                  get_ani_flags(cmb.flags),
		                  cmb.alias,
		                  cmb.direction == px::mds::animation_direction::forward ? "forward" : "reversed");
	              }),
	              ","),
	    fmt::join(obj.model_tags | std::views::transform([](const px::mds::model_tag& cmb) {
		              return fmt::format(R"({{"bone": "{}"}})", cmb.bone);
	              }),
	              ","),
	    fmt::join(
	        obj.animations | std::views::transform([&get_ani_flags](const px::mds::animation& cmb) {
		        return fmt::format(
		            R"({{"name": "{}", "layer": {}, "next": "{}", "blendIn": {}, "blendOut": {}, "flags": [{}], )"
		            R"("model": "{}", "direction": "{}", "firstFrame": {}, "lastFrame": {}, "fps": {}, )"
		            R"("speed": {}, "collisionVolumeScale": {}, "events": [{}], "pfx": [{}], )"
		            R"("pfxStop": [{}], "sfx": [{}], "sfxGround": [{}], "morph": [{}], "tremors": [{}]}})",
		            cmb.name,
		            cmb.layer,
		            cmb.next,
		            cmb.blend_in,
		            cmb.blend_out,
		            get_ani_flags(cmb.flags),
		            cmb.model,
		            cmb.direction == px::mds::animation_direction::forward ? "forward" : "reversed",
		            cmb.first_frame,
		            cmb.last_frame,
		            cmb.fps,
		            cmb.speed,
		            cmb.collision_volume_scale,

		            fmt::join(cmb.events | std::views::transform([](const px::mds::event_tag& b) {
			                      auto type = static_cast<int>(b.type);
			                      auto fight_mode = static_cast<int>(b.fight_mode);
			                      return fmt::format(
			                          R"({{"frame": {}, "type": "{}", "slot": "{}", "slot2": "{}", "item": "{}", )"
			                          R"("frames": [{}], "fightMode": "{}", "attached": {}}})",
			                          b.frame,
			                          type < 23 ? EVENT_TAG_TYPE_NAMES[type] : std::to_string(type),
			                          b.slot,
			                          b.slot2,
			                          b.item,
			                          fmt::join(b.frames, ","),
			                          fight_mode < 7 ? FIGHT_MODE_NAMES[fight_mode] : std::to_string(fight_mode),
			                          b.attached);
		                      }),
		                      ","),

		            fmt::join(cmb.pfx | std::views::transform([](const px::mds::event_pfx& b) {
			                      return fmt::format(
			                          R"({{"frame": {}, "index": {}, "name": "{}", "position": "{}", "attached": {}}})",
			                          b.frame,
			                          b.index,
			                          b.name,
			                          b.position,
			                          b.attached);
		                      }),
		                      ","),

		            fmt::join(cmb.pfx_stop | std::views::transform([](const px::mds::event_pfx_stop& b) {
			                      return fmt::format(R"({{"frame": {}, "index": {}}})", b.frame, b.index);
		                      }),
		                      ","),

		            fmt::join(cmb.sfx | std::views::transform([](const px::mds::event_sfx& b) {
			                      return fmt::format(R"({{"frame": {}, "name": "{}", "range": {}, "emptySlot": {}}})",
			                                         b.frame,
			                                         b.name,
			                                         b.range,
			                                         b.empty_slot);
		                      }),
		                      ","),

		            fmt::join(cmb.sfx_ground | std::views::transform([](const px::mds::event_sfx_ground& b) {
			                      return fmt::format(R"({{"frame": {}, "name": "{}", "range": {}, "emptySlot": {}}})",
			                                         b.frame,
			                                         b.name,
			                                         b.range,
			                                         b.empty_slot);
		                      }),
		                      ","),

		            fmt::join(cmb.morph | std::views::transform([](const px::mds::event_morph_animate& b) {
			                      return fmt::format(R"({{"frame": {}, "animation": "{}", "node": "{}"}})",
			                                         b.frame,
			                                         b.animation,
			                                         b.node);
		                      }),
		                      ","),

		            fmt::join(cmb.tremors | std::views::transform([](const px::mds::event_camera_tremor& b) {
			                      return fmt::format(
			                          R"({{"frame": {}, "field1": {}, "field2": {}, "field3": {}, "field4": {}}})",
			                          b.frame,
			                          b.field1,
			                          b.field2,
			                          b.field3,
			                          b.field4);
		                      }),
		                      ","));
	        }),
	        ","));
}
