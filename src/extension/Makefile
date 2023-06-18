# Standard Declarations for mfbuild

CC	=	gcc
CFLAGS	=	-g -Wall

EXECS	=	sunrise

all:	$(EXECS)

sunrise.o:	sunrise.c ../emulate/emulate.h ../assemble/assemble.h ../general/constants.h ../general/labelMap.h ../general/assembleFileUtils.h ../general/terminalColor.h patternMatch.h

../emulate/emulate.o:	../emulate/emulate.c emulate.h emulateFileUtils.h ../general/memory.h registers.h bitwiseShift.h dataProcessingImm.h dataProcessingReg.h singleDataTransfer.h branch.h

../assemble/assemble.o:	../assemble/assemble.c assemble.h ../general/memory.h ../general/labelMap.h ../general/assembleFileUtils.h key.h tokenUtils.h branch.h dataProcessing.h dataTransfer.h directive.h

../general/labelMap.o:	../general/labelMap.c labelMap.h constants.h

../general/assembleFileUtils.o:	../general/assembleFileUtils.c assembleFileUtils.h

../general/terminalColor.o:	../general/terminalColor.c terminalColor.h

patternMatch.o:	patternMatch.c patternMatch.h ../general/constants.h

../general/memory.o:	../general/memory.c memory.h constants.h

OBJS	=	 sunrise.o ../emulate/emulate.o ../assemble/assemble.o ../general/labelMap.o ../general/assembleFileUtils.o ../general/terminalColor.o patternMatch.o ../general/memory.o

clean:
	/bin/rm -f $(EXECS) $(OBJS) core a.out .nfs*

sunrise:		$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o sunrise $(LDFLAGS)
