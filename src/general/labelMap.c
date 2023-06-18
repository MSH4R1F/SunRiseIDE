//
// Created by Faraz Malik on 17/06/2023.
//

#include "labelMap.h"

#include "constants.h"

bool isLabel(char *opcode) {
    return strstr(opcode, ":");
}

LabelAddressMap *allocateLabelEntry(void) {
    LabelAddressMap *entry = malloc( sizeof(LabelAddressMap) );
    entry->wasAllocated = false;
    return entry;
}

LabelAddressMap **allocateLabelMap(void) {
    LabelAddressMap **mapPointer = malloc( ASSEMBLY_SIZE * sizeof(LabelAddressMap *) );

    for (int i = 0; i < ASSEMBLY_SIZE; i++) {
        mapPointer[i] = allocateLabelEntry();
    }

    return mapPointer;
}

void freeLabelMap(LabelAddressMap **mapPointer) {
    for (int i = 0; i < ASSEMBLY_SIZE; i++) {
        free(mapPointer[i]);
    }
    free(mapPointer);
}

void computeLabelMap(char **assemblyArray, LabelAddressMap **labelMap) {
    int i = 0;
    long long line = 0;
    long long address = 0;

    while (assemblyArray[line] != NULL) {
        if (isLabel(assemblyArray[line])) {
            char *label = assemblyArray[line];

            labelMap[i]->label = label;
            labelMap[i]->address = address;
            labelMap[i]->wasAllocated = true;
            i++;
        } else {
            address += 4;
        }

        line += 1;
    }
}

long long getMapAddress(LabelAddressMap **mapPointer, char *label) {
    for (int i = 0; i < ASSEMBLY_SIZE; i++) {
        if (strstr(mapPointer[i]->label, label)) {
            return mapPointer[i]->address;
        }
    }
    return 0;
}

bool mapContainsLabel(char *label, LabelAddressMap **mapPointer) {
    for (int i = 0; i < ASSEMBLY_SIZE; i++) {
        if (!mapPointer[i]->wasAllocated) {
            continue;
        }
        if (strstr(mapPointer[i]->label, label)) {
            return true;
        }
    }

    return false;
}