#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "utils.c"
#include "dataProcessingImm.c"
#include "dataProcessingReg.c"
#include "singleDataTransfer.c"
#include "branch.c"
#include "registers.h"

void processor();

int main(int argc, char **argv) {
    processor();
    return EXIT_SUCCESS;
}

// MARK: Fetch-Decode-Execute cycle

void processor() {
    struct PSTATE stateRegister = { false, false, false, false };

    struct Registers registers;
    registers.programCounter = 0;
    registers.zeroRegister = 0;
    registers.stateRegister = stateRegister;

    uint32_t instruction = fetchInstruction(registers.programCounter, "../add01_exp.bin");
    long long op0 = (instruction >> 25) & 0xF;

    if (isDataProcessingImm(op0)) {
        executeDataProcessingImm(instruction, &registers);
    } else if (isDataProcessingReg(op0)) {
        executeDataProcessingReg(instruction, &registers);
    } else if (isBranch(op0)) {
        executeBranch(instruction, registers);
    } else {
        executeDataTransfer(instruction, registers);
    }
}

// MARK: Data Processing Instruction (immediate)

enum ShiftOP {
    LSL,
    LSR,
    ASR,
    ROR
};
