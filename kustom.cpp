// Kustom

#include "SDK/amx/amx.h"

#include "sscanf.h"
#include "utils.h"
#include "data.h"
#include "specifiers.h"

#include <stdio.h>

extern logprintf_t
	logprintf;

extern int
	gOptions;

bool
	DoK(AMX * amx, char ** defaults, char ** input, cell * cptr, bool optional, bool all)
{
	// First, get the type of the array.
	char *
		type = GetMultiType(defaults);
	if (!type)
	{
		return false;
	}
	if (optional)
	{
		// Optional parameters - just collect the data for
		// now and use it later.
		char *
			opts = 0;
		if (**defaults == '(')
		{
			++(*defaults);
			SkipWhitespace(defaults);
			// Got the start of the values.
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
					logprintf("sscanf warning: Empty default values.");
					opts = 0;
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
				logprintf("sscanf warning: Unclosed default value.");
			}
			/*if (optional)
			{
				// Optional parameters are always separated by commans, not
				// whatever the coder may choose.
				TempDelimiter(",)");
				// We need to copy the old save value for optional parts.  If
				// we don't and save gets set to false in the middle of the
				// enum then when the code is called for a second time for the
				// real values then save will already be false and they won't
				// get saved.
				switch (DoEnumValues(type, &opts, cptr, true))
				{
					case SSCANF_TRUE_RETURN:
						break;
					case SSCANF_CONT_RETURN:
						logprintf("sscanf error: Insufficient default values.");
						// FALLTHROUGH
					default:
						RestoreDelimiter();
						return false;
				}
				RestoreDelimiter();
			}*/
		}
		else
		{
			logprintf("sscanf warning: No default value found.");
		}
		if (!input || IsStringEnd(**input))
		{
			if (cptr)
			{
				if (opts)
				{
					if (gOptions & 16)
					{
						char
							func[32];
						int
							idx;
						sprintf(func, "sscanf_%s", type);
						if (amx_FindPublic(amx, func, &idx))
						{
							logprintf("sscanf warning: Could not find function SSCANF:%s.", type);
						}
						else
						{
							cell
								ret,
								addr;
							amx_PushString(amx, &addr, 0, opts, 0, 0);
							amx_Exec(amx, &ret, idx);
							amx_Release(amx, addr);
							*cptr = ret;
							return true;
						}
					}
					else
					{
						*cptr = (cell)GetNumber(&opts);
						return true;
					}
				}
				*cptr = 0;
			}
			return true;
		}
	}
	else if (!input || IsStringEnd(**input))
	{
		return false;
	}
	char
		func[32];
	int
		idx;
	sprintf(func, "sscanf_%s", type);
	if (amx_FindPublic(amx, func, &idx))
	{
		logprintf("sscanf warning: Could not find function SSCANF:%s.", type);
	}
	else
	{
		char
			* string = *input,
			* outp = string,
			* start = string;
		if (IsDefaultDelimiter())
		{
			while ((all && !IsEnd(*string)) || !IsWhitespace(*string))
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
				++string;
			}
		}
		else
		{
			// Just a single word.  Note that if your delimiter is a backslash
			// you can't escape it - this is not a bug, just don't try use it as
			// a delimiter and still expect to be able to use it in a string.  I
			// suppose that technically you could see this as a bug, but I
			// choose to call it an undesirable feature (no-one has complained).
			while (!IsEnd(*string) && (all || !IsDelimiter(*string)))
			{
				if (*string == '\\')
				{
					if (IsEnd(*(string + 1)))
					{
						++string;
						break;
					}
					// Escape spaces, backspace and delimiters - this code
					// is context independent so you can use a string with
					// or without a delimiter and can still escape spaces.
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
		cell
			ret,
			addr;
		amx_PushString(amx, &addr, 0, start, 0, 0);
		amx_Exec(amx, &ret, idx);
		amx_Release(amx, addr);
		if (cptr) *cptr = ret;
	}
	return true;
}
