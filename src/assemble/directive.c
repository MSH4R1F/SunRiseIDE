//
// Created by Faraz Malik on 18/06/2023.
//

#include "directive.h"

#include "key.h"

bool isDirective(char *opcode) {
    return strcmp(opcode, ".int") == 0;
}

uint32_t assembleDirective(char *opcode, char **operands, int operandLength) {
    return encodeSimm(operands[0], false);
}