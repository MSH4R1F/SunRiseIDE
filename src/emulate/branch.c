#include "branch.h"

/// Returns whether op0 corresponds with branch
bool isBranch(long long op0) {
    long long match = 0xB;
    return (op0 | 0x1) == match;
}

/// Executes the branch instruction
void executeBranch(uint32_t instruction, struct RegisterStore *registerStore) {
    enum OffsetType branchType = instruction >> 30 & 0x3;
    //
    if (branchType == UNCONDITIONAL) {
        long long simm26 = (instruction & 0x3FFFFFF) << 2; // 000101 + 26 bits simm26

        if (simm26 & 0x8000000) {
            uint64_t signExtend = 0x3FFFFFFFFF << 26;
            simm26 = simm26 | signExtend;
        }

        registerStore->programCounter += simm26;
    } else if (branchType == REGISTER) {
        uint32_t xn = (instruction >> 5) & 0x1F;
        registerStore->programCounter = loadFromRegister(xn, registerStore, true);
    } else {
        enum BranchCondition cond = instruction & 0xF;

        long long simm19 = ((instruction >> 5) & 0x7FFFF);

        if (simm19 & 0x40000) {
            uint64_t signExtend = 0x1FFFFFFFFFFF << 19;
            simm19 = simm19 | signExtend;
        }

        struct PSTATE *pstate = registerStore->stateRegister;
        switch (cond) {
            case EQ: // EQ
                if (pstate->zeroFlag) {
                    registerStore->programCounter += simm19 * 4;
                } else {
                    registerStore->programCounter += 4;
                }
                break;
            case NE: // NE
                if (!pstate->zeroFlag) {
                    registerStore->programCounter += simm19 * 4;
                } else {
                    registerStore->programCounter += 4;
                }
                break;
            case GE: // GE
                if (pstate->zeroFlag == pstate->overflowFlag) {
                    registerStore->programCounter += simm19 * 4;
                } else {
                    registerStore->programCounter += 4;
                }
                break;
            case LT: // LT
                if (pstate->negativeFlag != pstate->overflowFlag) {
                    registerStore->programCounter += simm19 * 4;
                } else {
                    registerStore->programCounter += 4;
                }
                break;
            case GT: // GT
                if (pstate->zeroFlag == 0 && pstate->negativeFlag == pstate->overflowFlag) {
                    registerStore->programCounter += simm19 * 4;
                } else {
                    registerStore->programCounter += 4;
                }
                break;
            case LE: // LE
                if (!(pstate->zeroFlag == 0 && pstate->negativeFlag == pstate->overflowFlag)) {
                    registerStore->programCounter += simm19 * 4;
                } else {
                    registerStore->programCounter += 4;
                }
                break;
            default: // AL
                registerStore->programCounter += simm19 * 4;
                break;
        }
    }
}