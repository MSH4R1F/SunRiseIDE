#include "fileUtils.h"

#include "../general/memory.h"
#include "../general/constants.h"

/// Reads all the instructions from the input file and stores them in memory
void loadMemoryFromFile(uint8_t *memPointer, char *filename) {
    int counter = 0;
    FILE *file = fopen(filename, "rb");  // Open the binary file in read mode

    if (file == NULL) {
        return;
    }

    while (true) {
        uint8_t value;  // Variable to store the read integer

        if (fseek(file, counter, SEEK_SET) != 0) {
            fclose(file);
            return;
        }

        // Read 1 byte from the file into the variable 'value'
        if (fread(&value, sizeof(value), 1, file) != 1) {
            fclose(file);
            return;
        }

        memPointer[counter] = value;
        counter += 1;
    }
}

/// Provides the core functionality of printing to a file or standard output
static void output(struct __sFILE *target, struct RegisterStore *registers, struct PSTATE *stateRegister, uint8_t *memPointer) {
    fprintf(target, "Registers:\n");
    for (int i = 0; i < 31; i++) {
        fprintf(target, "X%02d    = %016llx\n", i, registers->registers[i]);
    }
    fprintf(target, "PC     = %016llx\n", registers->programCounter);
    char n_val = stateRegister->negativeFlag ? 'N' : '-';
    char z_val = stateRegister->zeroFlag ? 'Z' : '-';
    char c_val = stateRegister->carryFlag ? 'C' : '-';
    char v_val = stateRegister->overflowFlag ? 'V' : '-';
    fprintf(target, "PSTATE : %c%c%c%c\n", n_val, z_val, c_val, v_val);
    fprintf(target, "Non-Zero Memory:\n");

    long long memAddress = 0;
    uint32_t data = loadData(memAddress, memPointer, false);
    while (memAddress < MEMORY_COUNT) {
        memAddress += 4;
        if (data != 0) {
            // Load 32 bit word and convert from little endian
            fprintf(target, "0x%08llx : %08x\n", memAddress - 4, data);
        }
        data = loadData(memAddress, memPointer, false);
    }
}

/// Prints output to a file
void outputFile(struct RegisterStore *registers, struct PSTATE *stateRegister, uint8_t *memPointer, char* filename) {
    FILE *fp;
    fp = fopen(filename, "w");

    output(fp, registers, stateRegister, memPointer);

    fclose(fp); //Don't forget to close the file when finished
}

/// Prints output to terminal
void outputTerminal(struct RegisterStore *registers, struct PSTATE *stateRegister, uint8_t *memPointer) {
    output(stdout, registers, stateRegister, memPointer);
}