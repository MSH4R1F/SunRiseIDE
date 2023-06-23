#include "tokenUtils.h"

/// Returns the opcode from a given line of assembly
char *extractOpcode(char *instruction) {
    char *line_copy = strdup(instruction);
    char *opcode = strtok(line_copy, " ");
    if (opcode != NULL) {
        opcode = strdup(opcode);
    }
    free(line_copy);
    return opcode;
}

/// Returns the operands from a line of assembly as a single string
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

/// Splits the single string of operands into an array of operands, split by comma
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

/// A helper function which creates a copy of a string on the heap
char *createString(char *str) {
    char *heapString = malloc(strlen(str) + 1);
    if (str == NULL) {
        return NULL;
    }

    strcpy(heapString, str);
    heapString[strlen(str)] = '\0';
    return heapString;
}