//
// Created by Faraz Malik on 09/06/2023.
//

#include "registers.h"

long long loadFromRegister(uint32_t index, struct RegisterStore *registerStore, bool isDoubleWord) {
    long long data = registerStore->zeroRegister;
    if (index < 31) {
        data = registerStore->registers[index];
    }
    if (!isDoubleWord) {
        data &= 0xFFFFFFFF;
    }
    return data;
}

void storeToRegister(uint32_t index, long long data, struct RegisterStore *registerStore, bool isDoubleWord) {
    long long store = data;
    if (!isDoubleWord) {
        store &= 0xFFFFFFFF;
    }
    if (index < 31) {
        registerStore->registers[index] = store;
    }
}