// Copyright © 2022 Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/save_game.hh>
#include <phoenix/world.hh>

using namespace phoenix::unstable;

TEST_SUITE("save_game") {
	TEST_CASE("save-games of Gothic 1 are read correctly") {
		auto save = save_game::parse("./samples/SaveG1");
		CHECK(save.metadata.title == "sds");
		CHECK(save.metadata.world == "WORLD");
		CHECK(save.metadata.time_day == 0);
		CHECK(save.metadata.time_hour == 8);
		CHECK(save.metadata.time_minute == 6);
		CHECK(save.metadata.save_date == "24.12.2022 - 21:36");
		CHECK(save.metadata.version_major == 1);
		CHECK(save.metadata.version_minor == 87);
		CHECK(save.metadata.play_time_seconds == 49);

		CHECK(save.script.day == 0);
		CHECK(save.script.hour == 8);
		CHECK(save.script.minute == 6);

		// Try to parse the world data.
		(void) phoenix::world::parse(*save.open_world_save(save.current_world), phoenix::game_version::gothic_1);

		// TODO: Add more checks
	}

	TEST_CASE("save-games of Gothic 2 are read correctly") {
		auto save = save_game::parse("./samples/SaveG2");
		CHECK(save.metadata.title == "inminevalley");
		CHECK(save.metadata.world == "OLDWORLD");
		CHECK(save.metadata.time_day == 0);
		CHECK(save.metadata.time_hour == 0);
		CHECK(save.metadata.time_minute == 0);
		CHECK(save.metadata.save_date == "3.10.2022 - 11:15");
		CHECK(save.metadata.version_major == 2);
		CHECK(save.metadata.version_minor == 6);
		CHECK(save.metadata.play_time_seconds == 433);
		CHECK(save.metadata.version_point == 0);
		CHECK(save.metadata.version_int == 0);
		CHECK(save.metadata.version_app_name == "Gothic II - 2.6 (fix)");

		CHECK(save.script.day == 0);
		CHECK(save.script.hour == 0);
		CHECK(save.script.minute == 0);

		// TODO: Add more checks
	}
}