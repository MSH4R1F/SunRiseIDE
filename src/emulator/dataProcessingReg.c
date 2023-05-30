//
// Created by Faraz Malik on 29/05/2023.
//

#include "dataProcessingReg.h"
#include "stdint.h"
#include "registers.c"
#include "stdbool.h"
#include "bitwiseshift.h"

long long shiftFun(uint32_t shift, long long reg, uint32_t operand) {
    if (shift == 0) {
        return logicalLeftShift(rm_val, operand);
    } else if (shift == 1) {
        return logicalRightShift(rm_val, operand) ;
    } else if (shift == 2) {
        return arithmeticRightShift(rm_val, operand);
    } else {
        return rotateRight(rm_val, shift);
    }
}

void executeDataProcessingReg(uint32_t instruction, struct Registers *registers) {
    uint32_t opr = (instruction >> 21) & 0x1F;
    uint32_t first2 = (opr >> 3);
    if (first2 == 1) {
        executeArithmeticProcessingReg(instruction, registers);
    } else if (first2 == 0) {
        executeLogicProcessingReg(instruction, registers);
    } else {
        executeMultiplyProcessingReg(instruction, registers);
    }
}

void executeArithmeticProcessingReg(uint32_t instruction, struct Registers *registers) {
    uint32_t shift = (instruction >> 22) & 0x3;
    uint32_t opr = (instruction >> 10) & 0x1F;
    uint32_t rn = (instruction >> 4) & 0x1F;
    long long rn_val = registers->registers[rn];
    uint32_t rm = (instruction >> 16) & 0x1F;
    long long rm_val = registers->registers[rm];
    uint32_t opc = (instruction >> 28) & 0x1;
    uint32_t rd = instruction & 0x1F;
    long long rd_val = 0;
    long long op2 = shift(shift, rm_val, opr);
    rd_val = rn_val + (-1**(opc / 2))*op2;
    if (opc % 2 == 1) {
        bool V = overunderflow(rn_val, (-1**(opc / 2))*op2, rd_val);
        registers->stateRegister.negativeFlag = rd_val < 0;
        registers->stateRegister.zeroFlag = rd_val == 0;
        registers->stateRegister.carryFlag = carry((-1**(opc / 2)) == 1, V);
        registers->stateRegister.overflowFlag = V;
    }
    registers->registers[rd] = rd_val;
}



}

bool overunderflow(long long val1, long long val2, long long result) {
    bool sign1 = val1 < 0;
    bool sign2 = val2 < 0;
    bool signResult = result < 0;
    if (sign1 == sign2 && sign1 != signResult) {
        return true;
    }
    return false
}


void executeLogicProcessingReg(uint32_t instruction, struct Registers *registers) {
    uint32_t shift = (instruction >> 22) & 0x3;
    uint32_t N = (instruction >> 21) & 0x1;
    uint32_t opc = (instruction >> 29) & 0x3;
    uint32_t operand = (instruction >> 10) & 0x1F;
    uint32_t rn = (instruction >> 5) & 0x1F;
    long long rn_val = registers->registers[rn];
    uint32_t rm = (instruction >> 16) & 0x1F;
    long long rm_val = registers->registers[rm];
    rm_val = shiftFun(shift, rm_val, operand)
    uint32_t combined_opc = (opc << 1) + N;

    uint32_t rd = instruction & 0x1F;
    if (rd < 30) {
        int64_t rd_val = 0;
        switch (combined_opc) {
            case 0:
                rd_val = rn_val & rm_val;
                break;
            case 1:
                rd_val = rn_val & ~rm_val;
                break;
            case 2:
                rd_val = rn_val | rm_val;
                break;
            case 3:
                rd_val = rn_val | ~rm_val;
                break;
            case 4:
                rd_val = rn_val ^ ~rm_val;
                break;
            case 5:
                rd_val = rn_val ^ rm_val;
                break;
            case 6:
                rd_val = rn_val & rm_val;
                registers->stateRegister.negativeFlag = rd_val < 0;
                registers->stateRegister.zeroFlag = rd_val == 0;
                registers->stateRegister.carryFlag = false;
                registers->stateRegister.overflowFlag = false;
                break;
            case 7:
                rd_val = rn_val & ~rm_val;
                registers->stateRegister.negativeFlag = rd_val < 0;
                registers->stateRegister.zeroFlag = rd_val == 0;
                registers->stateRegister.carryFlag = false;
                registers->stateRegister.overflowFlag = false;
                break;
        }
        registers->registers[rd] = rd_val;
    }
}

void executeMultiplyProcessingReg(uint32_t instruction, struct Registers *registers);

