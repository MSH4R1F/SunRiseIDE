//
// Created by Faraz Malik on 18/06/2023.
//
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifndef ARMV8_27_DATAPROCESSING_H
    #define ARMV8_27_DATAPROCESSING_H

    bool isDataProcessing(char *opcode);
    uint32_t assembleDataProcessing(char *opcode, char **operands, int operandLength);
#endif //ARMV8_27_DATAPROCESSING_H
