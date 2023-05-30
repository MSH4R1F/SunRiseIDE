//
// Created by Faraz Malik on 29/05/2023.
//

#include "dataProcessingReg.h"
#include "registers.c"
#include "stdbool.h"
#include "bitwiseshift.h"

long long shiftFun(uint32_t shift, long long reg, uint32_t operand) {
    if (shift == 0) {
        return logicalLeftShift(reg, operand);
    } else if (shift == 1) {
        return logicalRightShift(reg, operand) ;
    } else if (shift == 2) {
        return arithmeticRightShift(reg, operand);
    } else {
        return rotateRight(reg, shift);
    }
}

bool overunderflow(long long val1, long long val2, long long result) {
    bool sign1 = val1 < 0;
    bool sign2 = val2 < 0;
    bool signResult = result < 0;
    if (sign1 == sign2 && sign1 != signResult) {
        return true;
    }
    return false;
}

bool carry(bool isPlus, bool overunderflow) {
    if (isPlus) {
        if (overunderflow) {
            return 1;
        }
        return 0;
    } else {
        if (overunderflow) {
            return 0;
        }
        return 1;
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
    long long rd_val;
    long long op2 = shiftFun(shift, rm_val, opr);
    int multiplier = -1;
    if (opc / 2 == 0) {
        multiplier *= -1;
    }
    rd_val = rn_val + multiplier*op2;
    if (opc % 2 == 1) {
        bool V = overunderflow(rn_val, multiplier*op2, rd_val);
        registers->stateRegister.negativeFlag = rd_val < 0;
        registers->stateRegister.zeroFlag = rd_val == 0;
        registers->stateRegister.carryFlag = carry(multiplier == 1, V);
        registers->stateRegister.overflowFlag = V;
    }
    registers->registers[rd] = rd_val;
}





void elxecuteLogicProcessingReg(uint32_t instruction, struct Registers *registers) {
    uint32_t shift = (instruction >> 22) & 0x3;
    uint32_t N = (instruction >> 21) & 0x1;
    uint32_t opc = (instruction >> 29) & 0x3;
    uint32_t operand = (instruction >> 10) & 0x1F;
    uint32_t rn = (instruction >> 5) & 0x1F;
    long long rn_val = registers->registers[rn];
    uint32_t rm = (instruction >> 16) & 0x1F;
    long long rm_val = registers->registers[rm];
    rm_val = shiftFun(shift, rm_val, operand);
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

