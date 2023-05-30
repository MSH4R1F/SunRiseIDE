//
// Created by Faraz Malik on 29/05/2023.
//

#include "dataProcessingReg.h"
#include "stdint.h"
#include "registers.c"
#include "stdbool.h"

void executeDataProcessingReg(uint_32_t instruction, struct Registers *registers) {
    uint_32_t opr = (instruction >> 21) & 0x1F;
    uint_32_t first2 = (opr >> 3);
    if (first2 == 1) {
        executeArithmeticProcessingReg(instruction, *registers);
    } else if (first2 == 0) {
        executeLogicProcessingReg(instruction, *registers);
    } else {
        executeMultiplyProcessingReg(instruction, *registers);
    }
}

void executeArithmeticProcessingReg(uint_32_t instruction, struct Registers *registers) {
    uint32_t shift = (instruction >> 22) & 0x3;
    uint32_t rn = (instruction >> 4) & 0x1F;
    uint32_t rn_val registers->registers[rn]
    uint32_t rm = (instruction >> 16) & 0x1F;
    uint64_t rm_val = registers->registers[rm];
    uint32_t opc = (instruction >> 28) & 0x1;
    uint32_t rd = instruction & 0x1F;
    uint64_t rd_val = 0;
    op2 = shift(shift);
    rd_val = rn_val + (-1**(opc / 2))*op2;
    if (opc % 2 == 1) {

    }
}


void executeLogicProcessingReg(uint32_t instruction, struct Registers *registers) {
    uint32_t shift = (instruction >> 22) & 0x3;
    uint32_t N = (instruction >> 21) & 0x1;
    uint32_t opc = (instruction >> 29) & 0x3;
    uint32_t rn = (instruction >> 5) & 0x1F;
    long long rn_val = registers->registers[rm];
    uint32_t rm = (instruction >> 16) & 0x1F;
    long long rm_val = registers->registers[rm];
    if (shift == 0) {
        rm_val = ;//lsl(rm_val, shift)
    } else if (shift == 1) {
        rm_val = ;//lsr(rm_val, shift)
    } else if (shift == 2) {
        rm_val = ;//asr(rm_val, shift)
    } else {
        rm_val = ;//ror(rm_val, shift)
    }
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
                if (rd_val < 0) {
                    registers->stateRegister.negativeFlag = true;
                } if (rd_val == 0) {
                    registers->stateRegister.zeroFlag = true;
                }
                registers->stateRegister.carryFlag = true;
                registers->stateRegister.overflowFlag = true;
                break;
            case 7:
                rd_val = rn_val & ~rm_val;
                if (rd_val < 0) {
                    registers->stateRegister.negativeFlag = true;
                } if (rd_val == 0) {
                    registers->stateRegister.zeroFlag = true;
                }
                registers->stateRegister.carryFlag = true;
                registers->stateRegister.overflowFlag = true;
                break;
        }
    }
}

void executeMultiplyProcessingReg(uint32_t instruction, struct Registers *registers);

