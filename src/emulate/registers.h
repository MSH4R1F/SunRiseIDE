//
// Created by Faraz Malik on 09/06/2023.
//

#include <stdint.h>
#include <stdbool.h>

#ifndef ARMV8_27_REGISTERS_H
    #define ARMV8_27_REGISTERS_H

    struct PSTATE {
        bool negativeFlag;
        bool zeroFlag;
        bool carryFlag;
        bool overflowFlag;
    };

    struct RegisterStore {
        // SPECIAL
        long long zeroRegister;
        long long programCounter;
        struct PSTATE *stateRegister;

        // GENERAL
        long long registers[31];
    };

    long long loadFromRegister(uint32_t index, struct RegisterStore *registerStore, bool isDoubleWord);
    void storeToRegister(uint32_t index, long long data, struct RegisterStore *registerStore, bool isDoubleWord);
#endif //ARMV8_27_REGISTERS_H
