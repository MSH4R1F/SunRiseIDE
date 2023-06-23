#include "key.h"

/// Returns the binary encoding of a register (24 for input R24)
uint32_t encodeRegister(char *operand) {
    if (strcmp(operand, "SP") == 0 || operand[1] == 'z') {
        return 0b11111;
    } else {
        operand++;
        return strtol(operand, NULL, 10);
    }
}

/// Returns the binary encoding for a simm (-34 for input #-34)
uint32_t encodeSimm(char *operand, bool hasHash) {
    if (hasHash) {
        operand++;
    }
    if (strstr(operand, "x")) {
        return strtol(operand, NULL, 16);
    } else {
        return strtol(operand, NULL, 10);
    }
}

/// Returns the binary encoding for an imm (34 for input #34)
uint32_t encodeImm(char *operand, bool hasHash) {
    return encodeSimm(operand, hasHash);
}

/// Returns the binary encoding for a shift depending on its type
uint32_t encodeShift(char *operand) {
    if (strcmp(operand, "lsl") == 0) {
        return 0;
    } else if (strcmp(operand, "lsr") == 0) {
        return 1;
    } else if (strcmp(operand, "asr") == 0) {
        return 2;
    } else {
        return 3;
    }
}

/// Calculates and returns the binary-encoded offset based on the literal provided
long long encodeLiteralToOffset(char *literal, long long currentAddress, LabelAddressMap **labelMap) {
    long long jumpAddress;
    if (mapContainsLabel(literal, labelMap)) {
        jumpAddress = getMapAddress(labelMap, literal);
    } else {
        jumpAddress = atoll(literal);
    }

    long long offset = jumpAddress - currentAddress;
    return offset;
}