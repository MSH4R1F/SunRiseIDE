#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>
#include <assert.h>

// CONSTANTS

#define MEMORY_CAPACITY 131072

// FILE: utils.h

void loadMemory(uint32_t *memPointer, char *filename);

// FILE: registers.h

struct PSTATE;
struct RegisterStore;

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

void executeDataTransfer(long long instruction, struct RegisterStore *registers);

// FILE: branch.h

bool isBranch(long long op0);
void executeBranch(long long instruction, struct RegisterStore *registers);

// FILE: utils.c

uint32_t *allocateMemory(void) {
    uint32_t *memPointer = calloc(MEMORY_CAPACITY, sizeof(uint32_t));
    assert( memPointer != NULL );
    return memPointer;
}

void loadMemory(uint32_t *memPointer, char *filename) {
    int counter = 0;
    FILE *file = fopen(filename, "rb");  // Open the binary file in read mode

    if (file == NULL) {
        printf("Failed to open the file.\n");
        return;
    }

    while (true) {
        uint32_t value;  // Variable to store the read integer

        if (fseek(file, 4 * counter, SEEK_SET) != 0) {
            printf("Failed to seek to the desired position.\n");
            fclose(file);
            return;
        }

        // Read 4 bytes from the file into the variable 'value'
        if (fread(&value, sizeof(value), 1, file) != 1) {
            printf("Failed to read from the file.\n");
            fclose(file);
            return;
        }

        // value = ntohl(value);
        memPointer[counter] = value;
        counter += 1;

    }
}

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

    uint32_t op = (instruction >> 10) & 0xFFF;
    uint32_t sh = (instruction >> 22) & 0x1;

    if (sh == 1) {
        op = op << 12;
    }

    enum DpOpcArithmetic opc = (instruction >> 29) & 0x3;

    bool setsFlags = opc == ADDS || opc == SUBS;
    if (rn == 31 && setsFlags) {
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
    }

    if (rd < 31) {
        registers->registers[rd] = res;
    }
}

void executeWideMoveProcessing(long long instruction, struct RegisterStore *registers) {
    printf("executed wide move\n");
    uint32_t rd = instruction & 0x1F;
    uint32_t hw = (instruction >> 21) & 0x3;
    uint64_t imm16 = (instruction >> 5) & 0xFFFF;
    printf("printing imm16: %llu\n", imm16);

    uint32_t shift = hw * 16;
    uint64_t op = imm16 << shift;
    printf("printing op: %llu\n", op);
    uint32_t sf = (instruction >> 31);

    enum DpOpcWideMove opc = (instruction >> 29) & 0x3;
    if (opc == 2) {
        printf("correctly decoded opcode\n");
    }

    long long res = registers->registers[rd];
    printf("%lld\n", res);
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
    printf("%lld\n", res);

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
//    if (isPlus) {
//        if (overunderflow) {
//            return 1;
//        }
//        return 0;
//    } else {
//        if (overunderflow) {
//            return 0;
//        }
//        return 1;
//    }
}

void executeDataProcessingReg(uint32_t instruction, struct RegisterStore *registers) {
    printf("DataProcessing called\n");
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
    printf("ArithmeticProcessing called\n");
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
    printf("Multiply Processing called\n");
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


void unsignedOffset(long long instruction, struct RegisterStore *registers) {
    uint64_t xn = (instruction >> 5) & 0x1F;
    uint64_t imm12 = (instruction >> 10) & 0x7FF;
    uint64_t address = xn + imm12;
    long long value = registers->registers[address];

}

void preAndPostIndex(long long instruction, struct RegisterStore *registers) {
    uint32_t xn = (instruction >> 5) & 0x1F;
    long long xn_value = registers->registers[xn];
    uint32_t simm9 = (instruction >> 12) & 0x1FF;
    long long simm9_value = registers->registers[simm9];
    long long address = simm9_value + xn_value;
    if (((instruction >> 11) & 1) == 0x1) {
        registers->registers[address] = address;
    } else {
        registers->registers[xn] = address;
    }
}


void regOffset(long long instruction, struct RegisterStore *registers) {

}

void literal(long long instruction, struct RegisterStore *registers) {

}

void executeDataTransfer(long long instruction, struct RegisterStore *registers) {
    if (instruction >> 31 == 1) {
        literal(instruction, registers);
    }
    if (((instruction >> 24) & 1) == 1) {
        unsignedOffset(instruction, registers);
    } else if (instruction >> 21 == 1) {
        regOffset(instruction, registers);
    } else {
        preAndPostIndex(instruction, registers);
    }
}

// MARK: branch.c

bool isBranch(long long op0) {
    long long match = 0xB;
    return (op0 | 0x1) == match;
}

void executeBranch(long long instruction, struct RegisterStore *registers) {

}

void outputFile(struct RegisterStore *registers, struct PSTATE *stateRegister,uint32_t *memPointer, char *filename) {
    FILE *fp;
    fp = fopen(filename, "w");// "w" means that we are going to write on this file
    for (int i = 0; i < 31; i++) {
        fprintf(fp, "X%02d    = %016lld\n", i, registers->registers[i]);
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



void processor(uint32_t *memPointer) {
    struct PSTATE stateRegister = { false, true, false, false };

    struct RegisterStore registerStore;
    registerStore.programCounter = 0;
    registerStore.zeroRegister = 0;
    registerStore.stateRegister = stateRegister;
    for (int i = 0; i < sizeof(registerStore.registers) / sizeof(registerStore.registers[0]); i++) {
        registerStore.registers[i] = 0;
    }

    while (true) {
        printf("PC=%lld\n", registerStore.programCounter);
        uint32_t instruction = memPointer[registerStore.programCounter / 4];
        printf("%x\n", instruction);
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
            executeDataTransfer(instruction, &registerStore);
        }
    }

    outputFile(&registerStore, &stateRegister, memPointer, "output.out");
    free(memPointer);

    for (int i = 0; i < sizeof(registerStore.registers) / sizeof(registerStore.registers[0]); i++) {
        long long res = registerStore.registers[i];
        if (res != 0) {
            printf("X%d   = %lld\n", i, registerStore.registers[i]);
        }
    }
    //Prints flags - delete later
    char n_val = stateRegister.negativeFlag == true ? 'N' : '-';
    char z_val = stateRegister.zeroFlag == true ? 'Z' : '-';
    char c_val = stateRegister.carryFlag == true ? 'C' : '-';
    char v_val = stateRegister.overflowFlag == true ? 'V' : '-';
    printf("PSTATE : %c%c%c%c\n", n_val, z_val, c_val, v_val);
}

int main(int argc, char **argv) {
    // Callocs memory of size 2MB
    uint32_t *memPointer = allocateMemory();
    loadMemory(memPointer, "src/mul01_exp.bin");
    processor(memPointer); //Second arg contains output file
    return EXIT_SUCCESS;
}