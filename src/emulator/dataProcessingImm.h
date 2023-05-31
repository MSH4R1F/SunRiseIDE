//
// Created by Faraz Malik on 29/05/2023.
//

#ifndef ARMV8_27_DATAPROCESSINGIMM_H
#define ARMV8_27_DATAPROCESSINGIMM_H

//functions

#include <stdbool.h>
#include "registers.h"

bool isDataProcessingImm(long long op0);
void executeDataProcessingImm(long long instruction, struct Registers *registers);

bool isArithmeticProcessing(long long opi);

void executeArithmeticProcessingImm(long long instruction, struct Registers *registers);
void executeWideMoveProcessing(long long instruction, struct Registers *registers);

#endif //ARMV8_27_DATAPROCESSINGIMM_H
