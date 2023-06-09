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

uint32_t encodeSimm(char *operand, bool hasHash) {
    if (hasHash) {
        operand++;
    }
    if (strstr(operand, "x")) {
        return strtol(operand, NULL, 16);
    } else {
        return strtol(operand, NULL, 10);
    }
}

uint32_t encodeImm(char *operand, bool hasHash) {
    return encodeSimm(operand, hasHash);
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
        char **data = calloc(lines, sizeof(char *));
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

        char **newArray = calloc(lines + 1, sizeof(char *));
        int next = 0;
        for (int i = 0; i < lines; i++) {
            while (data[i][0] == ' ') {
                data[i]++;
            }

            if (strlen(data[i]) != 1) {
                newArray[next] = data[i];
                newArray[next][strlen(newArray[next]) - 1] = '\0';

                int endIndex = strlen(newArray[next]) - 1;
                while (newArray[next][endIndex] == ' ') {
                    newArray[next][endIndex] = '\0';
                    endIndex--;
                }
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
    bool wasAllocated;
} LabelAddressMap;

LabelAddressMap *allocateLabelEntry(void) {
    LabelAddressMap *entry = malloc( sizeof(LabelAddressMap) );
    entry->wasAllocated = false;
    return entry;
}

LabelAddressMap **allocateLabelMap(void) {
    LabelAddressMap **mapPointer = malloc( ASSEMBLY_SIZE * sizeof(LabelAddressMap *) );

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
            char *label = assemblyArray[line];

            labelMap[i]->label = label;
            labelMap[i]->address = address;
            labelMap[i]->wasAllocated = true;
            i++;
        } else {
            address += 4;
        }

        line += 1;
    }
}

long long getMapAddress(LabelAddressMap **mapPointer, char *label) {
    for (int i = 0; i < ASSEMBLY_SIZE; i++) {
        if (strstr(mapPointer[i]->label, label)) {
            return mapPointer[i]->address;
        }
    }
    return 0;
}

bool mapContainsLabel(char *label, LabelAddressMap **mapPointer) {
    for (int i = 0; i < ASSEMBLY_SIZE; i++) {
        if (!mapPointer[i]->wasAllocated) {
            continue;
        }
        printf("small: '%s', big: '%s'\n", label, mapPointer[i]->label);
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

char *getOperands(char *opcode) {
    char opcodeDefinite[strlen(opcode) + 1];
    for (int i = 0; i < strlen(opcode); i++) {
        opcodeDefinite[i] = opcode[i];
    }
    opcodeDefinite[strlen(opcode)] = '\0';

    char* rest = opcodeDefinite;
    char* token = strtok_r(opcodeDefinite, " ", &rest);
    return createString(rest);
}

char **extractOperands(char *instruction) {
    char *line_copy = strdup(instruction);
    char *operands = strtok(line_copy, " ");
    operands = getOperands(instruction);
    char **splitOperands = calloc(4, sizeof(char *));
    char *token = strtok(operands, ",");
    int count = 0;
    while(token != NULL) {
        if (token[0] == ' ') {
            token++;
        }
        splitOperands[count] = token;
        count++;
        token = strtok(NULL, ",");
    }
    return splitOperands;
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


char *getShift(char *opcode) {
//    char opcodeDefinite[8];
//    int i = 0;
//    while (opcode[i] != '\0') {
//        opcodeDefinite[i] = opcode[i];
//        i++;
//    }
    char opcodeDefinite[strlen(opcode) + 1];
    for (int i = 0; i < strlen(opcode); i++) {
        opcodeDefinite[i] = opcode[i];
    }
    opcodeDefinite[strlen(opcode)] = '\0';

    char* rest = opcodeDefinite;
    char* token = strtok_r(opcodeDefinite, " ", &rest);

    return createString(token);
}

char *getImm(char *opcode) {
//    char opcodeDefinite[8];
//    int i = 0;
//    while (opcode[i] != '\0') {
//        opcodeDefinite[i] = opcode[i];
//        i++;
//    }
    char opcodeDefinite[strlen(opcode) + 1];
    for (int i = 0; i < strlen(opcode); i++) {
        opcodeDefinite[i] = opcode[i];
    }
    opcodeDefinite[strlen(opcode)] = '\0';

    char* rest = opcodeDefinite;
    char* token = strtok_r(opcodeDefinite, " ", &rest);
    //token = strtok_r(rest, " ", &rest);

    return createString(rest);
}

bool isDataProcessing(char *opcode) {
    char *ops[] = {"add", "adds", "sub", "subs", "cmp", "cmn", "neg", "negs", "and", "ands",
                   "bic", "bics", "eor", "orr", "eon", "orn", "tst", "movk", "movn",
                   "movz", "mov", "mvn", "madd", "msub", "mul", "mneg"};
    for (int i = 0; i < 26; i++) {
        if (strcmp(opcode, ops[i]) == 0) {
            return true;
        }
    }
    return false;
}


uint32_t assembleMaddMsub(char *opcode, char **operands) {
    uint32_t sf = operands[0][0] == 'x' ? 1 : 0;
    uint32_t instruction = sf << 31;
    instruction |= 0b0011011000 << 21;
    instruction |= encodeRegister(operands[2]) << 16;
    instruction |= (strcmp(opcode, "madd") == 0 ? 0 : 1) << 15;
    instruction |= encodeRegister(operands[3]) << 10;
    instruction |= encodeRegister(operands[1]) << 5;
    instruction |= encodeRegister(operands[0]);
    return instruction;
}

uint32_t assembleMultiply(char *opcode, char **operands) {
    char *zeroRegister = calloc(4, sizeof(char));
    zeroRegister[0] = operands[0][0];
    strcpy(zeroRegister + 1, "zr");
    operands[3] = zeroRegister;
    opcode = strcmp(opcode, "mul") == 0 ? "madd" : "msub";
    return assembleMaddMsub(opcode, operands);
}

uint32_t assembleMove(char *opcode, char **operands, int operandLength) {
    uint32_t sf = operands[0][0] == 'x' ? 1 : 0;
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
        hw = encodeImm(getImm(operands[2]), true) / 16;
    }
    instruction |= hw << 21;
    instruction |= encodeImm(operands[1], true) << 5;
    instruction |= encodeRegister(operands[0]);
    return instruction;
}

uint32_t assembleArithmeticLogic(char *opcode, char **operands, int operandLength) {
    printf("--ARITHMETIC LOGIC\n");
    printf("opcode:%s\n", opcode);
    char *ops[] = {"add", "adds", "sub", "subs"};
    uint32_t instruction = 0;
    instruction |= (operands[0][0] == 'x') << 31;  // sf bit
    uint32_t opc = 0;
    for (int i = 0; i < 4; i++) {
        printf("ops[%d]: %s\n", i, ops[i]);
        if (strcmp(opcode, ops[i]) == 0) {
            opc = i;
            break;
        }
    }
    printf("--OPC:   %d\n", opc);
    instruction |= opc << 29;  // opc bit
    if (operands[2][0] == '#') {  // Immediate instruction
        instruction |= 0b100 << 26; //bits 26 to 28
//        instruction |= 1 << 28;  // set bit 28 for "100"
        instruction |= 0b010 << 23;  // set bits 25:23 for "010"
        if (operandLength == 4) {
            printf("--operands[3]: %s\n", operands[3]);
            if (operands[3][5] == '1') {
                printf("--LEFT SHIFTED 12\n");
                instruction |= 1 << 22;
            }
        }
        printf("--operands[2]: %s\n", operands[2]);
        uint32_t operand = encodeImm(operands[2], true);
        printf("--operand: %d\n", operand);
        instruction |= operand << 10;  // operand bits
        instruction |= encodeRegister(operands[1]) << 5;
        uint32_t rd = encodeRegister(operands[0]);
        instruction |= rd;  // Rd bits

    } else {  // Register instruction
        printf("---REGISTER\n");
        instruction |= 0b0101 << 25;  // set bits 27:25 for "101"
        uint32_t shift = 0;
        uint32_t operand = 0;
        if (operandLength == 4) {
            printf("REACHES HERE\n");
            printf("get shift: %s\n", getShift(operands[3]));
            shift = encodeShift(getShift(operands[3]));
            printf("shift: %x\n", shift);
            printf("get imm: %s", getImm(operands[3]));
            operand = encodeImm(getImm(operands[3]), true);
            printf("operand: %x\n", operand);
        }
        printf("--shift: %d\n", shift);
        printf("--operand: %d\n", operand);
        instruction |= 1 << 24;  // first
        // bit in opr
        instruction |= shift << 22;  // shift bits with "1xx0" in opr

        uint32_t rm = encodeRegister(operands[2]);
        instruction |= rm << 16;  // Rm bits
        instruction |= operand << 10;  // operand bits

        uint32_t rn = encodeRegister(operands[1]);
        instruction |= rn << 5;  // Rn bits

        uint32_t rd = encodeRegister(operands[0]);
        instruction |= rd;  // Rd bits
    }
    printf("RETURNS\n");
    return instruction;
}

uint32_t assembleBitLogic(char *opcode, char **operands, int operandLength) {
    uint32_t shift = 0;
    uint32_t operand = 0;
    if (operandLength == 4) {
        shift = encodeShift(getShift(operands[3]));
        operand = encodeImm(getImm(operands[3]), true);
    }
    uint32_t instruction = 0;
    uint32_t sf = operands[0][0] == 'x';
    uint32_t rd = encodeRegister(operands[0]);
    uint32_t rn = encodeRegister(operands[1]);
    uint32_t rm = encodeRegister(operands[2]);

    uint32_t opc = 0;
    uint32_t n = 0;

    char *ops[] = {"and", "bic",
                   "orr", "orn",
                   "eor", "eon",
                   "ands", "bics"};
    for (int i = 0; i < 8; i++) {
        if (strcmp(ops[i], opcode) == 0) {
            opc = i >> 1;
            n = i & 0x1;
            break;
        }
    }
    uint32_t opr = (shift << 1) | n;
    instruction |= (sf << 31);
    instruction |= ((opc) << 29);
    instruction |= (5 << 25);
    instruction |= (opr << 21);
    instruction |= (rm << 16);
    instruction |= (operand << 10);
    instruction |= (rn << 5);
    instruction |= (rd);

    return instruction;
}

uint32_t assembleTst(char *opcode, char **operands, int operandLength) {
    char *zeroRegister = calloc(4, sizeof(char));
    zeroRegister[0] = operands[0][0];
    strcpy(zeroRegister + 1, "zr");

    char *shifter = "lsl #0";
    if (operands[2] != NULL) {
        shifter = operands[2];
    }

    char *newOperands[4];
    newOperands[0] = zeroRegister;
    newOperands[1] = operands[0];
    newOperands[2] = operands[1];
    newOperands[3] = createString(shifter);

    return assembleBitLogic("ands", newOperands, 4);
}

uint32_t assembleMov(char *opcode, char **operands, int operandLength) {
    char *shiftString = "lsl #0";
    if (operandLength == 3) {
        shiftString = operands[2];
    }
    char *newOps[] = {operands[0], "xzr", operands[1],  shiftString};
    opcode = "orr";
    return assembleBitLogic(opcode, newOps, 4);
}

uint32_t assembleNegNegs(char *opcode, char **operands, int operandLength) {
    char *zeroRegister = calloc(4, sizeof(char));
    zeroRegister[0] = operands[0][0];
    strcpy(zeroRegister + 1, "zr");

    char *newOperands[4];
    newOperands[0] = operands[0];
    newOperands[1] = zeroRegister;
    newOperands[2] = operands[1];

    if (operands[2] != NULL) {
        newOperands[3] = operands[2];
    }

    char *newOpcode;
    if (strlen(opcode) == 4) {
        newOpcode = "subs";
    } else {
        newOpcode = "sub";
    }

    return assembleArithmeticLogic(newOpcode, newOperands, operandLength + 1);
}

uint32_t assembleCmp(char *opcode, char **operands, int operandLength) {
    char *zeroRegister = calloc(4, sizeof(char));
    zeroRegister[0] = operands[0][0];
    strcpy(zeroRegister + 1, "zr");

    char *newOperands[4];
    newOperands[0] = zeroRegister;
    printf("xero register: %s\n", zeroRegister);
    newOperands[1] = operands[0];
    newOperands[2] = operands[1];

    if (operands[2] != NULL) {
        newOperands[3] = operands[2];
    }

    char *newOpcode;
    if (opcode[2] == 'p') {
        newOpcode = "subs";
    } else {
        newOpcode = "adds";
    }
    printf("newOpcode: %s\n", newOpcode);
    for (int i = 0; i < operandLength + 1; i++) {
        printf("operands %d: %s\n", i, newOperands[i]);
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
        if (strcmp(opcode, ops[i]) == 0) {
            index = i;
        }
    }

    uint32_t instruction = 0;
    if (index < 4) {
        instruction = assembleArithmeticLogic(opcode, operands, operandLength);
    } else if (index < 6) {
        instruction = assembleCmp(opcode, operands, operandLength);
    } else if (index < 8) {
        instruction = assembleNegNegs(opcode, operands, operandLength);
    } else if (index < 16) {
        instruction = assembleBitLogic(opcode, operands, operandLength);
    } else if (index < 17) {
        instruction = assembleTst(opcode, operands, operandLength);
    } else if (index < 20) {
        instruction = assembleMove(opcode, operands, operandLength);
    } else if (index < 22) {
        instruction = assembleMov(opcode, operands, operandLength);
    } else if (index < 24) {
        instruction = assembleMaddMsub(opcode, operands);
    } else {
        instruction = assembleMultiply(opcode, operands);
    }

    return instruction;
}

// FILE: dataTransfer.c

long long encodeLiteralToOffset(char *literal, long long currentAddress, LabelAddressMap **labelMap) {
    long long jumpAddress;
    if (mapContainsLabel(literal, labelMap)) {
        printf("literal '%s' is recognised\n", literal);
        jumpAddress = getMapAddress(labelMap, literal);
    } else {
        printf("literal '%s' not is recognised\n", literal);
        jumpAddress = atoll(literal);
    }

    long long offset = jumpAddress - currentAddress;
    printf("---jumping to %08llx\n", jumpAddress);
    return offset;
}

static char *removeLastLetter(char* string) {
    char *strPointer = malloc(strlen(string) * sizeof(char));
    for (int i = 0; i < strlen(string) - 1; i++) {
        strPointer[i] = string[i];
    }
    strPointer[strlen(string) - 1] = '\0';
    return strPointer;
}

bool isDataTransfer(char *opcode) {
    return strcmp(opcode, "ldr") == 0 || strcmp(opcode, "str") == 0;
}

uint32_t assembleLoadLiteral(char *opcode, char **operands, long long currentAddress, uint32_t sf, uint32_t destReg, LabelAddressMap **labelMap) {
    uint32_t instruction = 0x0;                                                         //left-most bit is constant
    instruction |= (sf << 30);                                                                //sf bit
    instruction |= (0b011000 << 24);//bits 24 to 29 are constant when not unsigned offset
    uint32_t simm19 = (encodeLiteralToOffset(operands[1], currentAddress, labelMap) / 4) & 0x7FFFF;
    instruction |= simm19 << 5; //type of data transfer
    instruction |= destReg;                                                                   //destination register
    return instruction;
}

uint32_t assemblePostIndex(char **operands, uint32_t destReg, uint32_t sf, uint32_t srcReg, uint32_t instructionType) {
    uint32_t instruction = 0x1 << 31;                       //left-most bit is constant
    instruction |= (sf << 30);                              //sf bit
    instruction |= (0b1110000 << 23);                       //bits 23 to 29 are constant when not unsigned offset
    instruction |= (instructionType << 22);                 //type of data transfer
    instruction |= (0 << 21);                               //21st bit if 0 for pre/post-index
    instruction |= (encodeSimm(operands[2], true) & 0x1FF) << 12;  //simm9
    instruction |= (0b01 << 10);                            //I and neighbouring bit
    instruction |= (srcReg << 5);                           //source register
    instruction |= destReg;                                 //destination register
    return instruction;
}

uint32_t assemblePreIndex(char *simmOffset, uint32_t destReg, uint32_t sf, uint32_t srcReg, uint32_t instructionType) {
    uint32_t instruction = 0x1 << 31;                       //left-most bit is constant
    instruction |= (sf << 30);                              //sf bit
    instruction |= (0b1110000 << 23);                       //bits 23 to 29 are constant when not unsigned offset
    instruction |= (instructionType << 22);                 //type of data transfer
    instruction |= (0 << 21);                               //21st bit if 0 for pre/post-index
    instruction |= ((encodeSimm(simmOffset, true) & 0x1FF) << 12); //simm9
    instruction |= (0b11 << 10);                            //I and neighbouring bit
    instruction |= (srcReg << 5);                           //source register
    instruction |= destReg;                                 //destination register
    return instruction;
}

uint32_t assembleUnsignedOffset(char *immOffset, uint32_t destReg, uint32_t sf, uint32_t srcReg, uint32_t instructionType) {
    uint32_t instruction = 0x1 << 31;                       //left-most bit is constant
    instruction |= (sf << 30);                              //sf bit
    instruction |= (0b1110010 << 23);                       //bits 23 to 29 are constant
    instruction |= (instructionType << 22);                 //type of data transfer
    uint32_t offsetInt = encodeImm(immOffset, true); // imm12 not shifted
    if (sf) {
        offsetInt /= 8;                             // offsetInt divided by 8 or 4 depending on bandwidth of Rt
    } else {
        offsetInt /= 4;
    }
    instruction |= (offsetInt & 0xFFF)<< 10;      //imm12
    instruction |= (srcReg << 5);                           //source register
    instruction |= destReg;                                 //destination register
    return instruction;
}

uint32_t assembleRegisterOffset(uint32_t destReg, uint32_t offsetReg, uint32_t sf, uint32_t srcReg, uint32_t instructionType) {
    uint32_t instruction = 0x1 << 31;                       //left-most bit is constant
    instruction |= (sf << 30);                              //sf bit
    instruction |= (0b1110000 << 23);                       //bits 23 to 29 are constant when not unsigned offset
    instruction |= (instructionType << 22);                 //type of data transfer
    instruction |= (1 << 21);                               //21st bit if 0 for pre/post-index
    instruction |= offsetReg << 16;                               //destination register
    instruction |= (0b011010 << 10);                        //I and neighbouring bit
    instruction |= (srcReg << 5);                           //source register
    instruction |= destReg;                                 //destination register
    return instruction;
}

uint32_t assembleDataTransfer(char *opcode, char **operands, int operandLength, long long currentAddress, LabelAddressMap **labelMap) {
    uint32_t destReg = encodeRegister(operands[0]);
    uint32_t sf = operands[0][0] == 'w' ? 0 : 1;

    uint32_t instruction = 0;
    if (operandLength == 2 && operands[1][0] != '[') {
        printf("LITERAL\n");
        instruction = assembleLoadLiteral(opcode, operands, currentAddress, sf, destReg, labelMap);
    } else {
        uint32_t instructionType = strcmp(opcode, "ldr") == 0 ? 1 : 0;
        operands[1]++;
        uint32_t srcReg = encodeRegister(operands[1]);
        if (operandLength == 3 && operands[1][strlen(operands[1]) - 1] == ']') {
            char *srcRegister = removeLastLetter(operands[1]);
            uint32_t srcRegisterInt = encodeRegister(srcRegister);
            free(srcRegister);
            instruction = assemblePostIndex(operands, destReg, sf, srcRegisterInt, instructionType);
        } else if (operandLength == 2 || (operands[2][0] == '#' && operands[2][strlen(operands[2]) - 1] != '!')) {
            char *immOffset;
            char *srcRegister;
            if (operandLength == 2) {
                immOffset = "#0";
                srcRegister = removeLastLetter(operands[1]);
            } else {
                immOffset = removeLastLetter(operands[2]);
                srcRegister = operands[1];
            }
            uint32_t srcRegisterInt = encodeRegister(srcRegister);
            printf("Src register: %x\n", srcRegisterInt);
            printf("Offset: %s\n", immOffset);
            printf("INSIDE FUNCTION\n");
            instruction = assembleUnsignedOffset(immOffset, destReg, sf, srcRegisterInt, instructionType);
            printf("RETURNS\n");
        } else if (operands[2][strlen(operands[2]) - 1] == '!') {
            char *firstLetterRemoved = removeLastLetter(operands[2]);
            char *simmOffset = removeLastLetter(firstLetterRemoved);
            instruction = assemblePreIndex(simmOffset, destReg, sf, srcReg, instructionType);
        } else  {
            uint32_t offsetReg = encodeRegister(removeLastLetter(operands[2]));
            printf("%d\n", offsetReg);
            instruction = assembleRegisterOffset(destReg, offsetReg, sf, srcReg, instructionType);
        }
    }

    printf("--Ainstruction = %x\n", instruction);
    return instruction;
}

// FILE: branch.c

bool isBranch(char *opcode) {
    return (strcmp(opcode, "b") == 0 || strcmp(opcode, "br") == 0 || strstr(opcode, "b."));
}

bool isLabel(char *opcode) {
    return strstr(opcode, ":");
}

uint32_t assembleBranchConditional(char *condition, char *literal, long long currentAddress, LabelAddressMap **labelMap) {
    printf("--BRANCH CONDITIONAL\n");
    uint32_t instruction = 0;

    if (strstr(condition, "eq")) {
        printf("--BEQ\n");
        instruction = 0;
    } else if (strstr(condition, "ne")) {
        printf("NE\n");
        instruction = 1;
    } else if (strstr(condition, "ge")) {
        instruction = 10;
    } else if (strstr(condition, "lt")) {
        instruction = 11;
    } else if (strstr(condition, "gt")) {
        instruction = 12;
    } else if (strstr(condition, "le")) {
        instruction = 13;
    } else {
        instruction = 14;
    }

    long long offset = encodeLiteralToOffset(literal, currentAddress, labelMap);
    long long simm19 = (offset / 4) & 0x7FFFF;

    instruction |= (simm19 << 5);
    instruction |= (0x15 << 26);

    return instruction;
}

uint32_t assembleBranchUnconditional(char *literal, long long currentAddress, LabelAddressMap **labelMap) {
    uint32_t instruction = 0;

    long long offset = encodeLiteralToOffset(literal, currentAddress, labelMap);
    long long simm26 = (offset / 4) & 0x3FFFFFF;

    instruction |= simm26;
    instruction |= (0x5 << 26);

    return instruction;
}

uint32_t assembleBranchRegister(char *registerName) {
    uint32_t instruction = 0;

    instruction |= (encodeRegister(registerName) << 5);
    instruction |= (0x1F << 16);
    instruction |= (0x6B << 25);

    return instruction;
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

    return createString(token);
}

uint32_t assembleBranch(char *opcode, char **operands, long long currentAddress, LabelAddressMap **labelMap) {
    if (strstr(opcode, ".")) {
        printf("BRANCH CONDITIONAL\n");
        return assembleBranchConditional(getCondition(opcode), operands[0], currentAddress, labelMap);
    } else {
        if (strcmp(opcode, "br") == 0) {
            return assembleBranchRegister(operands[0]);
        } else {
            return assembleBranchUnconditional(operands[0], currentAddress, labelMap);
        }
    }
}

// directive.c

bool isDirective(char *opcode) {
    return strcmp(opcode, ".int") == 0;
}

uint32_t assembleDirective(char *opcode, char **operands, int operandLength) {
    return encodeSimm(operands[0], false);
}

// assemble.c

bool isVoid(char *opcode) {
    return strcmp(opcode, "nop") == 0;
}

void outputFile(uint8_t *memoryArray, char* filename, int numInstructions) {
    FILE *fp;
    fp = fopen(filename, "wb");
//    int count = 0;
//    while (memoryArray[count] != 0) {
//        count++;
//    }
    fwrite(memoryArray, sizeof(uint32_t), numInstructions, fp); //count
    fclose(fp); //Don't forget to close the file when finished
}

//void outputFile(uint8_t *memoryArray, char* filename) {
//    FILE *fp;
//    fp = fopen(filename, "wb");
//    long long memAddress = 0;
//    uint32_t data = loadData(memAddress, memoryArray, false);
//    while (memAddress < MEMORY_COUNT) {
//        memAddress += 4;
//        if (data != 0) {
//            fwrite(memoryArray, sizeof(uint32_t), count, fp);
//            fprintf(fp, "0x%08llx : %08x\n", memAddress - 4, data);
//        }
//        data = loadData(memAddress, memoryArray, false);
//    }
//    fclose(fp); //Don't forget to close the file when finished
//}

void assemble(char **assemblyArray, uint8_t *memoryArray, char *filename) {
    LabelAddressMap **labelMap = allocateLabelMap();
    computeLabelMap(assemblyArray, labelMap);

    long long address = 0;
    int line = 0;

    while (assemblyArray[line] != NULL) {
        printf("-----ASSEMBLING LINE %d-----\n", line);
        printf("assembly: '%s'\n", assemblyArray[line]);
        //line++;
        if (isLabel(assemblyArray[line])) {
            line++;
            continue;
        }

        char *currentInstruction = assemblyArray[line];
        char *opcode = extractOpcode(currentInstruction);
        printf("opc: %s\n", opcode);
        uint32_t instruction = 0;

        if (isVoid(opcode)) {
            printf("-VOID\n");
            instruction = 0xD503201F;
        } else {
            char **operands = extractOperands(currentInstruction);
            int operandLength = 4;
            for (int i = 0; i < 4; i++) {
                if (operands[i] == 0) {
                    operandLength = i;
                    break;
                }
                printf("opr %d: %s\n", i, operands[i]);
            }

            printf("operandLength: %d\n", operandLength);

            if (isDataProcessing(opcode)) {
                printf("-DATA PROCESSING\n");
                instruction = assembleDataProcessing(opcode, operands, operandLength);
            } else if (isDataTransfer(opcode)) {
                printf("-DATA TRANSFER\n");
                instruction = assembleDataTransfer(opcode, operands, operandLength, address, labelMap);
            } else if (isBranch(opcode)) {
                printf("-BRANCH\n");
                instruction = assembleBranch(opcode, operands, address, labelMap);
            } else if (isDirective(opcode)) {
                printf("-DIRECTIVE\n");
                instruction = assembleDirective(opcode, operands, operandLength);
            } else {
                printf("-----ERROR-----\n");
                printf("opcode: %s\n", opcode);
                printf("NOT RECOGNISED: %s\n", assemblyArray[line]);
                return;
            }

            free(operands);
        }

        printf("Returned instruction: %x\n", instruction);

        storeData(instruction, address, memoryArray, false);
        printf("END\n");
        address += 4;
        line++;
    }

    outputFile(memoryArray, filename, line);
    freeLabelMap(labelMap);
}

int main(int argc, char **argv) {
    char **assemblyArray;
    char *outputFile;
    if (argc != 1) {
        assemblyArray = loadAssemblyFromFile(argv[1]);
        outputFile = argv[2];
    } else {
        assemblyArray = loadAssemblyFromFile("../../armv8_testsuite/test/test_cases/general/str01.s");
        outputFile = "output.bin";
    }

    uint8_t *memPointer = calloc(MEMORY_COUNT, sizeof(uint8_t));

//    LabelAddressMap **labelMap = allocateLabelMap();
//    computeLabelMap(assemblyArray, labelMap);

    assemble(assemblyArray, memPointer, outputFile);

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


