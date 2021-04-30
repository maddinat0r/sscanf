/*
 *  sscanf 2.10.3
 *
 *  Version: MPL 1.1
 *
 *  The contents of this file are subject to the Mozilla Public License Version
 *  1.1 (the "License"); you may not use this file except in compliance with
 *  the License. You may obtain a copy of the License at
 *  http://www.mozilla.org/MPL/
 *
 *  Software distributed under the License is distributed on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 *  for the specific language governing rights and limitations under the
 *  License.
 *
 *  The Original Code is the sscanf 2.0 SA:MP plugin.
 *
 *  The Initial Developer of the Original Code is Alex "Y_Less" Cole.
 *  Portions created by the Initial Developer are Copyright (C) 2020
 *  the Initial Developer. All Rights Reserved.
 *
 *  Contributor(s):
 *
 *      Cheaterman
 *      Emmet_
 *      karimcambridge
 *      leHeix
 *      maddinat0r
 *      Southclaws
 *      Y_Less
 *      ziggi
 *
 *  Special Thanks to:
 *
 *      SA:MP Team past, present, and future.
 *      maddinat0r, for hosting the repo for a very long time.
 *      Emmet_, for his efforts in maintaining it for almost a year.
 */

#pragma once

#include <stdint.h>

typedef
	void (* logprintf_t)(char *, ...);

typedef
	char * (* GetServer_t)();

#define SSCANF_FAIL_RETURN (-1)
#define SSCANF_CONT_RETURN (((unsigned int)-1) >> 1)
#define SSCANF_TRUE_RETURN (0)

// Capped for memory reasons.  I chose 32 because it's a reasonable length for
// many uses and frankly if they get warnings and don't fix them it's their
// own fault if they have problems with strings!
#define SSCANF_MAX_LENGTH (32)

// 32 bit special float values.
#define FLOAT_INFINITY          (0x7F800000)
#define FLOAT_NEG_INFINITY      (0xFF800000)
#define FLOAT_NAN               (0xFFFFFFFF)
#define FLOAT_NAN_E             (0x7FFFFFFF)
#define FLOAT_NEGATIVE_INFINITY (FLOAT_NEG_INFINITY)

#define SSCANF_QUIET 0

//void SscanfWarning(char const *, ...);
//void SscanfError(char const *, ...);

#define SscanfWarning(str,...) \
	do																								                    \
	{																								                    \
		if (SscanfErrLine()) logprintf("sscanf warning (%s:%d): " #str, gCallFile, gCallLine, ##__VA_ARGS__);           \
		else logprintf("%s warning (`%s`): " #str, gCallFile, gFormat, ##__VA_ARGS__);		                            \
	}																								                    \
	while (0)

#define SscanfError(str,...) \
	do																						                            \
	{																						                            \
		if (SscanfErrLine()) logprintf("sscanf error (%s:%d): " #str, gCallFile, gCallLine, ##__VA_ARGS__);             \
		else logprintf("%s error (`%s`): " #str, gCallFile, gFormat, ##__VA_ARGS__);	                                \
	}																						                            \
	while (0)

bool SscanfErrLine();
typedef int32_t cell;

extern char
	* gFormat,
	* gCallFile;

extern int
	gCallLine;

extern cell
	* gCallResolve;

#if defined __cplusplus
	#define PAWN_NATIVE_EXTERN extern "C"
#else
	#define PAWN_NATIVE_EXTERN extern
#endif

#if defined _WIN32 || defined __CYGWIN__
	#define PAWN_NATIVE_DLLEXPORT __declspec(dllexport)
	#define PAWN_NATIVE_DLLIMPORT __declspec(dllimport)
	#define PAWN_NATIVE_API __cdecl
#elif defined __linux__ || defined __APPLE__
	#define PAWN_NATIVE_DLLEXPORT __attribute__((visibility("default")))
	#define PAWN_NATIVE_DLLIMPORT 
	#define PAWN_NATIVE_API __attribute__((cdecl))
#endif

#define PAWN_NATIVE_EXPORT PAWN_NATIVE_EXTERN PAWN_NATIVE_DLLEXPORT
#define PAWN_NATIVE_IMPORT PAWN_NATIVE_EXTERN PAWN_NATIVE_DLLIMPORT

