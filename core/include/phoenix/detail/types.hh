// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <cstdint>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;
using f32 = float;
using f64 = double;

enum class game_version { gothic_1, gothic_2 };

struct color {
	u8 r, g, b, a;
};

inline bool operator==(color a, color b) {
	return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}
