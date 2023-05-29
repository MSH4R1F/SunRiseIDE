//
// Created by Faraz Malik on 29/05/2023.
//

#ifndef ARMV8_27_DATAPROCESSINGIMM_H
#define ARMV8_27_DATAPROCESSINGIMM_H

//functions

bool isDataProcessingImm(long long op0);
void executeDataProcessingImm(long long instruction, struct Registers registers);

#endif //ARMV8_27_DATAPROCESSINGIMM_H
