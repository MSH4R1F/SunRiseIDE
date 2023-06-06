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
    if (strcmp(operand, "SP") == 0) {
        return 0b11111;
    } else {
        operand++;
        return strtol(operand, NULL, 0);
    }
}

uint32_t encodeImm(char *operand) {
    operand++;
    return strtol(operand, NULL, 0);
}

uint32_t encodeSimm(char *operand) {
    operand++;
    return strtol(operand, NULL, 0);
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

bool isDataProcessing(char *opcode) {
    char *ops[] = {"add", "ads", "sub", "subs", "cmp", "cmn", "neg", "negs", "and", "ands",
                   "bic", "bics", "eor", "orr", "eon", "orn", "tst", "movk", "movn",
                   "movz", "mov", "mvn", "madd", "msub", "mul", "mneg"};
    for (int i = 0; i < ops; i++) {
        if (strcmp(opcode, ops[i]) == 0) {
            return true;
        }
    }
    return false;
}

uint32_t assembleDataProcessing(char *opcode, char **operands, int operandLength) {
    uint32_t instruction = 0;

    return instruction;
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

        char **newArray = malloc(lines * sizeof(char *));
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

        storeData(instruction, address, memoryArray, true);
        address += 4;
    }

    freeLabelMap(labelMap);
}

int main(int argc, char **argv) {
    char **assemblyArray;

//    assemblyArray[0] = "b execute";
//    assemblyArray[1] = "movz x2,#1";
//    assemblyArray[2] = "execute:";
//    assemblyArray[3] = "movz x1,#1";
//    assemblyArray[4] = "and x0,x0,x0";
    if (argc == 1) {
        assemblyArray = loadAssemblyFromFile("../../directpath");
    } else {
        assemblyArray = loadAssemblyFromFile(argv[1]);
    }

//
//    uint8_t *memoryArray = allocateMemory();
//    assemble(assemblyArray, memoryArray);
//
//    writeMachineToFile(memoryArray, "output.out");
//
//    free(assemblyArray);

    char *name = "hellO";
    char *other = "bye";

    char **array = malloc(4 * sizeof(char *) );
    array[0] = name;
    array[1] = other;

    return EXIT_SUCCESS;
}


