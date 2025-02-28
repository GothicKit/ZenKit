// Copyright © 2023-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"

#include <string_view>

namespace zenkit {
	/// \brief An enum for providing a game version hint to some functions
	enum class GameVersion {
		GOTHIC_1, ///< Represents any patch of Gothic
		GOTHIC_2, ///< Represents any patch of Gothic II, including _Night of the Raven_.

		gothic_1 ZKREM("renamed to GOTHIC_1") = GOTHIC_1, ///< Represents any patch of Gothic
		gothic_2 ZKREM("renamed to GOTHIC_2") =
		    GOTHIC_2, ///< Represents any patch of Gothic II, including _Night of the Raven_.
	};

	struct Vec2 {
		float x, y;

		constexpr Vec2() : Vec2(0.0f) {}
		constexpr Vec2(float x, float y) : x(x), y(y) {}
		explicit constexpr Vec2(float v) : Vec2(v, v) {}

		constexpr float& operator[](unsigned index) {
			return this->pointer()[index];
		}

		constexpr float const& operator[](unsigned index) const {
			return this->pointer()[index];
		}

		constexpr Vec2 operator*(float scalar) const {
			return Vec2(x * scalar, y * scalar);
		}

		constexpr Vec2 operator*(Vec2 const& v) const {
			return Vec2(x * v.x, y * v.y);
		}

		constexpr bool operator==(Vec2 const& v) const {
			return x == v.x && y == v.y;
		}

		constexpr float const* pointer() const {
			return &this->x;
		}

		constexpr float* pointer() {
			return &this->x;
		}
	};

	struct Vec3 {
		float x, y, z;

		constexpr Vec3() : Vec3(0.0f) {}
		constexpr Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
		explicit constexpr Vec3(float v) : Vec3(v, v, v) {}

		constexpr float& operator[](unsigned index) {
			return this->pointer()[index];
		}

		constexpr float const& operator[](unsigned index) const {
			return this->pointer()[index];
		}

		constexpr Vec3 operator*(float scalar) const {
			return Vec3(x * scalar, y * scalar, z * scalar);
		}

		constexpr Vec3 operator*(Vec3 const& v) const {
			return Vec3(x * v.x, y * v.y, z * v.z);
		}

		constexpr bool operator==(Vec3 const& v) const {
			return x == v.x && y == v.y && z == v.z;
		}

		[[nodiscard]] constexpr float const* pointer() const {
			return &this->x;
		}

		constexpr float* pointer() {
			return &this->x;
		}
	};

	struct Vec4 {
		float x, y, z, w;

		constexpr Vec4() : Vec4(0.0f) {}
		constexpr Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
		explicit constexpr Vec4(float v) : Vec4(v, v, v, v) {}

		constexpr float& operator[](unsigned index) {
			return this->pointer()[index];
		}

		constexpr float const& operator[](unsigned index) const {
			return this->pointer()[index];
		}

		constexpr Vec4 operator*(float scalar) const {
			return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
		}

		constexpr Vec4 operator*(Vec4 const& v) const {
			return Vec4(x * v.x, y * v.y, z * v.z, w * v.w);
		}

		constexpr bool operator==(Vec4 const& v) const {
			return x == v.x && y == v.y && z == v.z && w == v.w;
		}

		[[nodiscard]] constexpr float const* pointer() const {
			return &this->x;
		}

		constexpr float* pointer() {
			return &this->x;
		}
	};

	struct Color {
		unsigned char r, g, b, a;

		constexpr Color() : Color(0, 0, 0, 0) {}
		constexpr Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a) {}

		constexpr unsigned char& operator[](unsigned index) {
			return this->pointer()[index];
		}

		constexpr unsigned char const& operator[](unsigned index) const {
			return this->pointer()[index];
		}

		constexpr Color operator*(unsigned char scalar) const {
			return Color(r * scalar, g * scalar, b * scalar, a * scalar);
		}

		constexpr Color operator*(Color const& v) const {
			return Color(r * v.r, g * v.g, b * v.b, a * v.a);
		}

		constexpr bool operator==(Color const& v) const {
			return r == v.r && g == v.g && b == v.b && a == v.a;
		}

		[[nodiscard]] constexpr unsigned char const* pointer() const {
			return &this->r;
		}

		constexpr unsigned char* pointer() {
			return &this->r;
		}
	};

	struct Quat {
		float w, x, y, z;

		constexpr Quat() : Quat(0, 0, 0, 0) {}
		constexpr Quat(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

		constexpr float& operator[](unsigned index) {
			return this->pointer()[index];
		}

		constexpr float const& operator[](unsigned index) const {
			return this->pointer()[index];
		}

		constexpr bool operator==(Quat const& v) const {
			return x == v.x && y == v.y && z == v.z && w == v.w;
		}

		[[nodiscard]] constexpr float const* pointer() const {
			return &this->x;
		}

		constexpr float* pointer() {
			return &this->x;
		}
	};

	struct Mat3 {
		Vec3 columns[3] {};

		constexpr Mat3() = default;
		constexpr Mat3(float m00,
		               float m01,
		               float m02,
		               float m10,
		               float m11,
		               float m12,
		               float m20,
		               float m21,
		               float m22)
		    : Mat3({m00, m01, m02}, {m10, m11, m12}, {m20, m21, m22}) {}
		constexpr Mat3(Vec3 const& m0, Vec3 const& m1, Vec3 const& m2) : columns {m0, m1, m2} {}

		constexpr Vec3& operator[](unsigned index) {
			return columns[index];
		}

		constexpr Vec3 const& operator[](unsigned index) const {
			return columns[index];
		}

		constexpr bool operator==(Mat3 const& v) const {
			return columns[0] == v.columns[0] && columns[1] == v.columns[1] && columns[2] == v.columns[2];
		}

		[[nodiscard]] constexpr Mat3 transpose() const {
			return Mat3 {columns[0][0],
			             columns[1][0],
			             columns[2][0],
			             columns[0][1],
			             columns[1][1],
			             columns[2][1],
			             columns[0][2],
			             columns[1][2],
			             columns[2][2]};
		}

		[[nodiscard]] constexpr float const* pointer() const {
			return columns[0].pointer();
		}

		constexpr float* pointer() {
			return columns[0].pointer();
		}

		constexpr static Mat3 identity() {
			return Mat3 {1, 0, 0, 0, 1, 0, 0, 0, 1};
		}
	};

	struct Mat4 {
		Vec4 columns[4] {};
		constexpr Mat4() = default;
		constexpr Mat4(float m00,
		               float m01,
		               float m02,
		               float m03,
		               float m10,
		               float m11,
		               float m12,
		               float m13,
		               float m20,
		               float m21,
		               float m22,
		               float m23,
		               float m30,
		               float m31,
		               float m32,
		               float m33)
		    : Mat4(Vec4(m00, m01, m02, m03),
		           Vec4(m10, m11, m12, m13),
		           Vec4(m20, m21, m22, m23),
		           Vec4(m30, m31, m32, m33)) {}

		constexpr Mat4(Vec4 const& m0, Vec4 const& m1, Vec4 const& m2, Vec4 const& m3) : columns {m0, m1, m2, m3} {}

		constexpr Vec4& operator[](unsigned index) {
			return columns[index];
		}

		constexpr Vec4 const& operator[](unsigned index) const {
			return columns[index];
		}

		constexpr bool operator==(Mat4 const& v) const {
			return columns[0] == v.columns[0] && columns[1] == v.columns[1] && columns[2] == v.columns[2] &&
			    columns[3] == v.columns[3];
		}

		[[nodiscard]] constexpr Mat4 transpose() const {
			return Mat4 {columns[0][0],
			             columns[1][0],
			             columns[2][0],
			             columns[3][0],
			             columns[0][1],
			             columns[1][1],
			             columns[2][1],
			             columns[3][1],
			             columns[0][2],
			             columns[1][2],
			             columns[2][2],
			             columns[3][2],
			             columns[0][3],
			             columns[1][3],
			             columns[2][3],
			             columns[3][3]};
		}

		[[nodiscard]] constexpr float const* pointer() const {
			return columns[0].pointer();
		}

		constexpr float* pointer() {
			return columns[0].pointer();
		}

		constexpr static Mat4 identity() {
			return Mat4 {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
		}
	};

	/// \brief Tests whether two strings are equal when ignoring case.
	///
	/// Internally, uses std::tolower to compare the strings character by character.
	///
	/// \param a A string.
	/// \param b Another string.
	/// \return ``true`` if both strings are equal when ignoring case.
	ZKAPI bool iequals(std::string_view a, std::string_view b);

	/// \brief Tests whether \p a is lexicographically less than \p b.
	///
	/// Internally, uses std::tolower to compare the strings character by character.
	///
	/// \param a A string.
	/// \param b Another string.
	/// \return ``true`` if \p a is lexicographically less than \p b.
	ZKAPI bool icompare(std::string_view a, std::string_view b);
} // namespace zenkit
