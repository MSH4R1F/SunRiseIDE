#include "dataProcessing.h"

#include "tokenUtils.h"
#include "key.h"

/// Private function declarations
static char *getShift(char *opcode);
static char *getImm(char *opcode);
static uint32_t assembleMaddMsub(char *opcode, char **operands);
static uint32_t assembleMultiply(char *opcode, char **operands);
static uint32_t assembleMove(char *opcode, char **operands, int operandLength);
static uint32_t assembleArithmeticLogic(char *opcode, char **operands, int operandLength);
static uint32_t assembleBitLogic(char *opcode, char **operands, int operandLength);
static uint32_t assembleTst(char *opcode, char **operands, int operandLength);
static uint32_t assembleMov(char *opcode, char **operands, int operandLength);
static uint32_t assembleNegNegs(char *opcode, char **operands, int operandLength);
static uint32_t assembleCmp(char *opcode, char **operands, int operandLength);

/// Checks if the opcode refers to a data processing instruction
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

/// Calls the correct helper function to return the binary representation for a data processing instruction
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

/// Returns the shift as a string ("lsl" from input "lsl #12")
static char *getShift(char *opcode) {
    char opcodeDefinite[strlen(opcode) + 1];
    for (int i = 0; i < strlen(opcode); i++) {
        opcodeDefinite[i] = opcode[i];
    }
    opcodeDefinite[strlen(opcode)] = '\0';

    char* rest = opcodeDefinite;
    char* token = strtok_r(opcodeDefinite, " ", &rest);

    return createString(token);
}

/// Returns the immediate value as a string ("#12" from input "lsl #12")
static char *getImm(char *opcode) {
    char opcodeDefinite[strlen(opcode) + 1];
    for (int i = 0; i < strlen(opcode); i++) {
        opcodeDefinite[i] = opcode[i];
    }
    opcodeDefinite[strlen(opcode)] = '\0';

    char* rest = opcodeDefinite;
    char* token = strtok_r(opcodeDefinite, " ", &rest);

    return createString(rest);
}

/// Returns the binary instruction for a madd/msub instruction
static uint32_t assembleMaddMsub(char *opcode, char **operands) {
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

/// Returns the binary instruction for a mul instruction
static uint32_t assembleMultiply(char *opcode, char **operands) {
    char *zeroRegister = calloc(4, sizeof(char));
    zeroRegister[0] = operands[0][0];
    strcpy(zeroRegister + 1, "zr");
    operands[3] = zeroRegister;
    opcode = strcmp(opcode, "mul") == 0 ? "madd" : "msub";
    return assembleMaddMsub(opcode, operands);
}

/// Returns the binary instruction for a wide move instruction
static uint32_t assembleMove(char *opcode, char **operands, int operandLength) {
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

/// Returns the binary instruction for an arithmetic logic instruction
static uint32_t assembleArithmeticLogic(char *opcode, char **operands, int operandLength) {
    char *ops[] = {"add", "adds", "sub", "subs"};
    uint32_t instruction = 0;
    instruction |= (operands[0][0] == 'x') << 31;  // sf bit
    uint32_t opc = 0;
    for (int i = 0; i < 4; i++) {
        if (strcmp(opcode, ops[i]) == 0) {
            opc = i;
            break;
        }
    }
    instruction |= opc << 29;  // opc bit
    if (operands[2][0] == '#') {  // Immediate instruction
        instruction |= 0b100 << 26; //bits 26 to 28
        instruction |= 0b010 << 23;  // set bits 25:23 for "010"
        if (operandLength == 4) {
            if (operands[3][5] == '1') {
                instruction |= 1 << 22;
            }
        }
        uint32_t operand = encodeImm(operands[2], true);
        instruction |= operand << 10;  // operand bits
        instruction |= encodeRegister(operands[1]) << 5;
        uint32_t rd = encodeRegister(operands[0]);
        instruction |= rd;  // Rd bits

    } else {  // Register instruction
        instruction |= 0b0101 << 25;  // set bits 27:25 for "101"
        uint32_t shift = 0;
        uint32_t operand = 0;
        if (operandLength == 4) {
            shift = encodeShift(getShift(operands[3]));
            operand = encodeImm(getImm(operands[3]), true);
        }
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
    return instruction;
}

/// Returns the binary instruction for a bit logic instruction
static uint32_t assembleBitLogic(char *opcode, char **operands, int operandLength) {
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

/// Returns the binary instruction for a tst instruction
static uint32_t assembleTst(char *opcode, char **operands, int operandLength) {
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

/// Returns the binary instruction for a mov instruction
static uint32_t assembleMov(char *opcode, char **operands, int operandLength) {
    char *shiftString = "lsl #0";
    if (operandLength == 3) {
        shiftString = operands[2];
    }
    char *newOps[] = {operands[0], "xzr", operands[1],  shiftString};
    opcode = "orr";
    return assembleBitLogic(opcode, newOps, 4);
}

/// Returns the binary instruction for a neg/negs instruction
static uint32_t assembleNegNegs(char *opcode, char **operands, int operandLength) {
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

/// Returns the binary instruction for a cmp instruction
static uint32_t assembleCmp(char *opcode, char **operands, int operandLength) {
    char *zeroRegister = calloc(4, sizeof(char));
    zeroRegister[0] = operands[0][0];
    strcpy(zeroRegister + 1, "zr");

    char *newOperands[4];
    newOperands[0] = zeroRegister;
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
    return assembleArithmeticLogic(newOpcode, newOperands, operandLength + 1);
}
