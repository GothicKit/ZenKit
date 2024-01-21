// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Logger.hh"

#include <cstdarg>
#include <cstdio>
#include <ctime>

#define ANSI_RESET "\x1B[0m"
#define ANSI_GRAY "\x1B[90m"
#define ANSI_RED "\x1B[31m"
#define ANSI_GREEN "\x1B[32m"
#define ANSI_YELLOW "\x1B[33m"
#define ANSI_BLUE "\x1B[34m"
#define ANSI_MAGENTA "\x1B[35m"
#define ANSI_BOLD "\x1B[1m"

#define PREFIX "[" ANSI_MAGENTA ANSI_BOLD "ZenKit" ANSI_RESET "]"

namespace zenkit {
	static char zk_global_logger_buffer[4096];

	std::function<void(LogLevel, char const*, char const*)> Logger::_s_callback {};
	LogLevel Logger::_s_level {LogLevel::INFO};

	ZKINT static void zk_internal_logger_default(LogLevel level, char const* name, char const* message) {
		time_t now_t = time(nullptr);
		tm* now = gmtime(&now_t);

		fprintf(stderr,
		        ANSI_GRAY "%04d-%02d-%02d %02d:%02d:%02d " ANSI_RESET,
		        now->tm_year + 1900,
		        now->tm_mon + 1,
		        now->tm_mday,
		        now->tm_hour,
		        now->tm_min,
		        now->tm_sec);

		switch (level) {
		case LogLevel::ERROR:
			fprintf(stderr, PREFIX " (" ANSI_RED "ERROR  " ANSI_RESET ") › %s: %s\n", name, message);
			break;
		case LogLevel::WARNING:
			fprintf(stderr, PREFIX " (" ANSI_YELLOW "WARNING" ANSI_RESET ") › %s: %s\n", name, message);
			break;
		case LogLevel::INFO:
			fprintf(stderr, PREFIX " (" ANSI_BLUE "INFO   " ANSI_RESET ") › %s: %s\n", name, message);
			break;
		case LogLevel::DEBUG:
			fprintf(stderr, PREFIX " (" ANSI_GREEN "DEBUG  " ANSI_RESET ") › %s: %s\n", name, message);
			break;
		case LogLevel::TRACE:
			fprintf(stderr, PREFIX " (TRACE  ) › %s: %s\n", name, message);
			break;
		}
	}

	void Logger::use_logger(std::function<void(LogLevel, std::string const&)>&& callback) {
		set(LogLevel::INFO, [callback](LogLevel lvl, std::string_view name, std::string_view sv) {
			callback(lvl, std::string {name} + ": " + std::string {sv});
		});
	}

	void Logger::use_default_logger() {
		set_default(LogLevel::INFO);
	}

	void Logger::log(LogLevel lvl, char const* name, char const* fmt, ...) {
		va_list ap;
		va_start(ap, fmt);
		logv(lvl, name, fmt, ap);
		va_end(ap);
	}

	void Logger::logv(LogLevel lvl, char const* name, char const* fmt, va_list ap) {
		if (!_s_callback) return;
		if (lvl > _s_level) return;
		vsnprintf(zk_global_logger_buffer, sizeof zk_global_logger_buffer - 1, fmt, ap);
		_s_callback(lvl, name, zk_global_logger_buffer);
	}

	void Logger::set(LogLevel lvl, std::function<void(LogLevel, char const*, char const*)> const& cb) {
		_s_level = lvl;
		_s_callback = cb;
	}

	void Logger::set_default(LogLevel lvl) {
		_s_level = lvl;
		_s_callback = zk_internal_logger_default;
	}
} // namespace zenkit
