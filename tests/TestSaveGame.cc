// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <zenkit/SaveGame.hh>
#include <zenkit/World.hh>

TEST_SUITE("SaveGame") {
	TEST_CASE("SaveGame.load(GOTHIC1)") {
		zenkit::SaveGame save {zenkit::GameVersion::GOTHIC_1};
		save.load("./samples/G1/Save");

		CHECK_EQ(save.metadata.title, "sds");
		CHECK_EQ(save.metadata.world, "WORLD");
		CHECK_EQ(save.metadata.time_day, 0);
		CHECK_EQ(save.metadata.time_hour, 8);
		CHECK_EQ(save.metadata.time_minute, 6);
		CHECK_EQ(save.metadata.save_date, "24.12.2022 - 21:36");
		CHECK_EQ(save.metadata.version_major, 1);
		CHECK_EQ(save.metadata.version_minor, 87);
		CHECK_EQ(save.metadata.play_time_seconds, 49);

		CHECK_EQ(save.state.day, 0);
		CHECK_EQ(save.state.hour, 8);
		CHECK_EQ(save.state.minute, 6);

		// Try to parse the world data.
		auto wld = save.load_world();

		// TODO: Add more checks
	}

	TEST_CASE("SaveGame.load(GOTHIC1,BINARY)") {
		zenkit::SaveGame save {zenkit::GameVersion::GOTHIC_1};
		save.load("./samples/G1/SaveFast");

		CHECK_EQ(save.metadata.title, "sds_fast");
		CHECK_EQ(save.metadata.world, "WORLD");
		CHECK_EQ(save.metadata.time_day, 0);
		CHECK_EQ(save.metadata.time_hour, 8);
		CHECK_EQ(save.metadata.time_minute, 6);
		CHECK_EQ(save.metadata.save_date, "28.12.2022 - 19:32");
		CHECK_EQ(save.metadata.version_major, 1);
		CHECK_EQ(save.metadata.version_minor, 87);
		CHECK_EQ(save.metadata.play_time_seconds, 60);

		CHECK_EQ(save.state.day, 0);
		CHECK_EQ(save.state.hour, 8);
		CHECK_EQ(save.state.minute, 6);

		// Try to parse the world data.
		auto wld = save.load_world();

		// TODO: Add more checks
	}

	TEST_CASE("SaveGame.load(GOTHIC2)") {
		zenkit::Logger::set_default(zenkit::LogLevel::DEBUG);
		zenkit::SaveGame save {zenkit::GameVersion::GOTHIC_2};
		save.load("./samples/G2/Save");

		CHECK_EQ(save.metadata.title, "uwuowo");
		CHECK_EQ(save.metadata.world, "NEWWORLD");
		CHECK_EQ(save.metadata.time_day, 0);
		CHECK_EQ(save.metadata.time_hour, 12);
		CHECK_EQ(save.metadata.time_minute, 28);
		CHECK_EQ(save.metadata.save_date, "28.12.2022 - 18:26");
		CHECK_EQ(save.metadata.version_major, 2);
		CHECK_EQ(save.metadata.version_minor, 6);
		CHECK_EQ(save.metadata.play_time_seconds, 1277);
		CHECK_EQ(save.metadata.version_point, 0);
		CHECK_EQ(save.metadata.version_int, 0);
		CHECK_EQ(save.metadata.version_app_name, "Gothic II - 2.6 (fix)");

		CHECK_EQ(save.state.day, 0);
		CHECK_EQ(save.state.hour, 12);
		CHECK_EQ(save.state.minute, 28);

		auto wld = save.load_world();
		// TODO: Add more checks
	}

	TEST_CASE("SaveGame.load(GOTHIC2,BINARY)") {
		zenkit::SaveGame save {zenkit::GameVersion::GOTHIC_2};
		save.load("./samples/G2/SaveFast");

		CHECK_EQ(save.metadata.title, "inminevalley");
		CHECK_EQ(save.metadata.world, "OLDWORLD");
		CHECK_EQ(save.metadata.time_day, 0);
		CHECK_EQ(save.metadata.time_hour, 0);
		CHECK_EQ(save.metadata.time_minute, 0);
		CHECK_EQ(save.metadata.save_date, "3.10.2022 - 11:15");
		CHECK_EQ(save.metadata.version_major, 2);
		CHECK_EQ(save.metadata.version_minor, 6);
		CHECK_EQ(save.metadata.play_time_seconds, 433);
		CHECK_EQ(save.metadata.version_point, 0);
		CHECK_EQ(save.metadata.version_int, 0);
		CHECK_EQ(save.metadata.version_app_name, "Gothic II - 2.6 (fix)");

		CHECK_EQ(save.state.day, 0);
		CHECK_EQ(save.state.hour, 0);
		CHECK_EQ(save.state.minute, 0);

		auto wld = save.load_world();
		// TODO: Add more checks
	}
}
