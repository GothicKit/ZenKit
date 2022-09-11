// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <fmt/format.h>

#include <filesystem>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

#ifdef PX_LOG_LEVEL_DEBUG
#define PX_LOGE(...) phoenix::logging::log(phoenix::logging::level::error, fmt::format(__VA_ARGS__))
#define PX_LOGW(...) phoenix::logging::log(phoenix::logging::level::warn, fmt::format(__VA_ARGS__))
#define PX_LOGI(...) phoenix::logging::log(phoenix::logging::level::info, fmt::format(__VA_ARGS__))
#define PX_LOGD(...) phoenix::logging::log(phoenix::logging::level::debug, fmt::format(__VA_ARGS__))
#elif defined(PX_LOG_LEVEL_INFO)
#define PX_LOGE(...) phoenix::logging::log(phoenix::logging::level::error, fmt::format(__VA_ARGS__))
#define PX_LOGW(...) phoenix::logging::log(phoenix::logging::level::warn, fmt::format(__VA_ARGS__))
#define PX_LOGI(...) phoenix::logging::log(phoenix::logging::level::info, fmt::format(__VA_ARGS__))
#define PX_LOGD(...)
#elif defined(PX_LOG_LEVEL_WARN)
#define PX_LOGE(...) phoenix::logging::log(phoenix::logging::level::error, fmt::format(__VA_ARGS__))
#define PX_LOGW(...) phoenix::logging::log(phoenix::logging::level::warn, fmt::format(__VA_ARGS__))
#define PX_LOGI(...)
#define PX_LOGD(...)
#elif defined(PX_LOG_LEVEL_ERROR)
#define PX_LOGE(...) phoenix::logging::log(phoenix::logging::level::error, fmt::format(__VA_ARGS__))
#define PX_LOGW(...)
#define PX_LOGI(...)
#define PX_LOGD(...)
#else
#define PX_LOGE(...)
#define PX_LOGW(...)
#define PX_LOGI(...)
#define PX_LOGD(...)
#endif

namespace phoenix {
	class buffer;

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
	bool iequals(std::string_view a, std::string_view b);

	/// \brief A basic datetime structure used by the *ZenGin*.
	struct date {
		/// \brief Parses a date from a buffer.
		/// \param buf The buffer to read from
		/// \return The date.
		static date parse(buffer& buf);

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
		static void use_logger(std::function<void(level, const std::string&)>&& callback);

		/// \brief Use the default logger callback for phoenix.
		static void use_default_logger();

		/// \brief Send a logging event to the underlying log callback.
		/// \param lvl The level of the log message.
		/// \param message The message to send.
		static void log(level lvl, std::string&& message) {
			if (logging::callback) {
				(*logging::callback)(lvl, message);
			}
		}

	private:
		static std::optional<std::function<void(level, const std::string&)>> callback;
	};

	/// \brief Base class for all exceptions.
	class error : public std::exception {
	public:
		explicit error(std::string&& message);

		[[nodiscard]] inline const char* what() const noexcept override {
			return message.c_str();
		}

	public:
		[[maybe_unused]] const std::string message;
	};

	/// \brief An error representing a parsing failure of any kind.
	class parser_error : public error {
	public:
		explicit parser_error(std::string&& resource_type);
		explicit parser_error(std::string&& resource_type, std::string&& context);
		explicit parser_error(std::string&& resource_type, const std::exception& cause);
		explicit parser_error(std::string&& resource_type, const std::exception& cause, std::string&& context);

	public:
		const std::string resource_type;
		const std::optional<std::string> context {std::nullopt};
		const std::optional<std::exception> cause {std::nullopt};
	};
} // namespace phoenix
