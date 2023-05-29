#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "utils.h"

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
    char registers[31*8];
};

// MARK: Fetch-Decode-Execute cycle
// decoding
bool isDataProcessingReg(long long op0);
bool isBranch(long long op0);
// data processing (immediate)
void executeDataProcessingReg(long long instruction, struct Registers registers);
void executeBranch(long long instruction, struct Registers registers);
void executeDataTransfer(long long instruction, struct Registers registers);

void processor() {
    struct PSTATE stateRegister = { false, false, false, false };
    struct Registers registers = { 0, 0, stateRegister, "" };

    u_int32_t instruction = fetchInstruction(registers.programCounter, "src/add01_exp.bin");

    printf("%d", instruction);
    registers.programCounter += 4;
    u_int32_t instruction1 = fetchInstruction(registers.programCounter, "src/add01_exp.bin");
    printf("%d", instruction1);
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