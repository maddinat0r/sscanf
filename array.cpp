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

#include <stdio.h>

#include "sscanf.h"
#include "args.h"
#include "utils.h"
#include "data.h"
#include "specifiers.h"

extern logprintf_t
	logprintf;

#define GET_CPTR()                      \
	if (doSave)                         \
		cptr = args.Next()

#define SAVE_VALUE(m)                   \
	if (doSave)                         \
		*cptr++ = m

#define SAVE_VALUE_F(m)                 \
	if (doSave) {                       \
		float f = (float)m;             \
		*cptr++ = amx_ftoc(f); }

#define UPDATE_VALUES(m)                \
	if (defaults) {                     \
		if (count) {                    \
			diff.c = m - last.c;        \
			last.c = m;                 \
		} else {                        \
			last.c = m;                 \
			diff.c = 0; } }             \
	SkipOneSpacer(&string);             \
	++count

#define UPDATE_VALUES_F(m)              \
	if (defaults) {                     \
		if (count) {                    \
			diff.d = m - last.d;        \
			last.d = m;                 \
		} else {                        \
			last.d = m;                 \
			diff.d = 0; } }             \
	SkipOneSpacer(&string);             \
	++count

#define DO_LOOP(n)                                 \
	count < length && *string && Do##n(&string, &b)

// Macros for the regular values.
#define DO(m,n)                         \
	{ m b; GET_CPTR();                  \
	while (DO_LOOP(n)) {                \
		SAVE_VALUE((cell)b);            \
		UPDATE_VALUES((cell)b); } }     \
	break;

#define DOV(m,n)                        \
	{ m b; GET_CPTR();                  \
	while (count < length && *string) { \
		Do##n(&string, &b);             \
		SAVE_VALUE((cell)b);            \
		UPDATE_VALUES((cell)b); } }     \
	break;

#define DOF(m,n)                        \
	{ m b; GET_CPTR();                  \
	while (DO_LOOP(n)) {                \
		SAVE_VALUE_F(b)                 \
		UPDATE_VALUES_F(b); } }         \
	break;

#define OPTIONAL_INVALID \
	SscanfWarning("Optional types invalid in array specifiers, consider using 'A'.")

#define DX(m,n) \
	OPTIONAL_INVALID;

#define DXF(m,n) \
	OPTIONAL_INVALID;

void
	GetJaggedSlot(cell *, int, int, int, cell **, int *);

bool
	DoK(AMX * amx, char ** defaults, char ** input, cell * cptr, bool optional, bool all);

extern AMX *
	g_aCurAMX;

extern int
	gOptions;

union update_u
{
	cell
		c;
	double
		d;
};

int
	DoArrayValues(char * type, char ** input, struct args_s & args, int length, bool defaults, bool wholeString, bool doSave)
{
	int
		count = 0;
	union update_u
		last,
		diff;
	last.d = 0;
	diff.d = 0;
	char *
		string = *input;
	cell *
		cptr = NULL;
	switch (*type++)
	{
		// Copied directly from the main loop, just with different macros.
		case 'L':
			DX(bool, L)
			// FALLTHROUGH
		case 'l':
			{
				// The only custom code here.  If the result is either "true"
				// or "false" it makes no sense to alternate in defaults as we
				// will just end up with larger values of true.
				bool
					b;
				GET_CPTR();
				while (count < length && *string)
				{
					DoL(&string, &b);
					SAVE_VALUE((cell)b);
					if (defaults)
					{
						last.c = (cell)b;
					}
					SkipOneSpacer(&string);
					++count;
				}
			}
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
			{
				double
					b;
				GET_CPTR();
				while (DO_LOOP(G))
				{
					float
						f = (float)b;
					if (doSave)
					{
						*cptr++ = amx_ftoc(f);
					}
					if (defaults)
					{
						switch (amx_ftoc(f))
						{
							default:
								if (count)
								{
									diff.d = b - last.d;
									last.d = b;
									break;
								}
								// FALLTHROUGH
							case FLOAT_NEG_INFINITY:
							case FLOAT_INFINITY:
							case FLOAT_NAN:
							case FLOAT_NAN_E:
								last.d = b;
								diff.d = 0;
								break;
						}
					}
					SkipOneSpacer(&string);
					++count;
				}
			}
			break;
		case 'U':
			DX(int, U)
			// FALLTHROUGH
		case 'u':
			if (*type == '[')
			{
				SscanfWarning("User arrays are not supported in arrays.");
			}
			if (defaults)
			{
				GET_CPTR();
				if (gOptions & 1)
				{
					int
						b;
					while (count < length && *string)
					{
						DoU(&string, &b, 0);
						SAVE_VALUE((cell)b);
						last.c = b;
						diff.c = 0;
						SkipOneSpacer(&string);
						++count;
					}
				}
				else
				{
					int
						b;
					while (count < length && *string)
					{
						DoI(&string, &b);
						SAVE_VALUE((cell)b);
						last.c = b;
						diff.c = 0;
						SkipOneSpacer(&string);
						++count;
					}
				}
			}
			else
			#define DoU(m,n) DoU(m,n,0)
			{
				DOV(int, U)
			}
			#undef DoU
			break;
		case 'Q':
			DX(int, Q)
			// FALLTHROUGH
		case 'q':
			if (*type == '[')
			{
				SscanfWarning("User arrays are not supported in arrays.");
			}
			if (defaults)
			{
				GET_CPTR();
				if (gOptions & 1)
				{
					int
						b;
					while (count < length && *string)
					{
						DoQ(&string, &b, 0);
						SAVE_VALUE((cell)b);
						last.c = b;
						diff.c = 0;
						SkipOneSpacer(&string);
						++count;
					}
				}
				else
				{
					int
						b;
					while (count < length && *string)
					{
						DoI(&string, &b);
						SAVE_VALUE((cell)b);
						last.c = b;
						diff.c = 0;
						SkipOneSpacer(&string);
						++count;
					}
				}
			}
			else
			#define DoQ(m,n) DoQ(m,n,0)
			{
				DOV(int, Q)
			}
			#undef DoQ
			break;
		case 'R':
			DX(int, R)
			// FALLTHROUGH
		case 'r':
			if (*type == '[')
			{
				SscanfWarning("User arrays are not supported in arrays.");
			}
			if (defaults)
			{
				GET_CPTR();
				if (gOptions & 1)
				{
					int
						b;
					while (count < length && *string)
					{
						DoR(&string, &b, 0);
						SAVE_VALUE((cell)b);
						last.c = b;
						diff.c = 0;
						SkipOneSpacer(&string);
						++count;
					}
				}
				else
				{
					int
						b;
					while (count < length && *string)
					{
						DoI(&string, &b);
						SAVE_VALUE((cell)b);
						last.c = b;
						diff.c = 0;
						SkipOneSpacer(&string);
						++count;
					}
				}
			}
			else
			#define DoR(m,n) DoR(m,n,0)
			{
				DOV(int, R)
			}
			#undef DoR
			break;
		case 'A':
		case 'a':
			SscanfError("Multi-dimensional arrays are not supported.");
			return SSCANF_FAIL_RETURN;
		case '\'':
			SscanfError("Search strings are not supported in arrays.");
			return SSCANF_FAIL_RETURN;
		case 'P':
		case 'p':
			SscanfError("Delimiters are not supported in arrays.");
			return SSCANF_FAIL_RETURN;
		case '?':
			SscanfError("Options are not supported in arrays.");
			return SSCANF_FAIL_RETURN;
		case 'k':
			if (defaults)
			{
				GET_CPTR();
				if (DoK(g_aCurAMX, &type, &string, cptr, false, true) && doSave)
				{
					while (++count < length)
					{
						*(cptr + 1) = *cptr;
						++cptr;
					}
				}
			}
			else
			{
				char *
					f = type;
				GET_CPTR();
				while (count < length && *string && DoK(g_aCurAMX, &f, &string, cptr, false, wholeString && count == length - 1))
				{
					if (doSave) ++cptr;
					SkipOneSpacer(&string);
					++count;
					*(f - 1) = '>';
					f = type;
				}
			}
			break;
		case 's':
			//SscanfError("Strings are not supported in arrays.");
			// Now they are (or rather, now I would like them to be, I've not
			// actually WRITTEN the code yet...).
			// This code has to read the memory pointed to by "cptr", which for
			// a multi-dimensional array points to the array header and from
			// which we can actually read the array lengths.  Note that this
			// actually means that we don't need to include the string length in
			// the array specifier, and can thus support jagged arrays!
			if (defaults)
			{
				int
					nl = GetLength(&type, args),
					sl;
				char *
					dest;
				// Parse the default string.
				DoS(&string, &dest, 0x7FFFFFFF, true);
				GET_CPTR();
				if (doSave)
				{
					cell *
						sptr;
					// Send the string to PAWN.
					while (count < length)
					{
						// Get the true address as offset from the array
						// base address, and its length.
						GetJaggedSlot(cptr, length, nl, count, &sptr, &sl);
						amx_SetString(sptr, dest, 0, 0, sl);
						++count;
					}
				}
				break;
			}
			else
			{
				// Get the length.
				int
					nl = GetLength(&type, args),
					sl;
				char *
					dest;
				GET_CPTR();
				while (count < length && *string)
				{
					if (doSave)
					{
						cell *
							sptr;
						// Get the true address as offset from the array
						// base address, and its length.
						GetJaggedSlot(cptr, length, nl, count, &sptr, &sl);
						//printf("%d %d\n", nl, sl);
						//printf("%d %d %d", cptr, sptr, sl);
						DoS(&string, &dest, sl, wholeString && count == length - 1);
						amx_SetString(sptr, dest, 0, 0, sl);
					}
					else
					{
						DoS(&string, &dest, 0x7FFFFFFF, wholeString && count == length - 1);
					}
					SkipOneSpacer(&string);
					++count;
				}
				break;
			}
			//return SSCANF_FAIL_RETURN;
		case 'z':
			//SscanfError("Strings are not supported in arrays.");
			// Now they are (or rather, now I would like them to be, I've not
			// actually WRITTEN the code yet...).
			// This code has to read the memory pointed to by "cptr", which for
			// a multi-dimensional array points to the array header and from
			// which we can actually read the array lengths.  Note that this
			// actually means that we don't need to include the string length in
			// the array specifier, and can thus support jagged arrays!
			if (defaults)
			{
				int
					nl = GetLength(&type, args),
					sl;
				char *
					dest;
				// Parse the default string.
				DoS(&string, &dest, 0x7FFFFFFF, true);
				GET_CPTR();
				if (doSave)
				{
					cell *
						sptr;
					// Send the string to PAWN.
					while (count < length)
					{
						// Get the true address as offset from the array
						// base address, and its length.
						GetJaggedSlot(cptr, length, nl, count, &sptr, &sl);
						amx_SetString(sptr, dest, 1, 0, sl);
						++count;
					}
				}
				break;
			}
			else
			{
				// Get the length.
				int
					nl = GetLength(&type, args),
					sl;
				char *
					dest;
				GET_CPTR();
				while (count < length && *string)
				{
					if (doSave)
					{
						cell *
							sptr;
						// Get the true address as offset from the array
						// base address, and its length.
						GetJaggedSlot(cptr, length, nl, count, &sptr, &sl);
						//printf("%d %d\n", nl, sl);
						//printf("%d %d %d", cptr, sptr, sl);
						DoS(&string, &dest, sl, wholeString && count == length - 1);
						amx_SetString(sptr, dest, 1, 0, sl);
					}
					else
					{
						DoS(&string, &dest, 0x7FFFFFFF, wholeString && count == length - 1);
					}
					SkipOneSpacer(&string);
					++count;
				}
				break;
			}
			//return SSCANF_FAIL_RETURN;
		case '{':
		case '}':
			SscanfError("Quiet sections are not supported in arrays.");
			return SSCANF_FAIL_RETURN;
		default:
			SscanfError("Unknown format specifier '%c'.", *(type - 1));
			return SSCANF_FAIL_RETURN;
	}
	// Save the end of the string.
	*input = string;
	if (defaults)
	{
		if (count < length)
		{
			if (*string)
			{
				// Error in format specifier.
				SscanfWarning("Invalid values in array defaults.");
			}
			else
			{
				switch (*(type - 1))
				{
					case 'F':
					case 'f':
					case 'G':
					case 'g':
						// Float type.
						do
						{
							last.d += diff.d;
							SAVE_VALUE_F(last.d);
							++count;
						}
						while (count < length);
						break;
					case 'K':
					case 'k':
					case 's':
					case 'z':
						// There is no "progression" in optional strings - you
						// specify one and JUST one!
						break;
					default:
						do
						{
							// Other type.
							while (count < length)
							{
								last.c += diff.c;
								SAVE_VALUE(last.c);
								++count;
							}
						}
						while (count < length);
						break;
				}
			}
		}
		else if (*string)
		{
			SscanfWarning("Excess array defaults found.");
		}
	}
	else
	{
		if (count < length)
		{
			if (*string)
			{
				// Invalid value entered.
				return SSCANF_FAIL_RETURN;
			}
			else
			{
				// Ran out of values - check if this is an optional array at a
				// higher level.
				return SSCANF_CONT_RETURN;
			}
		}
	}
	return SSCANF_TRUE_RETURN;
}

bool
	DoA(char ** defaults, char ** input, struct args_s & args, bool optional, bool doSave)
{
	// First, get the type of the array.
	bool
		escape = false;
	char
		* type = GetMultiType(defaults),
		* opts = 0;
	if (!type) return false;
	switch (*type)
	{
		case 'S':
			OPTIONAL_INVALID;
			*type = 's';
			break;
		case 'Z':
			OPTIONAL_INVALID;
			*type = 'z';
			break;
		case 'K':
			OPTIONAL_INVALID;
			*type = 'k';
			break;
	}
	if (optional)
	{
		// Optional parameters - just collect the data for
		// now and use it later.
		if (**defaults == '(')
		{
			// Find the end of the options.  Based on
			// SkipDefaultEx, but saving the start point.
			++(*defaults);
			SkipWhitespace(defaults);
			// Got the start of the values.
			opts = *defaults;
			// Skip the defaults for now, we don't know the length yet.
			//while (**defaults) // && **defaults != ')')
			for ( ; ; )
			{
				switch (**defaults)
				{
					case '\0':
						goto DoA_after_loop;
					case '\\':
						escape = !escape;
						break;
					case ')':
						if (!escape)
						{
							// End the loop after ")" if it isn't escaped.
							goto DoA_after_loop;
						}
						// FALLTHROUGH.
					default:
						escape = false;
						break;
				}
				// Don't need to check for escaped ")"s here as you can't have
				// arrays of strings, which is the only place that would be.
				// Only now we do...
				++(*defaults);
			}
DoA_after_loop:
			if (**defaults)
			{
				if (opts == *defaults && !(*type == 's' || *type == 'z' || *type == 'k'))
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
		}
		else
		{
			SscanfWarning("No default value found.");
			optional = false;
		}
	}
	int
		length = GetLength(defaults, args);
	// We MAY need to go over the arguments twice.  Once to insert defaults, once for real values.
	// If we mark here and restore at the start of normal data it doesn't matter if we got defaults
	// or not - if so this is correct, if not the mark/restore won't change anything.
	args.Mark();
	if (length)
	{
		// Got the length of the array, it's all good.
		if (optional)
		{
			// Optional parameters are always separated by commans, not
			// whatever the coder may choose.
			if ((*type == 's' || *type == 'z' || *type == 'k')) TempDelimiter(")");
			else TempDelimiter(",)");
			if (DoArrayValues(type, &opts, args, length, true, false, doSave) == SSCANF_FAIL_RETURN)
			{
				RestoreDelimiter();
				return false;
			}
			RestoreDelimiter();
		}
		if (input)
		{
			args.Restore();
			switch (DoArrayValues(type, input, args, length, false, (*type == 's' || *type == 'z' || *type == 'k') && (IsEnd(**defaults) || (!doSave && **defaults == '}' && IsEnd(*(*defaults + 1)))), doSave))
			{
				case SSCANF_CONT_RETURN:
					if (optional)
					{
						return true;
					}
					else
					{
						return false;
					}
				case SSCANF_TRUE_RETURN:
					return true;
				default:
					return false;
			}
		}
		else
		{
			return true;
		}
	}
	// Default fail - only gets here if there is no length.
	return false;
}
