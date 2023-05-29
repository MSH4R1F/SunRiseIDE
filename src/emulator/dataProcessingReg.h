//
// Created by Faraz Malik on 29/05/2023.
//

#ifndef ARMV8_27_DATAPROCESSINGREG_H
#define ARMV8_27_DATAPROCESSINGREG_H

bool isDataProcessingReg(long long op0);

void executeDataProcessingReg(long long instruction, struct Registers registers);

bool isArithmetic(long long opr);
bool isLogic(long long opr);

void executeArithmeticProcessingReg(long long instruction, struct Registers registers);
void executeLogicProcessingReg(long long instruction, struct Registers registers);
void executeMultiplyProcessingReg(long long instruction, struct Registers registers);

#endif //ARMV8_27_DATAPROCESSINGREG_H
