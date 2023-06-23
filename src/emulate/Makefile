# Standard Declarations for mfbuild

CC	=	gcc
CFLAGS	=	-g -Wall

EXECS	=	emulate

all:	$(EXECS)

main.o: main.c emulate.h

emulate.o:	emulate.c emulateFileUtils.h ../general/memory.h ../general/terminalColor.h registers.h bitwiseShift.h dataProcessingImm.h dataProcessingReg.h singleDataTransfer.h branch.h

emulateFileUtils.o:	emulateFileUtils.c emulateFileUtils.h ../general/memory.h ../general/constants.h

../general/memory.o:	../general/memory.c ../general/memory.h ../general/constants.h
../general/terminalColor.o:  ../general/terminalColor.c ../general/terminalColor.h

registers.o:	registers.c registers.h

bitwiseShift.o:	bitwiseShift.c bitwiseShift.h

dataProcessingImm.o:	dataProcessingImm.c dataProcessingImm.h emulateFileUtils.h flags.h dataProcessing.h

dataProcessingReg.o:	dataProcessingReg.c dataProcessingReg.h bitwiseShift.h ../general/memory.h flags.h dataProcessing.h

singleDataTransfer.o:	singleDataTransfer.c singleDataTransfer.h ../general/memory.h

branch.o:	branch.c branch.h

flags.o:	flags.c flags.h

OBJS	= main.o emulate.o emulateFileUtils.o ../general/memory.o ../general/terminalColor.o registers.o bitwiseShift.o dataProcessingImm.o dataProcessingReg.o singleDataTransfer.o branch.o flags.o

clean:
	/bin/rm -f $(EXECS) $(OBJS) core a.out .nfs*

emulate:		$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o emulate $(LDFLAGS)
