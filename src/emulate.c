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
