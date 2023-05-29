CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o
emulate: emulate.o dataProcessingImm.o utils.o
dataProcessingIm.o: emulator/dataProcessingImm.h
test: emulator/utils.h

clean:
	$(RM) *.o assemble emulate
	
