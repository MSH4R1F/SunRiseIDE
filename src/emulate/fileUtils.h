//
// Created by Faraz Malik on 09/06/2023.
//

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "registers.h"

#ifndef ARMV8_27_UTILS_H
    #define ARMV8_27_UTILS_H

    void loadMemoryFromFile(uint8_t *memPointer, char *filename);

    void outputFile(struct RegisterStore *registers, struct PSTATE *stateRegister, uint8_t *memPointer, char* filename);
    void outputTerminal(struct RegisterStore *registers, struct PSTATE *stateRegister, uint8_t *memPointer);
#endif //ARMV8_27_UTILS_H
