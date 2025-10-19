// Copyright © 2022-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/ModelScript.hh"
#include "zenkit/Stream.hh"

#include <optional>
#include <string>
#include <string_view>

namespace zenkit {
	enum class MdsToken {
		KEYWORD = 0,
		INTEGER = 1,
		FLOAT = 2,
		STRING = 3,
		RBRACE = 6,
		LBRACE = 7,
		COLON = 8,
		END_OF_FILE = 9,
		NOTHING = 10,
	};

	class MdsTokenizer {
	public:
		explicit MdsTokenizer(Read* buf);

		MdsToken next();

		void backtrack();

		[[nodiscard]] std::string const& token_value() const;

		[[nodiscard]] bool eof() const;

		[[nodiscard]] std::string format_location() const;

	private:
		Read* _m_buffer;
		uint32_t _m_line {1}, _m_column {1};
		std::string _m_value;
		std::size_t _m_mark;
	};

	class MdsParser {
	public:
		explicit MdsParser(Read* buf);

		ModelScript parse_script(ModelScript& script);
		MdsSkeleton parse_meshAndTree();
		std::string parse_registerMesh();
		void parse_aniEnum(ModelScript& into);
		void parse_events(MdsAnimation& ani);
		void ignore_block();
		MdsEventTag parse_eventTag();
		MdsSoundEffect parse_eventSFX();
		MdsParticleEffect parse_eventPFX();
		MdsSoundEffectGround parse_eventSFXGrnd();
		MdsParticleEffectStop parse_eventPFXStop();
		MdsMorphAnimation parse_eventMMStartAni();
		MdsCameraTremor parse_eventCamTremor();
		MdsAnimation parse_ani();
		MdsAnimationCombine parse_aniComb();
		MdsAnimationAlias parse_aniAlias();
		MdsAnimationBlend parse_aniBlend();
		std::string parse_aniDisable();
		MdsModelTag parse_modelTag();

	private:
		[[nodiscard]] bool eof() const {
			return this->_m_stream.eof();
		}

		template <MdsToken kind>
		void expect();

		[[nodiscard]] std::string expect_string();
		[[nodiscard]] std::string expect_keyword();
		[[nodiscard]] std::optional<std::string> maybe_keyword();
		void expect_keyword(std::string_view value);
		[[nodiscard]] float expect_number();
		[[nodiscard]] int expect_int();
		[[nodiscard]] AnimationFlags expect_flags();
		[[nodiscard]] std::optional<AnimationFlags> maybe_flags();

		template <MdsToken kind>
		bool maybe();

		[[nodiscard]] std::optional<int> maybe_int();
		[[nodiscard]] std::optional<float> maybe_number();
		[[nodiscard]] std::optional<std::string> maybe_string();
		[[nodiscard]] bool maybe_keyword(std::string_view value);
		[[nodiscard]] std::optional<float> maybe_named(std::string_view name);

		MdsTokenizer _m_stream;
	};
} // namespace zenkit
