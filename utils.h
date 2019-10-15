/*  
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
 *  Portions created by the Initial Developer are Copyright (C) 2010
 *  the Initial Developer. All Rights Reserved.
 *  
 *  Contributor(s):
 *  
 *  Special Thanks to:
 *  
 *  SA:MP Team past, present and future
 */

#pragma once

int
	strincmp(const char *, const char *, const unsigned int);

bool
	strstrin(const char *, const char *, unsigned int);

int *
	GetConnected();

int *
	GetNPCs();

char *
	GetNames();

bool
	IsPlayerConnected(int playerid);

bool
	IsPlayerNPC(int playerid);

char *
	GetPlayerName(int playerid);

void
	TempDelimiter(char * ch);

void
	RestoreDelimiter();

bool
	IsDelimiter(char ch);

bool
	InitialiseDelimiter();

bool
	ResetDelimiter();

bool
	IsDefaultDelimiter();

bool
	IsStringEnd(char ch);

bool
	AddDelimiter(char ch);

bool
	AddDelimiters(char * ch);

bool
	AddDelimiter(char ch);

bool
	IsWhitespace(char ch);

bool
	IsSpacer(char ch);

bool
	IsEnd(char ch);

void
	SkipSpacer(char ** input);

void
	SkipOneSpacer(char ** input);

void
	FindSpacer(char ** input);

void
	FindDelimiter(char ** input);

bool
	GetReturn(char ** input);

bool
	GetReturnDefault(char ** input);

void
	SkipWhitespace(char ** input);

bool
	strichecks(char * st1, const char * st2);

const char *
	FindFirstOf(const char *str, char ch);
