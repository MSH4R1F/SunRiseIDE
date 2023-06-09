//
// Created by Faraz Malik on 09/06/2023.
//

#include <stdint.h>
#include <stdbool.h>

#include "registers.h"

#ifndef ARMV8_27_DATAPROCESSINGIMM_H
    #define ARMV8_27_DATAPROCESSINGIMM_H

    bool isDataProcessingImm(long long op0);
    void executeDataProcessingImm(uint32_t instruction, struct RegisterStore *registers);

    bool isArithmeticProcessing(long long opi);

    void executeArithmeticProcessingImm(uint32_t instruction, struct RegisterStore *registerStore);
    void executeWideMoveProcessing(uint32_t instruction, struct RegisterStore *registers);
#endif //ARMV8_27_DATAPROCESSINGIMM_H
