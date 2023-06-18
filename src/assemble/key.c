//
// Created by Faraz Malik on 17/06/2023.
//

#include "key.h"

uint32_t encodeRegister(char *operand) {
    if (strcmp(operand, "SP") == 0 || operand[1] == 'z') {
        return 0b11111;
    } else {
        operand++;
        return strtol(operand, NULL, 10);
    }
}

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

uint32_t encodeImm(char *operand, bool hasHash) {
    return encodeSimm(operand, hasHash);
}

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