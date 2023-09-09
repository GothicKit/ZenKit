// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Logger.hh"

#ifndef _MSC_VER
	#define ZKLOGT(...) zenkit::Logger::log(zenkit::LogLevel::TRACE, __VA_ARGS__)
	#define ZKLOGD(...) zenkit::Logger::log(zenkit::LogLevel::DEBUG, __VA_ARGS__)
	#define ZKLOGI(...) zenkit::Logger::log(zenkit::LogLevel::INFO, __VA_ARGS__)
	#define ZKLOGW(...) zenkit::Logger::log(zenkit::LogLevel::WARNING, __VA_ARGS__)
	#define ZKLOGE(...) zenkit::Logger::log(zenkit::LogLevel::ERROR, __VA_ARGS__)
#else
	#define ZKLOGT(...) zenkit::Logger::log(zenkit::LogLevel::TRACE, ##__VA_ARGS__)
	#define ZKLOGD(...) zenkit::Logger::log(zenkit::LogLevel::DEBUG, ##__VA_ARGS__)
	#define ZKLOGI(...) zenkit::Logger::log(zenkit::LogLevel::INFO, ##__VA_ARGS__)
	#define ZKLOGW(...) zenkit::Logger::log(zenkit::LogLevel::WARNING, ##__VA_ARGS__)
	#define ZKLOGE(...) zenkit::Logger::log(zenkit::LogLevel::ERROR, ##__VA_ARGS__)
#endif
