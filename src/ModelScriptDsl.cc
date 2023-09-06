// Copyright © 2023 Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "model_script_dsl.hh"

#include <iostream>

#define WARN_SYNTAX(...)                                                                                               \
	PX_LOGW("model script: syntax error (line ", _m_line, ", column ", _m_column, "): ", __VA_ARGS__)

namespace phoenix {
	syntax_error::syntax_error(std::string&& location, std::string&& msg)
	    : phoenix::parser_error("model_script (source)", "MDS syntax error at " + location + ": " + msg) {}

	namespace parser {
		constexpr std::string_view token_names[] =
		    {"keyword", "integer", "float", "string", "rparen", "lparen", "rbrace", "lbrace", "colon", "eof", "null"};

		tokenizer::tokenizer(buffer buf) : _m_buffer(std::move(buf)) {}

		token tokenizer::next() {
			_m_value.clear();
			while (_m_buffer.remaining() > 0) {
				_m_buffer.mark();
				auto chr = static_cast<unsigned char>(_m_buffer.get_char());
				_m_column += 1;

				// ignore spaces, quotation marks, semicolons and parentheses
				// NOTE: Quirk of original implementation: parens are not significant.
				// NOTE: Semicolons are not used in model scripts but can cause parsing failures if not ignored
				if (std::isspace(chr) || chr == '(' || chr == ')' || chr == ';') {
					if (chr == '\n') {
						_m_line += 1;
						_m_column = 1;
						// TODO? return token::line_feed;
					}

					continue;
				}

				// ignore comments
				if (chr == '/') {
					if (_m_buffer.get_char() != '/') {
						WARN_SYNTAX("comments must start with two slashes");
					}

					// skip everything until the end of the line
					while (_m_buffer.get_char() != '\n')
						_m_column += 1;

					_m_line += 1;
					_m_column = 1;
					continue;
				}

				// parse keywords
				if (std::isalpha(chr) || chr == '*' || chr == '_' || chr == '.') {
					do {
						_m_value.push_back(static_cast<char>(chr));
						chr = static_cast<unsigned char>(_m_buffer.get_char());
						_m_column += 1;
					} while (std::isalnum(chr) || chr == '_' || chr == '-' || chr == '.');

					// (backtrack one)
					_m_buffer.position(_m_buffer.position() - 1);
					_m_column -= 1;

					return token::keyword;
				}

				// parse strings
				if (chr == '"') {
					chr = static_cast<unsigned char>(_m_buffer.get_char());
					_m_column += 1;

					while (chr != '"' && chr != '\n' && chr != ')') {
						_m_value.push_back(static_cast<char>(chr));
						chr = static_cast<unsigned char>(_m_buffer.get_char());
						_m_column += 1;
					}

					if (chr != '"') {
						WARN_SYNTAX("string not terminated");
						_m_buffer.position(_m_buffer.position() - 1);
						_m_column -= 1;
					}

					return token::string;
				}

				// parse numbers
				if (std::isdigit(chr) || chr == '-') {
					bool floating_point = false;

					do {
						_m_value.push_back(static_cast<char>(chr));
						chr = static_cast<unsigned char>(_m_buffer.get_char());
						_m_column += 1;

						// (allow floating point numbers)
						if (chr == '.') {
							floating_point = true;

							_m_value.push_back(static_cast<char>(chr));
							chr = static_cast<unsigned char>(_m_buffer.get_char());
							_m_column += 1;
						}
					} while (std::isdigit(chr));

					// (backtrack one)
					_m_buffer.position(_m_buffer.position() - 1);
					_m_column -= 1;

					return floating_point ? token::float_ : token::integer;
				}

				switch (chr) {
				case '{':
					return token::lbrace;
				case '}':
					return token::rbrace;
				case ':':
					return token::colon;
				default:
					break;
				}

				WARN_SYNTAX("unknown token \"", chr, "\"");
				return token::null;
			}

			return token::eof;
		}

		std::string tokenizer::format_location() const {
			return "line " + std::to_string(_m_line) + " column " + std::to_string(_m_column);
		}

		// ========================== PARSER COMMON FUNCTIONALITY ========================== //

		parser::parser(buffer buf) : _m_stream(std::move(buf)) {}

		template <token kind>
		void parser::expect() {
			if (!this->maybe<kind>()) {
				throw syntax_error {_m_stream.format_location(), "expected " + std::string {token_names[int(kind)]}};
			}
		}

		std::string parser::expect_string() {
			this->expect<token::string>();
			return _m_stream.token_value();
		}

		std::string parser::expect_keyword() {
			this->expect<token::keyword>();
			return _m_stream.token_value();
		}

		std::optional<std::string> parser::maybe_keyword() {
			if (this->maybe<token::keyword>())
				return _m_stream.token_value();
			return std::nullopt;
		}

		void parser::expect_keyword(std::string_view value) {
			this->expect<token::keyword>();
			if (!phoenix::iequals(_m_stream.token_value(), value)) {
				throw syntax_error {_m_stream.format_location(),
				                    "expected the keyword \"" + std::string {value} + "\""};
			}
		}

		float parser::expect_number() {
			auto mb = this->maybe_number();
			if (!mb) {
				throw syntax_error {_m_stream.format_location(), "expected a number"};
			}
			return *mb;
		}

		int parser::expect_int() {
			this->expect<token::integer>();
			return std::stoi(_m_stream.token_value());
		}

		mds::animation_flags parser::expect_flags() {
			auto kw = this->expect_keyword();
			// NOTE: Quirk of original implementation: Normally, "." is used in flags to indicate
			//       _not set_ but sometimes ":" appears instead.
			(void) this->maybe<token::colon>();
			return mds::animation_flags_from_string(kw);
		}

		std::optional<mds::animation_flags> parser::maybe_flags() {
			// NOTE: Workaround for mod-specific issues
			auto kw = this->maybe_keyword();
			if (!kw) {
				return std::nullopt;
			}

			if (kw->find("ani") != std::string::npos || kw->find("model") != std::string::npos) {
				this->_m_stream.backtrack();
				return std::nullopt;
			}

			// NOTE: Quirk of original implementation: Normally, "." is used in flags to indicate
			//       _not set_ but sometimes ":" appears instead.
			(void) this->maybe<token::colon>();
			return mds::animation_flags_from_string(*kw);
		}

		template <token kind>
		bool parser::maybe() {
			if (_m_stream.next() != kind) {
				_m_stream.backtrack();
				return false;
			}

			return true;
		}

		std::optional<int> parser::maybe_int() {
			if (!this->maybe<token::integer>()) {
				return std::nullopt;
			}

			return std::stoi(_m_stream.token_value());
		}

		std::optional<float> parser::maybe_number() {
			auto n = this->_m_stream.next();
			if (n != token::integer && n != token::float_) {
				this->_m_stream.backtrack();
				return std::nullopt;
			}

			return std::stof(_m_stream.token_value());
		}

		std::optional<std::string> parser::maybe_string() {
			if (!this->maybe<token::string>()) {
				return std::nullopt;
			}

			return _m_stream.token_value();
		}

		bool parser::maybe_keyword(std::string_view value) {
			if (!this->maybe<token::keyword>()) {
				return false;
			}

			if (!phoenix::iequals(this->_m_stream.token_value(), value)) {
				this->_m_stream.backtrack();
				return false;
			}

			return true;
		}

		std::optional<float> parser::maybe_named(std::string_view name) {
			if (!this->maybe<token::keyword>()) {
				return std::nullopt;
			}

			if (!phoenix::iequals(this->_m_stream.token_value(), name)) {
				this->_m_stream.backtrack();
				return std::nullopt;
			}

			this->expect<token::colon>();
			return this->expect_number();
		}

		// ===================== PARSER IMPLEMENTATION ======================= //

		model_script parser::parse_script() {
			model_script script {};
			this->expect_keyword("Model");

			auto name = this->expect_string();
			this->expect<token::lbrace>();

			while (!this->eof()) {
				if (this->maybe<token::rbrace>() || this->maybe<token::eof>()) {
					break;
				}

				auto kw = this->expect_keyword();
				if (phoenix::iequals(kw, "meshAndTree")) {
					script.skeleton = this->parse_meshAndTree();
				} else if (phoenix::iequals(kw, "registerMesh")) {
					script.meshes.push_back(this->parse_registerMesh());
				} else if (phoenix::iequals(kw, "aniEnum")) {
					this->parse_aniEnum(script);
				} else {
					PX_LOGW("model_script: detected invalid use of keyword " + kw +
					        "in \"Model\" block. Ignoring rest of script.");
					break;
				}
			}

			return script;
		}

		mds::skeleton parser::parse_meshAndTree() {
			mds::skeleton skel {};
			skel.name = this->expect_string();
			skel.disable_mesh = this->maybe_keyword("DONT_USE_MESH");
			return skel;
		}

		std::string parser::parse_registerMesh() {
			return this->expect_string();
		}

		void parser::parse_aniEnum(model_script& into) {
			this->expect<token::lbrace>();

			while (!this->eof()) {
				if (this->maybe<token::rbrace>() || this->maybe<token::eof>()) {
					break;
				}

				auto kw = this->expect_keyword();
				if (phoenix::iequals(kw, "ani")) {
					into.animations.push_back(this->parse_ani());
				} else if (phoenix::iequals(kw, "aniBlend")) {
					into.blends.push_back(this->parse_aniBlend());
				} else if (phoenix::iequals(kw, "aniAlias")) {
					into.aliases.push_back(this->parse_aniAlias());
				} else if (phoenix::iequals(kw, "aniComb")) {
					into.combinations.push_back(this->parse_aniComb());
				} else if (phoenix::iequals(kw, "aniDisable")) {
					into.disabled_animations.push_back(this->parse_aniDisable());
				} else if (phoenix::iequals(kw, "modelTag")) {
					into.model_tags.push_back(this->parse_modelTag());
				} else {
					throw syntax_error {_m_stream.format_location(), "invalid keyword in \"aniEnum\" block: " + kw};
				}
			}
		}

		void parser::parse_events(mds::animation& ani) {
			while (!this->eof()) {
				if (this->maybe<token::rbrace>() || this->maybe<token::eof>()) {
					break;
				}

				auto kw = this->expect_keyword();
				if (phoenix::iequals(kw, "*eventTag")) {
					ani.events.push_back(this->parse_eventTag());
				} else if (phoenix::iequals(kw, "*eventSFX")) {
					ani.sfx.push_back(this->parse_eventSFX());
				} else if (phoenix::iequals(kw, "*eventSFXGrnd")) {
					ani.sfx_ground.push_back(this->parse_eventSFXGrnd());
				} else if (phoenix::iequals(kw, "*eventPFX")) {
					ani.pfx.push_back(this->parse_eventPFX());
				} else if (phoenix::iequals(kw, "*eventPFXStop")) {
					ani.pfx_stop.push_back(this->parse_eventPFXStop());
				} else if (phoenix::iequals(kw, "*eventMMStartAni")) {
					ani.morph.push_back(this->parse_eventMMStartAni());
				} else if (phoenix::iequals(kw, "*eventCamTremor")) {
					ani.tremors.push_back(this->parse_eventCamTremor());
				} else {
					throw syntax_error {_m_stream.format_location(), "invalid keyword in \"ani\" block: " + kw};
				}
			}
		}

		void parser::ignore_block() {
			while (this->_m_stream.next() != token::rbrace) {}
		}

		mds::event_tag parser::parse_eventTag() {
			auto frame = this->maybe_int().value_or(0);
			auto type = this->expect_string();
			auto a = this->maybe_string();
			auto b = this->maybe_string();
			auto attach = this->maybe_keyword("ATTACH");
			return mds::make_event_tag(frame, std::move(type), std::move(a), std::move(b), attach);
		}

		mds::event_sfx parser::parse_eventSFX() {
			mds::event_sfx sfx {};
			sfx.frame = this->expect_int();
			sfx.name = this->expect_string();
			sfx.range = this->maybe_named("R").value_or(1000.0f);

			// NOTE: Quirk of original implementation: Sometimes "EMTPY_SLOT" is used instead of "EMPTY_SLOT".
			sfx.empty_slot = this->maybe_keyword("EMPTY_SLOT") || this->maybe_keyword("EMTPY_SLOT");
			return sfx;
		}

		mds::event_pfx parser::parse_eventPFX() {
			mds::event_pfx pfx {};
			pfx.frame = this->expect_int();
			pfx.index = this->maybe_int().value_or(0);
			pfx.name = this->expect_string();
			pfx.position = this->expect_string();
			pfx.attached = false;

			// NOTE: Quirk of original implementation: Sometimes, "ATTACH" does not appear as a keyword.
			if (this->maybe_keyword("ATTACH") || this->maybe<token::string>()) {
				pfx.attached = true;
			}

			return pfx;
		}

		mds::event_sfx_ground parser::parse_eventSFXGrnd() {
			mds::event_sfx_ground sfx {};
			sfx.frame = this->expect_int();
			sfx.name = this->expect_string();

			// NOTE: Fix for mod-specific issues
			(void) this->maybe_keyword("EMPTY_SLOT");
			return sfx;
		}

		mds::event_pfx_stop parser::parse_eventPFXStop() {
			mds::event_pfx_stop pfx {};
			pfx.frame = this->expect_int();
			pfx.index = this->expect_int();
			return pfx;
		}

		mds::event_morph_animate parser::parse_eventMMStartAni() {
			mds::event_morph_animate morph {};
			morph.frame = this->expect_int();
			morph.animation = this->expect_string();
			morph.node = this->maybe_string().value_or("");
			return morph;
		}

		mds::event_camera_tremor parser::parse_eventCamTremor() {
			mds::event_camera_tremor tremor {};
			tremor.frame = this->expect_int();
			tremor.field1 = this->expect_int();
			tremor.field2 = this->expect_int();
			tremor.field3 = this->expect_int();
			tremor.field4 = this->expect_int();
			return tremor;
		}

		mds::animation parser::parse_ani() {
			mds::animation ani {};
			ani.name = this->expect_string();
			ani.layer = this->expect_int();
			ani.next = this->expect_string();
			ani.blend_in = this->expect_number();
			ani.blend_out = this->expect_number();
			ani.flags = this->expect_flags();
			ani.model = this->expect_string();

			auto kw = this->expect_keyword();
			if (!phoenix::iequals(kw, "F") && !phoenix::iequals(kw, "R")) {
				throw syntax_error {_m_stream.format_location(), "expected \"F\" or \"R\""};
			}

			ani.direction =
			    phoenix::iequals(kw, "R") ? mds::animation_direction::backward : mds::animation_direction::forward;

			ani.first_frame = this->expect_int();
			ani.last_frame = this->expect_int();
			ani.speed = 0; // TODO

			ani.fps = this->maybe_named("FPS").value_or(25);
			ani.collision_volume_scale = this->maybe_named("CVS").value_or(1);

			// Optional events block.
			if (this->maybe<token::lbrace>()) {
				this->parse_events(ani);
			}

			return ani;
		}

		mds::animation_combination parser::parse_aniComb() {
			mds::animation_combination comb {};
			comb.name = this->expect_string();
			comb.layer = this->expect_int();
			comb.next = this->expect_string();
			comb.blend_in = this->expect_number();
			comb.blend_out = this->expect_number();
			comb.flags = this->expect_flags();
			comb.model = this->expect_string();
			comb.last_frame = this->expect_int();

			// Optional events block.
			if (this->maybe<token::lbrace>()) {
				this->ignore_block();
			}

			return comb;
		}
		mds::animation_alias parser::parse_aniAlias() {
			mds::animation_alias alias {};
			alias.direction = mds::animation_direction::forward;
			alias.name = this->expect_string();
			alias.layer = this->expect_int();
			alias.next = this->expect_string();
			alias.blend_in = this->expect_number();
			alias.blend_out = this->expect_number();
			alias.flags = this->expect_flags();
			alias.alias = this->expect_string();
			alias.direction = mds::animation_direction::forward;

			if (this->maybe_keyword("F")) {
				alias.direction = mds::animation_direction::forward;
			} else if (this->maybe_keyword("R")) {
				alias.direction = mds::animation_direction::backward;
			}

			return alias;
		}

		mds::animation_blending parser::parse_aniBlend() {
			mds::animation_blending blend {};
			blend.name = this->expect_string();
			(void) this->maybe_int();
			blend.next = this->expect_string();
			blend.blend_in = this->maybe_number().value_or(0);
			blend.blend_out = this->maybe_number().value_or(0);

			// NOTE: Fix for mod-specific issues
			(void) this->maybe_flags();

			// Optional events block.
			if (this->maybe<token::lbrace>()) {
				this->ignore_block();
			}

			return blend;
		}

		std::string parser::parse_aniDisable() {
			return this->expect_string();
		}

		mds::model_tag parser::parse_modelTag() {
			mds::model_tag tag {};

			auto type = this->expect_string();
			if (!phoenix::iequals(type, "DEF_HIT_LIMB")) {
				throw syntax_error {_m_stream.format_location(), "expected a \"DEF_HIT_LIMB\""};
			}

			tag.bone = this->expect_string();
			return tag;
		}
	} // namespace parser
} // namespace phoenix
