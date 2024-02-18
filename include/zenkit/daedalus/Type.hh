// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <variant>

namespace zenkit::daedalus {
	class ClassSymbol;

	enum class PrimitiveType {
		VOID = 0,
		INT,
		FLOAT,
		STRING,
		FUNC,
	};

	class Type {
	public:
		explicit Type(PrimitiveType t);
		explicit Type(ClassSymbol* t);

		[[nodiscard]] bool is_convertible(Type other) const noexcept;

		[[nodiscard]] bool is_primitive() const noexcept;
		[[nodiscard]] bool is_void() const noexcept;

		[[nodiscard]] bool is_array() const noexcept;
		[[nodiscard]] Type to_array() const noexcept;
		[[nodiscard]] Type to_scalar() const noexcept;

		[[nodiscard]] PrimitiveType get_primitive_type() const;
		[[nodiscard]] ClassSymbol* get_complex_type() const;

		[[nodiscard]] std::string describe() const;

		static Type const VOID;
		static Type const INT;
		static Type const FLOAT;
		static Type const STRING;
		static Type const FUNC;

	private:
		std::variant<PrimitiveType, ClassSymbol*> _m_type;
		bool _m_array {false};
	};

	[[nodiscard]] bool operator==(Type const& a, Type const& b) noexcept;
	[[nodiscard]] bool operator!=(Type const& a, Type const& b) noexcept;
} // namespace zenkit::daedalus
