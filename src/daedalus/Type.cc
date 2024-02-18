// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/daedalus/Type.hh"
#include "zenkit/daedalus/Module.hh"

namespace zenkit::daedalus {
	static std::string_view const PRIMITIVE_TYPE_NAMES[] = {
	    "void",
	    "int",
	    "float",
	    "string",
	    "func",
	};

	Type const Type::VOID {PrimitiveType::VOID};
	Type const Type::INT {PrimitiveType::INT};
	Type const Type::FLOAT {PrimitiveType::FLOAT};
	Type const Type::STRING {PrimitiveType::STRING};
	Type const Type::FUNC {PrimitiveType::FUNC};

	Type::Type(PrimitiveType t) : _m_type(t) {}

	Type::Type(ClassSymbol* t) : _m_type(t) {}

	bool Type::is_convertible(Type other) const noexcept {
		if (*this == other) {
			return true;
		}

		if (this->is_primitive() && other.is_primitive()) {
			return (this->get_primitive_type() == PrimitiveType::INT &&
			        other.get_primitive_type() == PrimitiveType::FLOAT);
		} else if (!this->is_primitive() && other.is_primitive()) {
			// Instances are convertible to int.
			return other.get_primitive_type() == PrimitiveType::INT

#ifndef ZK_DAEDALUS_STRICT
			    || other.get_primitive_type() == PrimitiveType::FUNC
#endif
			    ;
		}

		return false;
	}

	bool Type::is_primitive() const noexcept {
		return std::holds_alternative<PrimitiveType>(_m_type);
	}

	bool Type::is_array() const noexcept {
		return _m_array;
	}

	Type Type::to_array() const noexcept {
		Type new_ = *this;
		new_._m_array = true;
		return new_;
	}

	Type Type::to_scalar() const noexcept {
		Type new_ = *this;
		new_._m_array = false;
		return new_;
	}

	PrimitiveType Type::get_primitive_type() const {
		return std::get<PrimitiveType>(_m_type);
	}

	ClassSymbol* Type::get_complex_type() const {
		return std::get<ClassSymbol*>(_m_type);
	}

	std::string Type::describe() const {
		std::string scalar;

		if (this->is_primitive()) {
			scalar = PRIMITIVE_TYPE_NAMES[(int) this->get_primitive_type()];
		} else {
			scalar = this->get_complex_type()->get_name();
		}

		return scalar + "[]";
	}

	bool Type::is_void() const noexcept {
		return this->is_primitive() && this->get_primitive_type() == PrimitiveType::VOID;
	}

	bool operator==(Type const& a, Type const& b) noexcept {
		if (a.is_primitive() && b.is_primitive()) {
			return a.get_primitive_type() == b.get_primitive_type() && a.is_array() == b.is_array();
		}

		if (!a.is_primitive() && !b.is_primitive()) {
			return a.get_complex_type() == b.get_complex_type() && a.is_array() == b.is_array();
		}

		return false;
	}

	bool operator!=(Type const& a, Type const& b) noexcept {
		return !(a == b);
	}
} // namespace zenkit::daedalus
