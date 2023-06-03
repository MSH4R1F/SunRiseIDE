#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>
#include <assert.h>

// CONSTANTS

#define MEMORY_COUNT 2097152

// FILE: registers.h

struct PSTATE;
struct RegisterStore;

// FILE: utils.h

void loadMemoryFromFile(uint8_t *memPointer, char *filename);

// FILE: bitwiseshift.h

uint64_t logicalLeftShift(uint64_t number, uint32_t shift, bool is64Bit);
uint64_t logicalRightShift(uint64_t number, uint32_t shift, bool is64Bit);
long long arithmeticRightShift(long long number, uint32_t shift, bool is64Bit);
long long rotateRight(long long number, uint32_t shift, bool is64Bit);

// FILE: dataProcessingImm.h

bool isDataProcessingImm(long long op0);
void executeDataProcessingImm(uint32_t instruction, struct RegisterStore *registers);

bool isArithmeticProcessing(long long opi);

void executeArithmeticProcessingImm(uint32_t instruction, struct RegisterStore *registerStore);
void executeWideMoveProcessing(uint32_t instruction, struct RegisterStore *registers);

// FILE: dataProcessingReg.h

bool isDataProcessingReg(long long op0);

void executeDataProcessingReg(uint32_t instruction, struct RegisterStore *registers);

void executeArithmeticProcessingReg(uint32_t instruction, struct RegisterStore *registers);
void executeLogicProcessingReg(uint32_t instruction, struct RegisterStore *registers);
void executeMultiplyProcessingReg(uint32_t instruction, struct RegisterStore *registers);

// FILE: singleDataTransfer.h

void executeDataTransfer(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registerStore);

// FILE: branch.h

bool isBranch(long long op0);
void executeBranch(uint32_t instruction, struct RegisterStore *registerStore);

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
    struct PSTATE *stateRegister;

    // GENERAL
    long long registers[31];
};

// FILE: utils.

bool overunderflow(long long val1, long long val2, long long result) {
    bool sign1 = val1 < 0;
    bool sign2 = val2 < 0;
    bool signResult = result < 0;
    return sign1 == sign2 && sign1 != signResult;
}

//bool carry(long long val1, long long val2, long long res, bool isPlus, bool is64Bit, bool V) {
//    printf("CARRY - comparing %lld and %lld\n", val1, val2);
//    uint64_t shift = is64Bit ? 63 : 31;
//    if (isPlus) {
//        return V;
//    } else {
//        return ((val1 >> shift) & 1) >= ((val2 >> shift) & 1);
//    }
//}

bool carry(long long val1, long long val2, long long res, bool isPlus, bool is64Bit, bool V) {
    printf("CARRY - comparing %lld and %lld\n", val1, val2);
    uint32_t shift = is64Bit ? 63 : 31;
    if (isPlus) {
        return V;
    } else {
        if (res < val1 && val2 < val1) {
            return false;
        } else {
            return ((val1 >> shift) & 1) >= ((val2 >> shift) & 1); //return true;
        }
    }
}

void printb(bool b) {
    if (b) {
        printf("true");
    } else {
        printf("false");
    }
}

long long loadData(long long address, uint8_t *memPointer, bool isDoubleWord) {
    int bytesCount = 4;
    if (isDoubleWord) {
        bytesCount = 8;
    }

    long long data = 0;
    for (int i = 0; i < bytesCount; i++) {
        long long currentByte = memPointer[address + i];
        uint32_t significance = 8 * i;
        data = data | (currentByte << significance);
    }

    if (!isDoubleWord) {
        if (data & 0x80000000) {
            long long extend = 0xFFFFFFFF;
            data = data | (extend << 32);
        }
    }

    return data;
}

uint32_t fetchInstruction(long long address, uint8_t *memPointer) {
    uint32_t data = loadData(address, memPointer, false);
    return data;
}

void storeData(long long data, long long address, uint8_t *memPointer, bool isDoubleWord) {
    int bytesCount = 4;
    if (isDoubleWord) {
        bytesCount = 8;
    }

    for (int i = 0; i < bytesCount; i++) {
        int significance = 8 * i;
        uint8_t currentByte = data >> significance;

        memPointer[address + i] = currentByte;
    }
}

uint8_t *allocateMemory(void) {
    uint8_t *memPointer = calloc(MEMORY_COUNT, sizeof(uint8_t));
    assert( memPointer != NULL );
    return memPointer;
}

void loadMemoryFromFile(uint8_t *memPointer, char *filename) {
    int counter = 0;
    FILE *file = fopen(filename, "rb");  // Open the binary file in read mode

    if (file == NULL) {
        return;
    }

    while (true) {
        uint8_t value;  // Variable to store the read integer

        if (fseek(file, counter, SEEK_SET) != 0) {
            fclose(file);
            return;
        }

        // Read 1 byte from the file into the variable 'value'
        if (fread(&value, sizeof(value), 1, file) != 1) {
            fclose(file);
            return;
        }

        // value = ntohl(value);
        memPointer[counter] = value;
        counter += 1;
    }
}

void outputFile(struct RegisterStore *registers, struct PSTATE *stateRegister, uint8_t *memPointer, char* filename) {
    FILE *fp;
    fp = fopen(filename, "w");
    fprintf(fp, "Registers:\n");
    for (int i = 0; i < 31; i++) {
        fprintf(fp, "X%02d    = %016llx\n", i, registers->registers[i]);
    }
    fprintf(fp, "PC     = %016llx\n", registers->programCounter);
    char n_val = stateRegister->negativeFlag ? 'N' : '-';
    char z_val = stateRegister->zeroFlag ? 'Z' : '-';
    char c_val = stateRegister->carryFlag ? 'C' : '-';
    char v_val = stateRegister->overflowFlag ? 'V' : '-';
    fprintf(fp, "PSTATE : %c%c%c%c\n", n_val, z_val, c_val, v_val);
    fprintf(fp, "Non-Zero Memory:\n");
    long long memAddress = 0;
    uint32_t data = loadData(memAddress, memPointer, false);
    while (memAddress < MEMORY_COUNT) {
        memAddress += 4;
        if (data != 0) {
            // Load 32 bit word and convert from little endian
            fprintf(fp, "0x%08llx : %08x\n", memAddress - 4, data);
            data = loadData(memAddress, memPointer, false);
        }
    }
    fclose(fp); //Don't forget to close the file when finished
}

long long loadFromRegister(uint32_t index, struct RegisterStore *registerStore, bool isDoubleWord) {
    long long data = registerStore->zeroRegister;
    if (index < 31) {
        data = registerStore->registers[index];
    }
    if (!isDoubleWord) {
        data &= 0xFFFFFFFF;
    }
    return data;
}

void storeToRegister(uint32_t index, long long data, struct RegisterStore *registerStore, bool isDoubleWord) {
    long long store = data;
    if (!isDoubleWord) {
        store &= 0xFFFFFFFF;
    }
    if (index < 31) {
        registerStore->registers[index] = store;
    }
}

// FILE: bitwiseshift.c

#define WORDSIZE  32

uint64_t logicalLeftShift(uint64_t number, uint32_t shift, bool is64Bit) {
    int wordSize = is64Bit ? 64 : 32;
    if (shift > wordSize) {
        return 0;
    }
    if (wordSize == 32) {
        number <<= shift;
        return number & 0xFFFFFFFF;
    } else {
        return number << shift;
    }
}

uint64_t logicalRightShift(uint64_t number, uint32_t shift, bool is64Bit) {
    int wordSize = is64Bit ? 64 : 32;
    if (shift > wordSize) {
        return 0;
    }
    if (wordSize == 32) {
        number &= 0xFFFFFFFF;
        return number >> shift;
    }
    return number >> shift;
}

long long arithmeticRightShift(long long number, uint32_t shift, bool is64Bit) {
    int wordSize = is64Bit ? 64 : 32;
    uint64_t mask = is64Bit ? 0xFFFFFFFFFFFFFFFF : 0xFFFFFFFF;
    if (shift > wordSize) {
        return 0;
    }
    number &= mask;
    bool signBit = (number >> (wordSize - 1)) & 1;
    if (signBit) {
        return ((mask << (wordSize - shift)) | (number >> shift)) & mask;
    }
    return logicalRightShift(number, shift, is64Bit);
}

uint64_t ToPow2(int power) {
    uint64_t x = 1;
    for (int i = 0; i < power; i++) {
        x *= 2;
    }
    return x;
}

long long rotateRight(long long number, uint32_t shift, bool is64Bit) {
    int wordSize = is64Bit ? 64 : 32;
    uint64_t mask = is64Bit ? 0xFFFFFFFFFFFFFFFF : 0xFFFFFFFF;
    if (shift > wordSize) {
        return 0;
    }
    number &= mask;
    uint64_t rotatedBits = (number & (ToPow2(shift) - 1)) << (wordSize - shift);
    uint64_t clearer = (ToPow2(wordSize - shift) - 1);
    printf("rotatedBits: %lx\n", rotatedBits);
    printf("shiftedNum: %llx\n", number >> shift);
    return (((number >> shift) & clearer) | rotatedBits) & mask;
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
void executeDataProcessingImm(uint32_t instruction, struct RegisterStore *registers) {
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

void executeArithmeticProcessingImm(uint32_t instruction, struct RegisterStore *registerStore) {
    uint32_t rd = instruction & 0x1F;
    uint32_t rn = (instruction >> 5) & 0x1F;
    uint32_t sf = instruction >> 31;

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

//    long long reg = registerStore->zeroRegister;
//    if (rn < 31) {
//        reg = registerStore->registers[rn];
//    }
    long long reg = loadFromRegister(rn, registerStore, true);

    long long res;
    bool multiplier = -1;
    if (opc == ADD || opc == ADDS) {
        res = reg + op;
        multiplier = 1;
    } else {
        res = reg - op;
    }
//    if (!sf) {
//        res &= 0xFFFFFFFF;
//    }

    // Complete the flags
    if (setsFlags) {
        registerStore->stateRegister->negativeFlag = res < 0;
        registerStore->stateRegister->zeroFlag = res == 0;

        printb(registerStore->stateRegister->negativeFlag);
        printf("\n");

        bool V = overunderflow(reg, multiplier * op, res);
        bool C = carry(reg, op, res, opc == ADDS, sf, V); //ADDS
        registerStore->stateRegister->carryFlag = C;
        registerStore->stateRegister->overflowFlag = reg > 0 && res < 0;
        // if (opc == ADDS) {
        //     registerStore->stateRegister->carryFlag = reg > 0 && res < 0;
        //     registerStore->stateRegister->overflowFlag = reg > 0 && res < 0;
        // } else {
        //     registerStore->stateRegister->carryFlag = !(reg < 0 && res > 0);
        //     registerStore->stateRegister->overflowFlag = reg < 0 && res > 0;
        // }
    }

//    if (rd < 31) {
//        registerStore->registers[rd] = res;
//    }

    storeToRegister(rd, res, registerStore, sf);
}

void executeWideMoveProcessing(uint32_t instruction, struct RegisterStore *registers) {
    uint32_t rd = instruction & 0x1F;
    uint32_t hw = (instruction >> 21) & 0x3;
    uint64_t imm16 = (instruction >> 5) & 0xFFFF;

    uint32_t shift = hw * 16;
    uint64_t op = imm16 << shift;
    bool sf = (instruction >> 31);

    enum DpOpcWideMove opc = (instruction >> 29) & 0x3;

    long long res = registers->registers[rd];
    if (opc == MOVZ) {
        res = op;
    } else if (opc == MOVN) {
        res = ~op;
    } else if (opc == MOVK) {
        long long mask = 0xFFFF;
        res = res & ~(mask << shift);
        res = res | op;
    } else {
        // INVALID
        return;
    }

    if (!sf) {
        res = res & 0xFFFFFFFF;
    }

    if (rd < 31) {
        registers->registers[rd] = res;
        printf("res: %llx\n", res);
    }
}

// FILE: dataProcessingReg.c

bool isDataProcessingReg(long long op0) {
    long long match = 0xD;
    return (op0 | 0x8) == match;
}

long long shiftFun(uint32_t shift, long long reg, uint32_t operand, bool sizeBit) {
    if (shift == 0) {
        return logicalLeftShift(reg, operand, sizeBit);
    } else if (shift == 1) {
        return logicalRightShift(reg, operand, sizeBit);
    } else if (shift == 2) {
        return arithmeticRightShift(reg, operand, sizeBit);
    } else {
        return rotateRight(reg, operand, sizeBit);
    }
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
    bool sf = instruction >> 31 & 0x1;
    uint32_t shift = (instruction >> 22) & 0x3;
    uint32_t operand = (instruction >> 10) & 0x3F;
    uint32_t rn = (instruction >> 5) & 0x1F;
    long long rn_val = registers->zeroRegister;
    if (rn < 31) {
        rn_val = registers->registers[rn];
    }
    uint32_t rm = (instruction >> 16) & 0x1F;
    long long rm_val = registers->zeroRegister;
    if (rm < 31) {
        rm_val = registers->registers[rm];
    }
    if (!sf) {
        rm_val &= 0xFFFFFFFF;
        rn_val &= 0xFFFFFFFF;
    }
    uint32_t opc = (instruction >> 29) & 0x3;
    uint32_t rd = instruction & 0x1F;
    long long rd_val;
    long long op2 = shiftFun(shift, rm_val, operand, sf);
    int multiplier = 1;
    if (opc >= 2) {
        multiplier = -1;
    }
    rd_val = rn_val + (multiplier * op2);
    bool signBit = rd_val >> 63;
    if (!sf) {
        rd_val &= 0xFFFFFFFF;
        signBit = rd_val >> 31;

        printf("sign bit = ");
        printb(signBit);
        printf("\n");
    }
    printf("rd_val: %llx\n", rd_val);
    if (opc % 2 == 1) {
        bool V = overunderflow(rn_val, multiplier * op2, rd_val);
        bool C = carry(rn_val, rm_val, rd_val, multiplier == 1, sf, V);
        registers->stateRegister->negativeFlag = signBit;
        registers->stateRegister->zeroFlag = rd_val == 0;
        registers->stateRegister->carryFlag = C;
        registers->stateRegister->overflowFlag = V;
    }
    printf("zero_flag: %d\n", registers->stateRegister->zeroFlag);
    registers->registers[rd] = rd_val;
}

void executeLogicProcessingReg(uint32_t instruction, struct RegisterStore *registers) {
    printf("executing logic processing...\n");

    uint32_t shift = (instruction >> 22) & 0x3;
    uint32_t N = (instruction >> 21) & 0x1;
    uint32_t opc = (instruction >> 29) & 0x3;
    uint32_t operand = (instruction >> 10) & 0x3F;
    uint32_t rn = (instruction >> 5) & 0x1F;
    long long rn_val = registers->zeroRegister;
    if (rn < 31) {
        rn_val = registers->registers[rn];
    }
    uint32_t rm = (instruction >> 16) & 0x1F;
    long long rm_val = registers->zeroRegister;
    if (rm < 31) {
        rm_val = registers->registers[rm];
    }
    bool sf = instruction >> 31;
    uint64_t mask = sf ? 0xFFFFFFFFFFFFFFFF : 0xFFFFFFFF;
    rm_val = shiftFun(shift, rm_val, operand, sf);
    uint32_t combined_opc = (opc << 1) + N;

    uint32_t rd = instruction & 0x1F;
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
            rd_val = rn_val ^ rm_val;
            break;
        case 5:
            rd_val = rn_val ^ ~rm_val;
            break;
        case 6:
            rd_val = rn_val & rm_val;
            bool signBit = rd_val >> (31 + sf * 32);
            printf("rd_val: %llx\n", rd_val);
            registers->stateRegister->negativeFlag = signBit;
            registers->stateRegister->zeroFlag = rd_val == 0;
            registers->stateRegister->carryFlag = false;
            registers->stateRegister->overflowFlag = false;
            break;
        case 7:
            rd_val = rn_val & ~rm_val;
            bool signBit2 = (rd_val >> (31 + sf * 32)) & 1;
            registers->stateRegister->negativeFlag = signBit2;
            registers->stateRegister->zeroFlag = rd_val == 0;
            registers->stateRegister->carryFlag = false;
            registers->stateRegister->overflowFlag = false;
            break;
    }

    if (rd < 31) {
        registers->registers[rd] = rd_val & mask;
    }

}

void executeMultiplyProcessingReg(uint32_t instruction, struct RegisterStore *registers) {
    bool sf = instruction >> 31;
    uint64_t mask = sf ? 0xFFFFFFFFFFFFFFFF : 0xFFFFFFFF;
    uint32_t rm = (instruction >> 16) & 0x1F;
    int x = ((instruction >> 15) & 0x1) == 0x1 ? -1 : 1;
    long long rm_val = registers->zeroRegister;
    if (rm < 31) {
        rm_val = registers->registers[rm];
    }
    uint32_t rn = (instruction >> 5) & 0x1F;
    long long rn_val = registers->zeroRegister;
    if (rn < 31) {
        rn_val = registers->registers[rn];
    }
    uint32_t rd = instruction & 0x1F;
    uint32_t ra = (instruction >> 10) & 0x1F;
    long long ra_val = registers->zeroRegister;
    if (ra < 31) {
        ra_val = registers->registers[ra];
    }
    long long rd_val = ra_val + x * (rn_val * rm_val);
    registers->registers[rd] = rd_val & mask;
}

// MARK: singleDataTransfer.c

void loadStore(bool forceLoad, uint32_t instruction, long long readAddress, uint8_t *memPointer, struct RegisterStore *registerStore) {
    bool isLoad = forceLoad | (instruction >> 22) & 0x1;
    bool isDoubleWord = (instruction >> 30) & 0x1;
    uint32_t rt = instruction & 0x1F;

    if (isLoad) {
        long long result = loadData(readAddress, memPointer, isDoubleWord);
        registerStore->registers[rt] = result;
    } else {
        storeData(registerStore->registers[rt], readAddress, memPointer, isDoubleWord);
    }
}

void executeImmediateOffset(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registers) {
    uint32_t xn = (instruction >> 5) & 0x1F;
    uint64_t imm12 = (instruction >> 10) & 0x7FF;

    long long xnValue = registers->registers[xn];

    long long readAddress = xnValue + imm12;

    loadStore(false, instruction, readAddress, memPointer, registers);
}

void executeRegisterOffset(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registers) {
    printf("Executing register offset...\n");
    uint32_t xn = (instruction >> 5) & 0x1F;
    uint32_t xm = (instruction >> 16) & 0x1F;

    long long xnValue = registers->registers[xn];
    long long xmValue = registers->registers[xm];

    long long readAddress = xnValue + xmValue;

    loadStore(false, instruction, readAddress, memPointer, registers);
}

void preAndPostIndex(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registers) {
    printf("Executing reg. indexed...\n");
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

void executeLoadLiteral(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registers) {
    printf("Executing literal...\n");
    uint32_t simm19 = (instruction >> 5) & 0x7FFFF;
    long long offset = simm19 * 4;
    long long readAddress = registers->programCounter + offset;

    loadStore(true, instruction, readAddress, memPointer, registers);
}

void executeDataTransfer(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registerStore) {
    printf("Executing data transfer...\n");
    bool isLiteral = (instruction >> 31) == 0;
    bool isImmediate = (instruction >> 24) & 1;
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

void executeBranch(uint32_t instruction, struct RegisterStore *registerStore) {
    enum offsetType branchType = instruction >> 29 & 0x3;

    if (branchType == UNCONDITIONAL) {
        long long simm26 = (instruction & 0x3FFFFFF) << 2; // 000101 + 26 bits simm26
        printf("simm26    = %llx\n", simm26);

        if (simm26 & 0x8000000) {
            uint64_t signExtend = 0x3FFFFFFFFF << 26;
            printf("signExtend    = %0xllx\n", signExtend);
            simm26 = simm26 | signExtend;
        }

        registerStore->programCounter += simm26;
    } else if (branchType == REGISTER) {
        uint32_t xn = (instruction >> 5) & 0x1F;
        registerStore->programCounter = registerStore->registers[xn];
    } else {
        uint32_t cond = instruction & 0xF;

        long long simm19 = ((instruction >> 5) & 0x7FFFF);

        if (simm19 & 0x40000) {
            uint64_t signExtend = 0x1FFFFFFFFFFF << 19;
            simm19 = simm19 | signExtend;
        }

        printf("simm19 = %08llx\n", simm19);

        struct PSTATE *pstate = registerStore->stateRegister;
        switch (cond) {
            case 0x0: // EQ
                printf("branch equality\n");
                if (pstate->zeroFlag) {
                    registerStore->programCounter += simm19 * 4;
                } else {
                    registerStore->programCounter += 4;
                }
                break;
            case 0x1: // NE
                printf("Zero Flag: "); printb(pstate->zeroFlag); printf("\n");
                if (!pstate->zeroFlag) {
                    registerStore->programCounter += simm19 * 4;
                    printf("nPC = %llx\n", registerStore->programCounter);
                } else {
                    registerStore->programCounter += 4;
                }
                break;
            case 0xA: // GE
                if (pstate->zeroFlag == pstate->overflowFlag) {
                    registerStore->programCounter += simm19 * 4;
                } else {
                    registerStore->programCounter += 4;
                }
                break;
            case 0xB: // LT
                if (pstate->negativeFlag != pstate->overflowFlag) {
                    registerStore->programCounter += simm19 * 4;
                } else {
                    registerStore->programCounter += 4;
                }
                break;
            case 0xC: // GT
                if (pstate->zeroFlag == 0 && pstate->negativeFlag == pstate->overflowFlag) {
                    registerStore->programCounter += simm19 * 4;
                } else {
                    registerStore->programCounter += 4;
                }
                break;
            case 0xD: // LE
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

void processor(uint8_t *memPointer, char* filename) {
    struct PSTATE stateRegister = { false, true, false, false };
    struct RegisterStore registerStore;
    registerStore.programCounter = 0;
    registerStore.zeroRegister = 0;
    registerStore.stateRegister = &stateRegister;

    for (int i = 0; i < sizeof(registerStore.registers) / sizeof(registerStore.registers[0]); i++) {
        registerStore.registers[i] = 0;
    }

    while (true) {
        printf("\n-----------BEGIN CYCLE----------\n\n");
        uint32_t instruction = fetchInstruction(registerStore.programCounter, memPointer);
        printf("PC = 0x%08llx\n", registerStore.programCounter);
        printf("IR = 0x%x\n", instruction);
        long long op0 = (instruction >> 25) & 0xF;

        if (instruction == 0x8a000000 || instruction == 0) {
            break;
        } else if (instruction == 0xD503201F) {
            registerStore.programCounter += 4;
            continue;
        } else if (isDataProcessingImm(op0)) {
            executeDataProcessingImm(instruction, &registerStore);
            registerStore.programCounter += 4;
        } else if (isDataProcessingReg(op0)) {
            executeDataProcessingReg(instruction, &registerStore);
            registerStore.programCounter += 4;
        } else if (isBranch(op0)) {
            executeBranch(instruction, &registerStore);
            //break;
        } else {
            executeDataTransfer(instruction, memPointer, &registerStore);
            registerStore.programCounter += 4;
        }


        for (int i = 0; i < 31; i++) {
            long long data = registerStore.registers[i];
            if (data != 0) {
                printf("X%d   =   0x%llx\n", i, data);
            }
        }
    }

    outputFile(&registerStore, &stateRegister, memPointer, filename);
}

int main(int argc, char **argv) {
    // Callocs memory of size 2MB
    uint8_t *memPointer = allocateMemory();
    if (argc > 1) {
        loadMemoryFromFile(memPointer,argv[1]);
        processor(memPointer, argv[2]);
    } else {
        loadMemoryFromFile(memPointer,
                           "../../armv8_testsuite/test/expected_results/general/loop01_exp.bin");
        processor(memPointer, "output.out");
    }
    free(memPointer);
    return EXIT_SUCCESS;
}