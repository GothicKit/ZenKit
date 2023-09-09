// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once

#ifdef ZKDYN
	#if defined(_WIN32) || defined(__CYGWIN__)
		#ifdef _ZKEXPORT
			#ifdef __GNUC__
				#define ZKAPI __attribute__((dllexport))
			#else
				#define ZKAPI __declspec(dllexport)
			#endif
		#else
			#ifdef __GNUC__
				#define ZKAPI __attribute__((dllimport))
			#else
				#define ZKAPI __declspec(dllimport)
			#endif
		#endif
		#define ZKINT
	#else
		#define ZKAPI __attribute__((visibility("default")))
		#define ZKINT __attribute__((visibility("hidden")))
	#endif
#else
	#define ZKAPI
	#define ZKINT
#endif

#ifndef ZKNO_REM
	#define ZKREM(reason) [[deprecated(reason)]]
#else
	#define ZKREM(reason)
#endif
