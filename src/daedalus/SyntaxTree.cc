// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/daedalus/SyntaxTree.hh"

#include "../Internal.hh"
#include "Tokenizer.hh"

#include <memory>
#include <utility>

#define TRY(x, ...)                                                                                                    \
	do {                                                                                                               \
		if (!(x)) {                                                                                                    \
			report(tk.location(), __VA_ARGS__);                                                                        \
		}                                                                                                              \
	} while (false)

#define WRN(x, ...)                                                                                                    \
	do {                                                                                                               \
		if (!(x)) {                                                                                                    \
			warn(tk.location(), __VA_ARGS__);                                                                          \
		}                                                                                                              \
	} while (false)

namespace zenkit::daedalus {
	static std::string KW_CONST = "const";
	static std::string KW_VAR = "var";
	static std::string KW_FUNC = "func";
	static std::string KW_CLASS = "class";
	static std::string KW_INSTANCE = "instance";
	static std::string KW_PROTOTYPE = "prototype";
	static std::string KW_IF = "if";
	static std::string KW_ELSE = "else";
	static std::string KW_RETURN = "return";
	static std::string KW_EXTERN = "extern";

	[[noreturn]] static void report(SourceLocation loc, std::string message) {
		throw SyntaxError {std::move(loc), std::move(message)};
	}

	[[maybe_unused]] static void warn(SourceLocation const& loc, char const* message) {
		auto filename = loc.source->path.empty() ? "<memory>" : loc.source->path;
		ZKLOGW("Daedalus.Compiler",
		       "\n\nSyntaxWarning: %s\n --> %s:%zu:%zu\n\n%s\n",
		       message,
		       filename.c_str(),
		       loc.line,
		       loc.column,
		       loc.format().c_str());
	}

	[[nodiscard]] static bool parse_stmt_block(Tokenizer&, AstStatementBlock*);
	[[nodiscard]] static bool parse_expr_assign(Tokenizer&, AstExpression::ptr*);
	[[nodiscard]] static bool parse_var_decl(Tokenizer&, std::vector<AstVariable>*);

	/// ident = (ALPHA / "_") *(ALPHA / DIGIT / "_")
	[[nodiscard]] static bool parse_ident(Tokenizer& tk, AstIdentifier* ast) {
		if (!tk.match(Token::IDENTIFIER)) {
			return false;
		}

		ast->location = tk.location();
		ast->value = tk.get_value();
		return true;
	}

	/// ident-qual = ident ["." ident]
	[[nodiscard]] static bool parse_ident_qual(Tokenizer& tk, AstQualifiedIdentifier* ast) {
		if (!parse_ident(tk, &ast->base)) {
			return false;
		}

		ast->location = tk.location();

		if (tk.match(Token::DOT)) {
			TRY(parse_ident(tk, &ast->element.emplace()), "Expected <ident>");
		}

		return true;
	}

	/// expr = expr-assign
	[[nodiscard]] static bool parse_expr(Tokenizer& tk, AstExpression::ptr* ast) {
		return parse_expr_assign(tk, ast);
	}

	/// literal-string = DQUOTE *(ALPHA / DIGIT / WSP / "!" / %x23-7E) DQUOTE
	[[nodiscard]] static bool parse_literal_string(Tokenizer& tk, AstExpression::ptr* ast) {
		if (!tk.match(Token::STRING)) {
			return false;
		}

		auto expr = new AstLiteralString;
		expr->location = tk.location();
		expr->value = tk.get_value();
		ast->reset(expr);
		return true;
	}

	/// literal-int = 1*DIGIT
	[[nodiscard]] static bool parse_literal_int(Tokenizer& tk, AstExpression::ptr* ast) {
		if (!tk.match(Token::INTEGER)) {
			return false;
		}

		auto expr = new AstLiteralInteger;
		expr->location = tk.location();
		expr->value = tk.get_value_int();
		ast->reset(expr);
		return true;
	}

	/// literal-float = 1*DIGIT ["." *DIGIT]
	[[nodiscard]] static bool parse_literal_float(Tokenizer& tk, AstExpression::ptr* ast) {
		if (!tk.match(Token::FLOAT)) {
			return false;
		}

		auto expr = new AstLiteralFloat;
		expr->location = tk.location();
		expr->value = tk.get_value_float();
		ast->reset(expr);
		return true;
	}

	/// literal-array = "{" expr *("," expr) [","] "}"
	[[nodiscard]] static bool parse_literal_array(Tokenizer& tk, AstExpression::ptr* ast) {
		if (!tk.match(Token::LBRACE)) {
			return false;
		}

		auto expr = new AstLiteralArray;
		expr->location = tk.location();
		ast->reset(expr);

		while (!tk.match(Token::RBRACE)) {
			auto& val = expr->value.emplace_back();
			TRY(parse_expr(tk, &val), "Expected <expr>");

			if (tk.match(Token::COMMA, Token::RBRACE)) {
				// Allow trailing commas.
				if (tk.last() == Token::RBRACE) break;
			} else {
				// Reject missing commas.
				report(tk.location(), "Expected ','");
			}
		}

		return true;
	}

	/// expr-call-args = [expr *("," expr)]
	/// expr-call = ident "(" expr-call-args ")"
	/// expr-ref = ident-qual [ "[" expr "]" ]
	[[nodiscard]] static bool parse_expr_ref_or_call(Tokenizer& tk, AstExpression::ptr* ast) {
		AstQualifiedIdentifier ident;
		if (!parse_ident_qual(tk, &ident)) {
			return false;
		}

		if (tk.match(Token::LPAREN)) {
			auto expr = new AstExpressionCall;
			expr->location = ident.location;
			expr->target = std::move(ident);
			ast->reset(expr);

			while (!tk.match(Token::RPAREN)) {
				auto& arg = expr->args.emplace_back();
				TRY(parse_expr(tk, &arg), "Expected <expr>");

				if (tk.match(Token::COMMA, Token::RPAREN)) {
					// Allow trailing commas.
					if (tk.last() == Token::RPAREN) {
						break;
					}
				} else {
					// Reject missing commas.
					report(tk.location(), "Expected ','");
				}
			}
		} else {
			auto expr = new AstExpressionReference;
			expr->location = ident.location;
			expr->name = std::move(ident);
			ast->reset(expr);

			if (tk.match(Token::LBRACKET)) {
				auto& sz = expr->index.emplace();
				TRY(parse_expr(tk, &sz), "Expected <expr>");
				TRY(tk.match(Token::RBRACKET), "Expected ']'");
			}
		}

		return true;
	}

	/// expr-group = "(" expr ")"
	[[nodiscard]] static bool parse_expr_group(Tokenizer& tk, AstExpression::ptr* ast) {
		if (!tk.match(Token::LPAREN)) {
			return false;
		}

		TRY(parse_expr(tk, ast), "Expected <expr>");
		TRY(tk.match(Token::RPAREN), "Expected ')'");
		return true;
	}

	/// expr-primary = literal-string / literal-int / literal-float / literal-array / expr-ref / expr-call / expr-group
	[[nodiscard]] static bool parse_expr_primary(Tokenizer& tk, AstExpression::ptr* ast) {
		return parse_literal_int(tk, ast) || parse_literal_float(tk, ast) || parse_literal_string(tk, ast) ||
		    parse_literal_array(tk, ast) || parse_expr_ref_or_call(tk, ast) || parse_expr_group(tk, ast);
	}

	/// expr-unary = (("!" / "+" / "-" / "~") expr-unary) / expr-primary
	[[nodiscard]] static bool parse_expr_unary(Tokenizer& tk, AstExpression::ptr* ast) {
		if (tk.match(Token::NOT, Token::PLUS, Token::MINUS, Token::COMPLEMENT)) {
			auto expr = new AstExpressionUnary();
			expr->location = tk.location();

			// clang-format off
			if (tk.last() == Token::NOT) expr->type = AstUnaryOp::UNARY_NOT;
			if (tk.last() == Token::PLUS) expr->type = AstUnaryOp::UNARY_PLUS;
			if (tk.last() == Token::MINUS) expr->type = AstUnaryOp::UNARY_MINUS;
			if (tk.last() == Token::COMPLEMENT) expr->type = AstUnaryOp::UNARY_COMPLEMENT;
			// clang-format on

			TRY(parse_expr_unary(tk, &expr->rhs), "Expected <expr-unary>");

			ast->reset(expr);
			return true;
		}

		return parse_expr_primary(tk, ast);
	}

	/// expr-factor = expr-unary [("*" / "/" / "%") expr]
	[[nodiscard]] static bool parse_expr_factor(Tokenizer& tk, AstExpression::ptr* ast) {
		if (!parse_expr_unary(tk, ast)) {
			return false;
		}

		if (tk.match(Token::MULTIPLY, Token::DIVIDE, Token::MODULO)) {
			auto expr = new AstExpressionBinary();
			expr->location = tk.location();

			// clang-format off
			if (tk.last() == Token::MULTIPLY) expr->type = AstBinaryOp::MULTIPLY;
			if (tk.last() == Token::DIVIDE) expr->type = AstBinaryOp::DIVIDE;
			if (tk.last() == Token::MODULO) expr->type = AstBinaryOp::MODULO;
			// clang-format on

			TRY(parse_expr(tk, &expr->rhs), "Expected <expr>");

			ast->swap(expr->lhs);
			ast->reset(expr);
		}

		return true;
	}

	/// expr-term = expr-factor [("+" / "-") expr]
	[[nodiscard]] static bool parse_expr_term(Tokenizer& tk, AstExpression::ptr* ast) {
		if (!parse_expr_factor(tk, ast)) {
			return false;
		}

		if (tk.match(Token::PLUS, Token::MINUS)) {
			auto expr = new AstExpressionBinary();
			expr->location = tk.location();

			// clang-format off
			if (tk.last() == Token::PLUS) expr->type = AstBinaryOp::ADD;
			if (tk.last() == Token::MINUS) expr->type = AstBinaryOp::SUBTRACT;
			// clang-format on

			TRY(parse_expr(tk, &expr->rhs), "Expected <expr>");

			ast->swap(expr->lhs);
			ast->reset(expr);
		}

		return true;
	}

	/// expr-shift = expr-term [("<<" / ">>") expr]
	[[nodiscard]] static bool parse_expr_shift(Tokenizer& tk, AstExpression::ptr* ast) {
		if (!parse_expr_term(tk, ast)) {
			return false;
		}

		if (tk.match(Token::RIGHT_SHIFT, Token::LEFT_SHIFT)) {
			auto expr = new AstExpressionBinary();
			expr->location = tk.location();

			// clang-format off
			if (tk.last() == Token::RIGHT_SHIFT) expr->type = AstBinaryOp::RIGHT_SHIFT;
			if (tk.last() == Token::LEFT_SHIFT) expr->type = AstBinaryOp::LEFT_SHIFT;
			// clang-format on

			TRY(parse_expr(tk, &expr->rhs), "Expected <expr>");

			ast->swap(expr->lhs);
			ast->reset(expr);
		}

		return true;
	}

	/// expr-rel = expr-shift [("<" / "<=" / ">=" / ">") expr]
	[[nodiscard]] static bool parse_expr_rel(Tokenizer& tk, AstExpression::ptr* ast) {
		if (!parse_expr_shift(tk, ast)) {
			return false;
		}

		if (tk.match(Token::LESS_THAN, Token::GREATER_THAN, Token::LESS_THAN_OR_EQUAL, Token::GREATER_THAN_OR_EQUAL)) {
			auto expr = new AstExpressionBinary();
			expr->location = tk.location();

			// clang-format off
			if (tk.last() == Token::LESS_THAN) expr->type = AstBinaryOp::LESS_THAN;
			if (tk.last() == Token::GREATER_THAN) expr->type = AstBinaryOp::GREATER_THAN;
			if (tk.last() == Token::LESS_THAN_OR_EQUAL) expr->type = AstBinaryOp::LESS_THAN_OR_EQUAL;
			if (tk.last() == Token::GREATER_THAN_OR_EQUAL) expr->type = AstBinaryOp::GREATER_THAN_OR_EQUAL;
			// clang-format on

			TRY(parse_expr(tk, &expr->rhs), "Expected <expr>");

			ast->swap(expr->lhs);
			ast->reset(expr);
		}

		return true;
	}

	/// expr-equal = expr-rel [("==" / "!=") expr]
	[[nodiscard]] static bool parse_expr_equal(Tokenizer& tk, AstExpression::ptr* ast) {
		if (!parse_expr_rel(tk, ast)) {
			return false;
		}

		if (tk.match(Token::EQUAL, Token::NOT_EQUAL)) {
			auto expr = new AstExpressionBinary();
			expr->location = tk.location();

			// clang-format off
			if (tk.last() == Token::EQUAL) expr->type = AstBinaryOp::EQUAL;
			if (tk.last() == Token::NOT_EQUAL) expr->type = AstBinaryOp::NOT_EQUAL;
			// clang-format on

			TRY(parse_expr(tk, &expr->rhs), "Expected <expr>");

			ast->swap(expr->lhs);
			ast->reset(expr);
		}

		return true;
	}

	/// expr-bit = expr-equal [("&" / "|") expr]
	[[nodiscard]] static bool parse_expr_bit(Tokenizer& tk, AstExpression::ptr* ast) {
		if (!parse_expr_equal(tk, ast)) {
			return false;
		}

		if (tk.match(Token::BITWISE_AND, Token::BITWISE_OR)) {
			auto expr = new AstExpressionBinary();
			expr->location = tk.location();

			// clang-format off
			if (tk.last() == Token::BITWISE_AND) expr->type = AstBinaryOp::BITWISE_AND;
			if (tk.last() == Token::BITWISE_OR) expr->type = AstBinaryOp::BITWISE_OR;
			// clang-format on

			TRY(parse_expr(tk, &expr->rhs), "Expected <expr>");

			ast->swap(expr->lhs);
			ast->reset(expr);
		}

		return true;
	}

	/// expr-logic = expr-bit [("&&" / "||") expr]
	[[nodiscard]] static bool parse_expr_logic(Tokenizer& tk, AstExpression::ptr* ast) {
		if (!parse_expr_bit(tk, ast)) {
			return false;
		}

		if (tk.match(Token::LOGICAL_AND, Token::LOGICAL_OR)) {
			auto expr = new AstExpressionBinary();
			expr->location = tk.location();

			// clang-format off
			if (tk.last() == Token::LOGICAL_AND) expr->type = AstBinaryOp::LOGICAL_AND;
			if (tk.last() == Token::LOGICAL_OR) expr->type = AstBinaryOp::LOGICAL_OR;
			// clang-format on

			TRY(parse_expr(tk, &expr->rhs), "Expected <expr>");

			ast->swap(expr->lhs);
			ast->reset(expr);
		}

		return true;
	}

	/// expr-assign = expr-logic [("=" / "+=" / "-=" / "*=" / "/=") expr]
	[[nodiscard]] bool parse_expr_assign(Tokenizer& tk, AstExpression::ptr* ast) {
		if (!parse_expr_logic(tk, ast)) {
			return false;
		}

		if (tk.match(Token::ASSIGN,
		             Token::ASSIGN_ADD,
		             Token::ASSIGN_SUBTRACT,
		             Token::ASSIGN_MULTIPLY,
		             Token::ASSIGN_DIVIDE)) {
			auto expr = new AstExpressionBinary();
			expr->location = tk.location();

			// clang-format off
			if (tk.last() == Token::ASSIGN) expr->type = AstBinaryOp::ASSIGN;
			if (tk.last() == Token::ASSIGN_ADD) expr->type = AstBinaryOp::ASSIGN_ADD;
			if (tk.last() == Token::ASSIGN_SUBTRACT) expr->type = AstBinaryOp::ASSIGN_SUBTRACT;
			if (tk.last() == Token::ASSIGN_MULTIPLY) expr->type = AstBinaryOp::ASSIGN_MULTIPLY;
			if (tk.last() == Token::ASSIGN_DIVIDE) expr->type = AstBinaryOp::ASSIGN_DIVIDE;
			// clang-format on

			TRY(parse_expr(tk, &expr->rhs), "Expected <expr>");

			ast->swap(expr->lhs);
			ast->reset(expr);
		}

		return true;
	}

	/// stmt-cond = "if" expr stmt-block *("else" "if" expr stmt-block) ["else" stmt-block]
	[[nodiscard]] static bool parse_stmt_cond(Tokenizer& tk, AstStatementBlock* root) {
		if (!tk.match_kw(KW_IF)) {
			return false;
		}

		auto ast = new AstStatementCondition;
		root->statements.emplace_back(ast);
		ast->location = tk.location();

		TRY(parse_expr(tk, &ast->condition), "Expected <expr>");
		TRY(parse_stmt_block(tk, &ast->body), "Expected <stmt-block>");

		// Accept multiple `else if`-blocks or a single `else`-block
		while (tk.match_kw(KW_ELSE)) {
			// Accept a single `else`-block
			if (!tk.match_kw(KW_IF)) {
				auto& else_ = ast->else_body.emplace();
				TRY(parse_stmt_block(tk, &else_), "Expected <stmt-block>");
				break;
			}

			// Accept an `else-if` block
			auto& elif = ast->else_if.emplace_back();
			elif.location = tk.location();

			TRY(parse_expr(tk, &elif.condition), "Expected <expr>");
			TRY(parse_stmt_block(tk, &elif.body), "Expected <stmt-block>");
		}

		return true;
	}

	/// stmt-return = "return" [expr]
	[[nodiscard]] static bool parse_stmt_return(Tokenizer& tk, AstStatementBlock* root) {
		if (!tk.match_kw(KW_RETURN)) {
			return false;
		}

		auto ast = new AstStatementReturn;
		root->statements.emplace_back(ast);
		ast->location = tk.location();

		AstExpression::ptr expr;
		if (parse_expr(tk, &expr)) {
			ast->value.emplace(std::move(expr));
		}

		return true;
	}

	/// stmt-decl = var-decl
	[[nodiscard]] static bool parse_stmt_decl(Tokenizer& tk, AstStatementBlock* root) {
		return parse_var_decl(tk, &root->locals);
	}

	/// stmt-expr = expr
	[[nodiscard]] static bool parse_stmt_expr(Tokenizer& tk, AstStatementBlock* root) {
		AstExpression::ptr expr;

		if (!parse_expr(tk, &expr)) {
			return false;
		}

		auto ast = new AstStatementExpression;
		root->statements.emplace_back(ast);
		ast->location = expr->location;
		ast->expr = std::move(expr);

		return true;
	}

	/// stmt = stmt-cond / stmt-return / stmt-decl / stmt-expr
	[[nodiscard]] static bool parse_stmt(Tokenizer& tk, AstStatementBlock* root) {
		return parse_stmt_cond(tk, root) || parse_stmt_return(tk, root) || parse_stmt_decl(tk, root) ||
		    parse_stmt_expr(tk, root);
	}

	/// stmt-block = "{" *(stmt [";"]) "}"
	[[nodiscard]] static bool parse_stmt_block(Tokenizer& tk, AstStatementBlock* root) {
		if (!tk.match(Token::LBRACE)) {
			return false;
		}

		root->location = tk.location();

		while (parse_stmt(tk, root)) {
#ifndef ZK_DAEDALUS_STRICT
			WRN(tk.match(Token::SEMICOLON), "Expected ';'");
#else
			TRY(tk.match(Token::SEMICOLON), "Expected ';' (strict-mode)");
#endif
		}

		TRY(tk.match(Token::RBRACE), "Expected '}'");
		return true;
	}

	static void parse_var_decl_base(Tokenizer& tk, AstVariable* ast) {
		ast->location = tk.location();

		TRY(parse_ident(tk, &ast->type), "Expected <ident>");
		TRY(parse_ident(tk, &ast->name), "Expected <ident>");

		if (tk.match(Token::LBRACKET)) {
			auto& expr = ast->size.emplace();
			TRY(parse_expr(tk, &expr), "Expected <expr>");
			TRY(tk.match(Token::RBRACKET), "Expected ']'");
		}
	}

	/// var-decl-single = "var" ident ident [ "[" expr "]" ]
	[[nodiscard]] static bool parse_var_decl(Tokenizer& tk, AstVariable* ast) {
		if (!tk.match_kw(KW_VAR)) {
			return false;
		}

		parse_var_decl_base(tk, ast);
		return true;
	}

	/// var-decl = var-decl-single / var-decl-multi
	[[nodiscard]] static bool parse_var_decl(Tokenizer& tk, std::vector<AstVariable>* root) {
		if (!tk.match_kw(KW_VAR)) {
			return false;
		}

		auto location = tk.location();
		std::vector<AstIdentifier> names;
		AstIdentifier type;

		TRY(parse_ident(tk, &type), "Expected <ident>");

		do {
			auto& name = names.emplace_back();
			TRY(parse_ident(tk, &name), "Expected <ident>");
		} while (tk.match(Token::COMMA));

		if (names.size() == 1) {
			auto& ast = root->emplace_back();
			ast.location = location;
			ast.name = std::move(names[0]);
			ast.type = type;

			if (tk.match(Token::LBRACKET)) {
				auto& expr = ast.size.emplace();
				TRY(parse_expr(tk, &expr), "Expected <expr>");
				TRY(tk.match(Token::RBRACKET), "Expected ']'");
			}

			return true;
		}

		for (auto& name : names) {
			auto& ast = root->emplace_back();
			ast.location = location;
			ast.name = std::move(name);
			ast.type = type;
		}

		return true;
	}

	/// var-decl-multi = "var" ident ident *("," ident)
	[[nodiscard]] static bool parse_var_decl(Tokenizer& tk, AstScript* root) {
		return parse_var_decl(tk, &root->variables);
	}

	static void parse_const_decl_base(Tokenizer& tk, AstConstant* ast) {
		ast->location = tk.location();

		TRY(parse_ident(tk, &ast->type), "Expected <ident>");
		TRY(parse_ident(tk, &ast->name), "Expected <ident>");

		if (tk.match(Token::LBRACKET)) {
			auto& expr = ast->size.emplace();
			TRY(parse_expr(tk, &expr), "Expected <expr>");
			TRY(tk.match(Token::RBRACKET), "Expected ']'");
		}

		TRY(tk.match(Token::ASSIGN), "Expected '='");
		TRY(parse_expr(tk, &ast->value), "Expected <expr>");
	}

	/// const-decl = "const" ident ident [ "[" expr "]" ] "=" expr
	[[nodiscard]] static bool parse_const_decl(Tokenizer& tk, AstScript* root) {
		if (!tk.match_kw(KW_CONST)) {
			return false;
		}

		auto& ast = root->constants.emplace_back();
		parse_const_decl_base(tk, &ast);
		return true;
	}

	/// class-decl = "class" ident "{" *(var-decl ";") "}"
	[[nodiscard]] static bool parse_class_decl(Tokenizer& tk, AstScript* root) {
		if (!tk.match_kw(KW_CLASS)) {
			return false;
		}

		auto& ast = root->classes.emplace_back();
		ast.location = tk.location();

		TRY(parse_ident(tk, &ast.name), "Expected <ident>");
		TRY(tk.match(Token::LBRACE), "Expected '{'");

		while (!tk.match(Token::RBRACE)) {
			TRY(parse_var_decl(tk, &ast.members), "Expected <var-decl>");
			TRY(tk.match(Token::SEMICOLON), "Expected ';'");
		}

		return true;
	}

	/// inst-decl = "instance" ident *("," ident) "(" ident ")" [stmt-block]
	[[nodiscard]] static bool parse_inst_decl(Tokenizer& tk, AstScript* root) {
		if (!tk.match_kw(KW_INSTANCE)) {
			return false;
		}

		auto location = tk.location();

		std::vector<AstIdentifier> names;
		AstIdentifier base;

		do {
			auto& name = names.emplace_back();
			TRY(parse_ident(tk, &name), "Expected <ident>");
		} while (tk.match(Token::COMMA));

		TRY(tk.match(Token::LPAREN), "Expected '('");
		TRY(parse_ident(tk, &base), "Expected <ident>");
		TRY(tk.match(Token::RPAREN), "Expected ')'");

		AstStatementBlock body;
		bool has_body = parse_stmt_block(tk, &body);

		// When declaring multiple instances at once, instance bodies are not supported.
		if (has_body && names.size() > 1) {
			report(body.location, "Unexpected <stmt-block>");
		}

		for (auto& name : names) {
			auto& ast = root->instances.emplace_back();
			ast.location = location;
			ast.name = std::move(name);
			ast.base = base;

			if (has_body) {
				ast.body = std::move(body);
				has_body = false;
			}
		}

		return true;
	}

	/// proto-decl = "prototype" ident "(" ident ")" stmt-block
	[[nodiscard]] static bool parse_proto_decl(Tokenizer& tk, AstScript* root) {
		if (!tk.match_kw(KW_PROTOTYPE)) {
			return false;
		}

		auto& ast = root->prototypes.emplace_back();
		ast.location = tk.location();

		TRY(parse_ident(tk, &ast.name), "Expected <ident>");
		TRY(tk.match(Token::LPAREN), "Expected '('");
		TRY(parse_ident(tk, &ast.base), "Expected <ident>");
		TRY(tk.match(Token::RPAREN), "Expected ')'");
		TRY(parse_stmt_block(tk, &ast.body), "Expected <stmt-block>");
		return true;
	}

	/// func-decl-args = "(" [ var-decl *("," var-decl) [","] ] ")"
	[[nodiscard]] static bool parse_func_decl_args(Tokenizer& tk, std::vector<AstVariable>* root) {
		if (!tk.match(Token::LPAREN)) {
			return false;
		}

		while (!tk.match(Token::RPAREN)) {
			auto& var = root->emplace_back();
			TRY(parse_var_decl(tk, &var), "Expected <var-decl> in <func-decl-args>");

			if (tk.match(Token::COMMA, Token::RPAREN)) {
				// Allow trailing commas.
				if (tk.last() == Token::RPAREN) break;
			} else {
				// Reject missing commas.
				report(tk.location(), "Expected ','");
			}
		}

		return true;
	}

	/// func-decl = "func" ident ident func-decl-args stmt-block
	[[nodiscard]] static bool parse_func_decl(Tokenizer& tk, AstScript* root) {
		if (!tk.match_kw(KW_FUNC)) {
			return false;
		}

		auto& ast = root->functions.emplace_back();
		ast.location = tk.location();

		TRY(parse_ident(tk, &ast.rtype), "Expected <ident>");
		TRY(parse_ident(tk, &ast.name), "Expected <ident>");

		TRY(parse_func_decl_args(tk, &ast.args), "Expected <func-decl-args>");
		TRY(parse_stmt_block(tk, &ast.body), "Expected <stmt-block>");

		return true;
	}

	/// extern-decl = "extern" "func" ident ident func-decl-args
	[[nodiscard]] static bool parse_extern_decl(Tokenizer& tk, AstScript* root) {
		if (!tk.match_kw(KW_EXTERN)) {
			return false;
		}

		TRY(tk.match_kw(KW_FUNC), "Expected 'func'");

		auto& ast = root->externals.emplace_back();
		ast.location = tk.location();

		TRY(parse_ident(tk, &ast.rtype), "Expected <ident>");
		TRY(parse_ident(tk, &ast.name), "Expected <ident>");

		TRY(parse_func_decl_args(tk, &ast.args), "Expected <func-decl-args>");
		return true;
	}

	/// script-decl-node = const-decl / var-decl / class-decl / proto-decl / inst-decl / func-decl / extern-decl
	[[nodiscard]] static bool parse_script_decl_node(Tokenizer& tk, AstScript* ast) {
		return parse_const_decl(tk, ast) || parse_var_decl(tk, ast) || parse_class_decl(tk, ast) ||
		    parse_proto_decl(tk, ast) || parse_inst_decl(tk, ast) || parse_func_decl(tk, ast) ||
		    parse_extern_decl(tk, ast);
	}

	/// script-decl = *(script-decl-node ";")
	void parse_script_decl(Tokenizer& tk, AstScript* ast) {
		do {
			if (tk.match(Token::END_OF_FILE)) {
				break;
			}

			TRY(parse_script_decl_node(tk, ast), "Expected <script-decl-node>");
			TRY(tk.match(Token::SEMICOLON), "Expected ';'");
		} while (true);
	}

	void parse_script(AstScript* ast, std::filesystem::path const& source) {
		Tokenizer tk {std::make_shared<CompilationSource>(source)};
		parse_script_decl(tk, ast);
	}

	void parse_script(AstScript* ast, std::unique_ptr<Read> source) {
		Tokenizer tk {std::make_shared<CompilationSource>(std::move(source))};
		parse_script_decl(tk, ast);
	}

	void parse_script(AstScript* ast, CompilationSource source) {
		Tokenizer tk {std::make_shared<CompilationSource>(std::move(source))};
		parse_script_decl(tk, ast);
	}

	void parse_script(AstScript* ast, std::shared_ptr<CompilationSource> source) {
		Tokenizer tk {std::move(source)};
		parse_script_decl(tk, ast);
	}
} // namespace zenkit::daedalus
