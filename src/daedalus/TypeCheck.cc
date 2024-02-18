// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "TypeCheck.hh"

namespace zenkit::daedalus {
	[[noreturn]] static void fail_mismatch(AstNode const* ast, Type expected, Type actual) {
		throw TypeError {ast->location,
		                 "Type mismatch: expected " + expected.describe() + ", got " + actual.describe()};
	}

	[[noreturn]] static void fail_unresolved(AstQualifiedIdentifier const& new_) {
		if (new_.element.has_value()) {
			throw TypeError {new_.location, "Unresolved reference: " + new_.base.value + "." + new_.element->value};
		} else {
			throw TypeError {new_.location, "Unresolved reference: " + new_.base.value};
		}
	}

	[[noreturn]] static void fail_invalid(SourceLocation const& new_) {
		// TODO(lmichaelis): Better error messages
		throw TypeError {new_, "Invalid reference"};
	}

	[[noreturn]] static void fail_invalid(AstQualifiedIdentifier const& new_) {
		if (new_.element.has_value()) {
			throw TypeError {new_.location, "Invalid reference: " + new_.base.value + "." + new_.element->value};
		} else {
			throw TypeError {new_.location, "Invalid reference: " + new_.base.value};
		}
	}

	[[noreturn]] static void fail_expr(AstExpression const& new_) {
		throw TypeError {new_.location, "Not a constant expression"};
	}

	class Locals {
	public:
		void push(std::vector<VarSymbol>* vars) {
			_m_vars.push_back(vars);
		}

		Symbol* get_var(AstQualifiedIdentifier const& name) {
			Symbol* base = nullptr;

			for (long i = (long) _m_vars.size() - 1; i >= 0; --i) {
				if (_m_vars[i] != nullptr) {
					for (auto& var : *_m_vars[i]) {
						if (var.get_name() == name.base.value) {
							base = &var;
						}
					}
				}
			}

			if (!name.element.has_value() || base == nullptr) {
				return base;
			}

			if (base->get_kind() == SymbolKind::CONST) {
				return nullptr;
			}

			auto type = ((VarSymbol*) base)->get_type();
			if (type.is_primitive()) {
				return nullptr;
			}

			return type.get_complex_type()->get_member(*name.element);
		}

	private:
		std::vector<std::vector<VarSymbol>*> _m_vars;
	};

	[[nodiscard]] static Type check_expr(TypeStore* db, AstExpression* ast, Locals&);
	[[nodiscard]] static std::optional<int> eval_expr_int(TypeStore* db, AstExpression* ast);

	std::optional<int> eval_expr_ref_int(TypeStore* db, AstExpressionReference* ast) {
		auto def = db->get(ast->name);
		if (def->get_kind() != SymbolKind::CONST) {
			return std::nullopt;
		}

		auto const_ = (ConstSymbol*) def;

		if (ast->index) {
			auto index_value = eval_expr_int(db, ast->index->get());
			if (!index_value) {
				return std::nullopt;
			}

			if (const_->get_size() == 0) {
				// TODO: Eval size
			}

			if (index_value >= const_->get_size()) {
				// TODO: better error messages
				fail_invalid(ast->location);
			}

			return eval_expr_int(
			    db,
			    ((AstLiteralArray*) const_->get_ast().value()->value.get())->value[*index_value].get());
		}

		return eval_expr_int(db, const_->get_ast().value()->value.get());
	}

	std::optional<int> eval_expr_binary_int(TypeStore* db, AstExpressionBinary* ast) {
		auto lhs = eval_expr_int(db, ast->lhs.get());
		auto rhs = eval_expr_int(db, ast->rhs.get());

		if (!lhs || !rhs) {
			return std::nullopt;
		}

		switch (ast->type) {
		case AstBinaryOp::LOGICAL_AND:
			return *lhs && *rhs;
		case AstBinaryOp::LOGICAL_OR:
			return *lhs || *rhs;
		case AstBinaryOp::BITWISE_AND:
			return *lhs & *rhs;
		case AstBinaryOp::BITWISE_OR:
			return *lhs | *rhs;
		case AstBinaryOp::EQUAL:
			return *lhs == *rhs;
		case AstBinaryOp::NOT_EQUAL:
			return *lhs != *rhs;
		case AstBinaryOp::LESS_THAN:
			return *lhs < *rhs;
		case AstBinaryOp::LESS_THAN_OR_EQUAL:
			return *lhs <= *rhs;
		case AstBinaryOp::GREATER_THAN:
			return *lhs > *rhs;
		case AstBinaryOp::GREATER_THAN_OR_EQUAL:
			return *lhs >= *rhs;
		case AstBinaryOp::RIGHT_SHIFT:
			return *lhs >> *rhs;
		case AstBinaryOp::LEFT_SHIFT:
			return *lhs << *rhs;
		case AstBinaryOp::ADD:
			return *lhs + *rhs;
		case AstBinaryOp::SUBTRACT:
			return *lhs - *rhs;
		case AstBinaryOp::MULTIPLY:
			return *lhs * *rhs;
		case AstBinaryOp::DIVIDE:
			return *lhs / *rhs;
		case AstBinaryOp::MODULO:
			return *lhs % *rhs;
		default:
			return std::nullopt;
		}
	}

	std::optional<int> eval_expr_unary_int(TypeStore* db, AstExpressionUnary* ast) {
		auto rhs = eval_expr_int(db, ast->rhs.get());

		if (!rhs) {
			return std::nullopt;
		}

		switch (ast->type) {
		case AstUnaryOp::UNARY_NOT:
			return !*rhs;
		case AstUnaryOp::UNARY_PLUS:
			return +*rhs;
		case AstUnaryOp::UNARY_MINUS:
			return -*rhs;
		case AstUnaryOp::UNARY_COMPLEMENT:
			return ~*rhs;
		}
	}

	std::optional<int> eval_expr_int(TypeStore* db, AstExpression* ast) {
		switch (ast->kind()) {
		case AstNodeKind::LITERAL_INT:
			return ((AstLiteralInteger*) ast)->value;
		case AstNodeKind::EXPR_REFERENCE:
			return eval_expr_ref_int(db, (AstExpressionReference*) ast);
		case AstNodeKind::EXPR_BINARY:
			return eval_expr_binary_int(db, (AstExpressionBinary*) ast);
		case AstNodeKind::EXPR_UNARY:
			return eval_expr_unary_int(db, (AstExpressionUnary*) ast);
		default:
			return std::nullopt;
		}
	}

	[[nodiscard]] static Type check_literal_array(TypeStore* db, AstLiteralArray* ast, Locals& locals) {
		auto type = check_expr(db, ast->value[0].get(), locals);

		// Check that every other type is convertible to the initial one.
		for (auto i = 1u; i < ast->value.size(); ++i) {
			auto next = check_expr(db, ast->value[i].get(), locals);
			if (!next.is_convertible(type)) {
				fail_mismatch(ast->value[i].get(), type, next);
			}
		}

		return type.to_array();
	}

	[[nodiscard]] static Type check_expr_ref(TypeStore* db, AstExpressionReference* ast, Locals& locals) {
		auto def = locals.get_var(ast->name);

		if (def == nullptr) def = db->get(ast->name);
		if (def == nullptr) fail_unresolved(ast->name);

		// Check the index type.
		if (ast->index) {
			auto type_index = check_expr(db, ast->index->get(), locals);
			if (!type_index.is_convertible(Type::INT)) {
				fail_mismatch(ast->index->get(), Type::INT, type_index);
			}
		}

		if (def->get_kind() == SymbolKind::VAR) {
			auto* var = (VarSymbol*) def;
			auto var_type = var->get_type();

			if (ast->index.has_value() && !var_type.is_array()) {
				// TODO(lmichaelis): better error messages
				fail_invalid(ast->name);
			}

			// Check if the index fits (if it's a const expr)
			if (ast->index) {
				auto array_size = eval_expr_int(db, (*var->get_ast())->size->get());
				auto array_idx = eval_expr_int(db, ast->index->get());

				if (!array_size.has_value()) {
					fail_expr(**(*var->get_ast())->size);
				}

				if (array_idx.has_value() && *array_idx >= *array_size) {
					// TODO: better error messages
					fail_invalid((*ast->index)->location);
				}
			}

			// TODO(lmichaelis): Do we have to allow raw arrays here?
			return var_type.to_scalar();
		}

		if (def->get_kind() == SymbolKind::CONST) {
			auto* var = (ConstSymbol*) def;
			auto var_type = var->get_type();

			// Check that the types match.
			if (var_type.is_array() != ast->index.has_value()) {
				// TODO(lmichaelis): better error messages
				fail_invalid(ast->name);
			}

			// Check if the index fits (if it's a const expr)
			if (ast->index) {
				auto array_size = eval_expr_int(db, (*var->get_ast())->size->get());
				auto array_idx = eval_expr_int(db, ast->index->get());

				if (!array_size.has_value()) {
					fail_expr(**(*var->get_ast())->size);
				}

				if (array_idx.has_value() && *array_idx >= *array_size) {
					// TODO: better error messages
					fail_invalid((*ast->index)->location);
				}
			}

			// TODO(lmichaelis): Do we have to allow raw arrays here?
			return var_type.to_scalar();
		}

		// Instances and funcs cannot be indexed.
		if (ast->index) {
			// TODO(lmichaelis): better error messages
			fail_invalid(ast->name);
		}

#ifndef ZK_DAEDALUS_STRICT
		if (def->get_kind() == SymbolKind::PROTOTYPE) {
			return Type {((PrototypeSymbol*) def)->get_base()};
		}
#endif

		if (def->get_kind() == SymbolKind::INSTANCE) {
			return Type {((InstanceSymbol*) def)->get_base_class()};
		}

		if (def->get_kind() == SymbolKind::FUNC) {
			return Type::FUNC;
		}

		// TODO(lmichaelis): better error messages
		fail_invalid(ast->name);
	}

	[[nodiscard]] static Type check_expr_call(TypeStore* db, AstExpressionCall* ast, Locals& locals) {
		auto def = db->get(ast->target);
		if (def == nullptr) {
			fail_unresolved(ast->target);
		}

		auto rtype = Type::VOID;
		std::vector<VarSymbol>* args;

		if (def->get_kind() == SymbolKind::FUNC) {
			auto* func = (FunctionSymbol*) def;
			rtype = func->get_return_type();
			args = &func->get_arguments();
		} else if (def->get_kind() == SymbolKind::EXTERN) {
			auto* extern_ = (ExternSymbol*) def;
			rtype = extern_->get_return_type();
			args = &extern_->get_arguments();
		} else {
			// TODO(lmichaelis): better error messages
			fail_invalid(ast->target);
		}

		// Check that we have the correct number of args.
		if (args->size() != ast->args.size()) {
			// TODO(lmichaelis): better error messages
			fail_invalid(ast->target);
		}

		// Check each argument's type
		for (auto i = 0u; i < args->size(); ++i) {
			auto* arg = ast->args[i].get();
			auto arg_type = check_expr(db, arg, locals);
			auto var_type = args->at(i).get_type();

			if (!arg_type.is_convertible(var_type)) {
				fail_mismatch(arg, var_type, arg_type);
			}
		}

		return rtype;
	}

	[[nodiscard]] static Type check_expr_binary(TypeStore* db, AstExpressionBinary* ast, Locals& locals) {
		auto lhs = check_expr(db, ast->lhs.get(), locals);
		auto rhs = check_expr(db, ast->rhs.get(), locals);

		switch (ast->type) {
		case AstBinaryOp::ASSIGN:
			if (ast->lhs->kind() != AstNodeKind::EXPR_REFERENCE) {
				// TODO(lmichaelis): better error messages
				fail_invalid(ast->lhs->location);
			}

			if (!rhs.is_convertible(lhs)) {
				fail_mismatch(ast->rhs.get(), lhs, rhs);
			}

			return Type::VOID;
		case AstBinaryOp::ASSIGN_ADD:
		case AstBinaryOp::ASSIGN_SUBTRACT:
		case AstBinaryOp::ASSIGN_MULTIPLY:
		case AstBinaryOp::ASSIGN_DIVIDE:
			if (ast->lhs->kind() != AstNodeKind::EXPR_REFERENCE) {
				// TODO(lmichaelis): better error messages
				fail_invalid(ast->lhs->location);
			}

			if (rhs != Type::INT) {
				fail_mismatch(ast->rhs.get(), Type::INT, rhs);
			}

			return Type::VOID;
		case AstBinaryOp::EQUAL:
		case AstBinaryOp::NOT_EQUAL:
		case AstBinaryOp::LESS_THAN:
		case AstBinaryOp::GREATER_THAN:
		case AstBinaryOp::GREATER_THAN_OR_EQUAL:
		case AstBinaryOp::LESS_THAN_OR_EQUAL:
			if (!rhs.is_convertible(Type::INT)) {
				fail_mismatch(ast->rhs.get(), Type::INT, rhs);
			}

			if (!lhs.is_convertible(Type::INT)) {
				fail_mismatch(ast->lhs.get(), Type::INT, lhs);
			}

			return Type::INT;
		default:
			if (lhs != Type::INT) {
				fail_mismatch(ast->lhs.get(), Type::INT, lhs);
			}

			if (rhs != Type::INT) {
				fail_mismatch(ast->rhs.get(), Type::INT, rhs);
			}

			return Type::INT;
		}
	}

	[[nodiscard]] static Type check_expr_unary(TypeStore* db, AstExpressionUnary* ast, Locals& locals) {
		auto rhs = check_expr(db, ast->rhs.get(), locals);

		// TODO: ~ and ! are not supported for FLOATs
		if (rhs != Type::INT && rhs != Type::FLOAT) {
			fail_mismatch(ast->rhs.get(), Type::INT, rhs);
		}

		return Type::INT;
	}

	[[nodiscard]] static Type check_expr(TypeStore* db, AstExpression* ast, Locals& locals) {
		switch (ast->kind()) {
		case AstNodeKind::LITERAL_INT:
			return Type::INT;
		case AstNodeKind::LITERAL_FLOAT:
			return Type::FLOAT;
		case AstNodeKind::LITERAL_STRING:
			return Type::STRING;
		case AstNodeKind::LITERAL_ARRAY:
			return check_literal_array(db, (AstLiteralArray*) ast, locals);
		case AstNodeKind::EXPR_REFERENCE:
			return check_expr_ref(db, (AstExpressionReference*) ast, locals);
		case AstNodeKind::EXPR_CALL:
			return check_expr_call(db, (AstExpressionCall*) ast, locals);
		case AstNodeKind::EXPR_BINARY:
			return check_expr_binary(db, (AstExpressionBinary*) ast, locals);
		case AstNodeKind::EXPR_UNARY:
			return check_expr_unary(db, (AstExpressionUnary*) ast, locals);
		default:
			break;
		}

		throw Error {"Internal compiler error: non-expr passed to `check_expr` during type checking!"};
	}

	[[nodiscard]] static Type check_expr(TypeStore* db, AstExpression* ast) {
		Locals locals;
		return check_expr(db, ast, locals);
	}

	static void check_var(TypeStore* db, VarSymbol* def) {
		auto* ast = def->get_ast().value_or(nullptr);
		if (ast == nullptr) return;

		// Check the array size.
		if (ast->size) {
			// Check the array type.
			auto type = check_expr(db, ast->size->get());
			if (!type.is_convertible(Type::INT)) {
				fail_mismatch(ast->size->get(), Type::INT, type);
			}

			// Calculate the array size.
			auto size = eval_expr_int(db, ast->size->get());
			if (!size.has_value()) {
				fail_expr(**ast->size);
			}

			def->set_size(*size);
		}
	}

	static void check_const(TypeStore* db, ConstSymbol* def) {
		auto* ast = def->get_ast().value_or(nullptr);
		if (ast == nullptr) return;

		// Check the value type.
		auto type = check_expr(db, ast->value.get());
		if (!type.is_convertible(def->get_type())) {
			fail_mismatch(ast->value.get(), def->get_type(), type);
		}

		// Check the array size.
		if (ast->size) {
			// Check the array type.
			type = check_expr(db, ast->size->get());
			if (!type.is_convertible(Type::INT)) {
				fail_mismatch(ast->size->get(), Type::INT, type);
			}

			// Calculate the array size.
			auto size = eval_expr_int(db, ast->size->get());
			if (!size.has_value()) {
				fail_expr(**ast->size);
			}

			def->set_size(*size);
		}

		// TODO(lmichaelis): Evaluate value expression
	}

	static void check_class(TypeStore* db, ClassSymbol* def) {
		// Check each class member.
		for (auto& member : def->get_members()) {
			check_var(db, &member);
		}
	}

	static void check_stmt_block(TypeStore* db, AstStatementBlock* ast, Locals& locals, Type rtype) {
		for (auto& stmt : ast->statements) {
			switch (stmt->kind()) {
			case AstNodeKind::STMT_CONDITION: {
				auto if_ = (AstStatementCondition*) stmt.get();
				auto if_cond = check_expr(db, if_->condition.get(), locals);

				if (!if_cond.is_convertible(Type::INT)) {
					fail_mismatch(if_->condition.get(), Type::INT, if_cond);
				}

				check_stmt_block(db, &if_->body, locals, rtype);

				for (auto& elif : if_->else_if) {
					if_cond = check_expr(db, elif.condition.get(), locals);
					if (!if_cond.is_convertible(Type::INT)) {
						fail_mismatch(elif.condition.get(), Type::INT, if_cond);
					}

					check_stmt_block(db, &elif.body, locals, rtype);
				}

				if (if_->else_body.has_value()) {
					check_stmt_block(db, &*if_->else_body, locals, rtype);
				}

				break;
			}
			case AstNodeKind::STMT_EXPRESSION: {
				auto type = check_expr(db, ((AstStatementExpression*) stmt.get())->expr.get(), locals);
				if (type != Type::VOID) {
					// TODO: WARN
				}
				break;
			}
			case AstNodeKind::STMT_RETURN: {
				auto ret = (AstStatementReturn*) stmt.get();
				auto expr = Type::VOID;

				if (ret->value) {
					expr = check_expr(db, ret->value->get(), locals);
				}

				if (!expr.is_convertible(rtype)) {
					fail_mismatch(ret, rtype, expr);
				}

				break;
			}
			default:
				break;
			}
		}
	}

	static void check_prototype(TypeStore* db, PrototypeSymbol* def) {
		auto* ast = def->get_ast().value_or(nullptr);
		if (ast == nullptr) return;

		auto* base = def->get_base();

		Locals locals;
		locals.push(&base->get_members());

		check_stmt_block(db, &ast->body, locals, Type::VOID);
	}

	static void check_instance(TypeStore* db, InstanceSymbol* def) {
		auto* ast = def->get_ast().value_or(nullptr);
		if (ast == nullptr) return;
		if (!ast->body) return;

		auto* base = def->get_base_class();

		Locals locals;

#ifndef ZK_DAEDALUS_STRICT
		// `slf.` is permitted
		std::vector<VarSymbol> orig_fixes;
		orig_fixes.emplace_back("SLF", Type {base});
		locals.push(&orig_fixes);
#endif

		locals.push(&base->get_members());

		check_stmt_block(db, &*ast->body, locals, Type::VOID);
	}

	static void check_func(TypeStore* db, FunctionSymbol* def) {
		auto* ast = def->get_ast().value_or(nullptr);
		if (ast == nullptr) return;

		Locals locals;
		locals.push(&def->get_arguments());
		locals.push(&def->get_scope().get_local_vars());

		check_stmt_block(db, &ast->body, locals, def->get_return_type());
	}

	void check_types(TypeStore* db) {
		db->each([db](Symbol* def) {
			if (def->get_kind() == SymbolKind::CONST) {
				check_const(db, (ConstSymbol*) def);
			} else if (def->get_kind() == SymbolKind::VAR) {
				check_var(db, (VarSymbol*) def);
			} else if (def->get_kind() == SymbolKind::CLASS) {
				check_class(db, (ClassSymbol*) def);
			}
		});

		db->each([db](Symbol* def) {
			if (def->get_kind() == SymbolKind::FUNC) {
				check_func(db, (FunctionSymbol*) def);
			} else if (def->get_kind() == SymbolKind::PROTOTYPE) {
				check_prototype(db, (PrototypeSymbol*) def);
			} else if (def->get_kind() == SymbolKind::INSTANCE) {
				check_instance(db, (InstanceSymbol*) def);
			}
		});
	}
} // namespace zenkit::daedalus
