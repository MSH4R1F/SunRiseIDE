//
// Created by Faraz Malik on 13/06/2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <assert.h>

#include "sunrise.h"

#define ASSEMBLY_SIZE 10
#define MAX_LINE_SIZE 256
#define MAX_ERROR_SIZE 256

// FILE: labelMap.c

bool isLabel(char *opcode) {
    return strstr(opcode, ":");
}

typedef struct {
    char *label;
    long long address;
    bool wasAllocated;
} LabelAddressMap;

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

// utils.c

//CEND      = '\33[0m';
//CBOLD     = '\33[1m';
//CITALIC   = '\33[3m';
//CURL      = '\33[4m';
//CBLINK    = '\33[5m';
//CBLINK2   = '\33[6m';
//CSELECTED = '\33[7m';
//
//CBLACK  = '\33[30m';
//CRED    = '\33[31m';
//CGREEN  = '\33[32m';
//CYELLOW = '\33[33m';
//CBLUE   = '\33[34m';
//CVIOLET = '\33[35m';
//CBEIGE  = '\33[36m';
//CWHITE  = '\33[37m';

//
//CGREY    = '\33[90m';
//CRED2    = '\33[91m';
//CGREEN2  = '\33[92m';
//CYELLOW2 = '\33[93m';
//CBLUE2   = '\33[94m';
//CVIOLET2 = '\33[95m';
//CBEIGE2  = '\33[96m';
//CWHITE2  = '\33[97m';
//


typedef enum {
    ITALIC,
    BOLD,
    RED,
    REDDER,
    POTATO,
    YELLOW,
    YELLOWER,
    GREY,
    BLUE,
    BLUER,
    VIOLET,
    VIOLETER,
    GREEN,
    RESET
} TerminalColour;

void setTerminalColour(TerminalColour colour) {
    switch (colour) {
        case ITALIC:
            printf("\33[3m"); break;
        case BOLD:
            printf("\33[1m"); break;
        case RED:
            printf("\33[1;31m"); break;
        case POTATO:
            printf("\33[93m"); break;
        case YELLOW:
            printf("\33[1;33m"); break;
        case YELLOWER:
            printf("\33[93m"); break;
        case REDDER:
            printf("\33[91m"); break;
        case GREY:
            printf("\33[90m"); break;
        case BLUE:
            printf("\33[34m"); break;
        case BLUER:
            printf("\33[94m"); break;
        case VIOLET:
            printf("\33[35m"); break;
        case VIOLETER:
            printf("\33[95m"); break;
        case GREEN:
            printf("\33[32m"); break;
        case RESET:
            printf("\033[0m"); break;
    }
}

void resetTerminalColour(void) {
    setTerminalColour(RESET);
}

char** loadAssemblyLinesFromFile(char *filename) {
    FILE *fp = fopen(filename, "r");

    if( !fp ) {
        fprintf(stderr, "Opened file: %s\n", filename);
    }

    // Count Lines
    char cr;
    size_t lines = 0;

    while( cr != EOF ) {
        if ( cr == '\n' ) {
            lines++;
        }
        cr = getc(fp);
    }
    rewind(fp);

    char **data = calloc(lines, sizeof(char *));
    size_t n;

    for (size_t i = 0; i < lines; i++) {
        data[i] = NULL;
        n = 0;

        getline(&data[i], &n, fp);

        if (ferror( fp )) {
            fprintf(stderr, "Error reading from file\n");
        }

        data[i][strlen(data[i]) - 1] = '\0';
    }
    fclose(fp);

    return data;
}

char **loadAssemblyFromLines(char **data, int lines) {
    char **newArray = calloc(lines + 1, sizeof(char *));
    int next = 0;

    for (int i = 0; i < lines; i++) {
        while (data[i][0] == ' ') {
            data[i]++;
        }

        if (strlen(data[i]) != 1) {
            newArray[next] = data[i];

            int endIndex = strlen(newArray[next]) - 1;
            while (newArray[next][endIndex] == ' ') {
                newArray[next][endIndex] = '\0';
                endIndex--;
            }
            next++;
        }

    }
    return newArray;
}

bool getEditorExists(void) {
    FILE *file = fopen("editor.s", "r");
    if (file == NULL) {
        return false;
    } else {
        fclose(file);
        return true;
    }
}

void openAssemblyEditor(void) {
    if (getEditorExists()) {
        printf("'editor.s' found!\n");
    } else {
        printf("Creating file 'editor.s'...\n");
        system("touch editor.s");
    }

    printf("Opening file...\n");
    system("open editor.s");
}

bool stringBeginsWith(char delimiter, char *string) {
    int i = 0;
    while (isspace(string[i])) {
        i++;
    }
    return string[i] == delimiter;
}

void addPrintMessage(char **errors, int line, char *lineString, char *message) {
    char *error = calloc(256, sizeof(char));
    sprintf(strlen(error) + error, "ERROR line %d:\n", line);
    sprintf(strlen(error) + error, "  code - '%s'\n", lineString);
    sprintf(strlen(error) + error, "  mesg - %s\n", message);

    errors[line - 1] = error;
}

// patternMatch.c

typedef enum {
    OPC,
    REG,
    XREG,
    SHIFT,
    IMM,
    LITERAL,
    SIMM
} keyPatterns;

char *getKeyPattern(keyPatterns willy) {
    char *pattern = calloc(MAX_LINE_SIZE, sizeof(char));
    switch (willy) {
        case OPC:
            sprintf(pattern, "[a-z]{1-4}"); break;
        case REG:
            sprintf(pattern, "(x|w)[0-9]{1,2}"); break;
        case XREG:
            sprintf(pattern, "x[0-9]{1,2}"); break;
        case SHIFT:
            sprintf(pattern, "((a|l)s(s|r))"); break;
        case IMM:
            sprintf(pattern, "(0x([0-9]|[a-z]+)|([0-9]+)"); break;
        case LITERAL:
            sprintf(pattern, "(.[a-z]+)|%s",
                    getKeyPattern(IMM)); break;
        case SIMM:
            sprintf(pattern, "-?%s",
                    getKeyPattern(IMM)); break;
    }
    return pattern;
}

typedef enum {
    SHIFT_IMM,
    LS_12,
    DP_NEGS,
    DP_ADD,
    DP_CMP,
    DP_BITWISE,
    DP_TST,
    DP_MOVX,
    DP_MOV,
    DP_MVN,
    DP_MADD,
    DP_MUL
} dpPatterns;

char *getDpPattern(dpPatterns willy) {
    char *pattern = calloc(MAX_LINE_SIZE, sizeof(char));
    switch (willy) {
        case LS_12:
            sprintf(pattern, " *, *lsl +#(0)|(12)"); break;
        case SHIFT_IMM:
            sprintf(pattern, " *, *%s +#%s",
                    getKeyPattern(SHIFT), getKeyPattern(IMM)); break;
        case DP_ADD:
            sprintf(pattern, "^ *(add)|(sub)s? +%s *, *%s *, *(#%s(%s)?)|(%s(%s)?) *$",
                    getKeyPattern(REG), getKeyPattern(REG),
                    getKeyPattern(IMM), getDpPattern(LS_12),
                    getKeyPattern(REG), getDpPattern(SHIFT_IMM)); break;
        case DP_CMP:
            sprintf(pattern, "^ *cmp|n +%s *, *(#%s(%s)?)|(%s(%s)?) *$",
                    getKeyPattern(REG), getKeyPattern(IMM),
                    getDpPattern(LS_12), getKeyPattern(REG),
                    getDpPattern(SHIFT_IMM)); break;
        case DP_NEGS:
            sprintf(pattern, "^ *negs? +%s *, *(#%s(%s)?)|(%s(%s)?) *$",
                    getKeyPattern(REG), getKeyPattern(IMM),
                    getDpPattern(LS_12), getKeyPattern(REG),
                    getDpPattern(SHIFT_IMM)); break;
        case DP_BITWISE:
            sprintf(pattern, "^ *(ands?)|(bics?)|(eor)|(orr)|(eon)|(orn) +%s *, *%s *, *%s%s *$",
                    getKeyPattern(REG), getKeyPattern(REG),
                    getKeyPattern(REG), getDpPattern(SHIFT_IMM)); break;
        case DP_TST:
            sprintf(pattern, "^ *tst +%s *, *%s(%s)? *$",
                    getKeyPattern(REG), getKeyPattern(REG),
                    getDpPattern(SHIFT_IMM)); break;
        case DP_MOVX:
            sprintf(pattern, "^ *movk|n|z +%s *, *#%s%s *$",
                    getKeyPattern(REG), getKeyPattern(IMM),
                    getDpPattern(SHIFT_IMM)); break;
        case DP_MOV:
            sprintf(pattern, "^ *(mov)|(mvn) +%s *, *%s *$",
                    getKeyPattern(REG), getKeyPattern(REG)); break;
        case DP_MVN:
            sprintf(pattern, "^ *(mov)|(mvn) +%s *, *%s%s *$",
                    getKeyPattern(REG), getKeyPattern(REG),
                    getDpPattern(SHIFT_IMM)); break;
        case DP_MADD:
            sprintf(pattern, "^ *m(add)|(sub) +%s *, *%s *, *%s *, *%s *$",
                    getKeyPattern(REG), getKeyPattern(REG),
                    getKeyPattern(REG), getKeyPattern(REG)); break;
        case DP_MUL:
            sprintf(pattern, "^ *m(ul)|(neg) +%s *, *%s *, *%s *$",
                    getKeyPattern(REG), getKeyPattern(REG),
                    getKeyPattern(REG)); break;
    }

    return pattern;
}

typedef enum {
    B,
    BR
} branchPatterns;

char *getBranchPattern(dpPatterns willy) {
    char *pattern = calloc(MAX_LINE_SIZE, sizeof(char));
    switch (willy) {
        case B:
            sprintf(pattern, "^ *b(.cond)? +%s *$",
                    getKeyPattern(LITERAL));
            break;
        case BR:
            sprintf(pattern, "^ *br +%s *$",
                    getKeyPattern(XREG));
            break;
    }
    return pattern;
}

bool linePatternFailsCheck(char *line) {
    regex_t regex;

    int value;
    value = regcomp(&regex, getKeyPattern(OPC), 0);

    assert(value == 0);
}

bool checkSyntax(char *filename) {
    char **assemblyArray = loadAssemblyLinesFromFile(filename);
    int i = 0;
    while (assemblyArray[i] != NULL) {
        i++;
    }
    int linesCount = i;
    char **errorsArray = calloc(i + 1, sizeof(char *));

    bool foundSyntaxError = false;
    for (i = 0; i < linesCount; i++) {
        if (strlen(assemblyArray[i]) == 0) {
            continue;
        }

        char *currentLine = assemblyArray[i];

        if (isLabel(currentLine)) {
            if (stringBeginsWith('.', currentLine)) {
                continue;
            } else {
                addPrintMessage(errorsArray, i + 1, currentLine, "label must begin with '.'");
                foundSyntaxError = true;
            }
        } else {
            if (linePatternFailsCheck(currentLine)) {
                addPrintMessage(errorsArray, i + 1, currentLine, "assembly not recognised");
                foundSyntaxError = true;
            }

            // run regex pattern match
            // if fails, print not-recognised style
        }
    }

    if (!foundSyntaxError) {
        return true;
    }

    setTerminalColour(YELLOW);
    printf("COMPILE FAILED\nERRORS FOUND:\n\n");

    setTerminalColour(RED);
    for (i = 0; i < linesCount; i++) {
        if (errorsArray[i] != NULL) {
            char *currentError = errorsArray[i];
            printf("%s\n", currentError);
        }
    }
    resetTerminalColour();
    return false;
}

bool assemble(void) {
    if (!getEditorExists()) {
        printf("'editor.s' not found.\n");
        return false;
    }

    if (!checkSyntax("editor.s")) {
        return false;
    }

    system("./assemble editor.s output.bin");
    return true;
}

void emulate(void) {
    printf("----RESULT----\n\n");
    system("./emulate output.bin");
}

int main(void) {
    system("clear");
    printf("Welcome to sunriseIDE\n");
    while (true) {
        char *target = malloc(16 * sizeof(char));
        printf("\n----MENU----\n\n");
        printf("  1. Open editor\n");
        printf("  2. Build \u2692 \n");
        printf("  3. Run \u25b6 \n");
        printf("  4. Debug \n");
        printf("  5. Exit\n");
        printf("\nChoice: ");
        char *option = fgets(target, 16, stdin);
        printf("\n");

        switch (option[0]) {
            case '1':
                openAssemblyEditor();
                break;
            case '2':
                if (!assemble()) {
                    continue;
                } else {
                    setTerminalColour(GREEN);
                    printf("\nCOMPILE SUCCEEDED\n\n");
                    resetTerminalColour();
                }
                break;
            case '3':
                if (!assemble()) {
                    continue;
                } else {
                    setTerminalColour(GREEN);
                    printf("\nCOMPILE SUCCEEDED\n\n");
                    resetTerminalColour();
                }
                emulate();
                break;
            case '5':
                system("clear");
                return EXIT_SUCCESS;
            default:
                printf("%c not recognised\n", option[0]);
        }
    }
}