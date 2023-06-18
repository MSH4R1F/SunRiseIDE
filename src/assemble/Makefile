# Standard Declarations for mfbuild

CC	=	gcc
CFLAGS	=	-g -Wall

EXECS	=	assemble

all:	$(EXECS)



assemble.o:	assemble.c

OBJS	=	 assemble.o

clean:
	/bin/rm -f $(EXECS) $(OBJS) core a.out .nfs*

assemble:		$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o assemble $(LDFLAGS)
