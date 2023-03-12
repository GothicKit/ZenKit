// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"

#include <filesystem>
#include <functional>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#if PHOENIX_LOG_LEVEL > 0
	#define PX_LOGE(...) phoenix::logging::log(phoenix::logging::level::error, __VA_ARGS__)
#else
	#define PX_LOGE(...)
#endif

#if PHOENIX_LOG_LEVEL > 1
	#define PX_LOGW(...) phoenix::logging::log(phoenix::logging::level::warn, __VA_ARGS__)
#else
	#define PX_LOGW(...)
#endif

#if PHOENIX_LOG_LEVEL > 2
	#define PX_LOGI(...) phoenix::logging::log(phoenix::logging::level::info, __VA_ARGS__)
#else
	#define PX_LOGI(...)
#endif

#if PHOENIX_LOG_LEVEL > 3
	#define PX_LOGD(...) phoenix::logging::log(phoenix::logging::level::debug, __VA_ARGS__)
#else
	#define PX_LOGD(...)
#endif

namespace phoenix {
	class buffer;
	class archive_reader;

	/// \brief An enum for providing a game version hint to some functions
	enum class game_version {
		gothic_1, ///< Represents any patch of Gothic
		gothic_2, ///< Represents any patch of Gothic II, including _Night of the Raven_.
	};

	/// \brief Tests whether two strings are equal when ignoring case.
	///
	/// Internally, uses std::tolower to compare the strings character by character.
	///
	/// \param a A string.
	/// \param b Another string.
	/// \return ``true`` if both strings are equal when ignoring case.
	PHOENIX_API bool iequals(std::string_view a, std::string_view b);

	/// \brief Tests whether \p a is lexicographically less than \p b.
	///
	/// Internally, uses std::tolower to compare the strings character by character.
	///
	/// \param a A string.
	/// \param b Another string.
	/// \return ``true`` if \p a is lexicographically less than \p b.
	PHOENIX_API bool icompare(std::string_view a, std::string_view b);

	/// \brief A basic datetime structure used by the *ZenGin*.
	struct date {
		/// \brief Parses a date from a buffer.
		/// \param buf The buffer to read from
		/// \return The date.
		PHOENIX_API static date parse(buffer& buf);

		std::uint32_t year;
		std::uint16_t month;
		std::uint16_t day;
		std::uint16_t hour;
		std::uint16_t minute;
		std::uint16_t second;
	};

	/// \brief Logging manager for phoenix.
	class logging {
	public:
		enum class level { error, warn, info, debug };

		/// \brief Supply a custom logger callback to be used for log output from phoenix.
		/// \param callback The callback to use.
		PHOENIX_API static void use_logger(std::function<void(level, const std::string&)>&& callback);

		/// \brief Use the default logger callback for phoenix.
		PHOENIX_API static void use_default_logger();

		/// \brief Send a logging event to the underlying log callback.
		/// \param lvl The level of the log message.
		/// \param message The message to send.
		template <typename... T>
		static void log(level lvl, const T&... msg) {
			if (logging::callback) {
				std::stringstream msg_buffer {};
				logging::_build_log_message(msg_buffer, msg...);
				(*logging::callback)(lvl, msg_buffer.str());
			}
		}

	private:
		template <typename T, typename... Ts>
		static void _build_log_message(std::stringstream& stream, const T& v, const Ts&... vals) {
			stream << v;

			if constexpr (sizeof...(vals) > 0) {
				_build_log_message(stream, vals...);
			}
		}

		static std::optional<std::function<void(level, const std::string&)>> callback;
	};

	/// \brief Base class for all exceptions.
	class error : public std::exception {
	public:
		PHOENIX_API explicit error(std::string&& message);

		[[nodiscard]] PHOENIX_API inline const char* what() const noexcept override {
			return message.c_str();
		}

	public:
		const std::string message;
	};

	/// \brief An error representing a parsing failure of any kind.
	class parser_error : public error {
	public:
		PHOENIX_INTERNAL explicit parser_error(std::string&& resource_type);
		PHOENIX_API explicit parser_error(std::string&& resource_type, std::string&& context);
		PHOENIX_INTERNAL explicit parser_error(std::string&& resource_type, const std::exception& cause);
		PHOENIX_INTERNAL explicit parser_error(std::string&& resource_type,
		                                       const std::exception& cause,
		                                       std::string&& context);

	public:
		const std::string resource_type;
		const std::optional<std::string> context {std::nullopt};
		const std::optional<std::exception> cause {std::nullopt};
	};

	template <typename T>
	T parse([[maybe_unused]] buffer& buf) {
		throw parser_error {"unknown", "parsing routine not implemented"};
	}

	template <typename T>
	inline T parse(buffer&& buf) {
		return parse<T>(buf);
	}

	template <typename T>
	T parse([[maybe_unused]] archive_reader& buf) {
		throw parser_error {"unknown", "parsing routine not implemented"};
	}
} // namespace phoenix
