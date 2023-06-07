# Standard Declarations for mfbuild

CC	=	gcc
CFLAGS	=	-g -Wall

EXECS	=	assemble emulate

all:	$(EXECS)

assemble.o:	assemble.c
emulate.o: emulate.c

OBJS	=	 assemble.o emulate.o

clean:
	/bin/rm -f $(EXECS) $(OBJS) core a.out .nfs*

