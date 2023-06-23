# Standard Declarations for mfbuild

CC	=	gcc
CFLAGS	=	-g -Wall

EXECS	=	sunrise

all:	$(EXECS)

#  sunrise
sunrise.o:	sunrise.c ../emulate/emulate.h ../assemble/assemble.h ../general/constants.h ../general/labelMap.h ../general/assembleFileUtils.h ../general/terminalColor.h patternMatch.h checkSyntax.h
patternMatch.o: patternMatch.c patternMatch.h ../general/constants.h
checkSyntax.o: checkSyntax.c checkSyntax.h ../general/constants.h ../general/terminalColor.h ../general/assembleFileUtils.h ../general/labelMap.h ../assemble/branch.h ../assemble/tokenUtils.h

#  general
../general/labelMap.o:	../general/labelMap.c ../general/labelMap.h ../general/constants.h
../general/assembleFileUtils.o: ../general/assembleFileUtils.c ../general/assembleFileUtils.h
../general/memory.o:	../general/memory.c ../general/memory.h ../general/constants.h
../general/terminalColor.o: ../general/terminalColor.c ../general/terminalColor.h

#  emulate
../emulate/emulate.o:	../emulate/emulate.c ../emulate/emulateFileUtils.h ../general/memory.h ../emulate/registers.h ../emulate/bitwiseShift.h ../emulate/dataProcessingImm.h ../emulate/dataProcessingReg.h ../emulate/singleDataTransfer.h ../emulate/branch.h
../emulate/emulateFileUtils.o:	../emulate/emulateFileUtils.c ../emulate/emulateFileUtils.h ../general/memory.h ../general/constants.h
../emulate/registers.o:	../emulate/registers.c ../emulate/registers.h
../emulate/bitwiseShift.o:	../emulate/bitwiseShift.c ../emulate/bitwiseShift.h
../emulate/dataProcessingImm.o:	../emulate/dataProcessingImm.c ../emulate/dataProcessingImm.h ../emulate/emulateFileUtils.h ../emulate/flags.h ../emulate/dataProcessing.h
../emulate/dataProcessingReg.o:	../emulate/dataProcessingReg.c ../emulate/dataProcessingReg.h ../emulate/bitwiseShift.h ../general/memory.h ../emulate/flags.h ../emulate/dataProcessing.h
../emulate/singleDataTransfer.o:	../emulate/singleDataTransfer.c ../emulate/singleDataTransfer.h ../general/memory.h
../emulate/branch.o:	../emulate/branch.c ../emulate/branch.h
../emulate/flags.o:	../emulate/flags.c ../emulate/flags.h

#  assemble
../assemble/assemble.o:	../assemble/assemble.c ../general/memory.h ../general/labelMap.h ../general/assembleFileUtils.h ../assemble/key.h ../assemble/tokenUtils.h ../assemble/branch.h ../assemble/dataProcessing.h ../assemble/dataTransfer.h ../assemble/directive.h
../assemble/key.o:	../assemble/key.c ../assemble/key.h
../assemble/tokenUtils.o:	../assemble/tokenUtils.c ../assemble/tokenUtils.h
../assemble/branch.o:	../assemble/branch.c ../assemble/branch.h ../assemble/key.h ../assemble/tokenUtils.h
../assemble/dataProcessing.o:	../assemble/dataProcessing.c ../assemble/dataProcessing.h ../assemble/tokenUtils.h ../assemble/key.h
../assemble/dataTransfer.o:	../assemble/dataTransfer.c ../assemble/dataTransfer.h ../assemble/key.h
../assemble/directive.o:	../assemble/directive.c ../assemble/directive.h ../assemble/key.h

OBJS = sunrise.o patternMatch.o checkSyntax.o ../general/labelMap.o ../general/assembleFileUtils.o ../general/memory.o ../general/terminalColor.o ../emulate/emulate.o ../emulate/emulateFileUtils.o ../emulate/registers.o ../emulate/bitwiseShift.o ../emulate/dataProcessingImm.o ../emulate/dataProcessingReg.o ../emulate/singleDataTransfer.o ../emulate/branch.o ../emulate/flags.o ../assemble/assemble.o ../assemble/key.o ../assemble/tokenUtils.o ../assemble/branch.o ../assemble/dataProcessing.o ../assemble/dataTransfer.o ../assemble/directive.o
clean:
	/bin/rm -f $(EXECS) $(OBJS) core a.out .nfs*

sunrise:		$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o sunrise $(LDFLAGS)
