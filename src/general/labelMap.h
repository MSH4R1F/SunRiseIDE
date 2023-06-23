//
// Created by Faraz Malik on 17/06/2023.
//

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifndef ARMV8_27_LABELMAP_H
    #define ARMV8_27_LABELMAP_H

    bool isLabel(char *opcode);

    typedef struct {
        char *label;
        long long address;
        bool wasAllocated;
    } LabelAddressMap;

    LabelAddressMap *allocateLabelEntry();
    LabelAddressMap **allocateLabelMap();
    void freeLabelMap(LabelAddressMap **mapPointer);

    void computeLabelMap(char **assemblyArray, LabelAddressMap **labelMap);
    long long getMapAddress(LabelAddressMap **mapPointer, char *label);
    bool mapContainsLabel(char *label, LabelAddressMap **mapPointer);
#endif //ARMV8_27_LABELMAP_H
