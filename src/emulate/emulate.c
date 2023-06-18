#include <stdlib.h>
#include <stdbool.h>

#include "fileUtils.h"
#include "../general/memory.h"

#include "registers.h"
#include "bitwiseShift.h"

#include "dataProcessingImm.h"
#include "dataProcessingReg.h"
#include "singleDataTransfer.h"
#include "branch.h"

void run(uint8_t *memPointer, char* filename, int argc) {
    struct PSTATE stateRegister = { false, true, false, false };
    struct RegisterStore registerStore;
    registerStore.programCounter = 0;
    registerStore.zeroRegister = 0;
    registerStore.stateRegister = &stateRegister;

    for (int i = 0; i < sizeof(registerStore.registers) / sizeof(registerStore.registers[0]); i++) {
        registerStore.registers[i] = 0;
    }

    while (true) {
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
        } else if (isBranch(op0)) {
            executeBranch(instruction, &registerStore);
        } else {
            executeDataTransfer(instruction, memPointer, &registerStore);
            registerStore.programCounter += 4;
        }
    }

    if (argc == 2) {
        outputTerminal(&registerStore, &stateRegister, memPointer);
    } else {
        outputFile(&registerStore, &stateRegister, memPointer, filename);
    }
}

int processor(int argc, char **argv, bool isDebug) {
    uint8_t *memPointer = allocateMemory();
    loadMemoryFromFile(memPointer,argv[1]);
    if (argc == 2) {
        if (isDebug) {

        } else {
            run(memPointer, "output.txt", argc);
        }
    } else {
        if (isDebug) {

        } else {
            run(memPointer, argv[2], argc);
        }
    }
    free(memPointer);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    return processor(argc, argv, false);
}