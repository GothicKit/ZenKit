// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <glm/gtc/type_ptr.hpp>

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
	fmt::print(R"({{"type": "font", "name": "{}", "glyphHeight": {}, "glyphs": [)", fnt.name(), fnt.height());

	for (int i = 0; i < fnt.glyphs().size(); ++i) {
		auto& glyph = fnt.glyphs()[i];
		if (i != 0) {
			fmt::print(",");
		}

		fmt::print(R"({{"char": "{}", "width": "{}", "uv": [{{"x": {}, "y": {}}}, {{"x": {}, "y": {}}}]}})",
		           escape(static_cast<char>(chr++)),
		           glyph.width,
		           glyph.uv[0].x,
		           glyph.uv[0].y,
		           glyph.uv[1].x,
		           glyph.uv[1].y);
	}

	fmt::print("]}})");
}

template <>
void dump_json<px::messages>(const px::messages& messages) {
	fmt::print(R"({{"type": "dialogs", "blocks": [)");

	for (int i = 0; i < messages.blocks().size(); ++i) {
		auto& block = messages.blocks()[0];
		if (i != 0) {
			fmt::print(",");
		}

		fmt::print(R"({{"name": "{}", "atomicMessage": {{"name": "{}", "type": {}, "text": "{}"}}}})",
		           block.name,
		           block.message.name,
		           block.message.type,
		           block.message.text);
	}

	fmt::print("]}})");
}

template <>
void dump_json<px::animation>(const px::animation& ani) {
	fmt::print(
	    R"({{"name": "{}", "next": "{}", "layer": {}, "frames": {}, "framesPerSecond": {}, "framesPerSecondAlt": {}, )"
	    R"("bbox": [{{"x": {}, "y": {}, "z": {}}}, {{"x": {}, "y": {}, "z": {}}}], "checksum": {}, "sourcePath": "{}", )"
	    R"(sourceScript": "{}", "samples": [)",
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
	    escape(ani.source_path()),
	    escape(ani.source_script()));

	for (int i = 0; i < ani.samples().size(); ++i) {
		auto& sample = ani.samples()[0];
		if (i != 0) {
			fmt::print(",");
		}

		fmt::print(
		    R"({{"position": {{"x": {}, "y": {}, "z": {}}}, "rotation": {{"x": {}, "y": {}, "z": {}, "w": {}}}}})",
		    sample.position.x,
		    sample.position.y,
		    sample.position.z,
		    sample.rotation.x,
		    sample.rotation.y,
		    sample.rotation.z,
		    sample.rotation.w);
	}

	fmt::print("], \"events\": [");

	for (int i = 0; i < ani.events().size(); ++i) {
		auto& evt = ani.events()[0];
		if (i != 0) {
			fmt::print(",");
		}

		auto evt_type = static_cast<int>(evt.type);
		fmt::print(R"({{"type": "{}", "no": {}, "tag": "{}", "probability": {}, "values": [{}], "content": [)",
		           evt_type < 12 ? EVENT_TYPE_NAMES[evt_type] : std::to_string(evt_type),
		           evt.no,
		           evt.tag,
		           fmt::join(evt.values, ","),
		           evt.probability);

		for (int i = 0; i < 4; ++i) {
			if (i != 0) {
				fmt::print(",");
			}

			fmt::print("\"{}\"", evt.content[i]);
		}

		fmt::print("]}}");
	}

	fmt::print(R"(], "nodeIndices": [{}]]}})", fmt::join(ani.node_indices(), ","));
}

template <>
void dump_json<px::model_hierarchy>(const px::model_hierarchy& obj) {
	fmt::print(R"({{"type": "skeleton", "rootTranslation": {{"x": {}, "y": {}, "z": {}}}, )"
	           R"("bbox": [{{"x": {}, "y": {}, "z": {}}}, {{"x": {}, "y": {}, "z": {}}}], )"
	           R"("collisionBbox": [{{"x": {}, "y": {}, "z": {}}}, {{"x": {}, "y": {}, "z": {}}}], "nodes": [)",
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
	           obj.collision_bbox().max.z);

	for (int i = 0; i < obj.nodes().size(); ++i) {
		auto& node = obj.nodes()[0];
		if (i != 0) {
			fmt::print(",");
		}

		fmt::print(R"({{"name": "{}", "parentIndex": {}}})", node.name, node.parent_index);
	}

	fmt::print("]}}");
}

std::string format_texture_json(const px::texture& tex) {
	auto fmt = static_cast<int>(tex.format());
	return fmt::format(
	    R"({{"type": "texture", "format": {}, "width": {}, "height": {}, "refWidth": {}, "refHeight": {}, )"
	    R"("averageColor": {}, "mipmapCount": {}, "mipmaps": [)",
	    fmt < 15 ? TEXTURE_FORMAT_NAMES[fmt] : std::to_string(fmt),
	    tex.width(),
	    tex.height(),
	    tex.ref_width(),
	    tex.ref_height(),
	    tex.average_color(),
	    tex.mipmaps());

	for (int i = 0; i < tex.mipmaps(); ++i) {
		auto data = tex.as_rgba8(i);
		if (i != 0) {
			fmt::print(",");
		}

		fmt::print(R"({{"width": {}, "height": {}, "data": "{}"}})",
		           tex.mipmap_height(i),
		           tex.mipmap_width(i),
		           base64_encode(data.data(), data.size()));
	}

	fmt::print("]}}");
}

template <>
void dump_json<px::texture>(const px::texture& tex) {
	fmt::print("{}", format_texture_json(tex));
}

std::string format_obb_json(const px::obb& bb) {
	std::string r = fmt::format(
	    R"({{"center": {{"x": {}, "y": {}, "z": {}}}, "axes": [{{"x": {}, "y": {}, "z": {}}}, )"
	    R"({{"x": {}, "y": {}, "z": {}}}, {{"x": {}, "y": {}, "z": {}}}], "halfWidth": {{"x": {}, "y": {}, "z": {}}}, )"
	    R"("children": [)",
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
	    bb.half_width.z);

	for (int i = 0; i < bb.children.size(); ++i) {
		if (i != 0) {
			r += ",";
		}

		r += format_obb_json(bb.children[i]);
	}

	return r + "]}}";
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

	fmt::print(R"({{"type": "mesh", "name": "{}", "timestamp": {}, "aabb": [{{"x": {}, "y": {}, "z": {}}}, )"
	           R"({{"x": {}, "y": {}, "z": {}}}], "obb": [{}], "materials": [)",
	           msh.name(),
	           timestamp,
	           msh.bbox().min.x,
	           msh.bbox().min.y,
	           msh.bbox().min.z,
	           msh.bbox().max.x,
	           msh.bbox().max.y,
	           msh.bbox().max.z,
	           format_obb_json(msh.oriented_bbox()));

	for (int i = 0; i < msh.materials().size(); ++i) {
		auto& mat = msh.materials()[i];

		if (i != 0) {
			fmt::print(",");
		}

		auto mat_group = static_cast<int>(mat.group);
		auto mat_alpha_func = static_cast<int>(mat.alpha_func);

		fmt::print(R"({{"name": "{}", "group": {}, "color": {}, "smoothAngle": {}, "texture": "{}", )"
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
	}

	fmt::print(R"(], "vertices": [)");

	for (int i = 0; i < msh.vertices().size(); ++i) {
		auto& v = msh.vertices()[i];

		if (i != 0) {
			fmt::print(",");
		}

		fmt::print("[{}, {}, {}]", v.x, v.y, v.z);
	}

	fmt::print(R"(], "features": [)");

	for (int i = 0; i < msh.features().size(); ++i) {
		auto& feat = msh.features()[i];

		if (i != 0) {
			fmt::print(",");
		}

		fmt::print(R"({{"uv":[{},{}],"light":{},"normal":[{},{},{}]}})",
		           feat.texture.x,
		           feat.texture.y,
		           feat.light,
		           feat.normal.x,
		           feat.normal.y,
		           feat.normal.z);
	}

	fmt::print(R"(], "polygons": [)");

	for (int i = 0; i < msh.vertices().size() / 3; ++i) {
		if (i != 0) {
			fmt::print(",");
		}

		fmt::print(R"({{"vertices": [{},{},{}], "features": [{},{},{}], "material": {}, "flags": {{"isPortal": {}, )"
		           R"("isOccluder": {}, "is_sector": {}, "shouldRelight": {}, "isOutdoor": {}, "isGhostOccluder": {},
)"
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
	}

	fmt::print(R"(], "lightmaps": [)");

	for (int i = 0; i < msh.lightmaps().size(); ++i) {
		auto& lm = msh.lightmaps()[i];

		if (i != 0) {
			fmt::print(",");
		}

		fmt::print(R"({{"texture": {}, "origin": {{"x": {}, "y": {}, "z": {}}}, )"
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
	}

	fmt::print("]}}");
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

	fmt::print(R"({{"type": "model_script", "skeleton": {{"name": "{}", "disableMesh": {}}}, "meshes": [)",
	           obj.skeleton.name,
	           obj.skeleton.disable_mesh);

	for (int i = 0; i < obj.meshes.size(); ++i) {
		auto& m = obj.meshes[i];

		if (i != 0) {
			fmt::print(",");
		}

		fmt::print("\"{}\"", m);
	}

	fmt::print(R"(], "disabledAnimations": [)");

	for (int i = 0; i < obj.disabled_animations.size(); ++i) {
		auto& m = obj.disabled_animations[i];

		if (i != 0) {
			fmt::print(",");
		}

		fmt::print("\"{}\"", m);
	}

	fmt::print(R"(], "combinations": [)");

	for (int i = 0; i < obj.combinations.size(); ++i) {
		auto& cmb = obj.combinations[i];

		if (i != 0) {
			fmt::print(",");
		}

		fmt::print(R"({{"name": "{}", "layer": {}, "next": "{}", "blendIn": {}, "blendOut": {}, "flags": [{}], )"
		           R"("model": "{}", "lastFrame": {}}})",
		           cmb.name,
		           cmb.layer,
		           cmb.next,
		           cmb.blend_in,
		           cmb.blend_out,
		           get_ani_flags(cmb.flags),
		           cmb.model,
		           cmb.last_frame);
	}

	fmt::print(R"(], "blends": [)");

	for (int i = 0; i < obj.blends.size(); ++i) {
		auto& cmb = obj.blends[i];

		if (i != 0) {
			fmt::print(",");
		}

		fmt::print(R"({{"name": "{}", "next": "{}", "blendIn": {}, "blendOut": {}}})",
		           cmb.name,
		           cmb.next,
		           cmb.blend_in,
		           cmb.blend_out);
	}

	fmt::print(R"(], "aliases": [)");

	for (int i = 0; i < obj.aliases.size(); ++i) {
		auto& cmb = obj.aliases[i];

		if (i != 0) {
			fmt::print(",");
		}

		fmt::print(R"({{"name": "{}", "layer": {}, "next": "{}", "blendIn": {}, "blendOut": {}, "flags": [{}], )"
		           R"("alias": "{}", "direction": "{}"}})",
		           cmb.name,
		           cmb.layer,
		           cmb.next,
		           cmb.blend_in,
		           cmb.blend_out,
		           get_ani_flags(cmb.flags),
		           cmb.alias,
		           cmb.direction == px::mds::animation_direction::forward ? "forward" : "reversed");
	}

	fmt::print(R"(], "modelTags": [)");

	for (int i = 0; i < obj.model_tags.size(); ++i) {
		auto& cmb = obj.model_tags[i];

		if (i != 0) {
			fmt::print(",");
		}

		fmt::print(R"({{"bone": "{}"}})", cmb.bone);
	}

	fmt::print(R"(], animations": [)");

	for (int i = 0; i < obj.animations.size(); ++i) {
		auto& cmb = obj.animations[i];

		if (i != 0) {
			fmt::print(",");
		}

		fmt::print(R"({{"name": "{}", "layer": {}, "next": "{}", "blendIn": {}, "blendOut": {}, "flags": [{}], )"
		           R"("model": "{}", "direction": "{}", "firstFrame": {}, "lastFrame": {}, "fps": {}, )"
		           R"("speed": {}, "collisionVolumeScale": {}, "events": [)",
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
		           cmb.collision_volume_scale);

		for (int i = 0; i < cmb.events.size(); ++i) {
			auto& b = cmb.events[i];
			auto type = static_cast<int>(b.type);
			auto fight_mode = static_cast<int>(b.fight_mode);

			if (i != 0) {
				fmt::print(",");
			}

			fmt::print(R"({{"frame": {}, "type": "{}", "slot": "{}", "slot2": "{}", "item": "{}", )"
			           R"("frames": [{}], "fightMode": "{}", "attached": {}}})",
			           b.frame,
			           type < 23 ? EVENT_TAG_TYPE_NAMES[type] : std::to_string(type),
			           b.slot,
			           b.slot2,
			           b.item,
			           fmt::join(b.frames, ","),
			           fight_mode < 7 ? FIGHT_MODE_NAMES[fight_mode] : std::to_string(fight_mode),
			           b.attached);
		}

		fmt::print(R"(], "pfx": [)");

		for (int i = 0; i < cmb.pfx.size(); ++i) {
			auto& b = cmb.pfx[i];

			if (i != 0) {
				fmt::print(",");
			}

			fmt::print(R"({{"frame": {}, "index": {}, "name": "{}", "position": "{}", "attached": {}}})",
			           b.frame,
			           b.index,
			           b.name,
			           b.position,
			           b.attached);
		}

		fmt::print(R"(], "pfxStop": [)");

		for (int i = 0; i < cmb.pfx_stop.size(); ++i) {
			auto& b = cmb.pfx_stop[i];

			if (i != 0) {
				fmt::print(",");
			}

			fmt::print(R"({{"frame": {}, "index": {}}})", b.frame, b.index);
		}

		fmt::print(R"(], "sfx": [)");

		for (int i = 0; i < cmb.sfx.size(); ++i) {
			auto& b = cmb.sfx[i];

			if (i != 0) {
				fmt::print(",");
			}

			fmt::print(R"({{"frame": {}, "name": "{}", "range": {}, "emptySlot": {}}})",
			           b.frame,
			           b.name,
			           b.range,
			           b.empty_slot);
		}

		fmt::print(R"(], "sfxGround": [)");

		for (int i = 0; i < cmb.sfx_ground.size(); ++i) {
			auto& b = cmb.sfx_ground[i];

			if (i != 0) {
				fmt::print(",");
			}

			fmt::print(R"({{"frame": {}, "name": "{}", "range": {}, "emptySlot": {}}})",
			           b.frame,
			           b.name,
			           b.range,
			           b.empty_slot);
		}

		fmt::print(R"(], "morph": [)");

		for (int i = 0; i < cmb.morph.size(); ++i) {
			auto& b = cmb.morph[i];

			if (i != 0) {
				fmt::print(",");
			}

			fmt::print(R"({{"frame": {}, "animation": "{}", "node": "{}"}})", b.frame, b.animation, b.node);
		}

		fmt::print(R"(], "tremors": [)");

		for (int i = 0; i < cmb.tremors.size(); ++i) {
			auto& b = cmb.tremors[i];

			if (i != 0) {
				fmt::print(",");
			}

			fmt::print(R"({{"frame": {}, "field1": {}, "field2": {}, "field3": {}, "field4": {}}})",
			           b.frame,
			           b.field1,
			           b.field2,
			           b.field3,
			           b.field4);
		}

		fmt::print("]}}");
	}

	fmt::print(R"(]}})");
}