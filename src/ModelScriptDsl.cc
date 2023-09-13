// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "ModelScriptDsl.hh"
#include "Internal.hh"

#include "zenkit/Stream.hh"

#define WARN_SYNTAX(msg) ZKLOGW("ModelScript", "Syntax error (line %d, column %d): %s", _m_line, _m_column, msg)

namespace zenkit {
	namespace mds {
		MdsEventTag
		make_event_tag(int32_t, std::string&&, std::optional<std::string>&&, std::optional<std::string>&&, bool);
		AnimationFlags animation_flags_from_string(std::string_view str);
	} // namespace mds

	struct ScriptSyntaxError : public zenkit::ParserError {
	public:
		ScriptSyntaxError(std::string&& location, std::string&& msg);
	};

	ScriptSyntaxError::ScriptSyntaxError(std::string&& location, std::string&& msg)
	    : zenkit::ParserError("ModelScript (source)", "MDS syntax error at " + location + ": " + msg) {}

	constexpr std::string_view token_names[] =
	    {"KEYWORD", "integer", "float", "string", "rparen", "lparen", "rbrace", "lbrace", "colon", "eof", "null"};

	MdsTokenizer::MdsTokenizer(Read* buf) : _m_buffer(buf) {}

	MdsToken MdsTokenizer::next() {
		_m_value.clear();
		while (!_m_buffer->eof()) {
			_m_mark = _m_buffer->tell();
			auto chr = static_cast<unsigned char>(_m_buffer->read_char());
			_m_column += 1;

			// ignore spaces, quotation marks, semicolons and parentheses
			// NOTE: Quirk of original implementation: parens are not significant.
			// NOTE: Semicolons are not used in model scripts but can cause parsing failures if not ignored
			if (std::isspace(chr) || chr == '(' || chr == ')' || chr == ';') {
				if (chr == '\n') {
					_m_line += 1;
					_m_column = 1;
				}

				continue;
			}

			// ignore comments
			if (chr == '/') {
				if (_m_buffer->read_char() != '/') {
					WARN_SYNTAX("comments must start with two slashes");
				}

				// skip everything until the end of the line
				while (_m_buffer->read_char() != '\n')
					_m_column += 1;

				_m_line += 1;
				_m_column = 1;
				continue;
			}

			// parse keywords
			if (std::isalpha(chr) || chr == '*' || chr == '_' || chr == '.') {
				do {
					_m_value.push_back(static_cast<char>(chr));
					chr = static_cast<unsigned char>(_m_buffer->read_char());
					_m_column += 1;
				} while (std::isalnum(chr) || chr == '_' || chr == '-' || chr == '.');

				// (backtrack one)
				_m_buffer->seek(-1, Whence::CUR);
				_m_column -= 1;

				return MdsToken::KEYWORD;
			}

			// parse strings
			if (chr == '"') {
				chr = static_cast<unsigned char>(_m_buffer->read_char());
				_m_column += 1;

				while (chr != '"' && chr != '\n' && chr != ')') {
					_m_value.push_back(static_cast<char>(chr));
					chr = static_cast<unsigned char>(_m_buffer->read_char());
					_m_column += 1;
				}

				if (chr != '"') {
					WARN_SYNTAX("String not terminated");
					_m_buffer->seek(-1, Whence::CUR);
					_m_column -= 1;
				}

				return MdsToken::STRING;
			}

			// parse numbers
			if (std::isdigit(chr) || chr == '-') {
				bool floating_point = false;

				do {
					_m_value.push_back(static_cast<char>(chr));
					chr = static_cast<unsigned char>(_m_buffer->read_char());
					_m_column += 1;

					// (allow floating point numbers)
					if (chr == '.') {
						floating_point = true;

						_m_value.push_back(static_cast<char>(chr));
						chr = static_cast<unsigned char>(_m_buffer->read_char());
						_m_column += 1;
					}
				} while (std::isdigit(chr));

				// (backtrack one)
				_m_buffer->seek(-1, Whence::CUR);
				_m_column -= 1;

				return floating_point ? MdsToken::FLOAT : MdsToken::INTEGER;
			}

			switch (chr) {
			case '{':
				return MdsToken::LBRACE;
			case '}':
				return MdsToken::RBRACE;
			case ':':
				return MdsToken::COLON;
			default:
				break;
			}

			WARN_SYNTAX("Illegal token");
			return MdsToken::NOTHING;
		}

		return MdsToken::END_OF_FILE;
	}

	std::string MdsTokenizer::format_location() const {
		return "line " + std::to_string(_m_line) + " column " + std::to_string(_m_column);
	}

	void MdsTokenizer::backtrack() {
		this->_m_buffer->seek(static_cast<ssize_t>(_m_mark), Whence::BEG);
	}

	bool MdsTokenizer::eof() const {
		return _m_buffer->eof();
	}

	std::string const& MdsTokenizer::token_value() const {
		return _m_value;
	}

	// ========================== PARSER COMMON FUNCTIONALITY ========================== //

	MdsParser::MdsParser(Read* buf) : _m_stream(buf) {}

	template <MdsToken kind>
	void MdsParser::expect() {
		if (!this->maybe<kind>()) {
			throw ScriptSyntaxError {_m_stream.format_location(), "expected " + std::string {token_names[int(kind)]}};
		}
	}

	std::string MdsParser::expect_string() {
		this->expect<MdsToken::STRING>();
		return _m_stream.token_value();
	}

	std::string MdsParser::expect_keyword() {
		this->expect<MdsToken::KEYWORD>();
		return _m_stream.token_value();
	}

	std::optional<std::string> MdsParser::maybe_keyword() {
		if (this->maybe<MdsToken::KEYWORD>()) return _m_stream.token_value();
		return std::nullopt;
	}

	void MdsParser::expect_keyword(std::string_view value) {
		this->expect<MdsToken::KEYWORD>();
		if (!phoenix::iequals(_m_stream.token_value(), value)) {
			throw ScriptSyntaxError {_m_stream.format_location(),
			                         "expected the KEYWORD \"" + std::string {value} + "\""};
		}
	}

	float MdsParser::expect_number() {
		auto mb = this->maybe_number();
		if (!mb) {
			throw ScriptSyntaxError {_m_stream.format_location(), "expected a number"};
		}
		return *mb;
	}

	int MdsParser::expect_int() {
		this->expect<MdsToken::INTEGER>();
		return std::stoi(_m_stream.token_value());
	}

	AnimationFlags MdsParser::expect_flags() {
		auto kw = this->expect_keyword();
		// NOTE: Quirk of original implementation: Normally, "." is used in flags to indicate
		//       _not set_ but sometimes ":" appears instead.
		(void) this->maybe<MdsToken::COLON>();
		return mds::animation_flags_from_string(kw);
	}

	std::optional<AnimationFlags> MdsParser::maybe_flags() {
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
		(void) this->maybe<MdsToken::COLON>();
		return mds::animation_flags_from_string(*kw);
	}

	template <MdsToken kind>
	bool MdsParser::maybe() {
		if (_m_stream.next() != kind) {
			_m_stream.backtrack();
			return false;
		}

		return true;
	}

	std::optional<int> MdsParser::maybe_int() {
		if (!this->maybe<MdsToken::INTEGER>()) {
			return std::nullopt;
		}

		return std::stoi(_m_stream.token_value());
	}

	std::optional<float> MdsParser::maybe_number() {
		auto n = this->_m_stream.next();
		if (n != MdsToken::INTEGER && n != MdsToken::FLOAT) {
			this->_m_stream.backtrack();
			return std::nullopt;
		}

		return std::stof(_m_stream.token_value());
	}

	std::optional<std::string> MdsParser::maybe_string() {
		if (!this->maybe<MdsToken::STRING>()) {
			return std::nullopt;
		}

		return _m_stream.token_value();
	}

	bool MdsParser::maybe_keyword(std::string_view value) {
		if (!this->maybe<MdsToken::KEYWORD>()) {
			return false;
		}

		if (!phoenix::iequals(this->_m_stream.token_value(), value)) {
			this->_m_stream.backtrack();
			return false;
		}

		return true;
	}

	std::optional<float> MdsParser::maybe_named(std::string_view name) {
		if (!this->maybe<MdsToken::KEYWORD>()) {
			return std::nullopt;
		}

		if (!phoenix::iequals(this->_m_stream.token_value(), name)) {
			this->_m_stream.backtrack();
			return std::nullopt;
		}

		this->expect<MdsToken::COLON>();
		return this->expect_number();
	}

	// ===================== PARSER IMPLEMENTATION ======================= //

	ModelScript MdsParser::parse_script(ModelScript& script) {
		this->expect_keyword("Model");

		auto name = this->expect_string();
		this->expect<MdsToken::LBRACE>();

		while (!this->eof()) {
			if (this->maybe<MdsToken::RBRACE>() || this->maybe<MdsToken::END_OF_FILE>()) {
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
				ZKLOGW("ModelScript",
				       "detected invalid use of KEYWORD \"%s\" in \"Model\" block. Ignoring rest of script.",
				       kw.c_str());
				break;
			}
		}

		return script;
	}

	MdsSkeleton MdsParser::parse_meshAndTree() {
		MdsSkeleton skel {};
		skel.name = this->expect_string();
		skel.disable_mesh = this->maybe_keyword("DONT_USE_MESH");
		return skel;
	}

	std::string MdsParser::parse_registerMesh() {
		return this->expect_string();
	}

	void MdsParser::parse_aniEnum(ModelScript& into) {
		this->expect<MdsToken::LBRACE>();

		while (!this->eof()) {
			if (this->maybe<MdsToken::RBRACE>() || this->maybe<MdsToken::END_OF_FILE>()) {
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
				throw ScriptSyntaxError {_m_stream.format_location(), "invalid KEYWORD in \"aniEnum\" block: " + kw};
			}
		}
	}

	void MdsParser::parse_events(MdsAnimation& ani) {
		while (!this->eof()) {
			if (this->maybe<MdsToken::RBRACE>() || this->maybe<MdsToken::END_OF_FILE>()) {
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
				throw ScriptSyntaxError {_m_stream.format_location(), "invalid KEYWORD in \"ani\" block: " + kw};
			}
		}
	}

	void MdsParser::ignore_block() {
		while (this->_m_stream.next() != MdsToken::RBRACE) {}
	}

	MdsEventTag MdsParser::parse_eventTag() {
		auto frame = this->maybe_int().value_or(0);
		auto type = this->expect_string();
		auto a = this->maybe_string();
		auto b = this->maybe_string();
		auto attach = this->maybe_keyword("ATTACH");
		return mds::make_event_tag(frame, std::move(type), std::move(a), std::move(b), attach);
	}

	MdsSoundEffect MdsParser::parse_eventSFX() {
		MdsSoundEffect sfx {};
		sfx.frame = this->expect_int();
		sfx.name = this->expect_string();
		sfx.range = this->maybe_named("R").value_or(1000.0f);

		// NOTE: Quirk of original implementation: Sometimes "EMTPY_SLOT" is used instead of "EMPTY_SLOT".
		sfx.empty_slot = this->maybe_keyword("EMPTY_SLOT") || this->maybe_keyword("EMTPY_SLOT");
		return sfx;
	}

	MdsParticleEffect MdsParser::parse_eventPFX() {
		MdsParticleEffect pfx {};
		pfx.frame = this->expect_int();
		pfx.index = this->maybe_int().value_or(0);
		pfx.name = this->expect_string();
		pfx.position = this->expect_string();
		pfx.attached = false;

		// NOTE: Quirk of original implementation: Sometimes, "ATTACH" does not appear as a keyword.
		if (this->maybe_keyword("ATTACH") || this->maybe<MdsToken::STRING>()) {
			pfx.attached = true;
		}

		return pfx;
	}

	MdsSoundEffectGround MdsParser::parse_eventSFXGrnd() {
		MdsSoundEffectGround sfx {};
		sfx.frame = this->expect_int();
		sfx.name = this->expect_string();
		// NOTE: Fix for mod-specific issues
		(void) this->maybe_keyword("EMPTY_SLOT");
		return sfx;
	}

	MdsParticleEffectStop MdsParser::parse_eventPFXStop() {
		MdsParticleEffectStop pfx {};
		pfx.frame = this->expect_int();
		pfx.index = this->expect_int();
		return pfx;
	}

	MdsMorphAnimation MdsParser::parse_eventMMStartAni() {
		MdsMorphAnimation morph {};
		morph.frame = this->expect_int();
		morph.animation = this->expect_string();
		morph.node = this->maybe_string().value_or("");
		return morph;
	}

	MdsCameraTremor MdsParser::parse_eventCamTremor() {
		MdsCameraTremor tremor {};
		tremor.frame = this->expect_int();
		tremor.field1 = this->expect_int();
		tremor.field2 = this->expect_int();
		tremor.field3 = this->expect_int();
		tremor.field4 = this->expect_int();
		return tremor;
	}

	MdsAnimation MdsParser::parse_ani() {
		MdsAnimation ani {};
		ani.name = this->expect_string();
		ani.layer = this->expect_int();
		ani.next = this->expect_string();
		ani.blend_in = this->expect_number();
		ani.blend_out = this->expect_number();
		ani.flags = this->expect_flags();
		ani.model = this->expect_string();

		auto kw = this->expect_keyword();
		if (!phoenix::iequals(kw, "F") && !phoenix::iequals(kw, "R")) {
			throw ScriptSyntaxError {_m_stream.format_location(), R"(expected "F" or "R")"};
		}

		ani.direction = phoenix::iequals(kw, "R") ? AnimationDirection::BACKWARD : AnimationDirection::FORWARD;
		ani.first_frame = this->expect_int();
		ani.last_frame = this->expect_int();
		ani.speed = 0; // TODO

		ani.fps = this->maybe_named("FPS").value_or(25);
		ani.collision_volume_scale = this->maybe_named("CVS").value_or(1);

		// Optional events block.
		if (this->maybe<MdsToken::LBRACE>()) {
			this->parse_events(ani);
		}

		return ani;
	}

	MdsAnimationCombine MdsParser::parse_aniComb() {
		MdsAnimationCombine comb {};
		comb.name = this->expect_string();
		comb.layer = this->expect_int();
		comb.next = this->expect_string();
		comb.blend_in = this->expect_number();
		comb.blend_out = this->expect_number();
		comb.flags = this->expect_flags();
		comb.model = this->expect_string();
		comb.last_frame = this->expect_int();

		// Optional events block.
		if (this->maybe<MdsToken::LBRACE>()) {
			this->ignore_block();
		}

		return comb;
	}

	MdsAnimationAlias MdsParser::parse_aniAlias() {
		MdsAnimationAlias alias {};
		alias.direction = AnimationDirection::FORWARD;
		alias.name = this->expect_string();
		alias.layer = this->expect_int();
		alias.next = this->expect_string();
		alias.blend_in = this->expect_number();
		alias.blend_out = this->expect_number();
		alias.flags = this->expect_flags();
		alias.alias = this->expect_string();

		if (this->maybe_keyword("F")) {
			alias.direction = AnimationDirection::FORWARD;
		} else if (this->maybe_keyword("R")) {
			alias.direction = AnimationDirection::BACKWARD;
		}

		return alias;
	}

	MdsAnimationBlend MdsParser::parse_aniBlend() {
		MdsAnimationBlend blend {};
		blend.name = this->expect_string();
		(void) this->maybe_int();
		blend.next = this->expect_string();
		blend.blend_in = this->maybe_number().value_or(0);
		blend.blend_out = this->maybe_number().value_or(0);

		// NOTE: Fix for mod-specific issues
		(void) this->maybe_flags();

		// Optional events block.
		if (this->maybe<MdsToken::LBRACE>()) {
			this->ignore_block();
		}

		return blend;
	}

	std::string MdsParser::parse_aniDisable() {
		return this->expect_string();
	}

	MdsModelTag MdsParser::parse_modelTag() {
		MdsModelTag tag {};

		auto type = this->expect_string();
		if (!phoenix::iequals(type, "DEF_HIT_LIMB")) {
			throw ScriptSyntaxError {_m_stream.format_location(), "expected a \"DEF_HIT_LIMB\""};
		}

		tag.bone = this->expect_string();
		return tag;
	}
} // namespace zenkit
