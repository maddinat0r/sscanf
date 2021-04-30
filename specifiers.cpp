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

#include <string.h>
#include <stdlib.h>

#include "sscanf.h"
#include "args.h"
#include "utils.h"
#include "data.h"
#include "specifiers.h"

extern logprintf_t
	logprintf;

extern int
	gAlpha,
	gForms,
	gOptions;

extern unsigned int
	g_iTrueMax,
	g_iInvalid,
	g_iMaxPlayerName;

//extern int
//	g_iServerVersion;

bool
	DoI(char ** input, int * ret)
{
	*ret = GetDec(input);
	return GetReturn(input);
}

bool
	DoN(char ** input, int * ret)
{
	*ret = GetNumber(input);
	return GetReturn(input);
}

bool
	DoH(char ** input, int * ret)
{
	*ret = GetHex(input);
	return GetReturn(input);
}

bool
	DoM(char ** input, unsigned int * ret)
{
	int type;
	// Colours.
	*ret = GetColour(input, &type, gAlpha);
	// Check the given form is allowed.
	return (gForms & type) && GetReturn(input);
}

bool
	DoO(char ** input, int * ret)
{
	*ret = GetOct(input);
	return GetReturn(input);
}

bool
	DoF(char ** input, double * ret)
{
	*ret = strtod(*input, input);
	return GetReturn(input);
}

bool
	DoC(char ** input, char * ret)
{
	*ret = **input;
	++(*input);
	if (*ret == '\\')
	{
		if (IsSpacer(**input))
		{
			// '\ '
			*ret = **input;
			++(*input);
		}
		else if (**input == '\\')
		{
			// '\\'
			++(*input);
		}
	}
	return GetReturn(input);
}

bool
	DoL(char ** input, bool * ret)
{
	*ret = GetLogical(input);
	return true;
}

bool
	DoB(char ** input, int * ret)
{
	*ret = (int)GetBool(input);
	return GetReturn(input);
}

bool
	DoG(char ** input, double * ret)
{
	char *
		string = *input;
	int
		temp = 0;
	switch (*string)
	{
		case 'N':
		case 'n':
			if (strichecks(string, "NAN_E"))
			{
				*input += 5;
				temp = FLOAT_NAN_E;
			}
			else if (strichecks(string, "NAN"))
			{
				*input += 3;
				temp = FLOAT_NAN;
			}
			else if (strichecks(string, "NEG_INFINITY"))
			{
				*input += 12;
				temp = FLOAT_NEG_INFINITY;
			}
			else if (strichecks(string, "NEGATIVE_INFINITY"))
			{
				*input += 17;
				temp = FLOAT_NEG_INFINITY;
			}
			*ret = (double)(*((float *)&temp));
			break;
		case 'I':
		case 'i':
			if (strichecks(string, "INFINITY"))
			{
				*input += 8;
				temp = FLOAT_INFINITY;
				*ret = (double)(*((float *)&temp));
			}
			break;
		case '-':
			if (strichecks(string + 1, "INFINITY"))
			{
				*input += 9;
				temp = FLOAT_NEG_INFINITY;
				*ret = (double)(*((float *)&temp));
				break;
			}
			// FALLTHROUGH
		default:
			// Read in the value and save the pointer - may as well use
			// existing, pre set up variables.
			*ret = strtod(string, input);
			break;
	}
	return GetReturn(input);
}

bool
	DoS(char ** input, char ** ret, int length, bool all)
{
	// Don't reallocate the memory, just set pointers to the current data and
	// add nulls to terminate.
	int
		i = 0;
	// Save the pointer to the start of the data.
	*ret = *input;
	// Check if we want the whole remaining string or just the next word.
	char
		* string = *input,
		* outp = string;
	if (all)
	{
		// Everything.
		while (!IsEnd(*string))
		{
			++i;
			// Cap at "length" characters long.
			if (i == length)
			{
				// Could let the loop exit properly as it would next time, but
				// there's no point - it's just extra work and we know it's OK.
				// We set the null before incrementing to ensure it's included
				// in the output.
				SscanfWarning("String buffer overflow.");
				// Removed the break - discard the rest of the string.
				//break;
			}
			else if (i < length)
			{
				if (*string == '\\')
				{
					if (IsEnd(*(string + 1)))
					{
						++string;
						break;
					}
					// You can only escape the escape character or spaces.
					// Spaces aren't technically used here, but they COULD be
					// used in the current string in other circumstances, so
					// they may be present even when not needed.
					if (*(string + 1) == '\\' || IsSpacer(*(string + 1)))
					{
						++string;
					}
				}
				if (outp != string)
				{
					// Compress the string only if we have a discrepancy
					// between input and output pos.
					*outp = *string;
				}
				++outp;
			}
			++string;
		}
	}
	else if (IsDefaultDelimiter())
	{
		while (!IsWhitespace(*string))
		{
			++i;
			if (i == length)
			{
				SscanfWarning("String buffer overflow.");
			}
			else if (i < length)
			{
				if (*string == '\\')
				{
					if (IsEnd(*(string + 1)))
					{
						++string;
						break;
					}
					if (*(string + 1) == '\\' || IsWhitespace(*(string + 1)))
					{
						++string;
					}
				}
				if (outp != string)
				{
					*outp = *string;
				}
				++outp;
			}
			++string;
		}
	}
	else
	{
		// Just a single word.  Note that if your delimiter is a backslash you
		// can't escape it - this is not a bug, just don't try use it as a
		// delimiter and still expect to be able to use it in a string.
		while (!IsEnd(*string) && !IsDelimiter(*string))
		{
			++i;
			if (i == length)
			{
				SscanfWarning("String buffer overflow.");
			}
			else if (i < length)
			{
				if (*string == '\\')
				{
					if (IsEnd(*(string + 1)))
					{
						++string;
						break;
					}
					// Escape spaces, backspace and delimiters - this code is
					// context independent so you can use a string with or
					// without a delimiter and can still escape spaces.
					if (*(string + 1) == '\\' || IsSpacer(*(string + 1)))
					{
						++string;
					}
				}
				if (outp != string)
				{
					*outp = *string;
				}
				++outp;
			}
			++string;
		}
	}
	if (!IsEnd(*string))
	{
		// Skip the final character.
		*input = string + 1;
	}
	else
	{
		// Save the return.
		*input = string;
	}
	// Add a null terminator.
	*outp = '\0';
	// Can't really fail on a string as everything goes!
	return true;
}

bool
	DoU(char ** input, int * ret, unsigned int start)
{
	char
		* end = NULL,
		* string = *input;
	unsigned int
		val = GetUserString(string, &end);
	if (val < g_iTrueMax && IsPlayerConnected(val))
	{
		*input = end;
		*ret = val;
		return false;
	}
	else
	{
		*ret = g_iInvalid;
		int
			* conn = GetConnected(),
			len = end - string;
		val = start;
		conn += start;
		char
			tmp = *end,
			* name = GetNames();
		name += start * g_iMaxPlayerName;
		*end = '\0';
		switch (gOptions & 6)
		{
			case 0:
				// Original.
				while (val < g_iTrueMax)
				{
					if (*conn++ && !strincmp(name, string, len))
					{
						break;
					}
					name += g_iMaxPlayerName;
					++val;
				}
				break;
			case 2:
				// Partial matches.
				while (val < g_iTrueMax)
				{
					if (*conn++ && strstrin(name, string, len))
					{
						break;
					}
					name += g_iMaxPlayerName;
					++val;
				}
				break;
			case 4:
				// Multiple matches.
				while (val < g_iTrueMax)
				{
					if (*conn++ && !strincmp(name, string, len))
					{
						if (*ret != g_iInvalid)
						{
							val = 0x80000000;
							break;
						}
						*ret = val;
					}
					name += g_iMaxPlayerName;
					++val;
				}
				break;
			case 6:
				// Both.
				while (val < g_iTrueMax)
				{
					if (*conn++ && strstrin(name, string, len))
					{
						if (*ret != g_iInvalid)
						{
							val = 0x80000000;
							break;
						}
						*ret = val;
					}
					name += g_iMaxPlayerName;
					++val;
				}
				break;
		}
		*end = tmp;
	}
	*input = end;
	if (val != g_iTrueMax)
	{
		*ret = val;
	}
	return true;
}

bool
	DoQ(char ** input, int * ret, unsigned int start)
{
	char
		* end = NULL,
		* string = *input;
	// Get the string.
	unsigned int
		val = GetUserString(string, &end);
	// We only have a less than check here as valid IDs start at 0, and
	// GetUserString can't return numbers less than 0, it's physically
	// impossible for it to do so as there's no code there for it to happen.
	// Check that the player is a valid ID, connected and a non-player
	// character, and if one of those checks FAILS, do the code below.  We
	// could attempt to write a C++ equivalent to foreach here, but this should
	// be pretty fast anyway as it uses direct memory access.
	if (val < g_iTrueMax && IsPlayerConnected(val) && IsPlayerNPC(val))
	{
		*input = end;
		*ret = val;
		return false;
	}
	else
	{
		*ret = g_iInvalid;
		// Find the NPC by name.
		int
			* conn = GetConnected(),
			* npc = GetNPCs(),
			len = end - string;
		val = start;
		conn += start;
		npc += start;
		// Save the end character for the name.
		char
			tmp = *end,
			* name = GetNames();
		// Make the input string shorter for comparison.
		*end = '\0';
		switch (gOptions & 6)
		{
			case 0:
				// Original.
				while (val < g_iTrueMax)
				{
					if (*conn && *npc && !strincmp(name, string, len))
					{
						break;
					}
					++conn;
					++npc;
					name += g_iMaxPlayerName;
					++val;
				}
				break;
			case 2:
				// Partial matches.
				while (val < g_iTrueMax)
				{
					if (*conn && *npc && strstrin(name, string, len))
					{
						break;
					}
					++conn;
					++npc;
					name += g_iMaxPlayerName;
					++val;
				}
				break;
			case 4:
				// Multiple matches.
				while (val < g_iTrueMax)
				{
					if (*conn && *npc && !strincmp(name, string, len))
					{
						if (*ret != g_iInvalid)
						{
							val = 0x80000000;
							break;
						}
						*ret = val;
					}
					++conn;
					++npc;
					name += g_iMaxPlayerName;
					++val;
				}
				break;
			case 6:
				// Both.
				// Loop through all the players and check that they're
				// connected, an NPC, and that their name is correct.
				while (val < g_iTrueMax)
				{
					if (*conn && *npc && strstrin(name, string, len))
					{
						if (*ret != g_iInvalid)
						{
							val = 0x80000000;
							break;
						}
						*ret = val;
					}
					// Can't do *npc++ above as it's not always reached and we
					// need it to be incremented (short circuiting).
					++conn;
					++npc;
					name += g_iMaxPlayerName;
					++val;
				}
				break;
		}
		*end = tmp;
	}
	// Save the pointer to the end of the name.
	*input = end;
	// Optimised from the PAWN version.  If it gets to here without having
	// found a valid player then 'val' will be g_iTrueMax.
	if (val != g_iTrueMax)
	{
		*ret = val;
	}
	return true;
}

bool
	DoR(char ** input, int * ret, unsigned int start)
{
	char
		* end = NULL,
		* string = *input;
	unsigned int
		val = GetUserString(string, &end);
	if (val < g_iTrueMax && IsPlayerConnected(val) && !IsPlayerNPC(val))
	{
		*input = end;
		*ret = val;
		return false;
	}
	else
	{
		*ret = g_iInvalid;
		int
			* conn = GetConnected(),
			* npc = GetNPCs(),
			len = end - string;
		val = start;
		conn += start;
		npc += start;
		char
			tmp = *end,
			* name = GetNames();
		*end = '\0';
		switch (gOptions & 6)
		{
			case 0:
				// Original.
				while (val < g_iTrueMax)
				{
					if (*conn && !*npc && !strincmp(name, string, len))
					{
						break;
					}
					++conn;
					++npc;
					name += g_iMaxPlayerName;
					++val;
				}
				break;
			case 2:
				// Partial matches.
				while (val < g_iTrueMax)
				{
					if (*conn && !*npc && strstrin(name, string, len))
					{
						break;
					}
					++conn;
					++npc;
					name += g_iMaxPlayerName;
					++val;
				}
				break;
			case 4:
				// Multiple matches.
				while (val < g_iTrueMax)
				{
					if (*conn && !*npc && !strincmp(name, string, len))
					{
						if (*ret != g_iInvalid)
						{
							val = 0x80000000;
							break;
						}
						*ret = val;
					}
					++conn;
					++npc;
					name += g_iMaxPlayerName;
					++val;
				}
				break;
			case 6:
				// Both.
				// Loop through all the players and check that they're
				// connected, an NPC, and that their name is correct.
				while (val < g_iTrueMax)
				{
					if (*conn && !*npc && strstrin(name, string, len))
					{
						if (*ret != g_iInvalid)
						{
							val = 0x80000000;
							break;
						}
						*ret = val;
					}
					// Can't do *npc++ above as it's not always reached and we
					// need it to be incremented (short circuiting).
					++conn;
					++npc;
					name += g_iMaxPlayerName;
					++val;
				}
				break;
		}
		*end = tmp;
	}
	*input = end;
	if (val != g_iTrueMax)
	{
		*ret = val;
	}
	return true;
}

bool
	DoID(char ** input, int * ret)
{
	if (!FindDefaultStart(input))
	{
		return false;
	}
	*ret = GetDec(input);
	return GetReturnDefault(input);
}

bool
	DoND(char ** input, int * ret)
{
	if (!FindDefaultStart(input))
	{
		return false;
	}
	*ret = GetNumber(input);
	return GetReturnDefault(input);
}

bool
	DoHD(char ** input, int * ret)
{
	if (!FindDefaultStart(input))
	{
		return false;
	}
	*ret = GetHex(input);
	return GetReturnDefault(input);
}

bool
	DoMD(char ** input, unsigned int * ret)
{
	if (!FindDefaultStart(input))
	{
		return false;
	}
	int type;
	*ret = GetColour(input, &type, gAlpha);
	// Don't check the form is specified in defaults.
	return GetReturnDefault(input);
}

bool
	DoOD(char ** input, int * ret)
{
	if (!FindDefaultStart(input))
	{
		return false;
	}
	*ret = GetOct(input);
	return GetReturnDefault(input);
}

bool
	DoFD(char ** input, double * ret)
{
	if (!FindDefaultStart(input))
	{
		return false;
	}
	*ret = strtod(*input, input);
	return GetReturnDefault(input);
}

bool
	DoCD(char ** input, char * ret)
{
	if (!FindDefaultStart(input))
	{
		return false;
	}
	*ret = **input;
	++(*input);
	if (*ret == '\\')
	{
		if (IsSpacer(**input))
		{
			// '\ '
			*ret = **input;
			++(*input);
		}
		else if (**input == '\\')
		{
			// '\\'
			++(*input);
		}
	}
	return GetReturnDefault(input);
}

bool
	DoBD(char ** input, int * ret)
{
	if (!FindDefaultStart(input))
	{
		return false;
	}
	*ret = (int)GetBool(input);
	return GetReturnDefault(input);
}

bool
	DoGD(char ** input, double * ret)
{
	if (!FindDefaultStart(input))
	{
		return false;
	}
	char *
		string = *input;
	int
		temp = 0;
	switch (*string)
	{
		case 'N':
		case 'n':
			if (strichecks(string, "NAN_E"))
			{
				*input += 5;
				temp = FLOAT_NAN_E;
			}
			else if (strichecks(string, "NAN"))
			{
				*input += 3;
				temp = FLOAT_NAN;
			}
			else if (strichecks(string, "NEG_INFINITY"))
			{
				*input += 12;
				temp = FLOAT_NEG_INFINITY;
			}
			else if (strichecks(string, "NEGATIVE_INFINITY"))
			{
				*input += 17;
				temp = FLOAT_NEG_INFINITY;
			}
			*ret = (double)(*((float *)&temp));
			break;
		case 'I':
		case 'i':
			if (strichecks(string, "INFINITY"))
			{
				*input += 8;
				temp = FLOAT_INFINITY;
				*ret = (double)(*((float *)&temp));
			}
			break;
		case '-':
			if (strichecks(string + 1, "INFINITY"))
			{
				*input += 9;
				temp = FLOAT_NEG_INFINITY;
				*ret = (double)(*((float *)&temp));
				break;
			}
			// FALLTHROUGH
		default:
			// Read in the value and save the pointer - may as well use
			// existing, pre set up variables.
			*ret = strtod(string, input);
			break;
	}
	return GetReturnDefault(input);
}

bool
	DoSD(char ** input, char ** ret, int * length, struct args_s & args)
{
	if (!FindDefaultStart(input))
	{
		return false;
	}
	// Don't reallocate the memory, just set pointers to the current data and
	// add nulls to terminate.
	int
		i = 0;
	// Save the pointer to the start of the data.
	*ret = *input;
	// Check if we want the whole remaining string or just the next word.
	char
		* string = *input,
		* outp = string;
	while (!IsEnd(*string) && !IsDelimiter(*string))
	{
		++i;
		if (*string == '\\')
		{
			if (IsEnd(*(string + 1)))
			{
				++string;
				break;
			}
			// Escape spaces, backspace and delimiters - this code is
			// context independent so you can use a string with or
			// without a delimiter and can still escape spaces.
			if (*(string + 1) == '\\' || IsWhitespace(*(string + 1)) || IsDelimiter(*(string + 1)))
			{
				++string;
			}
		}
		if (outp != string)
		{
			*outp = *string;
		}
		++outp;
		++string;
	}
	if (IsDelimiter(*string))
	{
		// Skip the final character.
		*input = string + 1;
		// NOW get the length.
		*length = GetLength(input, args);
	}
	else
	{
		SscanfWarning("Unclosed default value.");
		// Save the return.
		*input = string;
		SscanfError("String/array must include a length, please add a destination size.");
		*length = 1;
	}
	// Add a null terminator.
	if (i >= *length)
	{
		SscanfWarning("String buffer overflow.");
		*(*ret + *length - 1) = '\0';
	}
	else
	{
		*outp = '\0';
	}
	return true;
}

bool
	DoUD(char ** input, int * ret)
{
	if (!FindDefaultStart(input))
	{
		return false;
	}
	if (gOptions & 1) DoU(input, ret, 0);
	else DoN(input, ret);
	return GetReturnDefault(input);
}

bool
	DoQD(char ** input, int * ret)
{
	if (!FindDefaultStart(input))
	{
		return false;
	}
	if (gOptions & 1) DoQ(input, ret, 0);
	else DoN(input, ret);
	return GetReturnDefault(input);
}

bool
	DoRD(char ** input, int * ret)
{
	if (!FindDefaultStart(input))
	{
		return false;
	}
	if (gOptions & 1) DoR(input, ret, 0);
	else DoN(input, ret);
	return GetReturnDefault(input);
}

bool
	DoLD(char ** input, bool * ret)
{
	if (!FindDefaultStart(input))
	{
		return false;
	}
	DoL(input, ret);
	return GetReturnDefault(input);
}
