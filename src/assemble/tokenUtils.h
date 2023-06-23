//
// Created by Faraz Malik on 17/06/2023.
//

#include <stdlib.h>
#include <string.h>

#ifndef ARMV8_27_TOKENUTILS_H
    #define ARMV8_27_TOKENUTILS_H

    char *extractOpcode(char *instruction);
    char *getOperands(char *opcode);
    char **extractOperands(char *instruction);
    char *createString(char *str);
#endif //ARMV8_27_TOKENUTILS_H
