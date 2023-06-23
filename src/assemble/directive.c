#include "directive.h"

#include "key.h"

/// Checks if a given opcode is for a directive
bool isDirective(char *opcode) {
    return strcmp(opcode, ".int") == 0;
}

/// Returns the integer representation of the directive
uint32_t assembleDirective(char *opcode, char **operands, int operandLength) {
    return encodeSimm(operands[0], false);
}