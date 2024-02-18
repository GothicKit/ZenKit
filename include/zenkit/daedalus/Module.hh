// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/daedalus/SyntaxTree.hh"
#include "zenkit/daedalus/Type.hh"

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace zenkit::daedalus {
	enum class SymbolKind {
		VAR,
		CONST,
		CLASS,
		PROTOTYPE,
		INSTANCE,
		FUNC,
		EXTERN,
	};

	class Symbol {
	public:
		Symbol(SymbolKind type, std::string name);

		Symbol(Symbol&&) = default;
		virtual ~Symbol() noexcept = default;

		[[nodiscard]] SymbolKind get_kind() const noexcept;
		[[nodiscard]] std::string const& get_name() const noexcept;

	protected:
		SymbolKind _m_type;
		std::string const _m_name;
	};

	class VarSymbol : public Symbol {
	public:
		VarSymbol(std::string name, Type type);
		VarSymbol(std::string name, Type type, AstVariable ast);

		[[nodiscard]] Type get_type() const;

		void set_size(int size);
		[[nodiscard]] int get_size() const;

		[[nodiscard]] inline std::optional<AstVariable*> get_ast() noexcept {
			if (_m_ast) return &*_m_ast;
			return std::nullopt;
		}

	private:
		Type _m_value_type;
		int _m_size = 0;
		std::optional<AstVariable> _m_ast;
	};

	class ConstSymbol : public Symbol {
	public:
		ConstSymbol(std::string name, Type type);
		ConstSymbol(std::string name, Type type, AstConstant ast);

		[[nodiscard]] Type get_type() const;

		void set_size(int size);
		[[nodiscard]] int get_size() const;

		[[nodiscard]] inline std::optional<AstConstant*> get_ast() noexcept {
			if (_m_ast) return &*_m_ast;
			return std::nullopt;
		}

	private:
		Type _m_value_type;
		int _m_size = 0;
		std::optional<AstConstant> _m_ast;
	};

	class Scope {
	public:
		std::vector<VarSymbol>& get_local_vars();

		VarSymbol* add_local_var(std::string name, Type type, AstVariable ast);

	private:
		// TODO: For extra fine-grained control, properly scope these.
		std::vector<VarSymbol> _m_local_vars;
	};

	class ClassSymbol : public Symbol {
	public:
		explicit ClassSymbol(std::string name);
		ClassSymbol(std::string name, AstClass ast);

		VarSymbol* get_member(std::string const& name);
		VarSymbol* get_member(AstIdentifier const& name);

		void add_member(std::string name, Type type);
		void add_member(std::string name, Type type, AstVariable ast);

		std::vector<VarSymbol>& get_members();

		[[nodiscard]] inline std::optional<AstClass*> get_ast() noexcept {
			if (_m_ast) return &*_m_ast;
			return std::nullopt;
		}

	private:
		std::vector<VarSymbol> _m_members;
		std::optional<AstClass> _m_ast;
	};

	class PrototypeSymbol : public Symbol {
	public:
		PrototypeSymbol(std::string name, ClassSymbol* base);
		PrototypeSymbol(std::string name, ClassSymbol* base, AstPrototype ast);

		[[nodiscard]] ClassSymbol* get_base() const noexcept;

		Scope& get_scope();

		[[nodiscard]] inline std::optional<AstPrototype*> get_ast() noexcept {
			if (_m_ast) return &*_m_ast;
			return std::nullopt;
		}

	private:
		ClassSymbol* _m_base;
		std::optional<AstPrototype> _m_ast;
		Scope _m_scope;
	};

	class InstanceSymbol : public Symbol {
	public:
		InstanceSymbol(std::string name, Symbol* base);
		InstanceSymbol(std::string name, Symbol* base, AstInstance ast);

		[[nodiscard]] Symbol* get_base() const noexcept;
		[[nodiscard]] ClassSymbol* get_base_class() const noexcept;

		Scope& get_scope();

		[[nodiscard]] inline std::optional<AstInstance*> get_ast() noexcept {
			if (_m_ast) return &*_m_ast;
			return std::nullopt;
		}

	private:
		Symbol* _m_base;
		std::optional<AstInstance> _m_ast;
		Scope _m_scope;
	};

	class FunctionSymbol : public Symbol {
	public:
		FunctionSymbol(std::string name, Type rtype);
		FunctionSymbol(std::string name, Type rtype, AstFunction ast);

		[[nodiscard]] Type get_return_type() const;

		VarSymbol* get_argument(std::string const& name);
		VarSymbol* get_argument(AstIdentifier const& name);

		void add_argument(std::string name, Type type);
		void add_argument(std::string name, Type type, AstVariable ast);

		std::vector<VarSymbol>& get_arguments();

		Scope& get_scope();

		[[nodiscard]] inline std::optional<AstFunction*> get_ast() noexcept {
			if (_m_ast) return &*_m_ast;
			return std::nullopt;
		}

	private:
		Type _m_return_type;
		std::vector<VarSymbol> _m_args;
		std::optional<AstFunction> _m_ast;
		Scope _m_scope;
	};

	class ExternSymbol : public Symbol {
	public:
		ExternSymbol(std::string name, Type rtype);
		ExternSymbol(std::string name, Type rtype, AstExternal ast);

		[[nodiscard]] Type get_return_type() const;

		VarSymbol* get_argument(std::string const& name);
		VarSymbol* get_argument(AstIdentifier const& name);

		void add_argument(std::string name, Type type);
		void add_argument(std::string name, Type type, AstVariable ast);

		std::vector<VarSymbol>& get_arguments();

		[[nodiscard]] inline std::optional<AstExternal*> get_ast() noexcept {
			if (_m_ast) return &*_m_ast;
			return std::nullopt;
		}

	private:
		Type _m_return_type;
		std::vector<VarSymbol> _m_args;
		std::optional<AstExternal> _m_ast;
	};
} // namespace zenkit::daedalus
