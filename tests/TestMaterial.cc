// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <zenkit/Archive.hh>
#include <zenkit/Material.hh>
#include <zenkit/Stream.hh>

#include <doctest/doctest.h>
#include <fstream>

using namespace zenkit;

static void verify_g1(Read* r) {
	auto archive = ReadArchive::from(r);

	auto material_count = archive->read_int();
	CHECK_EQ(material_count, 1);

	Material m1 {};
	m1.load(*archive);

	CHECK_EQ(m1.name, "BODY");
	CHECK_EQ(m1.group, MaterialGroup::UNDEFINED);
	CHECK_EQ(m1.color, zenkit::Color {115, 91, 77, 255});
	CHECK_EQ(m1.smooth_angle, 60.0f);
	CHECK_EQ(m1.texture, "DEM_BODY_V0.TGA");
	CHECK_EQ(m1.texture_scale, zenkit::Vec2 {512.0f, 512.0f});
	CHECK_EQ(m1.texture_anim_fps, 0.0f);
	CHECK_EQ(m1.texture_anim_map_mode, AnimationMapping::NONE);
	CHECK_EQ(m1.texture_anim_map_dir, zenkit::Vec2 {9.9999997e-005, 0.0f});
	CHECK_FALSE(m1.disable_collision);
	CHECK_FALSE(m1.disable_lightmap);
	CHECK_FALSE(m1.dont_collapse);
	CHECK_EQ(m1.detail_object, "");
	CHECK_EQ(m1.default_mapping, zenkit::Vec2 {2.34375f, 2.34375f});
}

static void verify_g2(Read* r) {
	auto archive = ReadArchive::from(r);

	auto material_count = archive->read_int();
	CHECK_EQ(material_count, 1);

	Material m1 {};
	m1.load(*archive);

	CHECK_EQ(m1.name, "BODY");
	CHECK_EQ(m1.group, MaterialGroup::UNDEFINED);
	CHECK_EQ(m1.color, zenkit::Color {115, 91, 77, 255});
	CHECK_EQ(m1.smooth_angle, 60.0f);
	CHECK_EQ(m1.texture, "DEM_BODY_V0.TGA");
	CHECK_EQ(m1.texture_scale, zenkit::Vec2 {512.0f, 512.0f});
	CHECK_EQ(m1.texture_anim_fps, 0.0f);
	CHECK_EQ(m1.texture_anim_map_mode, AnimationMapping::NONE);
	CHECK_EQ(m1.texture_anim_map_dir, zenkit::Vec2 {0.0f, 0.0f});
	CHECK_FALSE(m1.disable_collision);
	CHECK_FALSE(m1.disable_lightmap);
	CHECK_FALSE(m1.dont_collapse);
	CHECK_EQ(m1.detail_object, "");
	CHECK_EQ(m1.default_mapping, zenkit::Vec2 {2.34375f, 2.34375f});
	CHECK_EQ(m1.alpha_func, AlphaFunction::NONE);
	CHECK_EQ(m1.detail_object_scale, 1.0f);
	CHECK_FALSE(m1.force_occluder);
	CHECK_FALSE(m1.environment_mapping);
	CHECK_EQ(m1.environment_mapping_strength, 1.0f);
	CHECK_EQ(m1.wave_mode, WaveMode::NONE);
	CHECK_EQ(m1.wave_speed, WaveSpeed::NORMAL);
	CHECK_FALSE(m1.force_occluder);
	CHECK_EQ(m1.wave_max_amplitude, 30.0);
	CHECK_EQ(m1.wave_grid_size, 100.0f);
	CHECK_FALSE(m1.ignore_sun);
}

TEST_SUITE("Material") {
	TEST_CASE("Material.load(GOTHIC1)") {
		auto in = Read::from("./samples/G1/DEMON_DIE_BODY.MAT");
		verify_g1(in.get());
	}

	TEST_CASE("Material.dump(GOTHIC1)") {
		Material m1 {};
		m1.name = "BODY";
		m1.group = MaterialGroup::UNDEFINED;
		m1.color = zenkit::Color {115, 91, 77, 255};
		m1.smooth_angle = 60.0f;
		m1.texture = "DEM_BODY_V0.TGA";
		m1.texture_scale = zenkit::Vec2 {512.0f, 512.0f};
		m1.texture_anim_fps = 0.0f;
		m1.texture_anim_map_mode = AnimationMapping::NONE;
		m1.texture_anim_map_dir = zenkit::Vec2 {9.9999997e-005, 0.0f};
		m1.disable_collision = false;
		m1.disable_lightmap = false;
		m1.dont_collapse = false;
		m1.detail_object = "";
		m1.default_mapping = zenkit::Vec2 {2.34375f, 2.34375f};

		std::vector<std::byte> data {};
		auto out = Write::to(&data);
		auto out_ar = WriteArchive::to(out.get(), ArchiveFormat::BINARY);

		out_ar->write_int("", 1);
		out_ar->write_string("", m1.name);
		out_ar->write_object("%", &m1, GameVersion::GOTHIC_1);
		out_ar->write_header();

		auto r = Read::from(&data);
		verify_g1(r.get());
	}

	TEST_CASE("Material.load(GOTHIC2)") {
		auto r = Read::from("./samples/G2/DEMON_DIE_BODY.MAT");
		verify_g2(r.get());
	}

	TEST_CASE("Material.dump(GOTHIC2)") {
		Material m1 {};
		m1.name = "BODY";
		m1.group = MaterialGroup::UNDEFINED;
		m1.color = zenkit::Color {115, 91, 77, 255};
		m1.smooth_angle = 60.0f;
		m1.texture = "DEM_BODY_V0.TGA";
		m1.texture_scale = zenkit::Vec2 {512.0f, 512.0f};
		m1.texture_anim_fps = 0.0f;
		m1.texture_anim_map_mode = AnimationMapping::NONE;
		m1.texture_anim_map_dir = zenkit::Vec2 {0.0f, 0.0f};
		m1.disable_collision = false;
		m1.disable_lightmap = false;
		m1.dont_collapse = false;
		m1.detail_object = "";
		m1.default_mapping = zenkit::Vec2 {2.34375f, 2.34375f};
		m1.alpha_func = AlphaFunction::NONE;
		m1.detail_object_scale = 1.0f;
		m1.force_occluder = false;
		m1.environment_mapping = false;
		m1.environment_mapping_strength = 1.0f;
		m1.wave_mode = WaveMode::NONE;
		m1.wave_speed = WaveSpeed::NORMAL;
		m1.force_occluder = false;
		m1.wave_max_amplitude = 30.0;
		m1.wave_grid_size = 100.0f;
		m1.ignore_sun = false;

		std::vector<std::byte> data {};
		auto out = Write::to(&data);
		auto out_ar = WriteArchive::to(out.get(), ArchiveFormat::BINARY);

		out_ar->write_int("", 1);
		out_ar->write_string("", m1.name);
		out_ar->write_object("%", &m1, GameVersion::GOTHIC_2);
		out_ar->write_header();

		auto r = Read::from(&data);
		verify_g2(r.get());
	}
}
