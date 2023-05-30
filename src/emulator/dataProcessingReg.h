//
// Created by Faraz Malik on 29/05/2023.
//

#ifndef ARMV8_27_DATAPROCESSINGREG_H
#define ARMV8_27_DATAPROCESSINGREG_H

bool isDataProcessingReg(uint32_t op0);

void executeDataProcessingReg(uint32_t instruction, struct Registers *registers);

void executeArithmeticProcessingReg(uint32_t instruction, struct Registers *registers);
void executeLogicProcessingReg(uint32_t instruction, struct Registers *registers);
void executeMultiplyProcessingReg(uint32_t instruction, struct Registers *registers);

#endif //ARMV8_27_DATAPROCESSINGREG_H
