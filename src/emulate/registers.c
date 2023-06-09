#include "registers.h"

/// Loads data from a register, checking for the zero register and masking appropriately
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

/// Stores data to a register, checking for the zero register and masking appropriately
void storeToRegister(uint32_t index, long long data, struct RegisterStore *registerStore, bool isDoubleWord) {
    long long store = data;
    if (!isDoubleWord) {
        store &= 0xFFFFFFFF;
    }
    if (index < 31) {
        registerStore->registers[index] = store;
    }
}