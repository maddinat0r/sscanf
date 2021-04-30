# sscanf 2.10.3

## NPC modes

To use sscanf in an NPC mode, download this file:

http://dl.dropbox.com/u/21683085/npcdll.rar

And extract it to your root server directory (so `amxsscanf.dll` is in the same directory as `samp-npc.exe`). Then use as normal. The only tiny difference is that `u`, `r`, and `q` don't know if a user is a bot or not, so just assume they are all players - use accordingly.

## Downloads

GitHub repo:

https://github.com/Y-Less/sscanf/

## Use

This behaves exactly as the old sscanf did, just MUCH faster and much more flexibly. To use it add:

```pawn
#include <sscanf2>
```

To your modes and remove the old sscanf (the new include will detect the old version and throw an error if it is detected). On Windows add:

```pawn
plugins sscanf
```

To `server.cfg`. On Linux add:

```pawn
plugins sscanf.so
```

The basic code looks like:

```pawn
if (sscanf(params, "ui", giveplayerid, amount))
{
    return SendClientMessage(playerid, 0xFF0000AA, "Usage: /givecash <playerid/name> <amount>");
}
```

However it should be noted that sscanf can be used for any text processing you like. For example an ini processor could look like (don't worry about what the bits mean at this stage):

```pawn
if (sscanf(szFileLine, "p<=>s[8]s[32]", szIniName, szIniValue))
{
    printf("Invalid INI format line");
}
```

There is also an alternate function name to avoid confusion with the C standard sscanf:

```pawn
if (unformat(params, "ui", giveplayerid, amount))
{
    return SendClientMessage(playerid, 0xFF0000AA, "Usage: /givecash <playerid/name> <amount>");
}
```

## Specifiers

The basic specifiers (the letters `u`, `i`, `s` etc. in the codes above) here.  There are more advanced ones in a later table.

|  Specifier(s)  |               Name                |                      Example values                       |
| -------------- | --------------------------------- | --------------------------------------------------------- |
|  `i`, `d`      |  Integer                          |  `1`, `42`, `-10`                                         |
|  `c`           |  Character                        |  `a`, `o`, `*`                                            |
|  `l`           |  Logical                          |  `true`, `false`                                          |
|  `b`           |  Binary                           |  `01001`, `0b1100`                                        |
|  `h`, `x`      |  Hex                              |  `1A`, `0x23`                                             |
|  `o`           |  Octal                            |  `045`, `12`                                              |
|  `n`           |  Number                           |  `42`, `0b010`, `0xAC`, `045`                             |
|  `f`           |  Float                            |  `0.7`, `-99.5`                                           |
|  `g`           |  IEEE Float                       |  `0.7`, `-99.5`, `INFINITY`, `-INFINITY`, `NAN`, `NAN_E`  |
|  `u`           |  User name/id (bots and players)  |  `Y_Less`, `0`                                            |
|  `q`           |  Bot name/id                      |  `ShopBot`, `27`                                          |
|  `r`           |  Player name/id                   |  `Y_Less`, `42`                                           |
|  `m`           |  Colour                           |  `{FF00AA}`, `0xFFFFFFFF`, `444`                          |

### Strings

The specifier `s` is used, as before, for strings - but they are now more advanced. As before they support collection, so doing:

```pawn
sscanf("hello 27", "si", str, val);
```

Will give:

```
hello
27
```

Doing:

```pawn
sscanf("hello there 27", "si", str, val);
```

Will fail as `there` is not a number. However doing:

```pawn
sscanf("hello there", "s", str);
```

Will give:

```
hello there
```

Because there is nothing after `s` in the specifier, the string gets everything. To stop this simply add a space:

```pawn
sscanf("hello there", "s ", str);
```

Will give:

```
hello
```

You can also escape parts of strings with `\\` - note that this is two backslashes as 1 is used by the compiler:

```pawn
sscanf("hello\\ there 27", "si", str, val);
```

Will give:

```
hello there
27
```

All these examples however will give warnings in the server as the new version has array sizes. The above code should be:

```pawn
new
    str[32],
    val;
sscanf("hello\\ there 27", "s[32]i", str, val);
```

As you can see - the format specifier now contains the length of the target string, ensuring that you can never have your strings overflow and cause problems. This can be combined with the SA:MP compiler's stringizing:

```pawn
#define STR_SIZE 32
new
    str[STR_SIZE],
    val;
sscanf("hello\\ there 27", "s[" #STR_SIZE "]i", str, val);
```

Or better yet, you can now use `[*]` to pass a string length as an additional parameter (see "Provided Lengths" below).

So when you change your string size you don't need to change your specifiers.

### Packed Strings

`z` and `Z` return packed strings.  They are otherwise identical to `s` and `S`, so see the `Strings` documentation above for more details.

### Arrays

One of the advanced new specifiers is `a`, which creates an array, obviously. The syntax is similar to that of strings and, as you will see later, the delimiter code:

```pawn
new
    arr[5];
sscanf("1 2 3 4 5", "a<i>[5]", arr);
```

The `a` specifier is immediately followed by a single type enclosed in angle brackets - this type can be any of the basic types listed above. It is the followed, as with strings now, by an array size. The code above will put the numbers 1 to 5 into the 5 indexes of the `arr` array variable.

Arrays can now also be combined with strings (see below), specifying the string size in the array type:

```
a<s[10]>[12]
```

This will produce an array of 12 strings, each up to 10 characters long (9 + NULL). Optional string arrays still follow the optional array syntax:

```
A<s[10]>(hello)[12]
```

However, unlike numbers you can't specify a progression and have it fill up. This code:

```
A<i>(0, 1)[4]
```

Will by default produce:

```
0, 1, 2, 3
```

However, this code:

```
A<s[10]>(hi, there)[4]
```

Will by default produce:

```
"hi, there", "hi, there", "hi, there", "hi, there"
```

As normal, you can add brackets in to the default string value with `\)`:

```
A<s[10]>(hi (code\))[4]
```

It should also be noted that there is NO length checking on default strings. If you do:

```
A<s[10]>(This is longer than 10 characters)[4]
```

You will probably just corrupt the PAWN stack. The length checking is to ensure no users enter malicious data; however, in this case it is up to the scripter to ensure that the data is correct as they are the only one affecting it and shouldn't be trying to crash their own server. Interestingly, arrays of strings actually also work with jagged arrays and arrays that have been shuffled by Slice's quicksort function (this isn't a side-effect, I specifically wrote them to do so).

### Enums

This is possibly the most powerful addition to sscanf ever. This gives you the ability to define the structure of an enum within your specifier string and read any data straight into it. The format takes after that of arrays, but with more types - and you can include strings in enums (but not other enums or arrays):

```pawn
enum
    E_DATA
{
    E_DATA_C,
    Float:E_DATA_X,
    E_DATA_NAME[32],
    E_DATA_Z
}

main
{
    new
        var[E_DATA];
    sscanf("1 12.0 Bob c", "e<ifs[32]c>", var);
}
```

Now I'll be impressed if you can read that code straight off, so I'll explain it slowly:

```pawn
e - Start of the `enum` type
< - Starts the specification of the structure of the enum
i - An integer, corresponds with E_DATA_C
f - A float, corresponds with E_DATA_X
s[32] - A 32 cell string, corresponds with E_DATA_NAME
c - A character, corresponds with E_DATA_Z
> - End of the enum specification
```

Note that an enum doesn't require a size like arrays and strings - it's size is determined by the number and size of the types. Most, but not all, specifiers can be used inside enums (notably arrays and other enums can't be).

### Provided Lengths

Both strings and arrays take a length, normally specified in the string with (say) `s[32]`.  However, this system has some extreme limitations - most notably macros.  This code will not work:

```pawn
#define LEN 32
sscanf(params, "s[LEN]", str);
```

This code will work:

```pawn
#define LEN 32
sscanf(params, "s["#LEN"]", str);
```

But this code won't even compile:

```pawn
#define LEN (32)
sscanf(params, "s["#LEN"]", str);
```

This code will work, but is a bit awkward:

```pawn
sscanf(params, "s[(32)]", str);
```

This code will compile, but then also won't work:

```pawn
#define LEN 8*4
sscanf(params, "s["#LEN"]", str);
```

For this reason you can pass string and array lengths as additional parameters using `*` for the length:

```pawn
#define LEN 8*4
sscanf(params, "s[*]", LEN, str);
```

The lengths appear BEFORE the destination, arrays first then strings:

```pawn
new int, arr[5][10], str[32];
sscanf(params, "ia<s[*]>[*]s[*]", int, sizeof (arr), sizeof (arr[]), arr, 32, str);
```

The same applies to strings in enums:

```pawn
enum E_EXAMPLE
{
	Float:FLOAT,
	STR_1[32],
	STR_2[64],
	INT,
}

new dest[E_EXAMPLE];
sscanf(params, "e<fs[*]s[*]i>", _:STR_1 - _:FLOAT, 64, dest);
```

And to arrays of users (see below):

```pawn
new ids[3], i;
sscanf(params, "u[*]", sizeof (ids), ids);
```

This allows you to pass variable lengths if you don't want to use all of a string, and use the full power of the pre-processor to generate lengths at compile-time.  It also bypasses the compiler stringise bug with brackets in strings.  `extract` (see below) now uses `*` for all strings and arrays as well, so will similarly fully use the pre-processor.

### Quiet

The two new specifiers `{` and `}` are used for what are known as `quiet` strings. These are strings which are read and checked, but not saved. For example:

```pawn
sscanf("42 -100", "{i}i", var);
```

Clearly there are two numbers and two `i`, but only one return variable. This is because the first `i` is quiet so is not saved, but affects the return value. The code above makes `var` `-100`. The code below will fail in an if check:

```pawn
sscanf("hi -100", "{i}i", var);
```

Although the first integer is not saved it is still read - and `hi` is not an integer. Quiet zones can be as long as you like, even for the whole string if you only want to check values are right, not save them:

```pawn
sscanf("1 2 3", "i{ii}", var);
sscanf("1 2 3", "{iii}");
sscanf("1 2 3", "i{a<i>[2]}", var);
```

You can also embed quiet sections inside enum specifications:

```pawn
sscanf("1 12.0 Bob 42 INFINITY c", "e<ifs[32]{ig}c>", var);
```

Quiet sections cannot contain other quiet sections, however they can include enums which contain quiet sections.

### Searches

Searches were in the last version of sscanf too, but I'm explaining them again anyway. Strings enclosed in single quotes (') are scanned for in the main string and the position moved on. Note that to search for a single quote you escape it as above using `\\`:

```pawn
sscanf("10 11 woo 12", "i'woo'i", var0, var1);
```

Gives:

```
10
12
```

You could achieve the same effect with:

```pawn
sscanf("10 11 woo 12", "i{is[1000]}i", var0, var1);
```

But that wouldn't check that the string was `woo`. Also note the use of `1000` for the string size. Quiet strings must still have a length, but as they aren't saved anywhere you can make this number as large as you like to cover any eventuality. Enum specifications can include search strings.

### Enums

This is a feature similar to quiet sections, which allows you to skip overwriting certain parts of an enum:

```
e<ii-i-ii>
```

Here the `-` is a `minus`, and tells sscanf that there is an enum element there, but not to do anything, so if you had:

```pawn
enum E
{
    E_A,
    E_B,
    E_C,
    E_D,
    E_E
}
```

And you only wanted to update the first two and the last fields and leave all others untouched you could use that specifier above. This way sscanf knows how to skip over the memory, and how much memory to skip. Note that this doesn't read anything, so you could also combine this with quiet sections:

```
e<ii-i-i{ii}i>
```

That will read two values and save them, skip over two memory locations, read two values and NOT save them, then read and save a last value. In this way you can have written down all the values for every slot in the enum, but have only used 3 of them. Note that this is the same with `E` - if you do:

```
E<ii-i-ii>
```

You should ONLY specify THREE defaults, not all five:

```
E<ii-i-ii>(11, 22, 55)
```

### Delimiters

The previous version of sscanf had `p` to change the symbol used to separate tokens. This specifier still exists but it has been formalised to match the array and enum syntax. What was previously:

```pawn
sscanf("1,2,3", "p,iii", var0, var1, var2);
```

Is now:

```pawn
sscanf("1,2,3", "p<,>iii", var0, var1, var2);
```

The old version will still work, but it will give a warning. Enum specifications can include delimiters, and is the only time `<>`s are contained in other `<>`s:

```pawn
sscanf("1 12.0 Bob,c", "e<ifp<,>s[32]c>", var);
```

Note that the delimiter will remain in effect after the enum is complete. You can even use `>` as a specifier by doing `p<\>>` (or the older `p>`).

When used with strings, the collection behaviour is overruled. Most specifiers are still space delimited, so for example this will work:

```pawn
sscanf("1 2 3", "p<;>iii", var0, var1, var2);
```

Despite the fact that there are no `;`s. However, strings will ONLY use the specified delimiters, so:

```pawn
sscanf("hello 1", "p<->s[32]i", str, var);
```

Will NOT work - the variable `str` will contain `hello 1`. On the other hand, the example from earlier, slightly modified:

```pawn
sscanf("hello there>27", "p<>>s[32]i", str, var);
```

WILL work and will give an output of:

```
hello there
27
```

You can now have optional delimiters using `P` (upper case `p` to match other `optional` specifiers). These are optional in the sense that you specify multiple delimiters and any one of them can be used to end the next symbol:

```pawn
sscanf("(4, 5, 6, 7)", "P<(),>{s[2]}iiii", a, b, c, d);
```

This uses a `quiet section` to ignore anything before the first `(`, and then uses multiple delimiters to end all the text. Example:

```pawn
sscanf("42, 43; 44@", "P<,;@>a<i>[3]", arr);
```

### Optional specifiers

EVERY format specifier (that is, everything except `''`, `{}` and `p`) now has an optional equivalent - this is just their letter capitalised. In addition to optional specifiers, there are also now default values:

```pawn
sscanf("", "I(12)", var);
```

The `()`s (round brackets) contain the default value for the optional integer and, as the main string has no data, the value of `var` becomes `12`. Default values come before array sizes and after specifications, so an optional array would look like:

```pawn
sscanf("1 2", "A<i>(3)[4]", arr);
```

Note that the size of the array is `4` and the default value is `3`. There are also two values which are defined, so the final value of `arr` is:

```
1, 2, 3, 3
```

Array default values are clever, the final value of:

```pawn
sscanf("", "A<i>(3,6)[4]", arr);
```

Will be:

```
3, 6, 9, 12
```

The difference between `3` and `6` is `3`, so the values increase by that every index. Note that it is not very clever, so:

```pawn
sscanf("", "A<i>(1,2,2)[4]", arr);
```

Will produce:

```
1, 2, 2, 2
```

The difference between `2` and `2` (the last 2 numbers in the default) is 0, so there will be no further increase. For `l` (logical) arrays, the value is always the same as the last value, as it is with `g` if the last value is one of the special values (INFINITY, NEG_INFINITY (same as -INFINITY), NAN or NAN_E). Note that:

```pawn
sscanf("", "a<I>(1,2,2)[4]", arr);
```

Is invalid syntax, the `A` must be the capital part.

Enums can also be optional:

```pawn
sscanf("4", "E<ifs[32]c>(1, 12.0, Bob, c)", var);
```

In that code all values except `4` will be default. Also, again, you can escape commas with `\\` in default enum strings. Some final examples:

```pawn
sscanf("1", "I(2)I(3)I(4)", var0, var1, var2);
sscanf("", "O(045)H(0xF4)B(0b0100)U(Y_Less)", octnum, hexnum, binnum, user);
sscanf("0xFF", "N(0b101)");
```

That last example is of a specifier not too well described yet - the `number` specifier, which will work out the format of the number from the leading characters (0x, 0b, 0 or nothing). Also note that the second example has changed - see the next section.

### Users

The `u`, `q`, and `r` specifiers search for a user by name or ID. The method of this search has changed in the latest versions of `sscanf`.

Additionally `U`, `Q`, and `R` used to take a name or ID as their default value - this has since been changed to JUST a number, and sscanf will not try and determine if this number is online:

Previous:

```pawn
sscanf(params, "U(Y_Less)", id);
if (id == INVALID_PLAYER_ID)
{
    // Y_Less or the entered player is not connected.
}
```

New:

```pawn
sscanf(params, "U(-1)", id);
if (id == -1)
{
    // No player was entered.
}
else if (id == INVALID_PLAYER_ID)
    // Entered player is not connected.
}
```

See the section on options for more details.

Users can now optionally return an ARRAY of users instead of just one. This array is just a list of matched IDs, followed by `INVALID_PLAYER_ID`. Given the following players:

```
0) Y_Less
1) [CLAN]Y_Less
2) Jake
3) Alex
4) Hass
```

This code:

```pawn
new ids[3], i;
if (sscanf("Le", "?<MATCH_NAME_PARTIAL=1>u[3]", ids)) printf("Error in input");
for (i = 0; ids[i] != INVALID_PLAYER_ID; ++i)
{
    if (ids[i] == cellmin)
    {
        printf("Too many matches");
        break;
    }
    printf("id = %d", ids[i]);
}
if (i == 0) printf("No matching players found.");
```

Will output:

```
id = 0
id = 1
Too many matches
```

Searching `Les` instead will give:

```
id = 0
id = 1
```

And searching without `MATCH_NAME_PARTIAL` will give:

```
No matching players found.
```

Basically, if an array of size `N` is passed, this code will return the first N-1 results. If there are less than `N` players whose name matches the given name then that many players will be returned and the next slot will be `INVALID_PLAYER_ID` to indicate the end of the list. On the other hand if there are MORE than `N - 1` players whose name matches the given pattern, then the last slot will be `cellmin` to indicate this fact.

When combined with `U` and returning the default, the first slot is always exactly the default value (even if that's not a valid connected player) and the next slot is always `INVALID_PLAYER_ID`.

Note also that user arrays can't be combined with normal arrays or enums, but normal single-return user specifiers still can be.

### Custom (kustom) specifiers

The latest version of sscanf adds a new `k` specifier to allow you to define your own specifers in PAWN:

```pawn
SSCANF:playerstate(string[])
{
    if ('0' <= string[0] <= '9')
    {
        new
            ret = strval(string);
        if (0 <= ret <= 9)
        {
            return ret;
        }
    }
    else if (!strcmp(string, "PLAYER_STATE_NONE")) return 0;
    else if (!strcmp(string, "PLAYER_STATE_ONFOOT")) return 1;
    else if (!strcmp(string, "PLAYER_STATE_DRIVER")) return 2;
    else if (!strcmp(string, "PLAYER_STATE_PASSENGER")) return 3;
    else if (!strcmp(string, "PLAYER_STATE_WASTED")) return 7;
    else if (!strcmp(string, "PLAYER_STATE_SPAWNED")) return 8;
    else if (!strcmp(string, "PLAYER_STATE_SPECTATING")) return 9;
}
```

The code above, when added to the top level of your mode, will add the `playerstate` specifier, allowing you to do:

```pawn
sscanf(params, "uk<playerstate>", playerid, state);
```

This system supports optional custom specifiers with no additional PAWN code:

```pawn
sscanf(params, "uK<playerstate>(PLAYER_STATE_NONE)", playerid, state);
```

The new version of `sscanf2.inc` includes functions for `k<weapon>` and `k<vehicle>` allowing you to enter either the ID or name and get the ID back, but both are VERY basic at the moment and I expect other people will improve on them.

Note that custom specifiers are not supported in either arrays or enumerations.

Note also that custom specifiers always take a string input and always return a number, but this can be a Float, bool, or any other single cell tag type.

The optional kustom specifier `K` takes a default value that is NOT (as of sscanf 2.8) parsed by the given callback:

```
K<vehicle>(999)
```

`999` is NOT a valid vehicle model, but if no other value is supplied then 999 will be returned, allowing you to differentiate between the user entering an invalid vehicle and not entering anything at all.

Also as of sscanf 2.8, `k` can be used in both arrays and enums.

### Colours

sscanf 2.10.0 introduced colours in addition to normal hex numbers.  They are parsed almost identically, but have slightly more constraints on their forms.  Colours must be HEX values exactly 3, 6, or 8 digits long.  3 digit numbers are as in CSS - `#RGB` becomes `0xRRGGBBAA` with default alpha, 6 digit numbers are already `0xRRGGBBAA` with default alpha, 8 digit numbers are the full colour with alpha.  The default default alpha is `255` (`FF`), but this can be changed with the `SSCANF_ALPHA` option; for example setting the default alpha to `AA` would be `?<SSCANF_ALPHA=170>`.  Why do they use `m`, not some sane letter?  Simple - all the good descriptive letters were already used.

The different lengths have slightly different semantics in what is accepted, to reduce the changes of incorrect values being parsed.  You can also customise exactly which input types you accept with the `SSCANF_COLOUR_FORMS` option.

#### 3 digits

A 3-digit hex value MUST be prefixed with `#` as in CSS, and each component is multiplied by `0x11` to give the final component value.  `#FAB` would become `0xFFAABBFF`, `#123` would become `0x112233FF`, `000` would be rejected because there is no `#`.

#### 6 digits

A 6-digit hex colour MAY be prefixed by `#` as in CSS, but doesn't have to be; it can also be prefixed by `0x` or nothing at all.  `#123456`, `0x123456`, and `123456` are all the same value, all valid, and will all give an output of `0x123456FF` with the default alpha value.  Furthermore, a 6-digit hex value may be optionally enclosed in `{}`s - `{8800DD}` is valid, but no other length in `{}`s are valid.

More valid examples:

* `FFFFFF`
* `0x000000`
* `0x010101`
* `#EEEEEE`
* `{000000}`

More invalid examples:

* `FFFFFFF` - 7 digits
* `0x00000` - 5 digits
* `#EEEE` - 4 digits
* `{}` - 0 digits
* `{BBB}` - 3 digits, but not `#` prefix
* `{12345678}` - 8 digits, but inside `{}`s`
* `{123456` - 6 digits, but no closing `}`.

#### 8 digits

8-digit colours are the simplest - the alpha is specified explicitly and there are only two possible input forms - `0x` prefix and no prefix.  I.e. either `0x88995566` or `88995566`.

## Options

The latest version of sscanf introduces several options that can be used to customise the way in which sscanf operates. There are two ways of setting these options - globally and locally:

```pawn
SSCANF_Option(SSCANF_QUIET, 1);
```

This sets the `SSCANF_QUIET` option globally. Every time `sscanf` is called the option (see below) will be in effect. Note that the use of `SSCANF_QUIET` instead of the string `"SSCANF_QUIET"` is entirely valid here - all the options are defined in the sscanf2 include already (but you can use the string if you want).

Alternatively you can use `?` to specify an option locally - i.e. only for the current sscanf call:

```pawn
sscanf(params, "si", str, num);
sscanf(params, "?<SSCANF_QUIET=1>si", str, num);
sscanf(params, "si", str, num);
```

`s` without a length is wrong, and the first and last `sscanf` calls will give an error in the console, but the second one won't as for just that one call prints have been disabled. The following code disables prints globally then enables them locally:

```pawn
SSCANF_Option(SSCANF_QUIET, 1);
sscanf(params, "si", str, num);
sscanf(params, "?<SSCANF_QUIET=0>si", str, num);
sscanf(params, "si", str, num);
```

Note that disabling prints is a VERY bad idea when developing code as you open yourself up to unreported buffer overflows when no length is specified on strings.

To specify multiple options requires multiple calls:

```pawn
SSCANF_Option(SSCANF_QUIET, 1);
SSCANF_Option(MATCH_NAME_PARTIAL, 0);
sscanf(params, "?<SSCANF_QUIET=1>?<MATCH_NAME_PARTIAL=0>s[10]i", str, num);
```

You can also read the current value of an option by ommitting the second parameter:

```pawn
new quiet = SSCANF_Option(SSCANF_QUIET);
```

The options are:

### OLD_DEFAULT_NAME:

The behaviour of `U`, `Q`, and `R` have been changed to take any number as a default, instead of a connected player. Setting `OLD_DEFAULT_NAME` to `1` will revert to the old version.

### MATCH_NAME_PARTIAL:

Currently sscanf will search for players by name, and will ALWAYS search for player whose name STARTS with the specified string. If you have, say `[CLAN]Y_Less` connected and someone types `Y_Less`, sscanf will not find `[CLAN]Y_Less` because there name doesn't start with the specified name. This option, when set to 1, will search ANYWHERE in the player's name for the given string.

### CELLMIN_ON_MATCHES:

Whatever the value of `MATCH_NAME_PARTIAL`, the first found player will always be returned, so if you do a search for `_` on an RP server, you could get almost anyone. To detect this case, if more than one player will match the specified string then sscanf will return an ID of `cellmin` instead. This can be combined with `U` for a lot more power:

```pawn
sscanf(params, "?<CELLMIN_ON_MATCHES=1>U(-1)", id);
if (id == -1)
{
	// No player was entered.
}
else if (id == cellmin)
{
	// Multiple matches found
}
else if (id == INVALID_PLAYER_ID)
{
	// Entered player is not connected.
}
else
{
	// Found just one player.
}
```

### SSCANF_QUIET:

Don't print any errors to the console. REALLY not recommended unless you KNOW your code is stable and in production.

### OLD_DEFAULT_KUSTOM:

As with `U`, `K` used to require a valid identifier as the default and would parse it using the specified callback, so this would NOT work:

```
K<vehicle>(Veyron)
```

Because that is not a valid vehicle name in GTA. The new version now JUST takes a number and returns that regardless:

```
K<vehicle>(9999)
```

This setting reverts to the old behaviour.

### SSCANF_ALPHA:

Specify the default alpha value for colours (`m`) which don't manually specify an alpha channel.  The alpha values are specified as a ***DECIMAL*** number, ***NOT*** a ***HEX*** number, so setting an alpha of `0x80` would be:

```pawn
SSCANF_Option(SSCANF_ALPHA, 128);
```

### SSCANF_COLOUR_FORMS:

There are multiple valid colour input formats, which you can enable or disable here.  The parameter is a bitmap for all the following values:

* `1` - `#RGB`
* `2` - `#RRGGBB`
* `4` - `0xRRGGBB`
* `8` - `RRGGBB`
* `16` - `{RRGGBB}`
* `32` - `0xRRGGBBAA`
* `64` - `RRGGBBAA`

So to ONLY accept SA:MP `SendClientMessage` colours use:

```pawn
SSCANF_Option(SSCANF_COLOUR_FORMS, 16);
```

To only accept 8-digit values use:

```pawn
SSCANF_Option(SSCANF_COLOUR_FORMS, 96);
```

Default values (those specified between `()`s for `M`) ignore this setting - they can always use any form.

### SSCANF_ARGB:

```pawn
SSCANF_Option(SSCANF_ARGB, 1); // Set 3- and 6-digit colour outputs to `AARRGGBB`.
SSCANF_Option(SSCANF_ARGB, 0); // Set 3- and 6-digit colour outputs to `RRGGBBAA` (default).
```

## All specifiers

For quick reference, here is a list of ALL the specifiers and their use:

|                  Format                  |                   Use                  |
| ---------------------------------------- | -------------------------------------- |
|  `A<type>(default)[length]`              |  Optional array of given type          |
|  `a<type>[length]`                       |  Array of given type                   |
|  `B(binary)`                             |  Optional binary number                |
|  `b`                                     |  Binary number                         |
|  `C(character)`                          |  Optional character                    |
|  `c`                                     |  Character                             |
|  `D(integer)`                            |  Optional integer                      |
|  `d`                                     |  Integer                               |
|  `E<specification>(default)`             |  Optional enumeration of given layout  |
|  `e<specification>`                      |  Enumeration of given layout           |
|  `F(float)`                              |  Optional floating point number        |
|  `f`                                     |  Floating point number                 |
|  `G(float/INFINITY/-INFINITY/NAN/NAN_E)` |  Optional float with IEEE definitions  |
|  `g`                                     |  Float with IEEE definitions           |
|  `H(hex value)`                          |  Optional hex number                   |
|  `h`                                     |  Hex number                            |
|  `I(integer)`                            |  Optional integer                      |
|  `i`                                     |  Integer                               |
|  `K<callback>(any format number)`        |  Optional custom operator              |
|  `k<callback>`                           |  Custom operator                       |
|  `L(true/false)`                         |  Optional logical truthity             |
|  `l`                                     |  Logical truthity                      |
|  `M(hex value)`                          |  Optional colour                       |
|  `m`                                     |  Colour                                |
|  `N(any format number)`                  |  Optional number                       |
|  `n`                                     |  Number                                |
|  `O(octal value)`                        |  Optional octal value                  |
|  `o`                                     |  Octal value                           |
|  `P<delimiters>`                         |  Multiple delimiters change            |
|  `p<delimiter>`                          |  Delimiter change                      |
|  `Q(any format number)`                  |  Optional bot (bot)                    |
|  `q`                                     |  Bot (bot)                             |
|  `R(any format number)`                  |  Optional player (player)              |
|  `r`                                     |  Player (player)                       |
|  `S(string)[length]`                     |  Optional string                       |
|  `s[length]`                             |  String                                |
|  `U(any format number)`                  |  Optional user (bot/player)            |
|  `u`                                     |  User (bot/player)                     |
|  `X(hex value)`                          |  Optional hex number                   |
|  `x`                                     |  Hex number                            |
|  `Z(string)[length]`                     |  Optional packed string                |
|  `z[length]`                             |  Packed string                         |
|  `'string'`                              |  Search string                         |
|  `{`                                     |  Open quiet section                    |
|  `}`                                     |  Close quiet section                   |
|  `%`                                     |  Deprecated optional specifier prefix  |
|  `?`                                     |  Local options specifier               |

## Full API

### `sscanf(const data[], const format[], {Float, _}:...);`

The main sscanf function.

### `unformat(const data[], const format[], {Float, _}:...);`

An alternate name for the main `sscanf` function, since the sscanf format specifiers do not conform
to the C API of the same name.

### `SSCANF_Option(const name[], value);`

Set an option.

### `SSCANF_Option(const name[]);`

Get an option.

### `SSCANF_SetOption(const name[], value);`

Set an option explicitly (no overloaded `SSCANF_Option` call).

### `SSCANF_GetOption(const name[], value);`

Get an option explicitly (no overloaded `SSCANF_Option` call).

### `SSCANF_Version(version[], size = sizeof (version));`

Get the SSCANF plugin version as a string (e.g. `"2.10.3"`).  Compare to the macro `SSCANF_VERSION_STRING`.

### `SSCANF_Version();`

Get the SSCANF plugin version as binary coded decimal (BCD) number (e.g. `0x2A3`).  Compare to the macro `SSCANF_VERSION_BCD`.

### `SSCANF_VersionString(version[], size = sizeof (version));`

Get the SSCANF plugin version as a string explicitly (no overloaded `SSCANF_Version` call).

### `SSCANF_VersionBCD();`

Get the SSCANF plugin version as BCD explicitly (no overloaded `SSCANF_Version` call).

### `SSCANF_VERSION_STRING`

The SSCANF include version as a string.

### `SSCANF_VERSION_BCD`

The SSCANF include version as BCD.

## `extract`

I've written some (extendable) macros so you can do:

```pawn
extract params -> new a, string:b[32], Float:c; else
{
    return SendClientMessage(playerid, COLOUR_RED, "FAIL!");
}
```

This will compile as:

```pawn
new a, string:b[32], Float:c;
if (unformat(params, "is[32]f", a, b, c))
{
    return SendClientMessage(playerid, COLOUR_RED, "FAIL!");
}
```

Note that `unformat` is the same as `sscanf`, also note that the `SendClientMessage` part is optional:

```pawn
extract params -> new a, string:b[32], Float:c;
```

Will simply compile as:

```pawn
new a, string:b[32], Float:c;
unformat(params, "is[32]f", a, b, c);
```

Basically it just simplifies sscanf a little bit (IMHO). I like new operators and syntax, hence this, examples:

```pawn
// An int and a float.
extract params -> new a, Float:b;
// An int and an OPTIONAL float.
extract params -> new a, Float:b = 7.0;
// An int and a string.
extract params -> new a, string:s[32];
// An int and a playerid.
extract params -> new a, player:b;
```

As I say, the syntax is extendable, so to add hex numbers you would do:

```pawn
#define hex_EXTRO:%0##%1,%2|||%3=%9|||%4,%5) EXTRY:%0##%1H"("#%9")"#,%2,%3|||%4|||%5)
#define hex_EXTRN:%0##%1,%2|||%3|||%4,%5) EXTRY:%0##%1h,%2,%3|||%4|||%5)
#define hex_EXTRW:%0##%1,%2|||%3[%7]|||%4,%5) EXTRY:%0##%1a<h>[*],%2,(%7),%3|||%4|||%5)
```

That will add the tag `hex` to the system. Yes, the lines look complicated (because they are), but the ONLY things you need to change are the name before the underscore and the letter near the middle (`H`, `h` and `a<h>` in the examples above for `optional`, `required` and `required array` (no optional arrays yet besides strings)).

New examples (with `hex` added):

```pawn
// A hex number and a player.
extract params -> new hex:a, player:b;
// 32 numbers then 32 players.
extract params -> new a[32], player:b[32];
// 11 floats, an optional string, then an optional hex number.
extract params -> new Float:f[11], string:s[12] = "optional", hex:end = 0xFF;
```

The code is actually surprisingly simple (I developed another new technique to simplify my `tag` macros and it paid off big style here). By default `Float`, `string`, `player` and `_` (i.e. no tag) are supported, and their individual letter definitions take up the majority of the code as demonstrated with the `hex` addition above. Note that `string:` is now used extensively in my code to differentiate from tagless arrays in cases like this, it is removed automatically but `player:` and `hex:` are not so you may wish to add:

```pawn
#define player:
#define hex:
```

To avoid tag mismatch warnings (to remove them AFTER the compiler has used them to determine the correct specifier).

The very first example had an `else`, this will turn:

```pawn
unformat(params, "ii", a, b);
```

In to:

```pawn
if (unformat(params, "ii", a, b))
```

You MUST put the `else` on the same line as `extract` for it to be detected, but then you can use normal single or multi-line statements. This is to cover common command use cases, you can even leave things on the same line:

```pawn
else return SendClientMessage(playerid, 0xFF0000AA, "Usage: /cmd <whatever>");
```

There is now the ability to split by things other than space (i.e. adds `P<?>` to the syntax - updated from using `p` to `P`):

```pawn
extract params<|> -> new a, string:b[32], Float:c;
```

Will simply compile as:

```pawn
new a, string:b[32], Float:c;
unformat(params, "P<|>is[32]f", a, b, c);
```

Note that for technical reasons you can use `<->` (because it looks like the arrow after the `extract` keyword). You also can't use `<;>`, `<,>`, or `<)>` because of a bug with `#`, but you can use any other character (most notably `<|>`, as is popular with SQL scripts). I'm thinking of adding enums and existing variables (currently you HAVE to declare new variables), but not right now.

## Errors/Warnings

### MSVRC100.dll not found

If you get this error, DO NOT just download the dll from a random website. This is part of the `Microsoft Visual Studio Redistributable Package`. This is required for many programs, but they often come with it. Download it here:

http://www.microsoft.com/download/en...s.aspx?id=5555

### sscanf error: System not initialised

If you get this error, you need to make sure that you have recompiled ALL your scripts using the LATEST version of `sscanf2.inc`. Older versions didn't really require this as they only had two natives - `sscanf` and `unformat`, the new version has some other functions - you don't need to worry about them, but you must use `sscanf2.inc` so that they are correctly called. If you think you have done this and STILL get the error then try again - make sure you are using the correct version of PAWNO for example.

### sscanf warning: String buffer overflow.

This error comes up when people try and put too much data in to a string. For example:

```pawn
new str[10];
sscanf("Hello there, how are you?", "s[10]", str);
```

That code will try and put the string `Hello there, how are you?` in to the variable called `str`. However, `str` is only 10 cells big and can thus only hold the string `Hello ther` (with a NULL terminator). In this case, the rest of the data is ignored - which could be good or bad:

```pawn
new str[10], num;
sscanf("Hello there you|42", "p<|>s[10]i", str, num);
```

In this case `num` is still correctly set to `42`, but the warning is given for lost data (`e you`).

Currently there is nothing you can do about this from a programming side (you can't even detect it - that is a problem I intend to address), as long as you specify how much data a user should enter this will simply discard the excess, or make the destination variable large enough to handle all cases.

### sscanf warning: Optional types invalid in array specifiers, consider using 'A'.

A specifier such as:

```
a<I(5)>[10]
```

Has been written - here indicating an array of optional integers all with the default value `5`. Instead you should use:

```
A<i>(5)[10]
```

This is an optional array of integers all with the default value `5`, the advantage of this is that arrays can have multiple defaults:

```
A<i>(5, 6)[10]
```

That will set the array to `5, 6, 7, 8, 9, 10, 11, 12, 13, 14` by default, incrementing by the found difference each time.

### sscanf warning: Optional types invalid in enum specifiers, consider using 'E'.

Similar to the previous warning, A specifier such as:

```
e<I(5)f>
```

Is invalid, instead use:

```
E<if>(42, 11.0)
```

This forces ALL the parts of an enum to be optional - anything less is not possible.

### sscanf error: Multi-dimensional arrays are not supported.

This is not allowed:

```pawn
sscanf(params, "a<a<i>[5]>[10]", arr);
```

A work-around can be done using:

```pawn
sscanf(params, "a<i>[50]", arr[0]);
```

That will correctly set up the pointers for the system.

### sscanf error: Search strings are not supported in arrays.

This is not allowed (see the section on search strings):

```
a<'hello'i>[10]
```

### sscanf error: Delimiters are not supported in arrays.

This is not allowed:

```
a<p<,>i>[10]
```

Instead use:

```
p<,>a<i>[10]
```

### sscanf error: Quiet sections are not supported in arrays.


This is not allowed:

```
a<{i}>[10]
```

Instead use:

```
{a<i>[10]}
```

### sscanf error: Unknown format specifier '?'.

The given specifier is not known (this post contains a full list of all the specifiers near the bottom).

### sscanf warning: Empty default values.

An optional specifier has been set as (for example):

```
I()
```

Instead of:

```
I(42)
```

This does not apply to strings as they can be legitimately empty.

### sscanf warning: Unclosed default value.

You have a default value on an optional specifier that looks like:

```
I(42
```

Instead of:

```
I(42)
```

### sscanf warning: No default value found.

You have no default value on an optional specifier:

```
I
```

Instead of:

```
I(42)
```

### sscanf warning: Unenclosed specifier parameter.

You are using the old style:

```
p,
```

Instead of:

```
p<,>
```

Alternatively a custom delimiter of:

```
p<
```

Was found with no matching `>` after one character. Instead use:

```
p<,>
```

Or, if you really do want a delimiter of `<` then use:

```
p<<>
```

Note that this does not need to be escaped; however, a delimiter of `>` does:

```
p<\>>
```

The `\` may also need to be escaped when writing actual PAWN strings, leading to:

```
p<\\>>
```

This also applies to array types (`a<` vs `a<i>`), and will result in an invalid array type.

### sscanf warning: No specified parameter found.

The format specifier just ends with:

```
p
```

This also applies to array types (`a` vs `a<i>`).

### sscanf warning: Missing string length end.
### sscanf warning: Missing length end.

A string has been written as:

```
s[10
```

Instead of:

```
s[10]
```

I.e. the length has not been closed.

### sscanf error: Invalid data length.

An invalid array or string size has been specified (0, negative, or not a number).

### sscanf error: Invalid character in data length.

A string or array has been given a length that is not a number.

### sscanf error: String/array must include a length, please add a destination size.

Arrays are newer than strings, so never had an implementation not requiring a length, so there is no compatability problems in REQUIRING a length to be given.

### sscanf warning: Can't have nestled quiet sections.

You have tried writing something like this:

```
{i{x}}
```

This has a quiet section (`{}`) inside another one, which makes no sense.

### sscanf warning: Not in a quiet section.

`}` was found with no corresponding `{`:

```
i}
```

### sscanf warning: Can't remove quiet in enum.

This is caused by specifiers such as:

```
{fe<i}x>
```

Where the quiet section is started before the enum, but finishes part way through it rather than after it. This can be emulated by:

```
{f}e<{i}x>
```

### sscanf error: Arrays are not supported in enums.


Basically, you can't do:

```
e<fa<i>[5]f>
```

You can, however, still do:

```
e<fiiiiif>
```

This is a little more awkward, but is actually more technically correct given how enums are compiled.

### sscanf warning: Unclosed string literal.

A specifier starts a string with `'`, but doesn't close it:

```
i'hello
```

### sscanf warning: sscanf specifiers do not require '%' before them.

`format` uses code such as `%d`, sscanf only needs `d`, and confusingly the C equivalent function (also called `sscanf`) DOES require `%`. Sorry.

### sscanf error: Insufficient default values.

Default values for arrays can be partially specified and the remainder will be inferred from the pattern of the last two:

```
A<i>(0, 1)[10]
```

That specifier will default to the numbers `0` to `9`. However, because enums have a mixture of types, all the default values for `E` must ALWAYS be specified:

```
E<iiff>(0, 1, 0.0, 1.0)
```

This will not do:

```
E<iiff>(0, 1)
```

### sscanf error: Options are not supported in enums.

### sscanf error: Options are not supported in arrays.

The `?` specifier for local options must appear outside any other specifier.

### sscanf error: No option value.

An option was specified with no value:

```
?<OLD_DEFAULT_NAME>
```

### sscanf error: Unknown option name.

The given option was not recognised. Check spelling and case:

```
?<NOT_A_VALID_NAME=1>
```

### sscanf warning: Could not find function SSCANF:?.

A `k` specifier has been used, but the corresponding function could not be found. If you think it is there check the spelling matches exactly - including the case.

### sscanf error: SSCANF_Init has incorrect parameters.
### sscanf error: SSCANF_Join has incorrect parameters.
### sscanf error: SSCANF_Leave has incorrect parameters.
### sscanf error: SSCANF_SetPlayerName has incorrect parameters.
### sscanf error: SSCANF_IsConnected has incorrect parameters.

You edited something in the sscanf2 include - undo it or redownload it.

## License

### Version: MPL 1.1

The contents of this file are subject to the Mozilla Public License Version
1.1 (the "License"); you may not use this file except in compliance with
the License. You may obtain a copy of the License at
http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the
License.

The Original Code is the sscanf 2.0 SA:MP plugin.

The Initial Developer of the Original Code is Alex "Y_Less" Cole.
Portions created by the Initial Developer are Copyright (C) 2020
the Initial Developer. All Rights Reserved.

### Contributor(s):

* Cheaterman
* Emmet_
* karimcambridge
* leHeix
* maddinat0r
* Southclaws
* Y_Less
* ziggi

### Special Thanks to:

* SA:MP Team past, present, and future.
* maddinat0r, for hosting the repo for a very long time.
* Emmet_, for his efforts in maintaining it for almost a year.

## Changelog

### sscanf 2.8.2 - 18/04/2015

* Fixed a bug where `u` wasn't working correctly after a server restart.

### sscanf 2.8.3 - 02/10/2018

* Allow `k` in arrays.
* Allow `k` to consume the rest of the line (like strings) when they are the last specifier.

### sscanf 2.9.0 - 04/11/2019

* Added `[*]` support.
* Fixed bracketed lengths (`[(32)]`).
* Ported readme to markdown.
* Added `z` and `Z` for packed strings (thus officially removing their deprecated optional use).
* Remove missing string length warnings - its now purely an error.
* Remove `p,` warnings - its now purely an error.

### sscanf 2.10.0 - 27/06/2020

* Added `m` for colours (ran out of useful letters).
* Added file and line details for errors.

### sscanf 2.10.1 - 27/06/2020

* Plugin backwards-compatability with older includes.

### sscanf 2.10.2 - 28/06/2020

* Fix bug in parameter counts.

### sscanf 2.10.3 - 28/04/2021

* Use prehooks in include.
* Export `PawnSScanf` function from dll to other plugins.
* `SSCANF_SetOption()` and `SSCANF_GetOption()` for more control of options.
* `SSCANF_VERSION` and `SSCANF_Version()` to compare include and plugin versions.
* Hide more internal functions.
* Fix the license.

