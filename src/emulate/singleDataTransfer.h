#include <stdint.h>

#include "registers.h"

#ifndef ARMV8_27_SINGLEDATATRANSFER_H
    #define ARMV8_27_SINGLEDATATRANSFER_H

    void executeDataTransfer(uint32_t instruction, uint8_t *memPointer, struct RegisterStore *registerStore);
#endif //ARMV8_27_SINGLEDATATRANSFER_H
