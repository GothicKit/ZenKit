// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/animation.hh>

using namespace phoenix;

TEST_SUITE("animation") {
	TEST_CASE("animations are read correctly") {
		auto in = buffer::open("./samples/animation.man");
		auto anim = animation::parse(in);

		CHECK(anim.name() == "S_BRUSH_S0");
		CHECK(anim.next() == "S_BRUSH_S0");
		CHECK(anim.layer() == 1);
		CHECK(anim.frames() == 1);
		CHECK(anim.frames_per_second() == 25);
		CHECK(anim.frames_per_second_alt() == 25);
		CHECK(anim.checksum() == 4170839982);
		CHECK(anim.source_path() == "\\_WORK\\DATA\\ANIMS\\BAB_SWEEP_M01.ASC");
		CHECK(
		    anim.source_script() ==
		    "\t\t\tANI\t\t\t(\"S_BRUSH_S0\"\t\t\t1\t\"S_BRUSH_S0\"\t0.0\t0.0\tM.\t\"BAB_SWEEP_M01.ASC\"\t\t\tF\t3\t3)");

		auto box0 = anim.bbox();
		CHECK(box0.min == glm::vec3 {-42.741993, -86.539772, -29.5238342});
		CHECK(box0.max == glm::vec3 {32.7615509, 55.5912437, 39.3346939});

		CHECK(anim.events().empty());
		CHECK(anim.samples().size() == 27);

		// Let's grab the first and last sample
		auto& sample0 = anim.samples()[0];
		auto& sample1 = anim.samples().back();

		CHECK(sample0.position == glm::vec3 {-1.11763525f, 85.9253082f, -2.8696866f});
		CHECK(sample0.rotation == glm::quat {0.720371067f, -0.0477454737f, 0.687823235f, 0.0753993988f});

		CHECK(sample1.position == glm::vec3 {12.0363817f, -0.0012588501f, 16.447361f});
		CHECK(sample1.rotation == glm::quat {0.707131922f, 0.0f, 0.707081676f, 0.0f});
	}
}
