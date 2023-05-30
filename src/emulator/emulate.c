#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "dataProcessingImm.h"
#include "utils.h"
#include "dataProcessingReg.h"
#include "singleDataTransfer.h"
#include "branch.h"

void processor();

int main(int argc, char **argv) {
    processor();
    return EXIT_SUCCESS;
}

// MARK: Registers and state

struct PSTATE {
    bool negativeFlag;
    bool zeroFlag;
    bool carryFlag;
    bool overflowFlag;
};

struct Registers {
    // SPECIAL
    long long zeroRegister;
    long long programCounter;
    struct PSTATE stateRegister;

    // GENERAL
    long long registers[31*8];
};

// MARK: Fetch-Decode-Execute cycle

void processor() {
    struct PSTATE stateRegister = { false, false, false, false };
    struct Registers registers = { 0, 0, stateRegister, "" };

    uint32_t instruction = fetchInstruction(registers.programCounter, "../add01_exp.bin");
    long long op0 = (instruction >> 25) & 0xF;

    if (isDataProcessingImm(op0)) {
        executeDataProcessingImm(instruction, registers);
    } else if (isDataProcessingReg(op0)) {
        executeDataProcessingReg(instruction, registers);
    } else if (isBranch(op0)) {
        executeBranch(instruction, registers);
    } else {
        executeDataTransfer(instruction, registers);
    }
}

// MARK: Data Processing Instruction (immediate)

enum DpOpcArithmetic {
    ADD,
    ADDS,
    SUB,
    SUBS
};

enum DpOpcWideMove {
    MOVZ,
    INVALID,
    MOVN,
    MOVK
};

enum ShiftOP {
    LSL,
    LSR,
    ASR,
    ROR
};
