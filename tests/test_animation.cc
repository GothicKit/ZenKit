// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "catch2.hh"
#include <phoenix/animation.hh>

using namespace phoenix;

TEST_CASE("animations are read correctly", "[animation]") {
	auto in = reader::from("./samples/animation.man");
	auto anim = animation::parse(in);

	REQUIRE(anim.name() == "S_BRUSH_S0");
	REQUIRE(anim.next() == "S_BRUSH_S0");
	REQUIRE(anim.layer() == 1);
	REQUIRE(anim.frames() == 1);
	REQUIRE(anim.frames_per_second() == 25);
	REQUIRE(anim.frames_per_second_alt() == 25);
	REQUIRE(anim.checksum() == 4170839982);
	REQUIRE(anim.source_path() == "\\_WORK\\DATA\\ANIMS\\BAB_SWEEP_M01.ASC");
	REQUIRE(anim.source_script() == "\t\t\tANI\t\t\t(\"S_BRUSH_S0\"\t\t\t1\t\"S_BRUSH_S0\"\t0.0\t0.0\tM.\t\"BAB_SWEEP_M01.ASC\"\t\t\tF\t3\t3)");

	auto [bbox0, bbox1] = anim.bbox();
	REQUIRE(bbox0 == glm::vec3 {-42.741993, -86.539772, -29.5238342});
	REQUIRE(bbox1 == glm::vec3 {32.7615509, 55.5912437, 39.3346939});

	REQUIRE(anim.events().empty());
	REQUIRE(anim.samples().size() == 27);

	// Let's grab the first and last sample
	auto& sample0 = anim.samples()[0];
	auto& sample1 = anim.samples().back();

	REQUIRE(sample0.position == glm::vec3 {-1.11763525f, 85.9253082f, -2.8696866f});
	REQUIRE(sample0.rotation == glm::vec4 {-0.0477454737f, 0.687823235f, 0.0753993988f, 0.720371067f});

	REQUIRE(sample1.position == glm::vec3 {12.0363817f, -0.0012588501f, 16.447361f});
	REQUIRE(sample1.rotation == glm::vec4 {0.0f, 0.707081676f, 0.0f, 0.707131922f});
}