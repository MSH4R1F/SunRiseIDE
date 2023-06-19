# Standard Declarations for mfbuild

CC	=	gcc
CFLAGS	=	-g -Wall

EXECS	=	assemble

all:	$(EXECS)

main.o: main.c assemble.h
assemble.o:	assemble.c ../general/memory.h ../general/labelMap.h ../general/assembleFileUtils.h key.h tokenUtils.h branch.h dataProcessing.h dataTransfer.h directive.h

../general/memory.o:	../general/memory.c ../general/memory.h ../general/constants.h
../general/labelMap.o:	../general/labelMap.c ../general/labelMap.h ../general/constants.h
../general/assembleFileUtils.o:	../general/assembleFileUtils.c ../general/assembleFileUtils.h

key.o:	key.c key.h
tokenUtils.o:	tokenUtils.c tokenUtils.h
branch.o:	branch.c branch.h key.h tokenUtils.h
dataProcessing.o:	dataProcessing.c dataProcessing.h tokenUtils.h key.h
dataTransfer.o:	dataTransfer.c dataTransfer.h key.h
directive.o:	directive.c directive.h key.h

OBJS	= main.o assemble.o ../general/memory.o ../general/labelMap.o ../general/assembleFileUtils.o key.o tokenUtils.o branch.o dataProcessing.o dataTransfer.o directive.o

clean:
	/bin/rm -f $(EXECS) $(OBJS) core a.out .nfs*

assemble:		$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o assemble $(LDFLAGS)
