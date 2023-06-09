//
// Created by Faraz Malik on 09/06/2023.
//

#include <stdbool.h>
#include <stdint.h>

#include "registers.h"

#ifndef ARMV8_27_DATAPROCESSINGREG_H
    #define ARMV8_27_DATAPROCESSINGREG_H

    bool isDataProcessingReg(long long op0);

    void executeDataProcessingReg(uint32_t instruction, struct RegisterStore *registers);
#endif //ARMV8_27_DATAPROCESSINGREG_H
