#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char **argv) {
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
    char registers[8 * 31];
};

// MARK: Fetch-Decode-Execute cycle
long long fetchInstruction(long long programCounter);
// decoding
bool isDataProcessingImm(long long op0);
bool isDataProcessingReg(long long op0);
bool isBranch(long long op0);
// data processing (immediate)
void executeDataProcessingImm(long long instruction, struct Registers registers);

void executeDataProcessingReg(long long instruction, struct Registers registers);
void executeBranch(long long instruction, struct Registers registers);
void executeDataTransfer(long long instruction, struct Registers registers);

void processor() {
    struct PSTATE stateRegister = { false, false, false, false };
    struct Registers registers = { 0, 0, stateRegister, "" };

    long long instruction = fetchInstruction(registers.programCounter);
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
