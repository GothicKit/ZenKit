// Copyright © 2023 Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once

#ifndef PHOENIX_STATIC
	#if defined(_WIN32) || defined(__CYGWIN__)
		#ifdef PHOENIX_EXPORTS
			#ifdef __GNUC__
				#define PHOENIX_API __attribute__((dllexport))
			#else
				#define PHOENIX_API __declspec(dllexport)
			#endif
		#else
			#ifdef __GNUC__
				#define PHOENIX_API __attribute__((dllimport))
			#else
				#define PHOENIX_API __declspec(dllimport)
			#endif
		#endif
		#define PHOENIX_INTERNAL
	#else
		#define PHOENIX_API __attribute__((visibility("default")))
		#define PHOENIX_INTERNAL __attribute__((visibility("hidden")))
	#endif
#else
	#define PHOENIX_API
	#define PHOENIX_INTERNAL
#endif

#define PHOENIX_DEPRECATED(reason) [[deprecated(reason)]]
