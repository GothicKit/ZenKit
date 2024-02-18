// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Stream.hh"
#include "zenkit/daedalus/Compiler.hh"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#define ZK_AST_NODE(T, k)                                                                                              \
public:                                                                                                                \
	using ptr = std::unique_ptr<T>;                                                                                    \
                                                                                                                       \
	[[nodiscard]] inline AstNodeKind kind() const noexcept override {                                                  \
		return AstNodeKind::k;                                                                                         \
	}

namespace zenkit::daedalus {
	enum class AstNodeKind {
		SCRIPT,
		IDENTIFIER,
		IDENTIFIER_QUALIFIED,
		CONSTANT,
		CLASS,
		VARIABLE,
		INSTANCE,
		PROTOTYPE,
		FUNCTION,
		EXTERNAL,

		LITERAL_INT,
		LITERAL_FLOAT,
		LITERAL_STRING,
		LITERAL_ARRAY,

		EXPR_REFERENCE,
		EXPR_CALL,
		EXPR_BINARY,
		EXPR_UNARY,

		STMT_BLOCK,
		STMT_CONDITION,
		STMT_EXPRESSION,
		STMT_RETURN,
	};

	enum class AstBinaryOp {
		ASSIGN = 0,
		ASSIGN_ADD,
		ASSIGN_SUBTRACT,
		ASSIGN_MULTIPLY,
		ASSIGN_DIVIDE,
		LOGICAL_AND,
		LOGICAL_OR,
		BITWISE_AND,
		BITWISE_OR,
		EQUAL,
		NOT_EQUAL,
		LESS_THAN,
		LESS_THAN_OR_EQUAL,
		GREATER_THAN,
		GREATER_THAN_OR_EQUAL,
		RIGHT_SHIFT,
		LEFT_SHIFT,
		ADD,
		SUBTRACT,
		MULTIPLY,
		DIVIDE,
		MODULO,
	};

	enum class AstUnaryOp {
		UNARY_NOT,
		UNARY_PLUS,
		UNARY_MINUS,
		UNARY_COMPLEMENT,
	};

	class AstNode {
	public:
		using ptr = std::unique_ptr<AstNode>;

		virtual ~AstNode() noexcept = default;
		[[nodiscard]] virtual AstNodeKind kind() const noexcept = 0;

	public:
		SourceLocation location {};
	};

	class AstIdentifier final : public AstNode {
		ZK_AST_NODE(AstIdentifier, IDENTIFIER);

	public:
		std::string value;
	};

	class AstQualifiedIdentifier final : public AstNode {
		ZK_AST_NODE(AstQualifiedIdentifier, IDENTIFIER_QUALIFIED);

	public:
		AstIdentifier base;
		std::optional<AstIdentifier> element;
	};

	class AstExpression : public AstNode {
	public:
		using ptr = std::unique_ptr<AstExpression>;
	};

	class AstLiteralInteger final : public AstExpression {
		ZK_AST_NODE(AstLiteralInteger, LITERAL_INT);

	public:
		int value {0};
	};

	class AstLiteralFloat final : public AstExpression {
		ZK_AST_NODE(AstLiteralFloat, LITERAL_FLOAT);

	public:
		float value {0};
	};

	class AstLiteralString final : public AstExpression {
		ZK_AST_NODE(AstLiteralString, LITERAL_STRING);

	public:
		std::string value;
	};

	class AstLiteralArray final : public AstExpression {
		ZK_AST_NODE(AstLiteralArray, LITERAL_ARRAY);

	public:
		using AstExpression::AstExpression;

	public:
		std::vector<AstExpression::ptr> value;
	};

	class AstExpressionReference final : public AstExpression {
		ZK_AST_NODE(AstExpressionReference, EXPR_REFERENCE);

	public:
		using AstExpression::AstExpression;

	public:
		AstQualifiedIdentifier name;
		std::optional<AstExpression::ptr> index;
	};

	class AstExpressionBinary final : public AstExpression {
		ZK_AST_NODE(AstExpressionBinary, EXPR_BINARY)

	public:
	public:
		AstBinaryOp type;
		AstExpression::ptr lhs;
		AstExpression::ptr rhs;
	};

	class AstExpressionUnary final : public AstExpression {
		ZK_AST_NODE(AstExpressionUnary, EXPR_UNARY);

	public:
		AstUnaryOp type;
		AstExpression::ptr rhs;
	};

	class AstExpressionCall final : public AstExpression {
		ZK_AST_NODE(AstExpressionCall, EXPR_CALL);

	public:
		using AstExpression::AstExpression;

	public:
		AstQualifiedIdentifier target;
		std::vector<AstExpression::ptr> args;
	};

	class AstVariable : public AstNode {
		ZK_AST_NODE(AstVariable, VARIABLE);

	public:
		using AstNode::AstNode;

	public:
		AstIdentifier name;
		AstIdentifier type;
		std::optional<AstExpression::ptr> size;
	};

	class AstConstant final : public AstVariable {
		ZK_AST_NODE(AstConstant, CONSTANT);

	public:
		using AstVariable::AstVariable;

	public:
		AstExpression::ptr value;
	};

	class AstStatement : public AstNode {
	public:
		using ptr = std::unique_ptr<AstStatement>;
		using AstNode::AstNode;
	};

	class AstStatementBlock final : public AstNode {
		ZK_AST_NODE(AstStatementBlock, STMT_BLOCK);

	public:
		std::vector<AstVariable> locals {};
		std::vector<AstStatement::ptr> statements {};
	};

	class AstStatementExpression final : public AstStatement {
		ZK_AST_NODE(AstStatementExpression, STMT_EXPRESSION);

	public:
		AstExpression::ptr expr;
	};

	class AstStatementReturn final : public AstStatement {
		ZK_AST_NODE(AstStatementReturn, STMT_RETURN);

	public:
		using AstStatement::AstStatement;

	public:
		std::optional<AstExpression::ptr> value;
	};

	class AstStatementCondition final : public AstStatement {
		ZK_AST_NODE(AstStatementCondition, STMT_CONDITION);

	public:
		using AstStatement::AstStatement;

	public:
		AstExpression::ptr condition;
		AstStatementBlock body;

		std::vector<AstStatementCondition> else_if;
		std::optional<AstStatementBlock> else_body;
	};

	class AstClass final : public AstNode {
		ZK_AST_NODE(AstClass, CLASS);

	public:
		using AstNode::AstNode;

	public:
		AstIdentifier name;
		std::vector<AstVariable> members;
	};

	class AstInstance final : public AstNode {
		ZK_AST_NODE(AstInstance, INSTANCE);

	public:
		using AstNode::AstNode;

	public:
		AstIdentifier name;
		AstIdentifier base;
		std::optional<AstStatementBlock> body;
	};

	class AstPrototype final : public AstNode {
		ZK_AST_NODE(AstPrototype, PROTOTYPE);

	public:
		using AstNode::AstNode;

	public:
		AstIdentifier name;
		AstIdentifier base;
		AstStatementBlock body;
	};

	class AstFunction final : public AstNode {
		ZK_AST_NODE(AstFunction, FUNCTION);

	public:
		AstIdentifier name {};
		AstIdentifier rtype {};
		std::vector<AstVariable> args {};
		AstStatementBlock body {};
	};

	class AstExternal final : public AstNode {
		ZK_AST_NODE(AstExternal, EXTERNAL);

	public:
		AstIdentifier name {};
		AstIdentifier rtype {};
		std::vector<AstVariable> args {};
	};

	class AstScript final : public AstNode {
		ZK_AST_NODE(AstScript, SCRIPT);

	public:
		AstScript() = default;

	public:
		std::vector<AstClass> classes;
		std::vector<AstPrototype> prototypes;
		std::vector<AstInstance> instances;
		std::vector<AstVariable> variables;
		std::vector<AstConstant> constants;
		std::vector<AstFunction> functions;
		std::vector<AstExternal> externals;
	};

	ZKAPI void parse_script(AstScript* ast, std::filesystem::path const& source);
	ZKAPI void parse_script(AstScript* ast, std::unique_ptr<Read> source);
	ZKAPI void parse_script(AstScript* ast, CompilationSource source);
	ZKAPI void parse_script(AstScript* ast, std::shared_ptr<CompilationSource> source);
} // namespace zenkit::daedalus
