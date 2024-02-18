// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Stream.hh"
#include "zenkit/daedalus/Compiler.hh"
#include "zenkit/daedalus/SyntaxTree.hh"

#include <string>
#include <type_traits>

namespace zenkit::daedalus {
	enum class Token {
		IDENTIFIER = 0,
		KEYWORD,
		INTEGER,
		FLOAT,
		STRING,

		DOT,
		PLUS,
		MINUS,
		MULTIPLY,
		DIVIDE,
		MODULO,
		NOT,
		COMPLEMENT,
		LESS_THAN,
		GREATER_THAN,
		LESS_THAN_OR_EQUAL,
		GREATER_THAN_OR_EQUAL,
		EQUAL,
		NOT_EQUAL,
		LOGICAL_OR,
		LOGICAL_AND,
		BITWISE_OR,
		BITWISE_AND,
		LEFT_SHIFT,
		RIGHT_SHIFT,
		ASSIGN,
		ASSIGN_ADD,
		ASSIGN_SUBTRACT,
		ASSIGN_MULTIPLY,
		ASSIGN_DIVIDE,

		SEMICOLON,
		COMMA,
		LPAREN,
		RPAREN,
		LBRACE,
		RBRACE,
		LBRACKET,
		RBRACKET,

		UNKNOWN,
		END_OF_FILE
	};

	class ZKINT Tokenizer {
	public:
		explicit Tokenizer(std::shared_ptr<CompilationSource> src);

		[[nodiscard]] Token last() const noexcept;

		template <typename... T>
		bool match(T... tts) {
			auto tt = next();

			bool any = ((tt == tts) || ...);
			if (!any) {
				this->backtrack();
				return false;
			}

			return true;
		}

		bool match_kw(std::string_view kw);

		[[nodiscard]] std::string const& get_value() const noexcept;
		[[nodiscard]] int get_value_int() const;
		[[nodiscard]] float get_value_float() const;
		[[nodiscard]] std::size_t get_tokens_parsed() const;

		[[nodiscard]] SourceLocation location() const noexcept;

	private:
		Token next();
		Token next_internal();

		void backtrack();

		char get();
		void unget();
		void mark();

	private:
		std::shared_ptr<CompilationSource> _m_src;

		std::string _m_value;
		Token _m_token {Token::UNKNOWN};
		std::size_t _m_tokens_parsed {0};

		std::size_t _m_bt_pos {0};
		std::size_t _m_bt_line {0}, _m_bt_column {0};

		std::size_t _m_line = 1, _m_column = 1;
	};
} // namespace zenkit::daedalus
