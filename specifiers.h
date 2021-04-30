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

bool
	DoI(char ** input, int * ret);

bool
	DoN(char ** input, int * ret);

bool
	DoH(char ** input, int * ret);

bool
	DoM(char ** input, unsigned int * ret);

bool
	DoO(char ** input, int * ret);

bool
	DoF(char ** input, double * ret);

bool
	DoC(char ** input, char * ret);

bool
	DoB(char ** input, int * ret);

bool
	DoL(char ** input, bool * ret);

bool
	DoG(char ** input, double * ret);

bool
	DoS(char ** input, char ** ret, int length, bool all);

bool
	DoU(char ** input, int * ret, unsigned int start);

bool
	DoQ(char ** input, int * ret, unsigned int start);

bool
	DoR(char ** input, int * ret, unsigned int start);

bool
	DoID(char ** input, int * ret);

bool
	DoND(char ** input, int * ret);

bool
	DoHD(char ** input, int * ret);

bool
	DoMD(char ** input, unsigned int * ret);

bool
	DoOD(char ** input, int * ret);

bool
	DoFD(char ** input, double * ret);

bool
	DoCD(char ** input, char * ret);

bool
	DoBD(char ** input, int * ret);

bool
	DoLD(char ** input, bool * ret);

bool
	DoGD(char ** input, double * ret);

bool
	DoSD(char ** input, char ** ret, int * length, struct args_s & args);

bool
	DoUD(char ** input, int * ret);

bool
	DoQD(char ** input, int * ret);

bool
	DoRD(char ** input, int * ret);
