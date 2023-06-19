#include "branch.h"

#include "key.h"
#include "tokenUtils.h"

/// Private functions declaration
static char *getBranchCondition(char *opcode);
static uint32_t assembleBranchConditional(char *condition, char *literal, long long currentAddress, LabelAddressMap **labelMap);
static uint32_t assembleBranchUnconditional(char *literal, long long currentAddress, LabelAddressMap **labelMap);
static uint32_t assembleBranchRegister(char *registerName);

/// Checks if the given opcode refers to a branch instruction
bool isBranch(char *opcode) {
    return (strcmp(opcode, "b") == 0 || strcmp(opcode, "br") == 0 || strstr(opcode, "b."));
}

/// Calls the correct helper function to return the binary representation for a branch instruction
uint32_t assembleBranch(char *opcode, char **operands, long long currentAddress, LabelAddressMap **labelMap) {
    if (strstr(opcode, ".")) {
        return assembleBranchConditional(getBranchCondition(opcode), operands[0], currentAddress, labelMap);
    } else {
        if (strcmp(opcode, "br") == 0) {
            return assembleBranchRegister(operands[0]);
        } else {
            return assembleBranchUnconditional(operands[0], currentAddress, labelMap);
        }
    }
}

/// Returns the binary instruction for a branch conditional instruction
static uint32_t assembleBranchConditional(char *condition, char *literal, long long currentAddress, LabelAddressMap **labelMap) {
    uint32_t instruction = 0;

    if (strstr(condition, "eq")) {
        instruction = 0;
    } else if (strstr(condition, "ne")) {
        instruction = 1;
    } else if (strstr(condition, "ge")) {
        instruction = 10;
    } else if (strstr(condition, "lt")) {
        instruction = 11;
    } else if (strstr(condition, "gt")) {
        instruction = 12;
    } else if (strstr(condition, "le")) {
        instruction = 13;
    } else {
        instruction = 14;
    }

    long long offset = encodeLiteralToOffset(literal, currentAddress, labelMap);
    long long simm19 = (offset / 4) & 0x7FFFF;

    instruction |= (simm19 << 5);
    instruction |= (0x15 << 26);

    return instruction;
}

/// Returns the binary instruction for an unconditional branch instruction
static uint32_t assembleBranchUnconditional(char *literal, long long currentAddress, LabelAddressMap **labelMap) {
    uint32_t instruction = 0;

    long long offset = encodeLiteralToOffset(literal, currentAddress, labelMap);
    long long simm26 = (offset / 4) & 0x3FFFFFF;

    instruction |= simm26;
    instruction |= (0x5 << 26);

    return instruction;
}

/// Returns the binary instruction for a branch register instruction
static uint32_t assembleBranchRegister(char *registerName) {
    uint32_t instruction = 0;

    instruction |= (encodeRegister(registerName) << 5);
    instruction |= (0x1F << 16);
    instruction |= (0x6B << 25);

    return instruction;
}

/// Returns the condition as a string ("ne" from "b.ne")
static char *getBranchCondition(char *opcode) {
    char opcodeDefinite[8];
    int i = 0;
    while (opcode[i] != '\0') {
        opcodeDefinite[i] = opcode[i];
        i++;
    }

    char* rest = opcodeDefinite;
    char* token = strtok_r(rest, ".", &rest);
    token = strtok_r(rest, ".", &rest);

    return createString(token);
}