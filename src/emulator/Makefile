# Standard Declarations for mfbuild

CC	=	gcc
CFLAGS	=	-g -Wall

EXECS	=	dataProcessingImm

all:	$(EXECS)

dataProcessingImm.o:	dataProcessingImm.c dataProcessingImm.h

OBJS	=	 dataProcessingImm.o

clean:
	/bin/rm -f $(EXECS) $(OBJS) core a.out .nfs*

dataProcessingImm:		$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o dataProcessingImm $(LDFLAGS)
