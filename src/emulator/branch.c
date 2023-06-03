#include "branch.h"
#include "registers.c"

enum offsetType {
    UNCONDITIONAL,
    CONDITIONAL,
    INVALID,
    REGISTER
};

void executeBranch(long long instruction, struct Registers *registers) {
    enum offsetType branchType = instruction >> 29 & 0x3;
    if (branchType == UNCONDITIONAL) {
        int simm26 = instruction >> 6 & 0x3FFFFFF;
        registers->programCounter = simm26 * 4;
    } else if (branchType == REGISTER) {
        int xn = (instruction >> 5) & 0x1F;
        registers->programCounter = registers->registers[xn];
    } else {
        long long cond = instruction & 0xF;
        long long simm19 = (instruction >> 5) & 0xFFFFF;

        struct PSTATE *pstate = registers->stateRegister;
        switch (cond) {
            case 0x0: // EQ
                if (pstate->zeroFlag == 1) {
                    registers->programCounter = simm19 * 4;
                }
                break;
            case 0x1: // NE
                if (pstate->zeroFlag == 0) {
                    registers->programCounter = simm19 * 4;
                }
                break;
            case 0xA: // GE
                if (pstate->zeroFlag == pstate->overflowFlag) {
                    registers->programCounter = simm19 * 4;
                }
                break;
            case 0xB: // LT
                if (pstate->zeroFlag != pstate->overflowFlag) {
                    registers->programCounter = simm19 * 4;
                }
                break;
            case 0xC: // GT
                if (pstate->zeroFlag == 0 && pstate->negativeFlag == pstate->overflowFlag) {
                    registers->programCounter = simm19 * 4;
                }
                break;
            case 0xD: // LE
                if (!(pstate->zeroFlag == 0 && pstate->negativeFlag == pstate->overflowFlag)) {
                    registers->programCounter = simm19 * 4;
                }
                break;
            default: // AL
                registers->programCounter = simm19 * 4;
                break;
        }
    }
}
