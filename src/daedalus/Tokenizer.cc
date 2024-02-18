// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "Tokenizer.hh"

#include "zenkit/Misc.hh"

#include <unordered_set>

namespace zenkit::daedalus {
	static std::unordered_set<std::string> KEYWORDS = {
	    "CONST",
	    "VAR",
	    "CLASS",
	    "INSTANCE",
	    "PROTOTYPE",
	    "IF",
	    "ELSE",
	    "RETURN",
	    "EXTERN",
	};

	Tokenizer::Tokenizer(std::shared_ptr<CompilationSource> src) : _m_src(std::move(src)) {}

	Token Tokenizer::last() const noexcept {
		return _m_token;
	}

	std::string const& Tokenizer::get_value() const noexcept {
		return _m_value;
	}

	int Tokenizer::get_value_int() const {
		return std::stoi(_m_value);
	}

	float Tokenizer::get_value_float() const {
		return std::stof(_m_value);
	}

	Token Tokenizer::next() {
		_m_value.clear();

		auto tt = this->next_internal();
		_m_token = tt;
		_m_tokens_parsed += 1;

		return tt;
	}

	Token Tokenizer::next_internal() {
		while (!_m_src->read->eof()) {
			this->mark();
			auto ntk = this->get();

			// Skip whitespace.
			if (std::isspace(ntk)) {
				continue;
			}

			// Skip comments.
			if (ntk == '/') {
				ntk = this->get();

				// :: Line comments.
				if (ntk == '/') {
					while (ntk != '\n' && !_m_src->read->eof()) {
						ntk = this->get();
					}

					continue;
				}

				// :: Block comments.
				if (ntk == '*') {
					bool check_eoc = false;
					while (!_m_src->read->eof()) {
						ntk = this->get();

						if (ntk == '*') {
							check_eoc = true;
						} else if (ntk == '/' && check_eoc) {
							break;
						} else {
							check_eoc = false;
						}
					}

					continue;
				}

				// :: Compound assignment.
				if (ntk == '=') {
					return Token::ASSIGN_DIVIDE;
				} else if (!std::isspace(ntk)) {
					this->unget();
				}

				return Token::DIVIDE;
			}

			// Report strings.
			if (ntk == '"') {
				while (!_m_src->read->eof()) {
					ntk = this->get();

					if (ntk == '"') {
						break;
					}

					_m_value.push_back(ntk);
				}

				return Token::STRING;
			}

			// Report numbers.
			if (std::isdigit(ntk)) {
				auto tt = Token::INTEGER;

				do {
					_m_value.push_back(ntk);

					ntk = this->get();
					if (tt == Token::INTEGER && ntk == '.') {
						tt = Token::FLOAT;
						ntk = this->get();
					}
				} while (std::isdigit(ntk) && !_m_src->read->eof());

				// :: Backtrack one if the next char is not a space.
				if (!std::isspace(ntk)) {
					this->unget();
				}

				return tt;
			}

			// Report identifiers.
			if (std::isalpha(ntk) || ntk == '_') {
				auto tt = Token::IDENTIFIER;

				do {
					_m_value.push_back(ntk);
					ntk = this->get();
				} while (!std::isspace(ntk) && (!std::ispunct(ntk) || ntk == '_') && !_m_src->read->eof());

				// :: Backtrack one if the next char is not a space.
				if (!std::isspace(ntk)) {
					this->unget();
				}

				std::transform(_m_value.begin(), _m_value.end(), _m_value.begin(), [](char c) {
					return (char) std::toupper(c);
				});

				auto it = KEYWORDS.find(_m_value);
				if (it != KEYWORDS.end()) {
					return Token::KEYWORD;
				}

				return tt;
			}

			// Report 2-char tokens.
			if (ntk == '<') {
				ntk = this->get();

				if (ntk == '<') {
					return Token::LEFT_SHIFT;
				} else if (ntk == '=') {
					return Token::LESS_THAN_OR_EQUAL;
				} else if (!std::isspace(ntk)) {
					this->unget();
				}
				return Token::LESS_THAN;
			}

			if (ntk == '>') {
				ntk = this->get();

				if (ntk == '>') {
					return Token::RIGHT_SHIFT;
				} else if (ntk == '=') {
					return Token::GREATER_THAN_OR_EQUAL;
				} else if (!std::isspace(ntk)) {
					this->unget();
				}
				return Token::GREATER_THAN;
			}

			if (ntk == '=') {
				ntk = this->get();

				if (ntk == '=') {
					return Token::EQUAL;
				} else if (!std::isspace(ntk)) {
					this->unget();
				}
				return Token::ASSIGN;
			}

			if (ntk == '+') {
				ntk = this->get();

				if (ntk == '=') {
					return Token::ASSIGN_ADD;
				} else if (!std::isspace(ntk)) {
					this->unget();
				}
				return Token::PLUS;
			}

			if (ntk == '-') {
				ntk = this->get();

				if (ntk == '=') {
					return Token::ASSIGN_SUBTRACT;
				} else if (!std::isspace(ntk)) {
					this->unget();
				}
				return Token::MINUS;
			}

			if (ntk == '*') {
				ntk = this->get();

				if (ntk == '=') {
					return Token::ASSIGN_MULTIPLY;
				} else if (!std::isspace(ntk)) {
					this->unget();
				}
				return Token::MULTIPLY;
			}

			if (ntk == '|') {
				ntk = this->get();

				if (ntk == '|') {
					return Token::LOGICAL_OR;
				} else if (!std::isspace(ntk)) {
					this->unget();
				}
				return Token::BITWISE_OR;
			}

			if (ntk == '&') {
				ntk = this->get();

				if (ntk == '&') {
					return Token::LOGICAL_AND;
				} else if (!std::isspace(ntk)) {
					this->unget();
				}
				return Token::BITWISE_AND;
			}

			if (ntk == '!') {
				ntk = this->get();

				if (ntk == '=') {
					return Token::NOT_EQUAL;
				} else if (!std::isspace(ntk)) {
					this->unget();
				}
				return Token::NOT;
			}

			switch (ntk) {
			case '%':
				return Token::MODULO;
			case '~':
				return Token::COMPLEMENT;
			case ';':
				return Token::SEMICOLON;
			case ',':
				return Token::COMMA;
			case '(':
				return Token::LPAREN;
			case ')':
				return Token::RPAREN;
			case '{':
				return Token::LBRACE;
			case '}':
				return Token::RBRACE;
			case '[':
				return Token::LBRACKET;
			case ']':
				return Token::RBRACKET;
			case '.':
				return Token::DOT;
			default:
				return Token::UNKNOWN;
			}
		}

		return Token::END_OF_FILE;
	}

	char Tokenizer::get() {
		auto c = _m_src->read->read_char();

		if (c == '\n') {
			_m_line += 1;
			_m_column = 1;
		} else {
			_m_column += 1;
		}

		return c;
	}

	void Tokenizer::mark() {
		_m_bt_pos = _m_src->read->tell();
		_m_bt_column = _m_column;
		_m_bt_line = _m_line;
	}

	void Tokenizer::backtrack() {
		_m_src->read->seek(static_cast<ssize_t>(_m_bt_pos), Whence::BEG);
		_m_column = _m_bt_column;
		_m_line = _m_bt_line;
		_m_tokens_parsed -= 1;
	}

	void Tokenizer::unget() {
		_m_src->read->seek(-1, Whence::CUR);
		_m_column -= 1;
	}

	bool Tokenizer::match_kw(std::string_view kw) {
		auto tt = next();

		if (tt != Token::IDENTIFIER && tt != Token::KEYWORD) {
			this->backtrack();
			return false;
		}

		if (!zenkit::iequals(kw, this->get_value())) {
			this->backtrack();
			return false;
		}

		return true;
	}

	SourceLocation Tokenizer::location() const noexcept {
		return SourceLocation {_m_src, _m_bt_line, _m_bt_column + 1};
	}

	std::size_t Tokenizer::get_tokens_parsed() const {
		return _m_tokens_parsed;
	}
} // namespace zenkit::daedalus
