// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "TypeStore.hh"

#include "zenkit/Misc.hh"

namespace zenkit::daedalus {
	static std::string_view TYPE_INT = "int";
	static std::string_view TYPE_STRING = "string";
	static std::string_view TYPE_FLOAT = "float";
	static std::string_view TYPE_FUNC = "func";
	static std::string_view TYPE_VOID = "void";

	[[noreturn]] static void fail_duplicate(SourceLocation const& loc, std::string const& name) {
		throw TypeError {loc, "Duplicate definition: " + name};
	}

	[[noreturn]] static void fail_unresolved(AstIdentifier const& new_) {
		throw TypeError {new_.location, "Unresolved reference: " + new_.value};
	}

	[[noreturn]] static void fail_invalid(AstIdentifier const& new_) {
		throw TypeError {new_.location, "Invalid type-id: " + new_.value};
	}

	static AstNode* get_ast(Symbol* sym) {
		switch (sym->get_kind()) {
		case SymbolKind::VAR:
			return ((VarSymbol*) sym)->get_ast().value_or(nullptr);
		case SymbolKind::CONST:
			return ((ConstSymbol*) sym)->get_ast().value_or(nullptr);
		case SymbolKind::CLASS:
			return ((ClassSymbol*) sym)->get_ast().value_or(nullptr);
		case SymbolKind::PROTOTYPE:
			return ((PrototypeSymbol*) sym)->get_ast().value_or(nullptr);
		case SymbolKind::INSTANCE:
			return ((InstanceSymbol*) sym)->get_ast().value_or(nullptr);
		case SymbolKind::FUNC:
			return ((FunctionSymbol*) sym)->get_ast().value_or(nullptr);
		case SymbolKind::EXTERN:
			return ((ExternSymbol*) sym)->get_ast().value_or(nullptr);
		}
	}

	static void parse_scope(TypeStore* db, Scope& scope, AstStatementBlock* ast) {
		for (auto& local : ast->locals) {
			auto name = local.name.value;
			auto type = db->type(local.type, local.size.has_value());

			if (type.is_void()) {
				fail_invalid(local.type);
			}

			scope.add_local_var(std::move(name), type, std::move(local));
		}

		for (auto& stmt : ast->statements) {
			if (stmt->kind() != AstNodeKind::STMT_CONDITION) continue;

			auto if_ = (AstStatementCondition*) stmt.get();
			parse_scope(db, scope, &if_->body);

			for (auto& elif : if_->else_if) {
				parse_scope(db, scope, &elif.body);
			}

			if (if_->else_body) {
				parse_scope(db, scope, &*if_->else_body);
			}
		}

		ast->locals.clear();
	}

	void TypeStore::add_script(AstScript ast) {
		for (auto& node : ast.classes)
			this->add_class(std::move(node));
		for (auto& node : ast.prototypes)
			this->add_prototype(std::move(node));
		for (auto& node : ast.instances)
			this->add_instance(std::move(node));
		for (auto& node : ast.functions)
			this->add_function(std::move(node));
		for (auto& node : ast.externals)
			this->add_external(std::move(node));
		for (auto& node : ast.constants)
			this->add_constant(std::move(node));
		for (auto& node : ast.variables)
			this->add_variable(std::move(node));
	}

	void TypeStore::add_class(AstClass ast) {
		auto new_members = std::move(ast.members);
		auto new_name = ast.name.value;
		auto new_ = std::make_unique<ClassSymbol>(std::move(new_name), std::move(ast));

		for (auto& member : new_members) {
			auto def = new_->get_member(member.name);
			if (def != nullptr) {
				fail_duplicate(member.name.location, member.name.value);
			}

			auto name = member.name.value;
			auto type = this->type(member.type, member.size.has_value());

			if (type.is_void()) {
				fail_invalid(member.type);
			}

			new_->add_member(std::move(name), type, std::move(member));
		}

		this->add(std::move(new_));
	}

	void TypeStore::add_prototype(AstPrototype ast) {
		auto* base = this->get(ast.base);
		if (base == nullptr) {
			fail_unresolved(ast.base);
		} else if (base->get_kind() != SymbolKind::CLASS) {
			fail_invalid(ast.base);
		}

		auto new_name = ast.name.value;
		auto new_ = std::make_unique<PrototypeSymbol>(std::move(new_name), (ClassSymbol*) base, std::move(ast));

		parse_scope(this, new_->get_scope(), &new_->get_ast().value()->body);
		this->add(std::move(new_));
	}

	void TypeStore::add_instance(AstInstance ast) {
		auto* base = this->get(ast.base);
		if (base == nullptr) {
			fail_unresolved(ast.base);
		} else if (base->get_kind() != SymbolKind::CLASS && base->get_kind() != SymbolKind::PROTOTYPE) {
			fail_invalid(ast.base);
		}

		auto new_name = ast.name.value;
		auto new_ = std::make_unique<InstanceSymbol>(std::move(new_name), base, std::move(ast));

		if ((*new_->get_ast())->body) {
			parse_scope(this, new_->get_scope(), &*new_->get_ast().value()->body);
		}

		this->add(std::move(new_));
	}

	void TypeStore::add_function(AstFunction ast) {
		auto new_args = std::move(ast.args);
		auto new_type = this->type(ast.rtype, false);
		auto new_name = ast.name.value;
		auto new_ = std::make_unique<FunctionSymbol>(std::move(new_name), new_type, std::move(ast));

		for (auto& arg : new_args) {
			auto def = new_->get_argument(arg.name);
			if (def != nullptr) {
				fail_duplicate(arg.name.location, arg.name.value);
			}

			auto name = arg.name.value;
			auto type = this->type(arg.type, false);

			if (arg.size.has_value() || type.is_void()) {
				fail_invalid(arg.type);
			}

			new_->add_argument(std::move(name), type, std::move(arg));
		}

		parse_scope(this, new_->get_scope(), &new_->get_ast().value()->body);
		this->add(std::move(new_));
	}

	void TypeStore::add_external(AstExternal ast) {
		auto new_args = std::move(ast.args);
		auto new_type = this->type(ast.rtype, false);
		auto new_name = ast.name.value;
		auto new_ = std::make_unique<ExternSymbol>(std::move(new_name), new_type, std::move(ast));

		for (auto& arg : new_args) {
			auto def = new_->get_argument(arg.name);
			if (def != nullptr) {
				fail_duplicate(arg.name.location, arg.name.value);
			}

			auto name = arg.name.value;
			auto type = this->type(arg.type, false);

			if (arg.size.has_value() || type.is_void()) {
				fail_invalid(arg.type);
			}

			new_->add_argument(std::move(name), type, std::move(arg));
		}

		this->add(std::move(new_));
	}

	void TypeStore::add_constant(AstConstant ast) {
		auto name = ast.name.value;
		auto type = this->type(ast.type, ast.size.has_value());

		if (!type.is_primitive() || type.is_void()) {
			fail_invalid(ast.type);
		}

		this->add(std::make_unique<ConstSymbol>(std::move(name), type, std::move(ast)));
	}

	void TypeStore::add_variable(AstVariable ast) {
		auto name = ast.name.value;
		auto type = this->type(ast.type, ast.size.has_value());

		if (type.is_void()) {
			fail_invalid(ast.type);
		}

		this->add(std::make_unique<VarSymbol>(std::move(name), type, std::move(ast)));
	}

	void TypeStore::add(std::unique_ptr<Symbol> sym) {
		auto xst = this->get(sym->get_name());
		if (xst != nullptr) {
			auto new_ast = get_ast(sym.get());
			fail_duplicate(new_ast->location, sym->get_name());
		}

		_m_symbols[sym->get_name()] = std::move(sym);
	}

	Symbol* TypeStore::get(std::string const& name) {
		auto it = _m_symbols.find(name);
		if (it == _m_symbols.end()) {
			return nullptr;
		}
		return it->second.get();
	}

	Symbol* TypeStore::get(AstIdentifier const& name) {
		return this->get(name.value);
	}

	Symbol* TypeStore::get(AstQualifiedIdentifier const& name) {
		auto base = this->get(name.base);

		if (base == nullptr || !name.element.has_value()) {
			return base;
		}

		ClassSymbol* cls;
		if (base->get_kind() == SymbolKind::VAR) {
			auto* var = (VarSymbol*) base;

			// The variable must be of type class.
			if (var->get_type().is_primitive()) {
				return nullptr;
			}

			cls = var->get_type().get_complex_type();
		} else if (base->get_kind() == SymbolKind::INSTANCE) {
			auto* instance = (InstanceSymbol*) base;
			cls = instance->get_base_class();
		} else {
			// Only vars and instances can be accessed through a qualified identifier.
			return nullptr;
		}

		return cls->get_member(*name.element);
	}

	Type TypeStore::type(AstIdentifier const& name, bool array) {
		if (zenkit::iequals(name.value, TYPE_INT)) return array ? Type::INT.to_array() : Type::INT;
		if (zenkit::iequals(name.value, TYPE_FLOAT)) return array ? Type::FLOAT.to_array() : Type::FLOAT;
		if (zenkit::iequals(name.value, TYPE_STRING)) return array ? Type::STRING.to_array() : Type::STRING;
		if (zenkit::iequals(name.value, TYPE_FUNC)) return array ? Type::FUNC.to_array() : Type::FUNC;

		if (array) fail_invalid(name);

		if (zenkit::iequals(name.value, TYPE_VOID)) return Type::VOID;

		auto def = this->get(name);
		if (def == nullptr) fail_unresolved(name);
		if (def->get_kind() != SymbolKind::CLASS) fail_invalid(name);

		return Type((ClassSymbol*) def);
	}

	void TypeStore::each(std::function<void(Symbol*)> const& cb) {
		for (auto const& item : _m_symbols) {
			cb(item.second.get());
		}
	}
} // namespace zenkit::daedalus
