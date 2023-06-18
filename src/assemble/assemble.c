
#include "assemble.h"

#include "../general/memory.h"
#include "../general/labelMap.h"
#include "../general/assembleFileUtils.h"
#include "key.h"
#include "tokenUtils.h"
#include "branch.h"
#include "dataProcessing.h"
#include "dataTransfer.h"
#include "directive.h"

// private function declarations
static bool isVoid(char *opcode);
static void runAssemble(char **assemblyArray, uint8_t *memoryArray, char *filename);

int assemble(int argc, char **argv) {
    char **assemblyArray;
    char *outputFile;
    if (argc != 1) {
        assemblyArray = loadAssemblyFromFile(argv[1]);
        outputFile = argv[2];
    } else {
        assemblyArray = loadAssemblyFromFile("../../../armv8_testsuite/test/test_cases/general/str01.s");
        outputFile = "output.bin";
    }

    uint8_t *memPointer = allocateMemory();
    runAssemble(assemblyArray, memPointer, outputFile);

    free(memPointer);

    return EXIT_SUCCESS;
}

static bool isVoid(char *opcode) {
    return strcmp(opcode, "nop") == 0;
}

static void runAssemble(char **assemblyArray, uint8_t *memoryArray, char *filename) {
    LabelAddressMap **labelMap = allocateLabelMap();
    computeLabelMap(assemblyArray, labelMap);

    long long address = 0;
    int line = 0;

    while (assemblyArray[line] != NULL) {
        printf("line: '%s'\n", assemblyArray[line]);
        if (isLabel(assemblyArray[line])) {
            line++;
            continue;
        }

        char *currentInstruction = assemblyArray[line];
        char *opcode = extractOpcode(currentInstruction);
        uint32_t instruction = 0;

        if (isVoid(opcode)) {
            instruction = 0xD503201F;
        } else {
            char **operands = extractOperands(currentInstruction);
            int operandLength = 4;
            for (int i = 0; i < 4; i++) {
                if (operands[i] == 0) {
                    operandLength = i;
                    break;
                }
            }

            if (isDataProcessing(opcode)) {
                instruction = assembleDataProcessing(opcode, operands, operandLength);
            } else if (isDataTransfer(opcode)) {
                instruction = assembleDataTransfer(opcode, operands, operandLength, address, labelMap);
            } else if (isBranch(opcode)) {
                instruction = assembleBranch(opcode, operands, address, labelMap);
            } else if (isDirective(opcode)) {
                instruction = assembleDirective(opcode, operands, operandLength);
            } else {
                return;
            }

            free(operands);
        }

        storeData(instruction, address, memoryArray, false);
        address += 4;
        line++;
    }

    writeMachineToFile(memoryArray, filename, line);
    freeLabelMap(labelMap);
}
