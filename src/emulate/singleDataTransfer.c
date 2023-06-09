//
// Created by Faraz Malik on 09/06/2023.
//

#include "singleDataTransfer.h"

#include "memory.h"

// Private function declarations
static void loadStore(bool forceLoad, uint32_t instruction, long long readAddress, uint8_t *memPointer, struct RegisterStore *registerStore);
static void executeImmediateOffset(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registers);
static void executeRegisterOffset(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registers);
static void preAndPostIndex(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registers);
static void executeLoadLiteral(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registers);

/// Determines which type of single data transfer instruction to execute based on the opcode
void executeDataTransfer(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registerStore) {
    bool isLiteral = (instruction >> 31) == 0;
    bool isImmediate = (instruction >> 24) & 1;
    if (isLiteral) {
        executeLoadLiteral(instruction, memPointer, registerStore);
    } else if (isImmediate) {
        executeImmediateOffset(instruction, memPointer, registerStore);
    } else if (((instruction >> 21) & 1) == 1) {
        executeRegisterOffset(instruction, memPointer, registerStore);
    } else {
        preAndPostIndex(instruction, memPointer, registerStore);
    }
}

void loadStore(bool forceLoad, uint32_t instruction, long long readAddress, uint8_t *memPointer, struct RegisterStore *registerStore) {
    bool isLoad = forceLoad | ((instruction >> 22) & 0x1);
    bool sf = (instruction >> 30) & 0x1;
    uint32_t rt = instruction & 0x1F;

    if (isLoad) {
        long long result = loadData(readAddress, memPointer, sf);
        storeToRegister(rt, result, registerStore, sf);
    } else {
        storeData(registerStore->registers[rt], readAddress, memPointer, sf);
    }
}

void executeImmediateOffset(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registers) {
    bool sf = instruction >> 31;

    uint32_t xn = (instruction >> 5) & 0x1F;
    uint64_t imm12 = (instruction >> 10) & 0xFFF;

    if (sf) {
        imm12 *= 8;
    } else {
        imm12 *= 4;
    }

    long long xnValue = loadFromRegister(xn, registers, true);

    long long readAddress = xnValue + imm12;

    loadStore(false, instruction, readAddress, memPointer, registers);
}

void executeRegisterOffset(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registers) {
    uint32_t xn = (instruction >> 5) & 0x1F;
    uint32_t xm = (instruction >> 16) & 0x1F;

    long long xnValue = loadFromRegister(xn, registers, true);
    long long xmValue = loadFromRegister(xm, registers, true);

    long long readAddress = xnValue + xmValue;

    loadStore(false, instruction, readAddress, memPointer, registers);
}

void preAndPostIndex(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registers) {
    uint32_t xn = (instruction >> 5) & 0x1F;
    long long xnValue = loadFromRegister(xn, registers, true);

    int simm9 = (instruction >> 12) & 0x1FF;
    if (simm9 & 0x100) {
        simm9 = simm9 | 0xFFFFFE00;
    }

    bool isPreIndexed = (instruction >> 11) & 0x1;
    long long readAddress;
    if (isPreIndexed) {
        readAddress = simm9 + xnValue;
    } else {
        readAddress = xnValue;
    }

    loadStore(false, instruction, readAddress, memPointer, registers);

    if (!isPreIndexed) {
        readAddress += simm9;
    }

    storeToRegister(xn, readAddress, registers, true);
}

void executeLoadLiteral(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registers) {
    int simm19 = (instruction >> 5) & 0x7FFFF;

    if (simm19 & 0x40000) {
        simm19 = simm19 | 0xFFF80000;
    }

    long long offset = simm19 * 4;

    long long readAddress = registers->programCounter + offset;

    loadStore(true, instruction, readAddress, memPointer, registers);
}

void executeDataTransfer(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registerStore) {
    bool isLiteral = (instruction >> 31) == 0;
    bool isImmediate = (instruction >> 24) & 1;
    if (isLiteral) {
        executeLoadLiteral(instruction, memPointer, registerStore);
    } else if (isImmediate) {
        executeImmediateOffset(instruction, memPointer, registerStore);
    } else if (((instruction >> 21) & 1) == 1) {
        executeRegisterOffset(instruction, memPointer, registerStore);
    } else {
        preAndPostIndex(instruction, memPointer, registerStore);
    }
}