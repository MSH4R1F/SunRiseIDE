//
// Created by Faraz Malik on 17/06/2023.
//

#include "tokenUtils.h"

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

char *createString(char *str) {
    char *heapString = malloc(strlen(str) + 1);
    if (str == NULL) {
        return NULL;
    }

    strcpy(heapString, str);
    heapString[strlen(str)] = '\0';
    return heapString;
}