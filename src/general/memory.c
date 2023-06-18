#include "memory.h"

#include "constants.h"

/// Loads data from memory, converting from little endian
long long loadData(long long address, uint8_t *memPointer, bool isDoubleWord) {
    int bytesCount = 4;
    if (isDoubleWord) {
        bytesCount = 8;
    }

    long long data = 0;
    for (int i = 0; i < bytesCount; i++) {
        long long currentByte = memPointer[address + i];
        uint32_t significance = 8 * i;
        data = data | (currentByte << significance);
    }

    if (!isDoubleWord) {
        if (data & 0x80000000) {
            long long extend = 0xFFFFFFFF;
            data = data | (extend << 32);
        }
    }

    return data;
}

/// Fetches an instruction from a specific address in memory
uint32_t fetchInstruction(long long address, uint8_t *memPointer) {
    uint32_t data = loadData(address, memPointer, false);
    return data;
}

/// Stores data to memory, converting to little endian
void storeData(long long data, long long address, uint8_t *memPointer, bool isDoubleWord) {
    int bytesCount = 4;
    if (isDoubleWord) {
        bytesCount = 8;
    }

    for (int i = 0; i < bytesCount; i++) {
        int significance = 8 * i;
        uint8_t currentByte = data >> significance;

        memPointer[address + i] = currentByte;
    }
}

/// Allocates the 2MB memory on the heap and clears it
uint8_t *allocateMemory(void) {
    uint8_t *memPointer = calloc(MEMORY_COUNT, sizeof(uint8_t));
    assert( memPointer != NULL );
    return memPointer;
}