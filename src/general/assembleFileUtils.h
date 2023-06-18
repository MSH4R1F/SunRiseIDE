//
// Created by Faraz Malik on 18/06/2023.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifndef ARMV8_27_ASSEMBLEFILEUTILS_H
    #define ARMV8_27_ASSEMBLEFILEUTILS_H

    char **loadAssemblyLinesFromFile(char *filename);
    char** loadAssemblyFromFile(char *filename);
    void writeMachineToFile(uint8_t *memPointer, char *filename, int numInstructions);
#endif //ARMV8_27_ASSEMBLEFILEUTILS_H
