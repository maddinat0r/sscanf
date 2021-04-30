#
#  sscanf 2.10.3
#
#  Version: MPL 1.1
#
#  The contents of this file are subject to the Mozilla Public License Version
#  1.1 (the "License"); you may not use this file except in compliance with
#  the License. You may obtain a copy of the License at
#  http://www.mozilla.org/MPL/
#
#  Software distributed under the License is distributed on an "AS IS" basis,
#  WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
#  for the specific language governing rights and limitations under the
#  License.
#
#  The Original Code is the sscanf 2.0 SA:MP plugin.
#
#  The Initial Developer of the Original Code is Alex "Y_Less" Cole.
#  Portions created by the Initial Developer are Copyright (C) 2020
#  the Initial Developer. All Rights Reserved.
#
#  Contributor(s):
#
#      Cheaterman
#      Emmet_
#      karimcambridge
#      leHeix
#      maddinat0r
#      Southclaws
#      Y_Less
#      ziggi
#
#  Special Thanks to:
#
#      SA:MP Team past, present, and future.
#      maddinat0r, for hosting the repo for a very long time.
#      Emmet_, for his efforts in maintaining it for almost a year.
#

# This file demonstrates how to compile the sscanf project on Linux.
#
# To compile SSCANF do:
#
# make SSCANF
#

GPP = g++
GCC = gcc
SSCANF_OUTFILE = "sscanf.so"

COMPILE_FLAGS = -m32 -fPIC -c -O2 -w -D LINUX -D PROJECT_NAME=\"sscanf\" -D HAVE_STDINT_H -I ./SDK/amx/

SSCANF = -D SSCANF $(COMPILE_FLAGS)

all: SSCANF

clean:
	-rm -f *~ *.o *.so

SSCANF: clean
	$(GPP) $(SSCANF) ./SDK/amx/*.c
	$(GPP) $(SSCANF) ./SDK/*.cpp
	$(GPP) $(SSCANF) *.cpp
	$(GPP) -m32 -fPIC -shared -o $(SSCANF_OUTFILE) *.o

