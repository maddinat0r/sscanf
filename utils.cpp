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

#include "utils.h"
#include "sscanf.h"

#include <ctype.h>

//int
//	g_iServerVersion = 0;

unsigned int
	g_iTrueMax = 0,
	g_iInvalid = 0,
	g_iMaxPlayerName = 0;

int *
	g_iConnected = 0;

int *
	g_iNPC = 0;

char *
	g_szPlayerNames = 0;

//char **
//	g_pServer = NULL;

static char
	* g_cDelim = " ",
	* g_cTemp = 0;

extern logprintf_t
	logprintf;

// Server information

int
	strincmp(const char * st1, const char * st2, unsigned int n)
{
	int
		ret;
	while (n--)
	{
		if (*st1 == '\0' || *st2 == '\0')
		{
			return 0;
		}
		ret = tolower(*st1++) - tolower(*st2++);
		if (ret)
		{
			return ret;
		}
	}
	return 0;
}

bool
	strstrin(const char * st1, const char * st2, unsigned int n)
{
	char
		f = tolower(*st2);
	while (*st1)
	{
		if (tolower(*st1) == f)
		{
			if (!strincmp(st1, st2, n))
			{
				return true;
			}
		}
		++st1;
	}
	return false;
}

int *
	GetConnected()
{
	//return (int *)(*g_pServer + 500 * 120 + 8); // g_iTrueMax
	return g_iConnected;
}

int *
	GetNPCs()
{
	//return (int *)(*g_pServer + 500 * 157 + 8); // g_iTrueMax
	return g_iNPC;
}

char *
	GetNames()
{
	//return *g_pServer + 500 * 128 + 8; // g_iTrueMax
	return g_szPlayerNames;
}

bool
	IsPlayerConnected(int playerid)
{
	// Check the IsPlayerConnected variable in memory.
	return GetConnected()[playerid] != 0;
}

bool
	IsPlayerNPC(int playerid)
{
	// Check the IsPlayerNPC variable in memory.
	return GetNPCs()[playerid] != 0;
}

char *
	GetPlayerName(int playerid)
{
	// Get the player name pointer from memory.
	return GetNames() + (playerid * g_iMaxPlayerName);
}

// Spacer information

// These are the delimiter functions.  The code is arranged in this way to
// allow for planned extensions to allow multiple custom delimiters without
// modifying any other code.

bool
	IsDelimiter(char ch)
{
	// Can easily modify this function when we want to add the ability to have
	// multiple delimiters.  This should be the only place that this check is
	// done now - use functions, not code!
	//return ch == g_cDelim;
	char *
		nu = g_cDelim;
	while (*nu)
	{
		if (*nu++ == ch) return true;
	}
	return false;
}

bool
	IsDefaultDelimiter()
{
	return g_cDelim[0] == ' ' && g_cDelim[1] == '\0';
}

void
	TempDelimiter(char * ch)
{
	g_cTemp = g_cDelim;
	g_cDelim = ch;
}

void
	RestoreDelimiter()
{
	g_cDelim = g_cTemp;
	g_cTemp = 0;
}

bool
	InitialiseDelimiter()
{
	return AddDelimiter(' ');
}

bool
	ResetDelimiter()
{
	return InitialiseDelimiter();
}

bool
	AddDelimiter(char ch)
{
	static char
		sStr[2] = {0, 0};
	// Only add new delimiters if we are not in a temporary delimiter area.
	// The only place this applies is when you do something like:
	// E<ip<;>ii>(1, 2, 3)
	sStr[0] = ch;
	sStr[1] = '\0';
	if (g_cTemp)
	{
		g_cTemp = sStr;
	}
	else
	{
		g_cDelim = sStr;
	}
	return true;
}

bool
	AddDelimiters(char * ch)
{
	// Only add new delimiters if we are not in a temporary delimiter area.
	// The only place this applies is when you do something like:
	// E<ip<;>ii>(1, 2, 3)
	if (g_cTemp)
	{
		g_cTemp = ch;
	}
	else
	{
		g_cDelim = ch;
	}
	return true;
}

// Generic spacer code - delimiters and whitespace.

bool
	IsWhitespace(char ch)
{
	return (unsigned char)ch <= (unsigned char)' ';
}

bool
	IsSpacer(char ch)
{
	return IsWhitespace(ch) || IsDelimiter(ch);
}

bool
	IsStringEnd(char ch)
{
	if (IsDefaultDelimiter())
	{
		return IsWhitespace(ch);
	}
	else
	{
		return IsEnd(ch) || IsDelimiter(ch);
	}
}

bool
	IsEnd(char ch)
{
	return ch == '\0';
}

void
	SkipSpacer(char ** input)
{
	char *
		str = *input;
	while (*str && IsSpacer(*str))
	{
		++str;
	}
	*input = str;
}

void
	FindSpacer(char ** input)
{
	char *
		str = *input;
	// Don't need a valid character check here as NULL is a spacer.
	while (!IsSpacer(*str))
	{
		++str;
	}
	*input = str;
}

void
	SkipDelimiter(char ** input)
{
	char *
		str = *input;
	while (*str && IsDelimiter(*str))
	{
		++str;
	}
	*input = str;
}

void
	FindDelimiter(char ** input)
{
	char *
		str = *input;
	while (!IsDelimiter(*str))
	{
		++str;
	}
	*input = str;
}

void
	SkipWhitespace(char ** input)
{
	char *
		str = *input;
	while (*str && IsWhitespace(*str))
	{
		++str;
	}
	*input = str;
}

void
	SkipOneSpacer(char ** input)
{
	char *
		str = *input;
	// Skip all whitespace before the explicit delimiter.
	while (*str && IsWhitespace(*str))
	{
		++str;
	}
	// We have found a delimiter - skip just one of them.
	if (IsDelimiter(*str))
	{
		++str;
		// Now skip any space after it too.
		while (*str && IsWhitespace(*str))
		{
			++str;
		}
	}
	*input = str;
}

void
	FindWhitespace(char ** input)
{
	char *
		str = *input;
	while (!IsWhitespace(*str))
	{
		++str;
	}
	*input = str;
}

bool
	GetReturn(char ** input)
{
	// Check the end is a valid end.
	if (IsSpacer(**input))
	{
		return true;
	}
	return false;
}

bool
	GetReturnDefault(char ** input)
{
	// Check the end is a valid end.
	SkipWhitespace(input);
	if (IsDelimiter(**input))
	{
		++(*input);
	}
	else
	{
		logprintf("sscanf warning: Unclosed default value.");
	}
	return true;
}

bool
	strichecks(char * st1, const char * st2)
{
	// Highly specialised strcmp function.  We don't need the lexical
	// difference, only to know if they're the same, so we only compare then
	// like that and return true/false.  Because the string we're comparing to
	// is constant we can use that for the length, we also know it's always
	// upper case.  Note that this is not a generic function, it only works on
	// capital letters and underlines.
	while (*st2 && (*st2 == (*st1 & ~0x20)))
	{
		++st1;
		++st2;
	}
	// Check if *st2 is null.
	return IsEnd(*st2);
}

const char *
	FindFirstOf(const char *str, char ch)
{
	while (*str != '\0')
	{
		if (*str == ch)
			return str;
		str++;
	}
	return str;
}
