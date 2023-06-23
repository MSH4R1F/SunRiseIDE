# Standard Declarations for mfbuild

CC	=	gcc
CFLAGS	=	-g -Wall

EXECS	=	sunrise

all:	$(EXECS)

#  sunrise
sunrise.o:	sunrise.c ../emulator/emulate.h ../assembler/assemble.h ../general/constants.h ../general/labelMap.h ../general/assembleFileUtils.h ../general/terminalColor.h patternMatch.h checkSyntax.h
patternMatch.o: patternMatch.c patternMatch.h ../general/constants.h
checkSyntax.o: checkSyntax.c checkSyntax.h ../general/constants.h ../general/terminalColor.h ../general/assembleFileUtils.h ../general/labelMap.h ../assembler/branch.h ../assembler/tokenUtils.h

#  general
../general/labelMap.o:	../general/labelMap.c ../general/labelMap.h ../general/constants.h
../general/assembleFileUtils.o: ../general/assembleFileUtils.c ../general/assembleFileUtils.h
../general/memory.o:	../general/memory.c ../general/memory.h ../general/constants.h
../general/terminalColor.o: ../general/terminalColor.c ../general/terminalColor.h

#  emulate
../emulator/emulate.o:	../emulator/emulate.c ../emulator/emulateFileUtils.h ../general/memory.h ../emulator/registers.h ../emulator/bitwiseShift.h ../emulator/dataProcessingImm.h ../emulator/dataProcessingReg.h ../emulator/singleDataTransfer.h ../emulator/branch.h
../emulator/emulateFileUtils.o:	../emulator/emulateFileUtils.c ../emulator/emulateFileUtils.h ../general/memory.h ../general/constants.h
../emulator/registers.o:	../emulator/registers.c ../emulator/registers.h
../emulator/bitwiseShift.o:	../emulator/bitwiseShift.c ../emulator/bitwiseShift.h
../emulator/dataProcessingImm.o:	../emulator/dataProcessingImm.c ../emulator/dataProcessingImm.h ../emulator/emulateFileUtils.h ../emulator/flags.h ../emulator/dataProcessing.h
../emulator/dataProcessingReg.o:	../emulator/dataProcessingReg.c ../emulator/dataProcessingReg.h ../emulator/bitwiseShift.h ../general/memory.h ../emulator/flags.h ../emulator/dataProcessing.h
../emulator/singleDataTransfer.o:	../emulator/singleDataTransfer.c ../emulator/singleDataTransfer.h ../general/memory.h
../emulator/branch.o:	../emulator/branch.c ../emulator/branch.h
../emulator/flags.o:	../emulator/flags.c ../emulator/flags.h

#  assemble
../assembler/assemble.o:	../assembler/assemble.c ../general/memory.h ../general/labelMap.h ../general/assembleFileUtils.h ../assembler/key.h ../assembler/tokenUtils.h ../assembler/branch.h ../assembler/dataProcessing.h ../assembler/dataTransfer.h ../assembler/directive.h
../assembler/key.o:	../assembler/key.c ../assembler/key.h
../assembler/tokenUtils.o:	../assembler/tokenUtils.c ../assembler/tokenUtils.h
../assembler/branch.o:	../assembler/branch.c ../assembler/branch.h ../assembler/key.h ../assembler/tokenUtils.h
../assembler/dataProcessing.o:	../assembler/dataProcessing.c ../assembler/dataProcessing.h ../assembler/tokenUtils.h ../assembler/key.h
../assembler/dataTransfer.o:	../assembler/dataTransfer.c ../assembler/dataTransfer.h ../assembler/key.h
../assembler/directive.o:	../assembler/directive.c ../assembler/directive.h ../assembler/key.h

OBJS = sunrise.o patternMatch.o checkSyntax.o ../general/labelMap.o ../general/assembleFileUtils.o ../general/memory.o ../general/terminalColor.o ../emulator/emulate.o ../emulator/emulateFileUtils.o ../emulator/registers.o ../emulator/bitwiseShift.o ../emulator/dataProcessingImm.o ../emulator/dataProcessingReg.o ../emulator/singleDataTransfer.o ../emulator/branch.o ../emulator/flags.o ../assembler/assemble.o ../assembler/key.o ../assembler/tokenUtils.o ../assembler/branch.o ../assembler/dataProcessing.o ../assembler/dataTransfer.o ../assembler/directive.o
clean:
	/bin/rm -f $(EXECS) $(OBJS) core a.out .nfs*

sunrise:		$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o sunrise $(LDFLAGS)
