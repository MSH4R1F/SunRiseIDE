# Standard Declarations for mfbuild

CC	=	gcc
CFLAGS	=	-g -Wall

EXECS	=	emulate

all:	$(EXECS)

emulate.o:	emulate.c fileUtils.h memory.h registers.h bitwiseShift.h dataProcessingImm.h dataProcessingReg.h singleDataTransfer.h branch.h

fileUtils.o:	fileUtils.c fileUtils.h memory.h constants.h

memory.o:	memory.c memory.h constants.h

registers.o:	registers.c registers.h

bitwiseShift.o:	bitwiseShift.c bitwiseShift.h

dataProcessingImm.o:	dataProcessingImm.c dataProcessingImm.h fileUtils.h flags.h dataProcessing.h

dataProcessingReg.o:	dataProcessingReg.c dataProcessingReg.h bitwiseShift.h memory.h flags.h dataProcessing.h

singleDataTransfer.o:	singleDataTransfer.c singleDataTransfer.h memory.h

branch.o:	branch.c branch.h

flags.o:	flags.c flags.h

OBJS	=	 emulate.o fileUtils.o memory.o registers.o bitwiseShift.o dataProcessingImm.o dataProcessingReg.o singleDataTransfer.o branch.o flags.o

clean:
	/bin/rm -f $(EXECS) $(OBJS) core a.out .nfs*

emulate:		$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o emulate $(LDFLAGS)
