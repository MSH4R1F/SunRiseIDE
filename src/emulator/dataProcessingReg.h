//
// Created by Faraz Malik on 29/05/2023.
//

#ifndef ARMV8_27_DATAPROCESSINGREG_H
#define ARMV8_27_DATAPROCESSINGREG_H

bool isDataProcessingReg(uint_32_t op0);

void executeDataProcessingReg(uint_32_t instruction, struct Registers *registers);

void executeArithmeticProcessingReg(uint_32_t instruction, struct Registers *registers);
void executeLogicProcessingReg(uint_32_t instruction, struct Registers *registers);
void executeMultiplyProcessingReg(uint_32_t instruction, struct Registers *registers);

#endif //ARMV8_27_DATAPROCESSINGREG_H
