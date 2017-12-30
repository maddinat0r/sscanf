/*
 *  sscanf 2.8.2
 *  Created by Y_Less, updated by Emmet_.
 *
 *  Version: MPL 1.1
 *
 *  The contents of this file are subject to the Mozilla Public License Version
 *  1.1 (the "License"); you may not use this file except in compliance with
 *  the License. You may obtain a copy of the License at
 *  [url]http://www.mozilla.org/MPL/[/url]
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
 *      SA:MP Team past, present and future
 */

#if defined _inc_a_npc
	#pragma library sscanf
#elseif !defined _samp_included
	#error Please include <a_npc> or <a_samp> first.
#endif

#define SSCANF:%0(%1) forward sscanf_%0(%1);public sscanf_%0(%1)

#if defined sscanf
	#error sscanf (possibly the PAWN version) already defined.
#endif

native sscanf(const data[], const format[], {Float,_}:...);
native unformat(const data[], const format[], {Float,_}:...) = sscanf;
native SSCANF_Init(players, invalid, len);
native SSCANF_Join(playerid, const name[], npc);
native SSCANF_Leave(playerid);
native SSCANF_IsConnected(playerid);

native SSCANF_Option(const name[], value);

stock const
	SSCANF_QUIET[] = "SSCANF_QUIET",
	OLD_DEFAULT_NAME[] = "OLD_DEFAULT_NAME",
	MATCH_NAME_PARTIAL[] = "MATCH_NAME_PARTIAL",
	CELLMIN_ON_MATCHES[] = "CELLMIN_ON_MATCHES",
	OLD_DEFAULT_KUSTOM[] = "OLD_DEFAULT_KUSTOM",
	OLD_DEFAULT_CUSTOM[] = "OLD_DEFAULT_CUSTOM";

static stock
	bool:SSCANF_gInit = false,
	SSCANF_g_sPlayers[MAX_PLAYERS char];

#if defined _inc_a_npc
	forward SSCANF_PlayerCheck();

	/*
	  OnNPCModeInit

	  Called when the script starts if it is a NPC mode, sets up the system,
	  then calls the "real" OnNPCModeInit (using the new ALS 2 hook method).
	*/

	public OnNPCModeInit()
	{
		SSCANF_Init(MAX_PLAYERS, INVALID_PLAYER_ID, MAX_PLAYER_NAME);
		#if !defined SSCANF_NO_PLAYERS
			// Initialise the system.
			SSCANF_PlayerCheck();
			SetTimer("SSCANF_PlayerCheck", 1, 1);
		#endif
		#if defined SSCANF_OnNPCModeInit
			SSCANF_OnNPCModeInit();
		#endif
		return 1;
	}

	#if defined _ALS_OnNPCModeInit
		#undef OnNPCModeInit
	#else
		#define _ALS_OnNPCModeInit
	#endif
	#define OnNPCModeInit SSCANF_OnNPCModeInit
	#if defined SSCANF_OnNPCModeInit
		forward SSCANF_OnNPCModeInit();
	#endif

	/*
	  SSCANF_PlayerCheck

	  NPC modes have no "OnPlayerConnect callback, so we need to simulate one.
	*/

	#if !defined SSCANF_NO_PLAYERS
		public SSCANF_PlayerCheck()
		{
			for (new i = 0; i != MAX_PLAYERS; ++i)
			{
				if (IsPlayerConnected(i))
				{
					if (!SSCANF_g_sPlayers{i})
					{
						new
							name[MAX_PLAYER_NAME + 1];
						GetPlayerName(i, name, sizeof(name));
						// We have no way to know if they are an NPC or not!
						SSCANF_Join(i, name, 0);
						SSCANF_g_sPlayers{i} = 1;
					}
				}
				else
				{
					if (SSCANF_g_sPlayers{i})
					{
						SSCANF_Leave(i);
						SSCANF_g_sPlayers{i} = 0;
					}
				}
			}
		}
	#endif
#else
	/*
	  OnFilterScriptInit

	  Called when the script starts if it is a filterscript, sets up the system,
	  then calls the "real" OnFilterScriptInit (using the new ALS 2 hook
	  method).
	*/

	public OnFilterScriptInit()
	{
	    new
     		name[MAX_PLAYER_NAME + 1];

		SSCANF_Init(GetMaxPlayers(), INVALID_PLAYER_ID, MAX_PLAYER_NAME);
		SSCANF_gInit = true;

		// Check if there are any players that aren't initialized.
		for (new i = 0; i < MAX_PLAYERS; i ++)
		{
		    if (IsPlayerConnected(i) && !SSCANF_IsConnected(i))
		    {
		        GetPlayerName(i, name, sizeof(name));
		        SSCANF_Join(i, name, IsPlayerNPC(i));
		    }
		}

		#if defined SSCANF_OnFilterScriptInit
			SSCANF_OnFilterScriptInit();
		#endif
		return 1;
	}

	#if defined _ALS_OnFilterScriptInit
		#undef OnFilterScriptInit
	#else
		#define _ALS_OnFilterScriptInit
	#endif
	#define OnFilterScriptInit SSCANF_OnFilterScriptInit
	#if defined SSCANF_OnFilterScriptInit
		forward SSCANF_OnFilterScriptInit();
	#endif

	/*
	  OnGameModeInit

	  Called when the script starts if it is a gamemode.  This callback is also
	  called in filterscripts so we don't want to reinitialise the system in
	  that case.
	*/

	public OnGameModeInit()
	{
		if (!SSCANF_gInit)
		{
		    new
		        name[MAX_PLAYER_NAME + 1];

			SSCANF_Init(GetMaxPlayers(), INVALID_PLAYER_ID, MAX_PLAYER_NAME);
			SSCANF_gInit = true;

			// Check if there are any players that aren't initialized.
			for (new i = 0; i < MAX_PLAYERS; i ++)
			{
			    if (IsPlayerConnected(i) && !SSCANF_IsConnected(i))
			    {
			        GetPlayerName(i, name, sizeof(name));
			        SSCANF_Join(i, name, IsPlayerNPC(i));
			    }
			}
		}
		#if defined SSCANF_OnGameModeInit
			SSCANF_OnGameModeInit();
		#endif
		return 1;
	}

	#if defined _ALS_OnGameModeInit
		#undef OnGameModeInit
	#else
		#define _ALS_OnGameModeInit
	#endif
	#define OnGameModeInit SSCANF_OnGameModeInit
	#if defined SSCANF_OnGameModeInit
		forward SSCANF_OnGameModeInit();
	#endif

	/*
	  OnPlayerConnect

	  Called when a player connects.  Actually increments an internal count so
	  that if a script ends and "OnPlayerDisconnect" is called then "sscanf"
	  still knows that the player is really connected.  Also stores their name
	  internally.
	*/

	public OnPlayerConnect(playerid)
	{
		new
			name[MAX_PLAYER_NAME + 1];
		GetPlayerName(playerid, name, sizeof(name));
		SSCANF_Join(playerid, name, IsPlayerNPC(playerid));
		#if defined SSCANF_OnPlayerConnect
			SSCANF_OnPlayerConnect(playerid);
		#endif
		return 1;
	}

	#if defined _ALS_OnPlayerConnect
		#undef OnPlayerConnect
	#else
		#define _ALS_OnPlayerConnect
	#endif
	#define OnPlayerConnect SSCANF_OnPlayerConnect
	#if defined SSCANF_OnPlayerConnect
		forward SSCANF_OnPlayerConnect(playerid);
	#endif

	/*
	  OnPlayerDisconnect

	  Called when a player disconnects, or when a script is ended.
	*/

	public OnPlayerDisconnect(playerid, reason)
	{
		#if defined SSCANF_OnPlayerDisconnect
			SSCANF_OnPlayerDisconnect(playerid, reason);
		#endif
		SSCANF_Leave(playerid);
		return 1;
	}

	#if defined _ALS_OnPlayerDisconnect
		#undef OnPlayerDisconnect
	#else
		#define _ALS_OnPlayerDisconnect
	#endif
	#define OnPlayerDisconnect SSCANF_OnPlayerDisconnect
	#if defined SSCANF_OnPlayerDisconnect
		forward SSCANF_OnPlayerDisconnect(playerid, reason);
	#endif
#endif

#define SSCANF_Init
#define SSCANF_Join
#define SSCANF_Leave

#define extract%0->%1; EXTRN%1;unformat(_:EXTRZ:EXTRV:EXTRX:%0,##,%1,,);
#define unformat(_:EXTRZ:EXTRV:EXTRX:%0,##,%1);%2else if (unformat(_:EXTRV:EXTRX:%0,##,%1))

#define EXTRV:EXTRX:%0<%3>##,%9new%1,%2) EXTRY:%0##P<%3>,|||%1|||%2)
#define EXTRX:%0##,%9new%1,%2) EXTRY:%0##,|||%1|||%2)
#define EXTRY: EXTR8:EXTR9:EXTR0:EXTR1:EXTR2:EXTR3:EXTR4:

#define EXTR8:EXTR9:EXTR0:EXTR1:EXTR2:EXTR3:EXTR4:%0##%1,%2|||%6:%3=%9|||%4) %6_EXTRO:%0##%1,%2|||%3=%9|||%4)
#define EXTR9:EXTR0:EXTR1:EXTR2:EXTR3:EXTR4:%0##%1,%2|||%3=%9|||%4) __EXTRO:%0##%1,%2|||%3=%9|||%4)
#define EXTR0:EXTR1:EXTR2:EXTR3:EXTR4:%0##%1,%2|||%6:%3[%7]|||%4) %6_EXTRW:%0##%1,%2|||%3[%7]|||%4)
#define EXTR1:EXTR2:EXTR3:EXTR4:%0##%1,%2|||%3[%7]|||%4) __EXTRW:%0##%1,%2|||%3|||%4)
#define EXTR2:EXTR3:EXTR4:%0##%1,%2|||%6:%3|||%4) %6_EXTRN:%0##%1,%2|||%3|||%4)
#define EXTR3:EXTR4:%0##%1,,%2||||||%4) %0##%1,%2)
#define EXTR4:%0##%1,%2|||%3|||%4) __EXTRN:%0##%1,%2|||%3|||%4)

// Optional specifiers.
#define __EXTRO:%0##%1,%2|||%3=%9|||%4,%5) EXTRY:%0##%1I"("#%9")"#,%2,%3|||%4|||%5)
#define Float_EXTRO:%0##%1,%2|||%3=%9|||%4,%5) EXTRY:%0##%1F"("#%9")"#,%2,%3|||%4|||%5)
#define player_EXTRO:%0##%1,%2|||%3=%9|||%4,%5) EXTRY:%0##%1U"("#%9")"#,%2,%3|||%4|||%5)
#define string_EXTRO:%0##%1,%2|||%3[%7]=%9|||%4,%5) EXTRY:%0##%1S"("#%9")"#[%7],%2,%3|||%4|||%5)

// Normal specifiers (the double underscore is to work for "_:".
#define __EXTRN:%0##%1,%2|||%3|||%4,%5) EXTRY:%0##%1i,%2,%3|||%4|||%5)
#define Float_EXTRN:%0##%1,%2|||%3|||%4,%5) EXTRY:%0##%1f,%2,%3|||%4|||%5)
#define player_EXTRN:%0##%1,%2|||%3|||%4,%5) EXTRY:%0##%1u,%2,%3|||%4|||%5)
//#define string_EXTRW:%0##%1,%2|||%3[%7]|||%4,%5) EXTRY:%0##%1s[%7],%2,%3|||%4|||%5)

// Array versions of normal specifiers.
#define __EXTRW:%0##%1,%2|||%3[%7]|||%4,%5) EXTRY:%0##%1a<i>[%7],%2,%3|||%4|||%5)
#define Float_EXTRW:%0##%1,%2|||%3[%7]|||%4,%5) EXTRY:%0##%1a<f>[%7],%2,%3|||%4|||%5)
#define player_EXTRW:%0##%1,%2|||%3[%7]|||%4,%5) EXTRY:%0##%1a<u>[%7],%2,%3|||%4|||%5)
#define string_EXTRW:%0##%1,%2|||%3[%7]|||%4,%5) EXTRY:%0##%1s[%7],%2,%3|||%4|||%5)

// Get rid of excess leading space which causes warnings.
#define EXTRN%0new%1; new%1;

#if !defined string
    #define string:
#endif

#define player:

#define hex:
#define hex_EXTRO:%0##%1,%2|||%3=%9|||%4,%5) EXTRY:%0##%1H"("#%9")"#,%2,%3|||%4|||%5)
#define hex_EXTRN:%0##%1,%2|||%3|||%4,%5) EXTRY:%0##%1h,%2,%3|||%4|||%5)
#define hex_EXTRW:%0##%1,%2|||%3[%7]|||%4,%5) EXTRY:%0##%1a<h>[%7],%2,%3|||%4|||%5)

#define bin:
#define bin_EXTRO:%0##%1,%2|||%3=%9|||%4,%5) EXTRY:%0##%1B"("#%9")"#,%2,%3|||%4|||%5)
#define bin_EXTRN:%0##%1,%2|||%3|||%4,%5) EXTRY:%0##%1b,%2,%3|||%4|||%5)
#define bin_EXTRW:%0##%1,%2|||%3[%7]|||%4,%5) EXTRY:%0##%1a<b>[%7],%2,%3|||%4|||%5)

#define kustom:%0<%1> %0
#define kustom_EXTRO:%0##%1,%2|||%3<%8>=%9|||%4,%5) EXTRY:%0##%1K<%8>"("#%9")"#,%2,%3|||%4|||%5)
#define kustom_EXTRN:%0##%1,%2|||%3<%8>|||%4,%5) EXTRY:%0##%1k<%8>,%2,%3|||%4|||%5)
//#define bin_EXTRW:%0##%1,%2|||%3[%7]|||%4,%5) EXTRY:%0##%1a<b>[%7],%2,%3|||%4|||%5)

SSCANF:weapon(string[])
{
	if ('0' <= string[0] <= '9') {
		new ret = strval(string);
		if (0 <= ret <= 18 || 22 <= ret <= 46) {
			return ret;
		}
	}

	static const
		weapon_ids[23][7] = {
			{30, -1}, // a
			{1, 5, -1}, // b
			{9, 15, 27, 43, -1}, // c
			{24, 40, -1}, // d
			{-1}, // e
			{42, 37, 14, 0, -1}, // f
			{2, 16, -1}, // g
			{-1}, // h
			{-1}, // i
			{-1}, // j
			{8, 4, 12, -1}, // k
			{-1}, // l
			{31, 28, 38, 36, 18, 29}, // m
			{3, 44, -1}, // n
			{-1}, // o
			{46, 22, 7, 10, -1}, // p
			{-1}, // q
			{33, 35, -1}, // r
			{39, 26, 25, 6, 23, 34, 41}, // s
			{17, 32, 45, -1}, // t
			{0, 28, -1}, // u
			{-1}, // v
			{11, 13, -1} // w
		},
		weapon_names[47][3][20] = {
			{"Fis", "Unarmed", -1}, // 0
			{"Bra", "BK", -1}, // 1
			{"Gol", "GC", -1}, // 2
			{"Nightstick", "NS", "Nit"}, // 3
			{"Kni", -1, -1}, // 4
			{"Bas", -1, -1}, // 5
			{"Show", -1, -1}, // 6
			{"Poo", "PC", -1}, // 7
			{"Kat", -1, -1}, // 8
			{"Cha", "CS", -1}, // 9
			{"Pur", "PD", -1}, // 10
			{"Dil", -1, -1}, // 11
			{"Vib", -1, -1}, // 12
			{"Silv", "SV", -1}, // 13
			{"Flo", -1, -1}, // 14
			{"Can", -1, -1}, // 15
			{"Gre", -1, -1}, // 16
			{"Tea", "TG", -1}, // 17
			{"Mol", "MC", -1}, // 18
			{-1, -1, -1},
			{-1, -1, -1},
			{-1, -1, -1},
			{"Pis", -1, -1}, // 22
			{"Sil", "SP", -1}, // 23
			{"De", -1, -1}, // 24
			{"Sho", "SG", -1}, // 25
			{"Saw", "SS", -1}, // 26
			{"Com", "CS", -1}, // 27
			{"Mic", "Uzi", -1}, // 28
			{"MP", -1, -1}, // 29
			{"AK", -1, -1}, // 30
			{"M4", -1, -1}, // 31
			{"Tec", -1, -1}, // 32
			{"Cou", "CR", "Rif"}, // 33
			{"Sni", "SR", -1}, // 34
			{"RPG", -1, -1}, // 35
			{"HS", "Heat", -1}, // 36
			{"Fla", "FT", -1}, // 37
			{"Min", -1, -1}, // 38
			{"Sac", "SC", -1}, // 39
			{"Det", -1, -1}, // 40
			{"Spr", -1, -1}, // 41
			{"Fir", "FE", -1}, // 42
			{"Cam", -1, -1}, // 43
			{"Nig", "NVG", -1}, // 44
			{"The", "TG", -1}, // 45
			{"Par", -1, -1} // 46
		};

	new
		i,
		j,
		weapon_id,
		id_index = tolower(string[0]) - 'a';

	if (!(0 <= id_index < sizeof(weapon_ids))) {
		return -1;
	}

	for (i = 0; i < sizeof(weapon_ids[]); i++) {
		weapon_id = weapon_ids[id_index][i];
		if (weapon_id == -1) {
			break;
		}

		for (j = 0; j < sizeof(weapon_names[]); j++) {
			if (weapon_names[weapon_id][j][0] == -1) {
				break;
			}

			if (!strcmp(string, weapon_names[weapon_id][j], true, strlen(weapon_names[weapon_id][j]))) {
				return weapon_id;
			}
		}
	}

	return -1;
}

SSCANF:vehicle(string[])
{
	if ('0' <= string[0] <= '9') {
		new ret = strval(string);
		if (400 <= ret <= 611) {
			return ret;
		}
	}

	static const
		vehicle_ids[26][29] = {
			{445, 602, 416, 592, 450, 591, 435, 577, -1}, // a
			{606, 607, 485, 568, 429, 433, 511, 499, 459, 424, 581, 509, 536, 496, 504, 481, 422, 498, 609, 401, 575, 538, 518, 402, 541, 482, 431, -1}, // b
			{438, 457, 527, 483, 548, 524, 415, 542, 589, 437, 472, 532, 480, 512, -1}, // c
			{578, 473, 593, 486, 406, 573, -1}, // d
			{507, 562, 585, 427, 419, 587, -1}, // e
			{462, 610, 490, 528, 521, 533, 544, 407, 565, 455, 530, 526, 463, 537, 590, 569, -1}, // f
			{604, 466, 492, -1}, // g
			{474, 588, 434, 494, 502, 503, 523, 425, 579, 545, 520}, // h
			{411, 546, 424, -1}, // i
			{559, 493, 508, -1}, // j
			{571, -1}, // k
			{400, 595, 417, 403, 596, 598, -1}, // l
			{517, 410, 484, 487, 551, 500, 556, 557, 444, 418, 510, 572, 423, 414, -1}, // m
			{516, 553, 582, 522, -1}, // n
			{467, -1}, // o
			{443, 470, 461, 404, 584, 603, 600, 448, 596, 598, 597, 497, 599, 413, 430, 426, 436, 547, -1}, // p
			{471, -1}, // q
			{489, 505, 441, 464, 594, 501, 465, 564, 453, 479, 534, 432, 515, 442, 440, 476, -1}, // r
			{601, 475, 605, 543, 488, 468, 495, 567, 447, 428, 405, 519, 460, 535, 458, 469, 452, 446, 580, 439, 561, 570, 409, 513, 560, 550, 506, 574, 597}, // s
			{566, 549, 514, 420, 576, 525, 531, 449, 408, 454, 608, 583, 451, -1}, // t
			{558, 611, 552, -1}, // u
			{540, 491, 412, 539, -1}, // v
			{478, 421, 586, 529, 555, 423, -1}, // w
			{-1}, // x
			{456, 554, -1}, // y
			{477, -1} // z
		},
		vehicle_names[212][3][20] = {
			{"Lan", -1, -1}, // 400
			{"Bra", -1, -1}, // 401
			{"Buf", -1, -1}, // 402
			{"Lin", -1, -1}, // 403
			{"Per", -1, -1}, // 404
			{"Sen", -1, -1}, // 405
			{"Dum", -1, -1}, // 406
			{"Fir", -1, -1}, // 407
			{"Tras", -1, -1}, // 408
			{"Stret", -1, -1}, // 409
			{"Man", -1, -1}, // 410
			{"Inf", -1, -1}, // 411
			{"Voo", -1, -1}, // 412
			{"Pon", -1, -1}, // 413
			{"Mul", -1, -1}, // 414
			{"Che", -1, -1}, // 415
			{"Amb", -1, -1}, // 416
			{"Lev", -1, -1}, // 417
			{"Moo", -1, -1}, // 418
			{"Esp", -1, -1}, // 419
			{"Tax", -1, -1}, // 420
			{"Was", -1, -1}, // 421
			{"Bob", -1, -1}, // 422
			{"Mr", "Who", -1}, // 423
			{"BFI", "Inj", -1}, // 424
			{"Hunte", -1, -1}, // 425
			{"Prem", -1, -1}, // 426
			{"Enf", -1, -1}, // 427
			{"Sec", -1, -1}, // 428
			{"Bans", -1, -1}, // 429
			{"Pred", -1, -1}, // 430
			{"Bus", -1, -1}, // 431
			{"Rhi", -1, -1}, // 432
			{"Bar", -1, -1}, // 433
			{"Hotk", -1, -1}, // 434
			{"ArticleTrailer", "AT", -1}, // 435
			{"Prev", -1, -1}, // 436
			{"Coac", -1, -1}, // 437
			{"Cab", -1, -1}, // 438
			{"Stal", -1, -1}, // 439
			{"Rum", -1, -1}, // 440
			{"RCBan", -1, -1}, // 441
			{"Rom", -1, -1}, // 442
			{"Pac", -1, -1}, // 443
			{"Mon", -1, -1}, // 444
			{"Adm", -1, -1}, // 445
			{"Squ", -1, -1}, // 446
			{"Sea", -1, -1}, // 447
			{"Piz", -1, -1}, // 448
			{"Tram", -1, -1}, // 449
			{"ArticleTrailer2", "AT2", -1}, // 450
			{"Tur", -1, -1}, // 451
			{"Spe", -1, -1}, // 452
			{"Ree", -1, -1}, // 453
			{"Tro", -1, -1}, // 454
			{"Flat", -1, -1}, // 455
			{"Yan", -1, -1}, // 456
			{"Cadd", -1, -1}, // 457
			{"Sol", -1, -1}, // 458
			{"Ber", -1, -1}, // 459
			{"Ski", -1, -1}, // 460
			{"PCJ", -1, -1}, // 461
			{"Fag", -1, -1}, // 462
			{"Free", -1, -1}, // 463
			{"RCBar", -1, -1}, // 464
			{"RCR", -1, -1}, // 465
			{"Glendale", -1, -1}, // 466
			{"Oce", -1, -1}, // 467
			{"Sanc", -1, -1}, // 468
			{"Spa", -1, -1}, // 469
			{"Pat", -1, -1}, // 470
			{"Qua", -1, -1}, // 471
			{"Coas", -1, -1}, // 472
			{"Din", -1, -1}, // 473
			{"Her", -1, -1}, // 474
			{"Sab", -1, -1}, // 475
			{"Rus", -1, -1}, // 476
			{"ZR", -1, -1}, // 477
			{"Wal", -1, -1}, // 478
			{"Reg", -1, -1}, // 479
			{"Come", -1, -1}, // 480
			{"BMX", -1, -1}, // 481
			{"Bur", -1, -1}, // 482
			{"Cam", -1, -1}, // 483
			{"Mar", -1, -1}, // 484
			{"Baggage", -1, -1}, // 485
			{"Doz", -1, -1}, // 486
			{"Mav", -1, -1}, // 487
			{"SANN", -1, -1}, // 488
			{"Rancher", -1, -1}, // 489
			{"FBIR", -1, -1}, // 490
			{"Vir", -1, -1}, // 491
			{"Gre", -1, -1}, // 492
			{"Jet", -1, -1}, // 493
			{"HotringRacer", "HR", -1}, // 494
			{"Sand", -1, -1}, // 495
			{"Bli", "BC", -1}, // 496
			{"PoliceMaverick", "PM", -1}, // 497
			{"Box", -1, -1}, // 498
			{"Ben", -1, -1}, // 499
			{"Mes", -1, -1}, // 500
			{"RCG", -1, -1}, // 501
			{"HotringRacerA", "HRA", -1}, // 502
			{"HotringRacerB", "HRB", -1}, // 503
			{"Blo", "BB", -1}, // 504
			{"RancherLure", "RL", -1}, // 505
			{"Sup", -1, -1}, // 506
			{"Ele", -1, -1}, // 507
			{"Jou", -1, -1}, // 508
			{"Bik", -1, -1}, // 509
			{"Mou", "MB", -1}, // 510
			{"Bea", -1, -1}, // 511
			{"Cro", -1, -1}, // 512
			{"Stu", -1, -1}, // 513
			{"Tan", -1, -1}, // 514
			{"Roa", -1, -1}, // 515
			{"Neb", -1, -1}, // 516
			{"Maj", -1, -1}, // 517
			{"Buc", -1, -1}, // 518
			{"Sha", -1, -1}, // 519
			{"Hyd", -1, -1}, // 520
			{"FCR", -1, -1}, // 521
			{"NRG", -1, -1}, // 522
			{"HPV", -1, -1}, // 523
			{"Cem", "CT", -1}, // 524
			{"Tow", -1, -1}, // 525
			{"Fort", -1, -1}, // 526
			{"Cadr", -1, -1}, // 527
			{"FBIT", -1, -1}, // 528
			{"Wil", -1, -1}, // 529
			{"Fork", -1, -1}, // 530
			{"Trac", -1, -1}, // 531
			{"Comb", "CH", -1}, // 532
			{"Fel", -1, -1}, // 533
			{"Rem", -1, -1}, // 534
			{"Sla", -1, -1}, // 535
			{"Bla", -1, -1}, // 536
			{"Freight", -1, -1}, // 537
			{"Brow", -1, -1}, // 538
			{"Vor", -1, -1}, // 539
			{"Vin", -1, -1}, // 540
			{"Bul", -1, -1}, // 541
			{"Clo", -1, -1}, // 542
			{"Sadler", -1, -1}, // 543
			{"FireLA", -1, -1}, // 544
			{"Hus", -1, -1}, // 545
			{"Int", -1, -1}, // 546
			{"Pri", -1, -1}, // 547
			{"Car", -1, -1}, // 548
			{"Tam", -1, -1}, // 549
			{"Sun", -1, -1}, // 550
			{"Mer", -1, -1}, // 551
			{"UtilityVan", "UV", -1}, // 552
			{"Nev", -1, -1}, // 553
			{"Yos", -1, -1}, // 554
			{"Win", -1, -1}, // 555
			{"MonsterA", "MonA", -1}, // 556
			{"MonsterB", "MonB", -1}, // 557
			{"Ura", -1, -1}, // 558
			{"Jes", -1, -1}, // 559
			{"Sul", -1, -1}, // 560
			{"Stra", -1, -1}, // 561
			{"Elegy", -1, -1}, // 562
			{"Rai", -1, -1}, // 563
			{"RCT", -1, -1}, // 564
			{"Flas", -1, -1}, // 565
			{"Tah", -1, -1}, // 566
			{"Sav", -1, -1}, // 567
			{"Band", -1, -1}, // 568
			{"FreightFlat", "FF", -1}, // 569
			{"StreakTrailer", "ST", -1}, // 570
			{"Kar", -1, -1}, // 571
			{"Mow", -1, -1}, // 572
			{"Dun", -1, -1}, // 573
			{"Swe", -1, -1}, // 574
			{"Broa", -1, -1}, // 575
			{"Tor", -1, -1}, // 576
			{"AT", -1, -1}, // 577
			{"DFT", -1, -1}, // 578
			{"Huntl", -1, -1}, // 579
			{"Staf", -1, -1}, // 580
			{"BF", -1, -1}, // 581
			{"New", -1, -1}, // 582
			{"Tug", -1, -1}, // 583
			{"Pet", "PT", -1}, // 584
			{"Emp", -1, -1}, // 585
			{"Way", -1, -1}, // 586
			{"Eur", -1, -1}, // 587
			{"Hotd", -1, -1}, // 588
			{"Club", -1, -1}, // 589
			{"FreightBox", "FB", -1}, // 590
			{"ArticleTrailer3", "AT3", -1}, // 591
			{"And", -1, -1}, // 592
			{"Dod", -1, -1}, // 593
			{"RCC", -1, -1}, // 594
			{"Lau", -1, -1}, // 595
			{"PoliceCarLSPD", "PCLSPD", "LSPD"}, // 596
			{"PoliceCarSFPD", "PCSFPD", "SFPD"}, // 597
			{"PoliceCarLVPD", "PCLVPD", "LVPD"}, // 598
			{"PoliceRanger", "PR", -1}, // 599
			{"Pic", -1, -1}, // 600
			{"S.W.A.T.", "SWAT", -1}, // 601
			{"Alp", -1, -1}, // 602
			{"Pho", -1, -1}, // 603
			{"GlendaleShit", "GS", -1}, // 604
			{"SadlerShit", "SS", -1}, // 605
			{"BaggageTrailerA", "BTA", -1}, // 606
			{"BaggageTrailerB", "BTB", -1}, // 607
			{"TugStairs", "TS", -1}, // 608
			{"Boxville2", "Box2", -1}, // 609
			{"Far", "FT", -1}, // 610
			{"UtilityTrailer", "UT", -1} // 611
		};

	new
		i,
		j,
		vehicle_id,
		id_index = tolower(string[0]) - 'a';

	if (!(0 <= id_index < sizeof(vehicle_ids))) {
		return -1;
	}

	for (i = 0; i < sizeof(vehicle_ids[]); i++) {
		vehicle_id = vehicle_ids[id_index][i] - 400;
		if (vehicle_id == -1) {
			break;
		}

		for (j = 0; j < sizeof(vehicle_names[]); j++) {
			if (vehicle_names[vehicle_id][j][0] == -1) {
				break;
			}

			if (!strcmp(string, vehicle_names[vehicle_id][j], true, strlen(vehicle_names[vehicle_id][j]))) {
				return vehicle_id + 400;
			}
		}
	}

	return -1;
}

// Fix the compiler crash when both the PAWN and Plugin versions of sscanf are
// found by renaming the old version at declaration.  (fixes.inc compatible
// naming scheme: "BAD_Function()").
#define sscanf(%0:...) BAD_sscanf(%0:...)
