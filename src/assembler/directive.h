//
// Created by Faraz Malik on 18/06/2023.
//
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifndef ARMV8_27_DIRECTIVE_H
    #define ARMV8_27_DIRECTIVE_H

    bool isDirective(char *opcode);
    uint32_t assembleDirective(char *opcode, char **operands, int operandLength);
#endif //ARMV8_27_DIRECTIVE_H
