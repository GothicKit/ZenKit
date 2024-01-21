// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <zenkit/ModelAnimation.hh>
#include <zenkit/Stream.hh>

using namespace zenkit;

static std::vector<uint32_t> const G1_NODE_INDICES {0,  1,  2,  3,  4,  5,  6,  8,  9,  10, 11, 12, 15,
                                                    16, 17, 18, 19, 26, 27, 28, 29, 30, 31, 32, 33};

static std::vector<uint32_t> const G2_NODE_INDICES {0,  1,  2,  3,  4,  5,  6,  8,  9,  10, 11, 12, 15,
                                                    16, 17, 18, 19, 26, 27, 28, 29, 30, 31, 32, 33};

static constexpr AnimationSample G1_SAMPLE0 {glm::vec3 {12.635274887084961f, 88.75251770019531f, -1.093428611755371f},
                                             glm::quat {0.7771535515785217f, 0.0f, 0.6293110251426697f, 0.0f}};
static constexpr AnimationSample G1_SAMPLE249 {
    glm::vec3 {12.626323699951172f, -0.00145721435546875f, 22.643518447875977f},
    glm::quat {0.7071319222450256f, 0.0f, 0.70708167552948f, 0.0f}};
static constexpr AnimationSample G1_SAMPLE499 {glm::vec3 {12.626323699951172, -0.00145721435546875, 22.643518447875977},
                                               glm::quat {0.7071319222450256, 0.0, 0.70708167552948, 0.0}};

static constexpr AnimationSample G2_SAMPLE0 {glm::vec3 {12.635274887084961f, 88.75251770019531f, -1.093428611755371f},
                                             glm::quat {0.7771535515785217f, 0.0f, 0.6293110251426697f, 0.0f}};
static constexpr AnimationSample G2_SAMPLE249 {
    glm::vec3 {12.626323699951172f, -0.00145721435546875f, 22.643518447875977f},
    glm::quat {0.7071319222450256f, 0.0f, 0.70708167552948f, 0.0f}};
static constexpr AnimationSample G2_SAMPLE499 {glm::vec3 {12.626323699951172, -0.00145721435546875, 22.643518447875977},
                                               glm::quat {0.7071319222450256, 0.0, 0.70708167552948, 0.0}};

TEST_SUITE("ModelAnimation") {
	TEST_CASE("ModelAnimation.load(GOTHIC1)") {
		auto in = Read::from("./samples/G1/HUMANS-S_FISTRUN.MAN");

		ModelAnimation anim {};
		anim.load(in.get());

		auto box0 = anim.bbox;
		CHECK_EQ(box0.max, glm::vec3 {46.33139419555664f, 67.0935287475586f, 49.88602828979492f});
		CHECK_EQ(box0.min, glm::vec3 {-51.09061050415039f, -94.02226257324219f, -31.280731201171875f});

		CHECK_EQ(anim.checksum, 3325331650);
		CHECK_EQ(anim.events.size(), 0);
		CHECK_EQ(anim.fps, 10.0f);
		CHECK_EQ(anim.fps_source, 25.0f);
		CHECK_EQ(anim.frame_count, 20);
		CHECK_EQ(anim.layer, 1);
		CHECK_EQ(anim.name, "S_FISTRUN");
		CHECK_EQ(anim.next, "S_FISTRUN");
		CHECK_EQ(anim.node_count, 25);
		CHECK_EQ(anim.node_indices.size(), 25);
		CHECK_EQ(anim.node_indices, G1_NODE_INDICES);
		CHECK_EQ(anim.sample_position_min, -9.01021957397461f);
		CHECK_EQ(anim.sample_position_scale, 0.001491763861849904f);

		CHECK_EQ(anim.samples.size(), 25 * 20 /* node_count * frame_count */);
		CHECK_EQ(anim.samples[0], G1_SAMPLE0);
		CHECK_EQ(anim.samples[249], G1_SAMPLE249);
		CHECK_EQ(anim.samples[499], G1_SAMPLE499);

		CHECK_EQ(anim.source_path, "\\_WORK\\DATA\\ANIMS\\HUM_AMB_FISTRUN_M01.ASC");
		CHECK_EQ(
		    anim.source_script,
		    "\t\t\tANI\t\t\t(\"S_FISTRUN\"\t\t\t\t1\t\"S_FISTRUN\"\t\t0.0 0.1 MI\t\"HUM_AMB_FISTRUN_M01.ASC\"\tF   "
		    "1\t50\tFPS:10)");
	}

	TEST_CASE("ModelAnimation.load(GOTHIC2)") {
		auto in = Read::from("./samples/G2/HUMANS-S_FISTRUN.MAN");

		ModelAnimation anim {};
		anim.load(in.get());

		auto box0 = anim.bbox;
		CHECK_EQ(box0.max, glm::vec3 {46.33139419555664f, 67.0935287475586f, 49.88602828979492f});
		CHECK_EQ(box0.min, glm::vec3 {-51.090614318847656f, -94.02226257324219f, -31.280733108520508f});

		CHECK_EQ(anim.checksum, 3325331650);
		CHECK_EQ(anim.events.size(), 0);
		CHECK_EQ(anim.fps, 10.0f);
		CHECK_EQ(anim.fps_source, 25.0f);
		CHECK_EQ(anim.frame_count, 20);
		CHECK_EQ(anim.layer, 1);
		CHECK_EQ(anim.name, "S_FISTRUN");
		CHECK_EQ(anim.next, "S_FISTRUN");
		CHECK_EQ(anim.node_count, 25);
		CHECK_EQ(anim.node_indices.size(), 25);
		CHECK_EQ(anim.node_indices, G2_NODE_INDICES);
		CHECK_EQ(anim.sample_position_min, -9.01021957397461f);
		CHECK_EQ(anim.sample_position_scale, 0.001491763861849904f);

		CHECK_EQ(anim.samples.size(), 25 * 20 /* node_count * frame_count */);
		CHECK_EQ(anim.samples[0], G2_SAMPLE0);
		CHECK_EQ(anim.samples[249], G2_SAMPLE249);
		CHECK_EQ(anim.samples[499], G2_SAMPLE499);

		CHECK_EQ(anim.source_path, "\\_WORK\\DATA\\ANIMS\\HUM_AMB_FISTRUN_M01.ASC");
		CHECK_EQ(
		    anim.source_script,
		    "\t\t\tANI\t\t\t(\"S_FISTRUN\"\t\t\t\t1\t\"S_FISTRUN\"\t\t0.0 0.1 MI\t\"HUM_AMB_FISTRUN_M01.ASC\"\tF   "
		    "1\t50\tFPS:10)");
	}
}
