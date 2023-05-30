//
// Created by Faraz Malik on 30/05/2023.
//

#include "stdbool.h"
#include "registers.h"


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
    long long registers[31];
};