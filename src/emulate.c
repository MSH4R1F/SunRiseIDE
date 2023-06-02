#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>
#include <assert.h>
#include <math.h>

// CONSTANTS

#define MEMORY_COUNT 2097152

// FILE: registers.h

struct PSTATE;
struct RegisterStore;

// FILE: utils.h

void loadMemoryFromFile(uint8_t *memPointer, char *filename);

// FILE: bitwiseshift.h

long long int logicalLeftShift(long long int number, uint32_t shift, bool is64Bit);
long long logicalRightShift(long long number, uint32_t shift, bool is64Bit);
long long arithmeticRightShift(long long number, uint32_t shift, bool is64Bit);
long long rotateRight(long long number, uint32_t shift, bool is64Bit);

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

void executeDataTransfer(long long instruction, uint8_t *memPointer, struct RegisterStore *registerStore);

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

long long fetchData(long long address, uint8_t *memPointer, bool isDoubleWord) {
    int bytesCount = 4;
    if (isDoubleWord) {
        bytesCount = 8;
    }

    long long data = 0;
    for (int i = 0; i < bytesCount; i++) {
        long long currentByte = memPointer[address + i];
        uint32_t significance = 8 * i;
        data = data | (currentByte << significance);
        printf("data = %llx\n", data);
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
    uint32_t data = fetchData(address, memPointer, false);
    printf("data %llx\n", data);
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
    uint32_t data = fetchData(memAddress, memPointer, false);
    while (memAddress < MEMORY_COUNT) {
        memAddress += 4;
        if (data != 0) {
            // Load 32 bit word and convert from little endian
            fprintf(fp, "0x%08llx : %08x\n", memAddress, data);
            data = fetchData(memAddress, memPointer, false);
        }
    }
    fclose(fp); //Don't forget to close the file when finished
}

// FILE: bitwiseshift.c

#define WORDSIZE  32

long long logicalLeftShift(long long number, uint32_t shift, bool is64Bit) {
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

long long logicalRightShift(long long number, uint32_t shift, bool is64Bit) {
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

        return (mask << (wordSize - shift)) | (number >> shift);
    }
    return logicalRightShift(number, shift, is64Bit);
}

long long rotateRight(long long number, uint32_t shift, bool is64Bit) {
    int wordSize = is64Bit ? 64 : 32;
    uint64_t mask = is64Bit ? 0xFFFFFFFFFFFFFFFF : 0xFFFFFFFF;
    if (shift > wordSize) {
        return 0;
    }
    return ((number >> shift) | (number << (wordSize - shift))) & mask;
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

long long shiftFun(uint32_t shift, long long reg, uint32_t operand, bool signBit) {
    if (shift == 0) {
        return logicalLeftShift(reg, operand, signBit);
    } else if (shift == 1) {
        return logicalRightShift(reg, operand, signBit);
    } else if (shift == 2) {
        return arithmeticRightShift(reg, operand, signBit);
    } else {
        return rotateRight(reg, operand, signBit);
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
    bool sf = instruction >> 31;
    long long op2 = shiftFun(shift, rm_val, opr, sf);
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
    uint32_t operand = (instruction >> 10) & 0x3F;
    uint32_t rn = (instruction >> 5) & 0x1F;
    long long rn_val = registers->registers[rn];
    uint32_t rm = (instruction >> 16) & 0x1F;
    long long rm_val = registers->registers[rm];
    bool sf = instruction >> 31;
    rm_val = shiftFun(shift, rm_val, operand, sf);
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

void loadStore(bool forceLoad, long long instruction, long long readAddress, uint8_t *memPointer, struct RegisterStore *registerStore) {
    bool isLoad = forceLoad | (instruction >> 22) & 0x1;
    bool isDoubleWord = (instruction >> 30) & 0x1;
    uint32_t rt = instruction & 0x1F;

    if (isLoad) {
        long long result = fetchData(readAddress, memPointer, isDoubleWord);
        registerStore->registers[rt] = result;
    } else {
        storeData(registerStore->registers[rt], readAddress, memPointer, isDoubleWord);
    }
}

void executeImmediateOffset(long long instruction, uint8_t *memPointer, struct RegisterStore *registers) {
    uint32_t xn = (instruction >> 5) & 0x1F;
    uint64_t imm12 = (instruction >> 10) & 0x7FF;

    long long xnValue = registers->registers[xn];

    long long readAddress = xnValue + imm12;

    loadStore(false, instruction, readAddress, memPointer, registers);
}

void executeRegisterOffset(long long instruction, uint8_t *memPointer, struct RegisterStore *registers) {
    printf("Executing register offset...\n");
    uint32_t xn = (instruction >> 5) & 0x1F;
    uint32_t xm = (instruction >> 16) & 0x1F;

    long long xnValue = registers->registers[xn];
    long long xmValue = registers->registers[xm];

    long long readAddress = xnValue + xmValue;

    loadStore(false, instruction, readAddress, memPointer, registers);
}

void preAndPostIndex(long long instruction, uint8_t *memPointer, struct RegisterStore *registers) {
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

void executeLoadLiteral(long long instruction, uint8_t *memPointer, struct RegisterStore *registers) {
    printf("Executing literal...\n");
    uint32_t simm19 = (instruction >> 5) & 0x7FFFF;
    long long offset = simm19 * 4;
    long long readAddress = registers->programCounter + offset;

    loadStore(true, instruction, readAddress, memPointer, registers);
}

void executeDataTransfer(long long instruction, uint8_t *memPointer, struct RegisterStore *registerStore) {
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

void processor(uint8_t *memPointer, char* filename) {
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
        printf("In processor: current instruction - %x\n", instruction);
        long long op0 = (instruction >> 25) & 0xF;

        if (instruction == 0x8a000000 || instruction == 0) {
            break;
        } else if (instruction == 0xD503201F) {
            continue;
        } else if (isDataProcessingImm(op0)) {
            executeDataProcessingImm(instruction, &registerStore);
        } else if (isDataProcessingReg(op0)) {
            executeDataProcessingReg(instruction, &registerStore);
        } else if (isBranch(op0)) {
            executeBranch(instruction, &registerStore);
        } else {
            executeDataTransfer(instruction, memPointer, &registerStore);
        }
        registerStore.programCounter += 4;
    }

    outputFile(&registerStore, &stateRegister, memPointer, filename);
}

int main(int argc, char **argv) {
    // Callocs memory of size 2MB
    uint8_t *memPointer = allocateMemory();
    loadMemoryFromFile(memPointer, "../../armv8_testsuite/test/expected_results/shifts/asr1_exp.bin"); //replace filename with "../../armv8_testsuite/test/expected_results/general/ldr01_exp.bin"
    processor(memPointer, "output.out"); //replace filename with second output.out
    free(memPointer);
    return EXIT_SUCCESS;
}