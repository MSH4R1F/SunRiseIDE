#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#define MEMORY_COUNT  2097152
#define ASSEMBLY_SIZE 10

// FILE branch.h

bool isLabel(char *opcode);

// FILE: key.c

uint32_t encodeRegister(char *operand) {
    if (strcmp(operand, "SP") == 0 || operand[1] == 'z') {
        return 0b11111;
    } else {
        operand++;
        return strtol(operand, NULL, 10);
    }
}

uint32_t encodeSimm(char *operand) {
    if (strstr(operand, "x")) {
        return strtol(operand, NULL, 16);
    } else {
        return strtol(operand, NULL, 10);
    }
}

uint32_t encodeImm(char *operand) {
    return encodeSimm(operand);
}

uint32_t encodeShift(char *operand) {
    if (strcmp(operand, "lsl") == 0) {
        return 0;
    } else if (strcmp(operand, "lsr") == 0) {
        return 1;
    } else if (strcmp(operand, "asr") == 0) {
        return 2;
    } else {
        return 3;
    }
}

// FILE: utils.c

char *createString(char *str) {
    char *heapString = malloc(strlen(str) + 1);
    if (str == NULL) {
        return NULL;
    }

    strcpy(heapString, str);
    heapString[strlen(str)] = '\0';
    return heapString;
}

char** loadAssemblyFromFile(char *filename) {
    FILE *fp = fopen(filename, "r");

    if( !fp ) {
        fprintf(stderr, "Opened file: %s\n", filename);
    }

    // Count Lines
    char cr;
    size_t lines = 0;

    while( cr != EOF ) {
        if ( cr == '\n' ) {
            lines++;
        }
        cr = getc(fp);
    }
    rewind(fp);

    // Read data
    {// 'goto' + data[lines] causes error, introduce block as a workaround
        char **data = malloc(lines * sizeof(char *));
        size_t n;

        for (size_t i = 0; i < lines; i++) {
            data[i] = NULL;
            n = 0;

            getline(&data[i], &n, fp);

            if (ferror( fp )) {
                fprintf(stderr, "Error reading from file\n");
            }


        }
        fclose(fp);

        char **newArray = calloc(lines, sizeof(char *));
        int next = 0;
        for (int i = 0; i < lines; i++) {
            if (strlen(data[i]) != 1) {
                printf("size = %lu\n", strlen(data[i]));
                newArray[next] = data[i];
                next++;
            }
        }
        return newArray;
    }

}

void writeMachineToFile(uint8_t *memPointer, char* filename) {

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

// FILE: labelMap.c

typedef struct {
    char *label;
    long long address;
} LabelAddressMap;

LabelAddressMap *allocateLabelEntry(void) {
    LabelAddressMap *entry = malloc( sizeof(LabelAddressMap) );
    return entry;
}

LabelAddressMap **allocateLabelMap(void) {
    LabelAddressMap **mapPointer = malloc( 10 * sizeof(LabelAddressMap *) );

    for (int i = 0; i < ASSEMBLY_SIZE; i++) {
        mapPointer[i] = allocateLabelEntry();
    }

    return mapPointer;
}

void freeLabelMap(LabelAddressMap **mapPointer) {
    for (int i = 0; i < ASSEMBLY_SIZE; i++) {
        free(mapPointer[i]);
    }
    free(mapPointer);
}

void computeLabelMap(char **assemblyArray, LabelAddressMap **labelMap) {
    int i = 0;
    long long line = 0;
    long long address = 0;

    while (assemblyArray[line] != NULL) {

        if (isLabel(assemblyArray[line])) {
            labelMap[i]->label = assemblyArray[line];
            labelMap[i]->address = address;
            i++;
        } else {
            address += 4;
        }

        line += 1;
    }
}

long long getMapAddress(LabelAddressMap **mapPointer, char *label) {
    for (int i = 0; mapPointer[i] != NULL; i++) {
        if (strstr(mapPointer[i]->label, label)) {
            return mapPointer[i]->address;
        }
    }
    return 0;
}

bool mapContainsLabel(char *label, LabelAddressMap **mapPointer) {
    for (int i = 0; mapPointer[i] != NULL; i++) {
        if (strstr(mapPointer[i]->label, label)) {
            return true;
        }
    }

    return false;
}

// token utilities

char *extractOpcode(char *instruction) {
    char *line_copy = strdup(instruction);
    char *opcode = strtok(line_copy, " ");
    if (opcode != NULL) {
        opcode = strdup(opcode);
    }
    free(line_copy);
    return opcode;
}

// MARK: FIX NAME TO CAMEL CASE, BIT VAGUE AS WELL
char *extract_ith_opcode(char *instruction) {
    char *line_copy = strdup(instruction);
    char *token = strtok(line_copy, " ");
    for (int j = 1; j < 1 && token != NULL;) {
        token = strtok(NULL, " ");
    }
    char *result = NULL;
    if (token != NULL) {
        result = strdup(token);
    }
    free(line_copy);
    return result;
}

// FILE: dataProcessing.c

uint32_t assembleDataProcessing3(char *opcode, char **operands);

char *getShift(char *opcode) {
    char opcodeDefinite[8];
    int i = 0;
    while (opcode[i] != '\0') {
        opcodeDefinite[i] = opcode[i];
        i++;
    }

    char* rest = opcodeDefinite;
    char* token = strtok_r(rest, " ", &rest);
    // token = strtok_r(rest, ".", &rest);

    return createString(token);
}

char *getImm(char *opcode) {
    char opcodeDefinite[8];
    int i = 0;
    while (opcode[i] != '\0') {
        opcodeDefinite[i] = opcode[i];
        i++;
    }

    char* rest = opcodeDefinite;
    char* token = strtok_r(rest, " ", &rest);
    token = strtok_r(rest, " ", &rest);

    return createString(token);
}

bool isDataProcessing(char *opcode) {
    char *ops[] = {"add", "adds", "sub", "subs", "cmp", "cmn", "neg", "negs", "and", "ands",
                   "bic", "bics", "eor", "orr", "eon", "orn", "tst", "movk", "movn",
                   "movz", "mov", "mvn", "madd", "msub", "mul", "mneg"};
    for (int i = 0; i < 8; i++) {
        if (strcmp(opcode, ops[i]) == 0) {
            return true;
        }
    }
    return false;
}

uint32_t assembleDataProcessing2(char *opcode, char **operands) {
    uint32_t instruction = 0;
    if (strcmp(opcode, "tst") == 0) {
        char *rn = operands[0];
        char *op2 = operands[1];
        char *newOperands[] = {"rzr", rn, op2};
        return assembleDataProcessing3("and", newOperands);
    }
    if (*opcode == 'm') {
        return -1;
    } else {
        return -1;
    }
    return instruction;
}

uint32_t assembleMaddMsub(char *opcode, char **operands) {
    uint32_t sf = operands[0][0] == 'X' ? 64 : 32;
    uint32_t instruction = sf << 31;
    instruction |= 0b0011011000 << 21;
    instruction |= encodeRegister(operands[2]) << 16;
    instruction |= strcmp(opcode, "madd") == 0 ? 0 : 1;
    instruction |= encodeRegister(operands[3]) << 10;
    instruction |= encodeRegister(operands[1]) << 5;
    instruction |= encodeRegister(operands[0]);
    return instruction;
}

uint32_t assembleMultiply(char *opcode, char **operands) {
    operands[3] = "xzr";
    opcode = strcmp(opcode, "mul") == 0 ? "madd" : "msub";
    return assembleMaddMsub(opcode, operands);
}

uint32_t assembleMove(char *opcode, char **operands, int operandLength) {
    uint32_t sf = operands[0][0] == 'X' ? 64 : 32;
    uint32_t instruction = sf << 31;
    uint32_t opc = 0;
    if (strcmp(opcode, "movz") == 0) {
        opc = 0b10;
    } else if (strcmp(opcode, "movk") == 0) {
        opc = 0b11;
    }
    instruction |= opc << 29;
    instruction |= 0b100101 << 23;
    uint32_t hw = 0;
    if (operandLength == 3) {
        hw = encodeImm(getImm(operands[2])) / 16;
    }
    instruction |= encodeImm(operands[1]) << 5;
    instruction |= encodeRegister(operands[0]);
    return instruction;
}

uint32_t assembleArithmeticLogic(char *opcode, char **operands, int operandlength) {
    char *ops[] = {"add", "adds", "sub", "subs"};
    uint32_t instruction = 0;
    instruction |= (operands[0][0] == 'x') << 31;
    instruction |= encodeRegister(operands[0]);
    uint32_t opc = 0;
    printf("%s\n", operands[0]);
    printf("%s\n", operands[1]);
    printf("%s\n", operands[2]);
    printf("%s\n", operands[3]);
    for (int i = 0; i < 4; i++) {
        printf("%s\n", operands[i]);
        if (strcmp(opcode, ops[i]) == 0) {
            opc = i;
            break;
        }
    }
    instruction |= opc << 29;

    if (operands[2][0] != '#') {
        instruction |= opc << 29;

        uint32_t shift = encodeShift(getShift(operands[3]));
        uint32_t rn = encodeRegister(operands[1]);
        instruction = instruction | (1 << 24 | shift << 22);
        uint32_t rm = encodeRegister(operands[2]);
        instruction |= 5 << 25;
        instruction |= rm << 16;
        instruction |= encodeImm(getImm(operands[3]));
        instruction |= rn << 5;

    }
    else {
        uint32_t shift = encodeShift(getShift(operands[3]));
        uint32_t rn = encodeRegister(operands[1]);
        uint32_t imm12 = encodeImm(operands[2]);
        instruction |= 4 << 26;
        instruction |= rn << 5;
        instruction |= imm12 << 10;
        instruction |= shift << 22;
        instruction |= 2 << 23;
    }
    return instruction;
}
uint32_t assembleBitLogic(char *opcode, char **operands) {
    uint32_t instruction = 0;
    uint32_t sf = operands[0][0] == 'x';
    uint32_t rd = encodeRegister(operands[0]);
    uint32_t rn = encodeRegister(operands[1]);
    uint32_t rm = encodeRegister(operands[2]);
    uint32_t shift = encodeShift(getShift(operands[3]));
    uint32_t operand = encodeImm(getImm(operands[3]));
    uint32_t opc;
    uint32_t n;
    char *ops[] = {"and", "bic",
                   "orr", "orn",
                   "eor", "eon",
                   "ands", "bics"};
    for (int i = 0; i < 8; ++i) {
        if (strcmp(ops[i], opcode) == 0) {
            opc = i >> 1;
            n = i & 0x1;
            break;
        }
    }
    uint32_t opr = (shift << 1) | n;
    instruction |= (sf << 31);
    instruction |= ((opc >> 1) << 29);
    instruction |= (5 << 25);
    instruction |= (opr << 21);
    instruction |= (rm << 16);
    instruction |= (operand << 10);
    instruction |= (rn << 5);
    instruction |= (rd);
    return instruction;
}

uint32_t assembleDataProcessing4(char *opcode, char **operands) {
    if (strcmp(opcode, "madd") == 0 || strcmp(opcode, "msub") == 0) {
        return assembleMaddMsub(opcode, operands);
    } else if (opcode[1] == 'd' || opcode[0] == 's') {
        return assembleArithmeticLogic(opcode, operands, 4);
    } else {
        return assembleBitLogic(opcode, operands);
    }
}

uint32_t assembleNegNegs(char *opcode, char **operands, int operandLength) {
    char *zeroRegister = calloc(4, sizeof(char));
    zeroRegister[0] = operands[0][0];
    strcpy(zeroRegister + 1, "zr");

    char **newOperands = calloc(4, sizeof(char *));
    newOperands[0] = operands[0];
    newOperands[1] = zeroRegister;
    newOperands[2] = operands[1];

    if (operands[2] != NULL) {
        newOperands[3] = operands[2];
    }

    char *newOpcode = calloc(sizeof(opcode) + 1, sizeof(char));

    if (strlen(opcode) == 4) {
        strcpy(newOpcode, "subs");
    } else {
        strcpy(newOpcode, "sub");
    }

    return assembleArithmeticLogic(newOpcode, newOperands, operandLength + 1);
}

uint32_t assembleDataProcessing(char *opcode, char **operands, int operandLength) {
    char *ops[] = {
            "add", "adds", "sub", "subs",
            "cmp", "cmn", "neg", "negs",
            "and", "ands","bic", "bics",
            "eor", "orr", "eon", "orn",
            "tst", "movk", "movn","movz",
            "mov", "mvn", "madd", "msub",
            "mul", "mneg"
    };
    int index;
    for (int i = 0; i < 26; ++i) {
        if (ops[i] == opcode) {
            index = i;
        }
    }
    if (index < 6) {
        assembleArithmeticLogic(opcode, operands, operandLength);
    } else if (index < 8) {
        assembleNegNegs(opcode, operands, operandLength);
    } else if (index < 16) {
        assembleBitLogic(opcode, operands);
    } else if (index < 17) {
        assembleTst(opcode, operands, operandLength);
    } else if (index < 20) {
        assembleMove(opcode, operands, operandLength);
    } else if (index < 22) {
        assembleMov(opcode, operands, operandLength);
    } else if (index < 29) {
        assembleMaddMsub(opcode, operands);
    } else {
        assembleMultiply(opcode, operands);
    }
}

// FILE: dataTransfer.c

long long encodeLiteralToOffset(char *operand, long long currentAddress, LabelAddressMap **labelMap) {
    long long address;
    if (operand[0] == '#') {
        operand++;
        address = atoll(operand);
    } else {
        address = getMapAddress(labelMap, operand);
    }
    long long offset = address - currentAddress;
    return offset;
}

bool isDataTransfer(char *opcode) {
    return strcmp(opcode, "ldr") == 0 || strcmp(opcode, "str") == 0;
}

uint32_t assembleDataTransfer(char *opcode, char **operands, int operandLength, long long currentAddress) {
    return 0;
}

// FILE: branch.c

bool isBranch(char *opcode) {
    return (strcmp(opcode, "b") == 0 || strcmp (opcode, "br") == 0 || strcmp(opcode, "b.cond") == 0);
}

bool isLabel(char *opcode) {
    return strstr(opcode, ":");
}

char *getCondition(char *opcode) {
    char opcodeDefinite[8];
    int i = 0;
    while (opcode[i] != '\0') {
        opcodeDefinite[i] = opcode[i];
        i++;
    }

    char* rest = opcodeDefinite;
    char* token = strtok_r(rest, ".", &rest);
    token = strtok_r(rest, ".", &rest);

    return token;
}

uint32_t assembleBranch(char *opcode, char **operands, int operandLength, long long currentAddress, LabelAddressMap **labelMap) {
    uint32_t instruction = 0;
    if (strstr(opcode, ".")) {
        char *condition = getCondition(opcode);
    } else {
        if (strcmp(opcode, "br") == 0) {
            printf("was undonditional register branch");
        } else {
            printf("was unconditional immediate branch");
        }
    }
    return instruction;
}

// directive.c

bool isDirective(char *opcode) {
    return strcmp(opcode, ".int") == 0;
}

uint32_t assembleDirective(char *opcode, char **operands, int operandLength) {
    return 0;
}

// assemble.c

bool isVoid(char *opcode) {
    return strcmp(opcode, "nop") == 0;
}

void outputFile(uint8_t *memoryArray, char* filename) {
    FILE *fp;
    fp = fopen(filename, "wb");
    int count = 0;
    while (memoryArray[count] != 0) {
        count++;
    }
    fwrite(memoryArray, sizeof(uint32_t), count, fp);
    fclose(fp); //Don't forget to close the file when finished
}

void assemble(char **assemblyArray, uint8_t *memoryArray) {
    LabelAddressMap **labelMap = allocateLabelMap();
    computeLabelMap(assemblyArray, labelMap);

    long long address = 0;
    int line = 0;

    while (assemblyArray[line] != NULL) {
        line++;
        if (isLabel(assemblyArray[line])) {
            continue;
        }

        char *currentInstruction = assemblyArray[line];
        char *opcode = extractOpcode(currentInstruction);
        char **operands = malloc(4 * sizeof(char *));
        int operandLength = 2; //Change to num of operands

        uint32_t instruction;
        if (isDataProcessing(opcode)) {
            instruction = assembleDataProcessing(opcode, operands, operandLength);
        } else if (isDataTransfer(opcode)) {
            instruction = assembleDataTransfer(opcode, operands, operandLength, address);
        } else if (isBranch(opcode)) {
            instruction = assembleBranch(opcode, operands, operandLength, address, labelMap);
        } else if (isVoid(opcode)) {
            instruction = 0xD503201F;
        } else if (isDirective(opcode)) {
            instruction = assembleDirective(opcode, operands, operandLength);
        } else {
            printf("NOT RECOGNISED: %s\n", assemblyArray[line]);
            return;
        }
        free(operands);

        storeData(instruction, address, memoryArray, false);
        address += 4;
    }

    freeLabelMap(labelMap);
}

int main(int argc, char **argv) {
    char **assemblyArray = loadAssemblyFromFile(argv[1]);
    uint8_t *memPointer = calloc(MEMORY_COUNT, sizeof(uint8_t));

    LabelAddressMap **labelMap = allocateLabelMap();
    computeLabelMap(assemblyArray, labelMap);

    assemble(assemblyArray, memPointer, argv[2]);

    freeLabelMap(labelMap);
    free(memPointer);

    return EXIT_SUCCESS;
}

//int main(int argc, char **argv) {
//    char **assemblyArray = loadAssemblyFromFile(argv[1]);//calloc(ASSEMBLY_SIZE, sizeof(char *));
//    uint8_t *memPointer = calloc(MEMORY_COUNT, sizeof(uint8_t));
//
//    assemblyArray[0] = "b execute"; //      0x00
//    assemblyArray[1] = "movz x2, #1"; //    0x04
//    assemblyArray[2] = "execute:";    //    0x08
//    assemblyArray[3] = "movz x1, #1"; //    0x08
//    assemblyArray[4] = "movz x3, #1"; //    0x0C
//    assemblyArray[5] = "cmp x1, x3"; //     0x10
//    assemblyArray[6] = "b.ge execute";   // 0x14
//    assemblyArray[7] = "and x0, x0, x0"; // 0x18
//
//    LabelAddressMap **labelMap = allocateLabelMap();
//    computeLabelMap(assemblyArray, labelMap);
//    assemble(assemblyArray, memPointer, argv[2])
//
//    // ----BEGIN TESTS----
//
//
//
//    // -----END TESTS-----
//
//    freeLabelMap(labelMap);
//    free(memPointer);
//
//    return EXIT_SUCCESS;
//}


