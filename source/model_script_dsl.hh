// Copyright © 2023 Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include <iterator>
#include <optional>
#include <phoenix/buffer.hh>
#include <phoenix/model_script.hh>
#include <phoenix/phoenix.hh>

#include <stdexcept>
#include <string>
#include <string_view>

namespace phoenix::mds {
	event_tag make_event_tag(int32_t, std::string&&, std::optional<std::string>&&, std::optional<std::string>&&, bool);
}

namespace phoenix::parser {
	enum class token {
		keyword = 0,
		integer = 1,
		float_ = 2,
		string = 3,
		rbrace = 6,
		lbrace = 7,
		colon = 8,
		eof = 9,
		null = 10,
	};

	class tokenizer {
	public:
		explicit tokenizer(buffer buf);

		token next();

		void backtrack() {
			this->_m_buffer.reset();
		}

		[[nodiscard]] std::string const& token_value() const {
			return _m_value;
		}

		[[nodiscard]] bool eof() const {
			return _m_buffer.remaining() == 0;
		}

		[[nodiscard]] std::string format_location() const;

	private:
		buffer _m_buffer;
		uint32_t _m_line {1}, _m_column {1};
		std::string _m_value;
	};

	class parser {
	public:
		explicit parser(buffer buf);

		model_script parse_script();
		mds::skeleton parse_meshAndTree();
		std::string parse_registerMesh();
		void parse_aniEnum(model_script& into);
		void parse_events(mds::animation& ani);
		void ignore_block();
		mds::event_tag parse_eventTag();
		mds::event_sfx parse_eventSFX();
		mds::event_pfx parse_eventPFX();
		mds::event_sfx_ground parse_eventSFXGrnd();
		mds::event_pfx_stop parse_eventPFXStop();
		mds::event_morph_animate parse_eventMMStartAni();
		mds::event_camera_tremor parse_eventCamTremor();
		mds::animation parse_ani();
		mds::animation_combination parse_aniComb();
		mds::animation_alias parse_aniAlias();
		mds::animation_blending parse_aniBlend();
		std::string parse_aniDisable();
		mds::model_tag parse_modelTag();

	private:
		[[nodiscard]] bool eof() const {
			return this->_m_stream.eof();
		}

		template <token kind>
		void expect();

		[[nodiscard]] std::string expect_string();
		[[nodiscard]] std::string expect_keyword();
		void expect_keyword(std::string_view value);
		[[nodiscard]] float expect_number();
		[[nodiscard]] int expect_int();
		[[nodiscard]] mds::animation_flags expect_flags();

		template <token kind>
		bool maybe();

		[[nodiscard]] std::optional<int> maybe_int();
		[[nodiscard]] std::optional<float> maybe_number();
		[[nodiscard]] std::optional<std::string> maybe_string();
		[[nodiscard]] bool maybe_keyword(std::string_view value);
		[[nodiscard]] std::optional<float> maybe_named(std::string_view name);

	private:
		tokenizer _m_stream;
	};
} // namespace phoenix::parser
