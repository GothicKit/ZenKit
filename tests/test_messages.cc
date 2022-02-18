// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "catch2.hh"
#include <phoenix/messages.hh>

TEST_CASE("message databases are read correctly", "[messages][proprietary]") {
	auto msgs = phoenix::messages::parse("./samples/ou.proprietary.bin");

	REQUIRE(msgs.blocks().size() == 7360);

	auto* msg20 = msgs.block_by_name("DIA_ARTO_PERM_15_00");
	auto* msg100 = msgs.block_by_name("DIA_BaalKagan_WasDrin_13_01");
	auto* msg200 = &msgs.blocks()[200];
	auto* msg_none = msgs.block_by_name("nonexistent");

	REQUIRE(msg20 != nullptr);
	REQUIRE(msg100 != nullptr);
	REQUIRE(msg200 != nullptr);
	REQUIRE(msg_none == nullptr);

	REQUIRE(msg20->message.type == 0);
	REQUIRE(msg20->message.text == "Du redest nicht viel, was?");
	REQUIRE(msg20->message.name == "DIA_ARTO_PERM_15_00.WAV");

	REQUIRE(msg100->message.type == 0);
	REQUIRE(msg100->message.text == "Ich kann dich auf viele Arten entlohnen.");
	REQUIRE(msg100->message.name == "DIA_BAALKAGAN_WASDRIN_13_01.WAV");

	REQUIRE(msg200->message.type == 0);
	REQUIRE(msg200->message.text == "Stimmt genau.");
	REQUIRE(msg200->message.name == "DIA_BAALTARAN_INTOCASTLE_EXACTLY_15_00.WAV");
}