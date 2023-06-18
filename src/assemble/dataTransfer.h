//
// Created by Faraz Malik on 18/06/2023.
//

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../general/labelMap.h"

#ifndef ARMV8_27_DATATRANSFER_H
    #define ARMV8_27_DATATRANSFER_H

    bool isDataTransfer(char *opcode);
    uint32_t assembleDataTransfer(char *opcode, char **operands, int operandLength, long long currentAddress, LabelAddressMap **labelMap);
#endif //ARMV8_27_DATATRANSFER_H
