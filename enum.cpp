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

#include <string.h>

#include "sscanf.h"
#include "args.h"
#include "utils.h"
#include "specifiers.h"
#include "data.h"

extern logprintf_t
	logprintf;

#define SAVE_VALUE(m)                   \
	if (doSave)                         \
		*cptr++ = m

#define SAVE_VALUE_F(m)                 \
	if (doSave) {                       \
		float f = (float)m;             \
		*cptr++ = amx_ftoc(f); }

// Macros for the regular values.
#define DO(m,n)                         \
	{ m b;                              \
	if (Do##n(&string, &b)) {           \
		SAVE_VALUE((cell)b);            \
		break; }                        \
	*input = string;                    \
	return SSCANF_FAIL_RETURN; }

#define DOV(m,n)                        \
	{ m b;                              \
	Do##n(&string, &b);                 \
	SAVE_VALUE((cell)b); }

#define DOF(m,n)                        \
	{ m b;                              \
	if (Do##n(&string, &b)) {           \
		SAVE_VALUE_F(b)                 \
		break; }                        \
	*input = string;                    \
	return SSCANF_FAIL_RETURN; }

#define OPTIONAL_INVALID \
	SscanfWarning("Optional types invalid in enum specifiers, consider using 'E'.")

#define DX(m,n) \
	OPTIONAL_INVALID;

#define DXF(m,n) \
	OPTIONAL_INVALID;

extern AMX *
	g_aCurAMX;

extern int
	gOptions;

bool
	DoK(AMX * amx, char ** defaults, char ** input, cell * cptr, bool optional, bool all);

int
	DoEnumValues(char * format, char ** input, cell * cptr, bool defaults, struct args_s & args)
{
	// If cptr is NULL we never save - regardless of quiet sections.
	bool
		doSave = cptr != NULL;
	char *
		string = *input;
	// Copied directly from the main loop, just with different macros.
	while (*string)
	{
		if (!*format)
		{
			// Happy with this return.
			*input = string;
			return SSCANF_TRUE_RETURN;
		}
		else if (IsWhitespace(*format))
		{
			++format;
		}
		else
		{
			switch (*format++)
			{
				case 'L':
					DX(bool, L)
					// FALLTHROUGH
				case 'l':
					DOV(bool, L)
					break;
				case 'B':
					DX(int, B)
					// FALLTHROUGH
				case 'b':
					DO(int, B)
				case 'N':
					DX(int, N)
					// FALLTHROUGH
				case 'n':
					DO(int, N)
				case 'C':
					DX(char, C)
					// FALLTHROUGH
				case 'c':
					DO(char, C)
				case 'I':
				case 'D':
					DX(int, I)
					// FALLTHROUGH
				case 'i':
				case 'd':
					DO(int, I)
				case 'H':
				case 'X':
					DX(int, H)
					// FALLTHROUGH
				case 'h':
				case 'x':
					DO(int, H)
				case 'M':
					DX(unsigned int, M)
					// FALLTHROUGH
				case 'm':
					DO(unsigned int, M)
				case 'O':
					DX(int, O)
					// FALLTHROUGH
				case 'o':
					DO(int, O)
				case 'F':
					DXF(double, F)
					// FALLTHROUGH
				case 'f':
					DOF(double, F)
				case 'G':
					DXF(double, G)
					// FALLTHROUGH
				case 'g':
					DOF(double, G)
				case '{':
					if (doSave)
					{
						doSave = false;
					}
					else if (cptr)
					{
						// Already in a quiet section.
						SscanfWarning("Can't have nestled quiet sections.");
					}
					continue;
				case '}':
					if (doSave)
					{
						SscanfWarning("Not in a quiet section.");
					}
					else
					{
						if (cptr)
						{
							doSave = true;
						}
						else
						{
							SscanfWarning("Can't remove quiet in enum.");
						}
					}
					continue;
				case 'P':
					{
						ResetDelimiter();
						char *
							t = GetMultiType(&format);
						if (t) AddDelimiters(t);
						else return SSCANF_FAIL_RETURN;
						continue;
					}
					//SscanfWarning("You can't have an optional delimiter.");
					// FALLTHROUGH
				case 'p':
					// 'P' doesn't exist.
					// Theoretically, for compatibility, this should be:
					// p<delimiter>, but that will break backwards
					// compatibility with anyone doing "p<" to use '<' as a
					// delimiter (doesn't matter how rare that may be).  Also,
					// writing deprecation code and both the new and old code
					// is more trouble than it's worth, and it's slow.
					// UPDATE: I wrote the "GetSingleType" code for 'a' and
					// figured out a way to support legacy and new code, while
					// still maintaining support for the legacy "p<" separator,
					// so here it is:
					ResetDelimiter();
					AddDelimiter(GetSingleType(&format));
					continue;
				case 'K':
					OPTIONAL_INVALID;
					// FALLTHROUGH
				case 'k':
					//DOF(double, K)
					if (defaults && !(gOptions & 16))
					{
						GetMultiType(&format);
						if (doSave)
						{
							int
								b;
							DoI(&string, &b);
							*cptr++ = b;
						}
						else
						{
							int
								b;
							DoI(&string, &b);
						}
						*(format - 1) = '>';
						break;
					}
					else if (doSave)
					{
						if (DoK(g_aCurAMX, &format, &string, cptr, false, false))
						{
							*(format - 1) = '>';
							++cptr;
							break;
						}
					}
					else
					{
						if (DoK(g_aCurAMX, &format, &string, NULL, false, false))
						{
							*(format - 1) = '>';
							break;
						}
					}
					*input = string;
					return SSCANF_FAIL_RETURN;
				case 'S':
					OPTIONAL_INVALID;
					// FALLTHROUGH
				case 's':
					{
						// Get the length.
						int
							lole = GetLength(&format, args);
						if (!lole)
						{
							return SSCANF_FAIL_RETURN;
						}
						char *
							dest;
						DoS(&string, &dest, lole, IsEnd(*format));
						// Send the string to PAWN.
						if (doSave)
						{
							// Save the string.
							amx_SetString(cptr, dest, 0, 0, lole);
							// Increase the pointer by the MAXIMUM length of
							// the string - that's how enum strings work.
							cptr += lole;
						}
					}
					break;
				case 'Z':
					OPTIONAL_INVALID;
					// FALLTHROUGH
				case 'z':
					{
						// Get the length.
						int
							lole = GetLength(&format, args);
						if (!lole)
						{
							return SSCANF_FAIL_RETURN;
						}
						char *
							dest;
						DoS(&string, &dest, lole, IsEnd(*format));
						// Send the string to PAWN.
						if (doSave)
						{
							// Save the string.
							amx_SetString(cptr, dest, 1, 0, lole);
							// Increase the pointer by the MAXIMUM length of
							// the string - that's how enum strings work.
							cptr += lole;
						}
					}
					break;
				case 'U':
					DX(int, U)
					// FALLTHROUGH
				case 'u':
					if (*format == '[')
					{
						SscanfWarning("User arrays are not supported in enums.");
						SkipLength(&format);
					}
					#define DoU(m,n) DoU(m,n,0)
					if (defaults && !(gOptions & 1))
					{
						DOV(int, I)
					}
					else
					{
						DOV(int, U)
					}
					#undef DoU
					break;
				case 'Q':
					DX(int, Q)
					// FALLTHROUGH
				case 'q':
					if (*format == '[')
					{
						SscanfWarning("User arrays are not supported in enums.");
						SkipLength(&format);
					}
					#define DoQ(m,n) DoQ(m,n,0)
					if (defaults && !(gOptions & 1))
					{
						DOV(int, I)
					}
					else
					{
						DOV(int, Q)
					}
					#undef DoQ
					break;
				case 'R':
					DX(int, R)
					// FALLTHROUGH
				case 'r':
					if (*format == '[')
					{
						SscanfWarning("User arrays are not supported in enums.");
						SkipLength(&format);
					}
					#define DoR(m,n) DoR(m,n,0)
					if (defaults && !(gOptions & 1))
					{
						DOV(int, I)
					}
					else
					{
						DOV(int, R)
					}
					#undef DoR
					break;
				case 'A':
				case 'a':
					SscanfError("Arrays are not supported in enums.");
					*input = string;
					return SSCANF_FAIL_RETURN;
				case 'E':
				case 'e':
					SscanfError("Enums are not supported in enums.");
					*input = string;
					return SSCANF_FAIL_RETURN;
				case '\'':
					// Find the end of the literal.
					{
						char
							* str = format,
							* write = format;
						bool
							escape = false;
						while (!IsEnd(*str) && (escape || *str != '\''))
						{
							if (*str == '\\')
							{
								if (escape)
								{
									// "\\" - Go back a step to write this
									// character over the last character (which
									// just happens to be the same character).
									--write;
								}
								escape = !escape;
							}
							else
							{
								if (*str == '\'')
								{
									// Overwrite the escape character with the
									// quote character.  Must have been
									// preceeded by a slash or it wouldn't have
									// got to here in the loop.
									--write;
								}
								escape = false;
							}
							// Copy the string over itself to get rid of excess
							// escape characters.
							// Not sure if it's faster in the average case to
							// always do the copy or check if it's needed.
							// This write is always safe as it makes the string
							// shorter, so we'll never run out of space.  It
							// will also not overwrite the original string.
							*write++ = *str++;
						}
						if (*str == '\'')
						{
							// Correct end.  Make a shorter string to search
							// for.
							*write = '\0';
							// Find the current section of format in string.
							char *
								find = strstr(string, format);
							if (!find)
							{
								// Didn't find the string.
								*input = string;
								return SSCANF_FAIL_RETURN;
							}
							// Found the string.  Update the current string
							// position to the length of the search term
							// further along from the start of the term.  Use
							// "write" here as we want the escaped string
							// length.
							string = find + (write - format);
							// Move to after the end of the search string.  Use
							// "str" here as we want the unescaped string
							// length.
							format = str + 1;
						}
						else
						{
							SscanfWarning("Unclosed string literal.");
							char *
								find = strstr(string, format);
							if (!find)
							{
								*input = string;
								return SSCANF_FAIL_RETURN;
							}
							string = find + (write - format);
							format = str;
						}
					}
					break;
				case '?':
					SscanfError("Options are not supported in enums.");
					return SSCANF_FAIL_RETURN;
				case '%':
					SscanfWarning("sscanf specifiers do not require '%' before them.");
					continue;
				case '-':
					{
						int
							len = 1;
						switch (*format++)
						{
							case 'i':
							case 'f':
							case 'l':
							case 'b':
							case 'n':
							case 'c':
							case 'd':
							case 'h':
							case 'm':
							case 'x':
							case 'o':
							case 'g':
								break;
							case 'I':
							case 'F':
							case 'L':
							case 'B':
							case 'N':
							case 'C':
							case 'D':
							case 'H':
							case 'M':
							case 'X':
							case 'O':
							case 'G':
								OPTIONAL_INVALID;
								break;
							case 'K':
								OPTIONAL_INVALID;
							case 'k':
								GetMultiType(&format);
								break;
							case 'P':
							case 'p':
								SscanfWarning("A minus delimiter makes no sense.");
								len = 0;
								break;
							case '{':
							case '}':
								SscanfWarning("A minus quiet section makes no sense.");
								len = 0;
								break;
							case 'U':
							case 'Q':
							case 'R':
								OPTIONAL_INVALID;
							case 'u':
							case 'q':
							case 'r':
								if (*format == '[')
								{
									len = GetLength(&format, args);
								}
								break;
							case 'A':
								OPTIONAL_INVALID;
							case 'a':
								len = GetLength(&format, args);
								break;
							case 'E':
								OPTIONAL_INVALID;
							case 'e':
								SscanfError("Enums are not supported in enums.");
								*input = string;
								return SSCANF_FAIL_RETURN;
							case 'Z':
								OPTIONAL_INVALID;
								// FALLTHROUGH
							case 'z':
								len = GetLength(&format, args);
								break;
							case 'S':
								OPTIONAL_INVALID;
								// FALLTHROUGH
							case 's':
								len = GetLength(&format, args);
								break;
							case '?':
								SscanfWarning("A minus option makes no sense.");
								len = 0;
								break;
							case '%':
								SscanfWarning("sscanf specifiers do not require '%' before them.");
								len = 0;
								break;
							case '-':
								SscanfWarning("A minus minus makes no sense.");
								len = 0;
								break;
							default:
								SscanfWarning("Unknown format specifier '%c', skipping.", *(format - 1));
								len = 0;
								break;
						}
						if (doSave)
						{
							cptr += len;
						}
					}
					break;
				default:
					SscanfWarning("Unknown format specifier '%c', skipping.", *(format - 1));
					continue;
			}
			// Loop cleanup - only skip one spacer so that we can detect
			// multiple explicit delimiters in a row, for example:
			// 
			// hi     there
			// 
			// is NOT multiple explicit delimiters in a row (they're
			// whitespace).  This however is:
			// 
			// hi , , , there
			// 
			SkipOneSpacer(&string);
		}
	}
	// Save the end of the string.
	*input = string;
	if (*format)
	{
		if (*format == '}' && IsEnd(*(format + 1)))
		{
			return SSCANF_TRUE_RETURN;
		}
		// Ran out of values - check if this is an optional array at a higher
		// level.
		return SSCANF_CONT_RETURN;
	}
	return SSCANF_TRUE_RETURN;
}

bool
	DoE(char ** defaults, char ** input, struct args_s & args, bool optional, bool doSave)
{
	// First, get the type of the array.
	char *
		type = GetMultiType(defaults);
	cell *
		cptr = NULL;
	args.Mark();
	if (!type)
	{
		return false;
	}
	if (optional)
	{
		// Optional parameters - just collect the data for
		// now and use it later.
		if (**defaults == '(')
		{
			++(*defaults);
			SkipWhitespace(defaults);
			// Got the start of the values.
			char *
				opts = *defaults;
			// Skip the defaults for now, we don't know the length yet.
			bool
				escape = false;
			while (**defaults && (escape || **defaults != ')'))
			{
				if (**defaults == '\\')
				{
					escape = !escape;
				}
				else
				{
					escape = false;
				}
				++(*defaults);
			}
			if (**defaults)
			{
				if (opts == *defaults)
				{
					// No defaults found.
					SscanfWarning("Empty default values.");
					optional = false;
				}
				// Found a valid end.  Make it null for
				// later array getting (easy to detect a
				// null end and we'll never need to
				// backtrack to here in the specifiers).
				**defaults = '\0';
				++(*defaults);
			}
			else
			{
				SscanfWarning("Unclosed default value.");
			}
			if (optional)
			{
				// Optional parameters are always separated by commans, not
				// whatever the coder may choose.
				TempDelimiter(",)");
				// We need to copy the old save value for optional parts.  If
				// we don't and save gets set to false in the middle of the
				// enum then when the code is called for a second time for the
				// real values then save will already be false and they won't
				// get saved.
				if (doSave)
				{
					char *
						tmp = opts;
					DoEnumValues(type, &tmp, NULL, true, args);
					cptr = args.Next();
					args.Restore();
				}
				// Do this twice.  Once to get the lengths, once for the data.
				switch (DoEnumValues(type, &opts, cptr, true, args))
				{
					case SSCANF_TRUE_RETURN:
						break;
					case SSCANF_CONT_RETURN:
						SscanfError("Insufficient default values.");
						// FALLTHROUGH
					default:
						RestoreDelimiter();
						return false;
				}
				RestoreDelimiter();
				if (cptr)
					args.Next();
			}
		}
		else
		{
			SscanfWarning("No default value found.");
			optional = false;
		}
	}
	if (input)
	{
		if (doSave && !cptr)
		{
			args.Mark();
			char *
				tmp = *input;
			DoEnumValues(type, &tmp, NULL, false, args);
			cptr = args.Next();
		}
		args.Restore();
		switch (DoEnumValues(type, input, cptr, false, args))
		{
			case SSCANF_TRUE_RETURN:
				if (cptr)
					args.Next();
				return true;
			case SSCANF_CONT_RETURN:
				if (optional)
				{
					if (cptr)
						args.Next();
					return true;
				}
				// FALLTHROUGH
			default:
				return false;
		}
	}
	return true;
}
