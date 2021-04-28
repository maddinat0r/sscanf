# This file demonstrates how to compile the sscanf project on Linux.
#
# To compile SSCANF do:
#
# make SSCANF
#

GPP = g++
GCC = gcc
SSCANF_OUTFILE = "sscanf.so"

COMPILE_FLAGS = -m32 -fPIC -c -O2 -w -D LINUX -D PROJECT_NAME=\"sscanf\" -I ./SDK/amx/

SSCANF = -D SSCANF $(COMPILE_FLAGS)

all: SSCANF

clean:
	-rm -f *~ *.o *.so

SSCANF: clean
	$(GPP) $(SSCANF) ./SDK/amx/*.c
	$(GPP) $(SSCANF) ./SDK/*.cpp
	$(GPP) $(SSCANF) *.cpp
	$(GPP) -m32 -fPIC -shared -o $(SSCANF_OUTFILE) *.o
