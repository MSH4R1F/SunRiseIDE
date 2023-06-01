# Standard Declarations for mfbuild

CC	=	gcc
CFLAGS	=	-g -Wall

EXECS	=	emulate

all:	$(EXECS)

emulate.o:	emulate.c

OBJS	=	 emulate.o

clean:
	/bin/rm -f $(EXECS) $(OBJS) core a.out .nfs*

emulate:		$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o emulate $(LDFLAGS)
