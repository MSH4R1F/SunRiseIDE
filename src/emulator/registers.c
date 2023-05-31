//
// Created by Faraz Malik on 30/05/2023.
//

#include "stdbool.h"
#include "registers.h"


struct PSTATE {
    bool negativeFlag; // N
    bool zeroFlag; // Z
    bool carryFlag; // C
    bool overflowFlag; // V
};

struct Registers {
    // SPECIAL
    long long zeroRegister;
    long long programCounter;
    struct PSTATE* stateRegister;

    // GENERAL
    long long registers[31];
};