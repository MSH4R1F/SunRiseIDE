# Standard Declarations for mfbuild

CC	=	gcc
CFLAGS	=	-g -Wall

EXECS	= sunrise

all:	$(EXECS)

sunrise.o:	sunrise.c

OBJS	=	sunrise.o

clean:
	/bin/rm -f $(EXECS) $(OBJS) core a.out .nfs*

sunrise:	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o sunrise $(LDFLAGS)