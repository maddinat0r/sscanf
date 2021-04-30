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

void
	RestoreOpts(int, int, int);

char
	GetSingleType(char ** format);

char *
	GetMultiType(char ** format);

char *
	GetMultiType(char ** format);

unsigned int
	GetUserString(char * string, char ** end);

int
	GetDec(char ** const input);

int
	GetOct(char ** const input);

int
	GetHex(char ** const input);

unsigned int
	GetColour(char ** const input, int * type, unsigned int alpha);

int
	GetBool(char ** const input);

int
	GetNumber(char ** const input);

bool
	GetLogical(char ** const input);

void
	SkipDefault(char ** const str);

void
	SkipDefaultEx(char ** const data);

void
	SkipLength(char ** const input);

int
	GetLength(char ** const input, struct args_s & args);

bool
	FindDefaultStart(char ** const str);
