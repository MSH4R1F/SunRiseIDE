
#include "emulate.h"

#include "../general/memory.h"
#include "../general/terminalColor.h"

#include "registers.h"
#include "bitwiseShift.h"
#include "emulateFileUtils.h"

#include "dataProcessingImm.h"
#include "dataProcessingReg.h"
#include "singleDataTransfer.h"
#include "branch.h"

// private function declarations
static void runEmulate(uint8_t *memPointer, char* filename, int argc, bool isDebug, char **assemblyArray);

int emulate(int argc, char **argv, bool isDebug, char **assemblyArray) {
    uint8_t *memPointer = allocateMemory();
    loadMemoryFromFile(memPointer,argv[1]);
    if (argc == 2) {
        runEmulate(memPointer, "output.txt", argc, isDebug, assemblyArray);
    } else {
        runEmulate(memPointer, argv[2], argc, isDebug, assemblyArray);
    }
    free(memPointer);
    return EXIT_SUCCESS;
}

static void runEmulate(uint8_t *memPointer, char* filename, int argc, bool isDebug, char **assemblyArray) {
    struct PSTATE stateRegister = { false, true, false, false };
    struct RegisterStore registerStore;
    registerStore.programCounter = 0;
    registerStore.zeroRegister = 0;
    registerStore.stateRegister = &stateRegister;

    for (int i = 0; i < sizeof(registerStore.registers) / sizeof(registerStore.registers[0]); i++) {
        registerStore.registers[i] = 0;
    }

    while (true) {
        if (isDebug) {
            system("clear");

            if (assemblyArray != NULL) {
                setTerminalColour(GREY);
                char *assemblyLine = assemblyArray[registerStore.programCounter / 4];
                printf("Executing Instruction: \n");
                printf("%s\n", assemblyLine);
            }
        }

        uint32_t instruction = fetchInstruction(registerStore.programCounter, memPointer);

        long long op0 = (instruction >> 25) & 0xF;

        if (instruction == 0x8a000000 || instruction == 0) {
            break;
        } else if (instruction == 0xD503201F) {
            registerStore.programCounter += 4;
            continue;
        } else if (isDataProcessingImm(op0)) {
            executeDataProcessingImm(instruction, &registerStore);
            registerStore.programCounter += 4;
        } else if (isDataProcessingReg(op0)) {
            executeDataProcessingReg(instruction, &registerStore);
            registerStore.programCounter += 4;
        } else if (isBranchM(op0)) {
            executeBranch(instruction, &registerStore);
        } else {
            executeDataTransfer(instruction, memPointer, &registerStore);
            registerStore.programCounter += 4;
        }

        if (isDebug) {
            setTerminalColour(VIOLET);
            printf("\nCURRENT CPU STATE\n\n");

            setTerminalColour(GREEN);
            printf("PC:  %08llx\n", registerStore.programCounter);

            setTerminalColour(BLUE);
            outputTerminal(&registerStore, &stateRegister, memPointer);

            setTerminalColour(YELLOW);
            printf("Proceed? (enter Y) ");

            char placeholder[256];
            scanf("%s", placeholder);
            printf("%s\n", placeholder);
        }
    }

    if (isDebug) {
        setTerminalColour(VIOLET);
        printf("\nFINAL CPU STATE\n\n");
        setTerminalColour(BLUE);
        outputTerminal(&registerStore, &stateRegister, memPointer);
        resetTerminalColour();
        return;
    }

    if (argc == 2) {
        outputTerminal(&registerStore, &stateRegister, memPointer);
    } else {
        outputFile(&registerStore, &stateRegister, memPointer, filename);
    }
}