//
// Created by Faraz Malik on 29/05/2023.
//

#ifndef ARMV8_27_BRANCH_H
#define ARMV8_27_BRANCH_H

bool isBranch(long long op0);
void executeBranch(long long instruction, struct Registers registers);

#endif //ARMV8_27_BRANCH_H
