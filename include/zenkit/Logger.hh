// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"

#include <cstdint>
#include <functional>
#include <string>

#if defined(__GNUC__) || defined(__clang__)
	#define ZK_PRINTF_LIKE(a, b) [[gnu::format(printf, a, b)]]
#else
	#define ZK_PRINTF_LIKE(a, b)
#endif

namespace zenkit {
	enum class LogLevel : std::uint8_t {
		ERROR = 0,
		WARNING = 1,
		INFO = 2,
		DEBUG = 3,
		TRACE = 4,

		error ZKREM("renamed to LogLevel::ERROR") = ERROR,
		warn ZKREM("renamed to LogLevel::WARNING") = WARNING,
		info ZKREM("renamed to LogLevel::INFO") = INFO,
		debug ZKREM("renamed to LogLevel::DEBUG") = DEBUG
	};

	class Logger {
	public:
		using level ZKREM("renamed to zenkit::LogLevel") = LogLevel;

		/// \brief Supply a custom logger callback to be used for log output from phoenix.
		/// \param callback The callback to use.
		ZKREM("renamed to ::set")
		ZKAPI static void use_logger(std::function<void(LogLevel, std::string const&)>&& callback);

		/// \brief Use the default logger callback for phoenix.
		ZKREM("renamed to ::set_default") ZKAPI static void use_default_logger();

		ZK_PRINTF_LIKE(3, 4) ZKAPI static void log(LogLevel lvl, char const* name, char const* fmt, ...);
		ZKAPI static void set(LogLevel lvl, std::function<void(LogLevel, char const*, char const*)> const& cb);
		ZKAPI static void set_default(LogLevel lvl);

	private:
		static std::function<void(LogLevel, char const*, char const*)> _s_callback;
		static LogLevel _s_level;
	};
} // namespace zenkit
