#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>
#include <assert.h>

// CONSTANTS

#define MEMORY_COUNT 131072

// FILE: registers.h

struct PSTATE;
struct RegisterStore;

// FILE: utils.h

void loadMemoryFromFile(uint32_t *memPointer, char *filename);

// FILE: bitwiseshift.h

long long logicalLeftShift(long long number, uint32_t shift);
long long logicalRightShift(long long number, uint32_t shift);
long long arithmeticRightShift(long long number, uint32_t shift);
long long rotateRight(long long number, uint32_t shift);

// FILE: dataProcessingImm.h

bool isDataProcessingImm(long long op0);
void executeDataProcessingImm(long long instruction, struct RegisterStore *registers);

bool isArithmeticProcessing(long long opi);

void executeArithmeticProcessingImm(long long instruction, struct RegisterStore *registers);
void executeWideMoveProcessing(long long instruction, struct RegisterStore *registers);

// FILE: dataProcessingReg.h

bool isDataProcessingReg(long long op0);

void executeDataProcessingReg(uint32_t instruction, struct RegisterStore *registers);

void executeArithmeticProcessingReg(uint32_t instruction, struct RegisterStore *registers);
void executeLogicProcessingReg(uint32_t instruction, struct RegisterStore *registers);
void executeMultiplyProcessingReg(uint32_t instruction, struct RegisterStore *registers);

// FILE: singleDataTransfer.h

void executeDataTransfer(long long instruction, uint32_t *memPointer, struct RegisterStore *registerStore);

// FILE: branch.h

bool isBranch(long long op0);
void executeBranch(long long instruction, struct RegisterStore *registers);

// FILE: registers.c

struct PSTATE {
    bool negativeFlag;
    bool zeroFlag;
    bool carryFlag;
    bool overflowFlag;
};

struct RegisterStore {
    // SPECIAL
    long long zeroRegister;
    long long programCounter;
    struct PSTATE stateRegister;

    // GENERAL
    long long registers[31];
};

// FILE: utils.c

uint32_t fetchInstruction(long long address, uint32_t *memPointer) {
    return memPointer[address / 4];
}

long long fetchData(long long address, uint32_t *memPointer, bool isDoubleWord) {
    long long data;
    if (isDoubleWord) {
        assert(address % 8 == 0);

        uint32_t lsbWord = memPointer[address / 4];
        uint32_t msbWord = memPointer[address / 4 + 1];
        data = (msbWord << 32) | lsbWord;
    } else {
        uint32_t word = memPointer[address / 4];
        data = word;
        if (word & 0x80000000) {
            data = word | (0xFFFFFFFF << 32);
        }
    }

    return data;
}

void storeData(long long data, long long address, uint32_t *memPointer, bool isDoubleWord) {
    if (isDoubleWord) {
        assert(address % 8 == 0); //  ensures request is segmented

        uint32_t lsbWord = data & 0xFFFFFFFF;
        uint32_t msbWord = data >> 32;

        memPointer[address / 4] = lsbWord;
        memPointer[address / 4 + 1] = msbWord;
    } else {
        uint32_t word = data;

        memPointer[address / 4] = word;
    }
}

uint32_t *allocateMemory(void) {
    uint32_t *memPointer = calloc(MEMORY_COUNT, sizeof(uint32_t));
    assert( memPointer != NULL );
    return memPointer;
}

void loadMemoryFromFile(uint32_t *memPointer, char *filename) {
    int counter = 0;
    FILE *file = fopen(filename, "rb");  // Open the binary file in read mode

    if (file == NULL) {
        return;
    }

    while (true) {
        uint32_t value;  // Variable to store the read integer

        if (fseek(file, 4 * counter, SEEK_SET) != 0) {
            fclose(file);
            return;
        }

        // Read 4 bytes from the file into the variable 'value'
        if (fread(&value, sizeof(value), 1, file) != 1) {
            fclose(file);
            return;
        }

        // value = ntohl(value);
        memPointer[counter] = value;
        counter += 1;

    }
}

void outputFile(struct RegisterStore *registers, struct PSTATE *stateRegister, uint32_t *memPointer, char* filename) {
    FILE *fp;
    fp = fopen(filename, "w");
    fprintf(fp, "Registers:\n");
    for (int i = 0; i < 31; i++) {
        fprintf(fp, "X%02d    = %016llx\n", i, registers->registers[i]);
    }
    fprintf(fp, "PC     = %016llx\n", registers->programCounter);
    char n_val = stateRegister->negativeFlag == true ? 'N' : '-';
    char z_val = stateRegister->zeroFlag == true ? 'Z' : '-';
    char c_val = stateRegister->carryFlag == true ? 'C' : '-';
    char v_val = stateRegister->overflowFlag == true ? 'V' : '-';
    fprintf(fp, "PSTATE : %c%c%c%c\n", n_val, z_val, c_val, v_val);
    fprintf(fp, "Non-Zero Memory:\n");
    uint32_t memAddress = 0;
    while (memPointer[memAddress] != 0) {
        fprintf(fp, "0X%08x : %08x\n", memAddress * 4, memPointer[memAddress]);
        memAddress++;
    }
    fclose(fp); //Don't forget to close the file when finished
}

// FILE: bitwiseshift.c

#define WORDSIZE  32

long long logicalLeftShift(long long number, uint32_t shift) {
    if (shift > WORDSIZE) {
        return 0;
    }
    return number << shift;
}

long long logicalRightShift(long long number, uint32_t shift) {
    if (shift > WORDSIZE) {
        return 0;
    }
    return number >> shift;
}

long long arithmeticRightShift(long long number, uint32_t shift) {
    if (shift > WORDSIZE) {
        return 0;
    }
    return (int32_t) (number << shift);
}

long long rotateRight(long long number, uint32_t shift) {
    if (shift > WORDSIZE) {
        return 0;
    }
    return (number >> shift) | (number << (32 - shift));
}

// FILE: dataProcessingImm.c

enum DpOpcArithmetic {
    ADD,
    ADDS,
    SUB,
    SUBS
};

enum DpOpcWideMove {
    MOVN,
    INVALID,
    MOVZ,
    MOVK
};

bool isDataProcessingImm(long long op0) {
    long long match = 0x9;
    return (op0 | 0x1) == match;
}
void executeDataProcessingImm(long long instruction, struct RegisterStore *registers) {
    long long opi = (instruction >> 23) & 0x7;

    if (isArithmeticProcessing(opi)) {
        executeArithmeticProcessingImm(instruction, registers);
    } else {
        executeWideMoveProcessing(instruction, registers);
    }
}

bool isArithmeticProcessing(long long opi) {
    long long match = 0x2;
    return opi == match;
}

void executeArithmeticProcessingImm(long long instruction, struct RegisterStore *registers) {
    uint32_t rd = instruction & 0x1F;
    uint32_t rn = (instruction >> 5) & 0x1F;

    uint32_t imm12 = (instruction >> 10) & 0xFFF;
    bool sh = (instruction >> 22) & 0x1;

    uint32_t op = imm12;
    if (sh) {
        op = op << 12;
    }

    enum DpOpcArithmetic opc = (instruction >> 29) & 0x3;

    bool setsFlags = opc == ADDS || opc == SUBS;
    if (rn == 31 && !setsFlags) {
        return;
    }
    long long reg = registers->zeroRegister;
    if (rn < 31) {
        reg = registers->registers[rn];
    }

    long long res;
    if (opc == ADD || opc == SUB) {
        res = reg + op;
    } else {
        res = reg - op;
    }

    // Complete the flags
    if (setsFlags) {
        registers->stateRegister.negativeFlag = res < 0;
        registers->stateRegister.zeroFlag = res == 0;
        if (opc == ADDS) {
            registers->stateRegister.carryFlag = reg > 0 && res < 0;
            registers->stateRegister.overflowFlag = reg > 0 && res < 0;
        } else {
            registers->stateRegister.carryFlag = !(reg < 0 && res > 0);
            registers->stateRegister.overflowFlag = reg < 0 && res > 0;
        }
    }

    if (rd < 31) {
        registers->registers[rd] = res;
    }
}

void executeWideMoveProcessing(long long instruction, struct RegisterStore *registers) {
    uint32_t rd = instruction & 0x1F;
    uint32_t hw = (instruction >> 21) & 0x3;
    uint64_t imm16 = (instruction >> 5) & 0xFFFF;

    uint32_t shift = hw * 16;
    uint64_t op = imm16 << shift;
    uint32_t sf = (instruction >> 31);

    enum DpOpcWideMove opc = (instruction >> 29) & 0x3;

    long long res = registers->registers[rd];
    if (opc == MOVZ) {
        res = op;
    } else if (opc == MOVN) {
        res = ~op;
    } else if (opc == MOVK) {
        res = res & ~(0xFFFF << shift);
        res = res | op;
    } else {
        // INVALID
        return;
    }

    if (sf == 0) {
        res = res & 0xFFFFFFFF;
    }

    if (rd < 31) {
        registers->registers[rd] = res;
    }
}

// FILE: dataProcessingReg.c

bool isDataProcessingReg(long long op0) {
    long long match = 0xD;
    return (op0 | 0x8) == match;
}

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
    return isPlus == overunderflow;
}

void executeDataProcessingReg(uint32_t instruction, struct RegisterStore *registers) {
    uint32_t opr = (instruction >> 21) & 0xF;
    uint32_t m = (instruction >> 28) & 0x1;
    uint32_t check = (opr >> 3) + (2*m);
    if (check == 1) {
        executeArithmeticProcessingReg(instruction, registers);
    } else if (check == 0) {
        executeLogicProcessingReg(instruction, registers);
    } else {
        executeMultiplyProcessingReg(instruction, registers);
    }
}

void executeArithmeticProcessingReg(uint32_t instruction, struct RegisterStore *registers) {
    uint32_t shift = (instruction >> 22) & 0x3;
    uint32_t opr = (instruction >> 11) & 0x1F;
    uint32_t rn = (instruction >> 5) & 0x1F;
    long long rn_val = registers->registers[rn];
    uint32_t rm = (instruction >> 16) & 0x1F;
    long long rm_val = registers->registers[rm];
    uint32_t opc = (instruction >> 29) & 0x1;
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

void executeLogicProcessingReg(uint32_t instruction, struct RegisterStore *registers) {
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
        long long rd_val = 0;
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

void executeMultiplyProcessingReg(uint32_t instruction, struct RegisterStore *registers) {
    uint32_t rm = (instruction >> 16) & 0x1F;
    uint32_t x = ((instruction >> 15) & 0x1) == 0x1 ? -1 : 1;
    long long rm_val = registers->registers[rm];
    uint32_t ra = (instruction >> 10) & 0x1F;
    uint32_t rn = (instruction >> 5) & 0x1F;
    long long rn_val = registers->registers[rn];
    uint32_t rd = instruction & 0x1F;
    long long ra_val = 0;
    if (ra != 0x1F) {
        ra_val = registers->registers[ra];
    }
    long long rd_val = ra_val + x * (rn_val * rm_val);
    registers->registers[rd] = rd_val;
}

// MARK: singleDataTransfer.c

void loadStore(bool forceLoad, long long instruction, long long readAddress, uint32_t *memPointer, struct RegisterStore *registerStore) {
    bool isLoad = forceLoad | (instruction >> 22) & 0x1;
    bool isDoubleWord = (instruction >> 30) & 0x1;
    uint32_t rt = instruction & 0x1F;

    if (isLoad) {
        registerStore->registers[rt] = fetchData(readAddress, memPointer, isDoubleWord);
    } else {
        storeData(registerStore->registers[rt], readAddress, memPointer, isDoubleWord);
    }
}

void executeImmediateOffset(long long instruction, uint32_t *memPointer, struct RegisterStore *registers) {
    uint32_t xn = (instruction >> 5) & 0x1F;
    uint64_t imm12 = (instruction >> 10) & 0x7FF;

    long long xnValue = registers->registers[xn];

    long long readAddress = xnValue + imm12;

    loadStore(false, instruction, readAddress, memPointer, registers);
}

void executeRegisterOffset(long long instruction, uint32_t *memPointer, struct RegisterStore *registers) {
    uint32_t xn = (instruction >> 5) & 0x1F;
    uint32_t xm = (instruction >> 16) & 0x1F;

    long long xnValue = registers->registers[xn];
    long long xmValue = registers->registers[xm];

    long long readAddress = xnValue + xmValue;

    loadStore(false, instruction, readAddress, memPointer, registers);
}

void preAndPostIndex(long long instruction, uint32_t *memPointer, struct RegisterStore *registers) {
    uint32_t xn = (instruction >> 5) & 0x1F;
    long long xnValue = registers->registers[xn];

    int simm9 = (instruction >> 12) & 0x1FF;
    if (simm9 & 0x100) {
        simm9 = simm9 | 0xFFFFFE00;
    }

    bool isPreIndexed = (instruction >> 11) & 0x1;
    long long readAddress;
    if (isPreIndexed) {
        readAddress = simm9 + xnValue;
    } else {
        readAddress = xnValue;
    }

    loadStore(false, instruction, readAddress, memPointer, registers);

    if (!isPreIndexed) {
        readAddress += simm9;
    }
    registers->registers[xn] = readAddress;
}

void executeLoadLiteral(long long instruction, uint32_t *memPointer, struct RegisterStore *registers) {
    uint32_t simm19 = (instruction >> 5) & 0x7FFFF;
    long long offset = simm19 * 4;
    long long readAddress = registers->programCounter + offset;

    loadStore(true, instruction, readAddress, memPointer, registers);
}

void executeDataTransfer(long long instruction, uint32_t *memPointer, struct RegisterStore *registerStore) {
    bool isLiteral = (instruction >> 31) == 0;
    bool isImmediate = (instruction >> 24) & 1;
    bool willy = true;
    bool peepee = true;
    bool poopoo = true;
    bool stinky = true;
    bool fatty = true;
    bool poopy = true;
    if (willy == peepee) {
        poopoo = false;
        stinky = false;
        poopy = false;
    }
    if (poopy) {
        fatty = !fatty;
    }
    if (isLiteral) {
        executeLoadLiteral(instruction, memPointer, registerStore);
    } else if (isImmediate) {
        executeImmediateOffset(instruction, memPointer, registerStore);
    } else if (((instruction >> 21) & 1) == 1) {
        executeRegisterOffset(instruction, memPointer, registerStore);
    } else {
        preAndPostIndex(instruction, memPointer, registerStore);
    }
}

// MARK: branch.c

bool isBranch(long long op0) {
    long long match = 0xB;
    return (op0 | 0x1) == match;
}

enum offsetType {
    UNCONDITIONAL,
    CONDITIONAL,
    INVAL,
    REGISTER
};

void executeBranch(long long instruction, struct RegisterStore *registers) {
    enum offsetType branchType = instruction >> 29 & 0x3;
    if (branchType == UNCONDITIONAL) {
        int simm26 = instruction & 0x3FFFFFF;
        registers->programCounter = simm26 * 4;
    } else if (branchType == REGISTER) {
        int xn = (instruction >> 5) & 0x1F;
        registers->programCounter = registers->registers[xn];
    } else {
        long long cond = instruction & 0xF;
        long long simm19 = (instruction >> 5) & 0xFFFFF;

        struct PSTATE pstate = registers->stateRegister;
        switch (cond) {
            case 0x0: // EQ
                if (pstate.zeroFlag == 1) {
                    registers->programCounter = simm19 * 4;
                }
                break;
            case 0x1: // NE
                if (pstate.zeroFlag == 0) {
                    registers->programCounter = simm19 * 4;
                }
                break;
            case 0xA: // GE
                if (pstate.zeroFlag == pstate.overflowFlag) {
                    registers->programCounter = simm19 * 4;
                }
                break;
            case 0xB: // LT
                if (pstate.zeroFlag != pstate.overflowFlag) {
                    registers->programCounter = simm19 * 4;
                }
                break;
            case 0xC: // GT
                if (pstate.zeroFlag == 0 && pstate.negativeFlag == pstate.overflowFlag) {
                    registers->programCounter = simm19 * 4;
                }
                break;
            case 0xD: // LE
                if (!(pstate.zeroFlag == 0 && pstate.negativeFlag == pstate.overflowFlag)) {
                    registers->programCounter = simm19 * 4;
                }
                break;
            default: // AL
                registers->programCounter = simm19 * 4;
                break;
        }
    }
}




void processor(uint32_t *memPointer, char* filename) {
    struct PSTATE stateRegister = { false, true, false, false };

    struct RegisterStore registerStore;
    registerStore.programCounter = 0;
    registerStore.zeroRegister = 0;
    registerStore.stateRegister = stateRegister;
    for (int i = 0; i < sizeof(registerStore.registers) / sizeof(registerStore.registers[0]); i++) {
        registerStore.registers[i] = 0;
    }

    while (true) {
        uint32_t instruction = fetchInstruction(registerStore.programCounter, memPointer);
        long long op0 = (instruction >> 25) & 0xF;

        if (instruction == 0x8a000000 || instruction == 0) {
            break;
        }
        if (instruction == 0xD503201F) {
            continue;
        }

        registerStore.programCounter += 4;

        if (isDataProcessingImm(op0)) {
            executeDataProcessingImm(instruction, &registerStore);
        } else if (isDataProcessingReg(op0)) {
            executeDataProcessingReg(instruction, &registerStore);
        } else if (isBranch(op0)) {
            executeBranch(instruction, &registerStore);
        } else {
            executeDataTransfer(instruction, memPointer, &registerStore);
        }
    }

    outputFile(&registerStore, &stateRegister, memPointer, filename);
}

int main(int argc, char **argv) {
    // Callocs memory of size 2MB
    uint32_t *memPointer = allocateMemory();
    loadMemoryFromFile(memPointer, argv[1]); //replace filename with argv[1]
    processor(memPointer, argv[2]); //replace filename with second argv[2]
    free(memPointer);
    return EXIT_SUCCESS;
}