// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/daedalus/Module.hh"
#include "zenkit/daedalus/SyntaxTree.hh"

namespace zenkit::daedalus {
	std::vector<VarSymbol>& Scope::get_local_vars() {
		return _m_local_vars;
	}

	VarSymbol* Scope::add_local_var(std::string name, Type type, AstVariable ast) {
		return &_m_local_vars.emplace_back(std::move(name), type, std::move(ast));
	}

	Symbol::Symbol(SymbolKind type, std::string name) : _m_type(type), _m_name(std::move(name)) {}

	SymbolKind Symbol::get_kind() const noexcept {
		return _m_type;
	}

	std::string const& Symbol::get_name() const noexcept {
		return _m_name;
	}

	VarSymbol::VarSymbol(std::string name, Type type) : Symbol(SymbolKind::VAR, std::move(name)), _m_value_type(type) {}

	VarSymbol::VarSymbol(std::string name, Type type, AstVariable ast)
	    : Symbol(SymbolKind::VAR, std::move(name)), _m_value_type(type), _m_ast(std::move(ast)) {}

	Type VarSymbol::get_type() const {
		return _m_value_type;
	}

	void VarSymbol::set_size(int size) {
		_m_size = size;
	}

	int VarSymbol::get_size() const {
		return _m_size;
	}

	ClassSymbol::ClassSymbol(std::string name) : Symbol(SymbolKind::CLASS, std::move(name)) {}

	ClassSymbol::ClassSymbol(std::string name, AstClass ast)
	    : Symbol(SymbolKind::CLASS, std::move(name)), _m_ast(std::move(ast)) {}

	VarSymbol* ClassSymbol::get_member(std::string const& name) {
		for (auto& member : _m_members) {
			if (member.get_name() == name) {
				return &member;
			}
		}
		return nullptr;
	}

	VarSymbol* ClassSymbol::get_member(AstIdentifier const& name) {
		return get_member(name.value);
	}

	void ClassSymbol::add_member(std::string name, Type type) {
		_m_members.emplace_back(std::move(name), type);
	}

	void ClassSymbol::add_member(std::string name, Type type, AstVariable ast) {
		_m_members.emplace_back(std::move(name), type, std::move(ast));
	}

	std::vector<VarSymbol>& ClassSymbol::get_members() {
		return _m_members;
	}

	PrototypeSymbol::PrototypeSymbol(std::string name, ClassSymbol* base)
	    : Symbol(SymbolKind::PROTOTYPE, std::move(name)), _m_base(base) {}

	PrototypeSymbol::PrototypeSymbol(std::string name, ClassSymbol* base, AstPrototype ast)
	    : Symbol(SymbolKind::PROTOTYPE, std::move(name)), _m_base(base), _m_ast(std::move(ast)) {}

	ClassSymbol* PrototypeSymbol::get_base() const noexcept {
		return _m_base;
	}

	Scope& PrototypeSymbol::get_scope() {
		return _m_scope;
	}

	InstanceSymbol::InstanceSymbol(std::string name, Symbol* base)
	    : Symbol(SymbolKind::INSTANCE, std::move(name)), _m_base(base) {}

	InstanceSymbol::InstanceSymbol(std::string name, Symbol* base, AstInstance ast)
	    : Symbol(SymbolKind::INSTANCE, std::move(name)), _m_base(base), _m_ast(std::move(ast)) {}

	Symbol* InstanceSymbol::get_base() const noexcept {
		return _m_base;
	}

	Scope& InstanceSymbol::get_scope() {
		return _m_scope;
	}

	ClassSymbol* InstanceSymbol::get_base_class() const noexcept {
		if (_m_base->get_kind() == SymbolKind::CLASS) {
			return (ClassSymbol*) _m_base;
		}
		return ((PrototypeSymbol*) _m_base)->get_base();
	}

	FunctionSymbol::FunctionSymbol(std::string name, Type rtype)
	    : Symbol(SymbolKind::FUNC, std::move(name)), _m_return_type(rtype) {}

	FunctionSymbol::FunctionSymbol(std::string name, Type rtype, AstFunction ast)
	    : Symbol(SymbolKind::FUNC, std::move(name)), _m_return_type(rtype), _m_ast(std::move(ast)) {}

	VarSymbol* FunctionSymbol::get_argument(std::string const& name) {
		for (auto& arg : _m_args) {
			if (arg.get_name() == name) {
				return &arg;
			}
		}

		return nullptr;
	}

	VarSymbol* FunctionSymbol::get_argument(AstIdentifier const& name) {
		return this->get_argument(name.value);
	}

	void FunctionSymbol::add_argument(std::string name, Type type) {
		_m_args.emplace_back(std::move(name), type);
	}

	void FunctionSymbol::add_argument(std::string name, Type type, AstVariable ast) {
		_m_args.emplace_back(std::move(name), type, std::move(ast));
	}

	Type FunctionSymbol::get_return_type() const {
		return _m_return_type;
	}

	std::vector<VarSymbol>& FunctionSymbol::get_arguments() {
		return _m_args;
	}

	Scope& FunctionSymbol::get_scope() {
		return _m_scope;
	}

	ConstSymbol::ConstSymbol(std::string name, Type type)
	    : Symbol(SymbolKind::CONST, std::move(name)), _m_value_type(type) {}

	ConstSymbol::ConstSymbol(std::string name, Type type, AstConstant ast)
	    : Symbol(SymbolKind::CONST, std::move(name)), _m_value_type(type), _m_ast(std::move(ast)) {}

	Type ConstSymbol::get_type() const {
		return _m_value_type;
	}

	void ConstSymbol::set_size(int size) {
		_m_size = size;
	}

	int ConstSymbol::get_size() const {
		return _m_size;
	}

	ExternSymbol::ExternSymbol(std::string name, Type rtype)
	    : Symbol(SymbolKind::EXTERN, std::move(name)), _m_return_type(rtype) {}

	ExternSymbol::ExternSymbol(std::string name, Type rtype, AstExternal ast)
	    : Symbol(SymbolKind::EXTERN, std::move(name)), _m_return_type(rtype), _m_ast(std::move(ast)) {}

	VarSymbol* ExternSymbol::get_argument(std::string const& name) {
		for (auto& arg : _m_args) {
			if (arg.get_name() == name) {
				return &arg;
			}
		}

		return nullptr;
	}

	VarSymbol* ExternSymbol::get_argument(AstIdentifier const& name) {
		return this->get_argument(name.value);
	}

	void ExternSymbol::add_argument(std::string name, Type type) {
		_m_args.emplace_back(std::move(name), type);
	}

	void ExternSymbol::add_argument(std::string name, Type type, AstVariable ast) {
		_m_args.emplace_back(std::move(name), type, std::move(ast));
	}

	Type ExternSymbol::get_return_type() const {
		return _m_return_type;
	}

	std::vector<VarSymbol>& ExternSymbol::get_arguments() {
		return _m_args;
	}
} // namespace zenkit::daedalus
