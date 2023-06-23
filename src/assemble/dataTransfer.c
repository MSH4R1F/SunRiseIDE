#include "dataTransfer.h"

#include "key.h"

/// Private function declarations
static char *removeLastLetter(char* string);
static uint32_t assembleLoadLiteral(char **operands, long long currentAddress, uint32_t sf, uint32_t destReg, LabelAddressMap **labelMap);
static uint32_t assemblePostIndex(char **operands, uint32_t destReg, uint32_t sf, uint32_t srcReg, uint32_t instructionType);
static uint32_t assemblePreIndex(char *simmOffset, uint32_t destReg, uint32_t sf, uint32_t srcReg, uint32_t instructionType);
static uint32_t assembleUnsignedOffset(char *immOffset, uint32_t destReg, uint32_t sf, uint32_t srcReg, uint32_t instructionType);
static uint32_t assembleRegisterOffset(uint32_t destReg, uint32_t offsetReg, uint32_t sf, uint32_t srcReg, uint32_t instructionType);

/// Checks if an opcode refers to a data transfer instruction
bool isDataTransfer(char *opcode) {
    return strcmp(opcode, "ldr") == 0 || strcmp(opcode, "str") == 0;
}

/// Calls the correct helper function to return the binary representation for a data transfer instruction
uint32_t assembleDataTransfer(char *opcode, char **operands, int operandLength, long long currentAddress, LabelAddressMap **labelMap) {
    uint32_t destReg = encodeRegister(operands[0]);
    uint32_t sf = operands[0][0] == 'w' ? 0 : 1;

    uint32_t instruction = 0;
    if (operandLength == 2 && operands[1][0] != '[') {
        instruction = assembleLoadLiteral(operands, currentAddress, sf, destReg, labelMap);
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
            instruction = assembleUnsignedOffset(immOffset, destReg, sf, srcRegisterInt, instructionType);
        } else if (operands[2][strlen(operands[2]) - 1] == '!') {
            char *firstLetterRemoved = removeLastLetter(operands[2]);
            char *simmOffset = removeLastLetter(firstLetterRemoved);
            instruction = assemblePreIndex(simmOffset, destReg, sf, srcReg, instructionType);
        } else  {
            uint32_t offsetReg = encodeRegister(removeLastLetter(operands[2]));
            instruction = assembleRegisterOffset(destReg, offsetReg, sf, srcReg, instructionType);
        }
    }

    return instruction;
}

/// Helper function which removes the last letter of a string
static char *removeLastLetter(char* string) {
    char *strPointer = malloc(strlen(string) * sizeof(char));
    for (int i = 0; i < strlen(string) - 1; i++) {
        strPointer[i] = string[i];
    }
    strPointer[strlen(string) - 1] = '\0';
    return strPointer;
}

/// Returns the binary encoding for a load literal assembly instruction
static uint32_t assembleLoadLiteral(char **operands, long long currentAddress, uint32_t sf, uint32_t destReg, LabelAddressMap **labelMap) {
    uint32_t instruction = 0x0;                                                         //left-most bit is constant
    instruction |= (sf << 30);                                                                //sf bit
    instruction |= (0b011000 << 24);//bits 24 to 29 are constant when not unsigned offset
    uint32_t simm19 = (encodeLiteralToOffset(operands[1], currentAddress, labelMap) / 4) & 0x7FFFF;
    instruction |= simm19 << 5; //type of data transfer
    instruction |= destReg;                                                                   //destination register
    return instruction;
}

/// Returns the binary encoding for a post-index data transfer instruction
static uint32_t assemblePostIndex(char **operands, uint32_t destReg, uint32_t sf, uint32_t srcReg, uint32_t instructionType) {
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

/// Returns the binary encoding for a pre-index data transfer instruction
static uint32_t assemblePreIndex(char *simmOffset, uint32_t destReg, uint32_t sf, uint32_t srcReg, uint32_t instructionType) {
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

/// Returns the binary encoding for an unsigned offset data transfer instruction
static uint32_t assembleUnsignedOffset(char *immOffset, uint32_t destReg, uint32_t sf, uint32_t srcReg, uint32_t instructionType) {
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

/// Returns the binary encoding for a register offset data transfer instruction
static uint32_t assembleRegisterOffset(uint32_t destReg, uint32_t offsetReg, uint32_t sf, uint32_t srcReg, uint32_t instructionType) {
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