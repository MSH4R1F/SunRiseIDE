//
// Created by Faraz Malik on 09/06/2023.
//

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#ifndef ARMV8_27_MEMORY_H
    #define ARMV8_27_MEMORY_H

    uint8_t *allocateMemory(void);
    long long loadData(long long address, uint8_t *memPointer, bool isDoubleWord);
    void storeData(long long data, long long address, uint8_t *memPointer, bool isDoubleWord);
    uint32_t fetchInstruction(long long address, uint8_t *memPointer);
#endif //ARMV8_27_MEMORY_H
