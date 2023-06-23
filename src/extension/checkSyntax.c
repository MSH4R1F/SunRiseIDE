//
// Created by Faraz Malik on 18/06/2023.
//

#include "checkSyntax.h"

#include "../general/constants.h"
#include "../general/terminalColor.h"
#include "../general/assembleFileUtils.h"
#include "../general/labelMap.h"

#include "../assembler/branch.h"
#include "../assembler/tokenUtils.h"

#include "patternMatch.h"

// private functions declaration
static bool stringBeginsWithDot(char *string);
static bool checkLabels(char **assemblyLines, char **errorsArray);
static void addErrorMessage(char **errors, int line, char *lineString, char *message);
static bool matchFails(char *lineString, char *pattern);
static bool lineWasValid(char *lineString, char **errorsArray, int line);

bool checkSyntax(char *filename) {
    char **assemblyLines = loadAssemblyLinesFromFile(filename);
    int i = 0;
    while (assemblyLines[i] != NULL) {
        i++;
    }
    int linesCount = i;
    char **errorsArray = calloc(linesCount + 1, sizeof(char *));

    bool foundSyntaxError = false;
    for (i = 0; i < linesCount; i++) {
        char *currentLine = assemblyLines[i];

        while (currentLine[0] == ' ') {
            currentLine++;
        }
        if (strlen(assemblyLines[i]) == 0) {
            continue;
        }

        if (isLabel(currentLine)) {
            if (stringBeginsWithDot(currentLine)) {
                continue;
            } else {
                addErrorMessage(errorsArray, i + 1, currentLine,
                                "label must begin with '.'");
                foundSyntaxError = true;
            }
        } else {
            if (!lineWasValid(currentLine, errorsArray, i + 1)) {
                foundSyntaxError = true;
            }
        }
    }

    // only checks labels if syntax ok
    if (!foundSyntaxError && checkLabels(assemblyLines, errorsArray)) {
        return true;
    }

    system("clear");

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

static bool checkLabels(char **assemblyLines, char **errorsArray) {
    LabelAddressMap **labelMap = allocateLabelMap();
    computeLabelMap(assemblyLines, labelMap);

    bool foundLabelError = false;
    for (int i = 0; assemblyLines[i] != NULL; i++) {
        while (assemblyLines[i][0] == ' ') {
            assemblyLines[i]++;
        }
        if (strlen(assemblyLines[i]) == 0) {
            continue;
        }

        char *opcode = extractOpcode(assemblyLines[i]);
        if (isBranch(opcode)) {
            printf("branch: %s\n", assemblyLines[i]);

            char *literal = extractOperands(assemblyLines[i])[0];
            if (literal[0] == '.') {
                if (!mapContainsLabel(literal, labelMap)) {
                    char message[MAX_LINE_LENGTH];
                    sprintf(message, "label '%s' not recognised", literal);
                    addErrorMessage(errorsArray, i + 1, assemblyLines[i], message);
                    foundLabelError = true;
                }
            } else {
                char *endPointer = NULL;
                strtoll(literal, &endPointer, 10);
                printf("Hello\n");
                printf("literal:     '%s'\n", literal);
                printf("end pointer: '%s'\n", endPointer);
                if (endPointer != NULL && strcmp(literal, endPointer) == 0) {
                    addErrorMessage(errorsArray, i + 1, assemblyLines[i],
            "label must begin with '.'");
                    foundLabelError = true;
                }
            }
        }
    }

    return !foundLabelError;
}

static bool stringBeginsWithDot(char *string) {
    int i = 0;
    while (isspace(string[i])) {
        i++;
    }
    return string[i] == '.';
}

static void addErrorMessage(char **errors, int line, char *lineString, char *message) {
    char *error = calloc(MAX_LINE_LENGTH, sizeof(char));
    sprintf(strlen(error) + error, "ERROR line %d:\n", line);
    sprintf(strlen(error) + error, "  code - '%s'\n", lineString);
    sprintf(strlen(error) + error, "  mesg - %s\n", message);

    errors[line - 1] = error;
}

static bool matchFails(char *lineString, char *pattern) {
    regex_t regex;
    int value = regcomp(&regex, pattern, REG_EXTENDED|REG_NOSUB);
    assert(value == 0);
    return regexec(&regex, lineString, 0, NULL, 0) != 0;
}

static bool lineWasValid(char *lineString, char **errorsArray, int line) {
    char lineCopy[MAX_LINE_LENGTH];
    sprintf(lineCopy, "%s", lineString);
    char *opcode = strtok(lineCopy, " ");

    if (!matchFails(lineString, "^( *and *x0 *, *x0 *, *x0 *)$")) {

    } else if (strcmp(opcode, "add") == 0 || strcmp(opcode, "adds") == 0
        || strcmp(opcode, "sub") == 0 || strcmp(opcode, "subs") == 0) {
        if (matchFails(lineString, getDpPattern(DP_ADD))) {
            addErrorMessage(errorsArray, line, lineString,
    "usage for add/sub(s)?: <Rd|SP>, <Rn|SP>, #<imm>{, lsl #(0|12)}\n     "
            "OR <Rd>, <Rn>, <Rm>{, <shift> #<imm>}");
            return false;
        }
    } else if (strcmp(opcode, "cmp") == 0 || strcmp(opcode, "cmn") == 0) {
        if (matchFails(lineString, getDpPattern(DP_CMP))) {
            addErrorMessage(errorsArray, line, lineString,
    "usage for cm(p/n): <Rn|SP>, #<imm>{, <lsl #(0|12)>}<Rn>, <Rm>{, <shift> #<imm>}");
            return false;
        }
    } else if (strcmp(opcode, "neg") == 0 || strcmp(opcode, "negs") == 0) {
        if (matchFails(lineString, getDpPattern(DP_NEGS))) {
            addErrorMessage(errorsArray, line, lineString,
    "usage for neg(s)?: <Rd|SP>,  #<imm>{, lsl #(0|12)}<Rd>, <Rm>{, <shift> #<imm>}");
            return false;
        }
        return getDpPattern(DP_CMP);
    } else if (strcmp(opcode, "and") == 0 || strcmp(opcode, "ands") == 0
               || strcmp(opcode, "bic") == 0 || strcmp(opcode, "bics") == 0
               || strcmp(opcode, "eor") == 0 || strcmp(opcode, "orr") == 0
               || strcmp(opcode, "eon") == 0 || strcmp(opcode, "orn") == 0) {
        if (matchFails(lineString, getDpPattern(DP_BITWISE))) {
            addErrorMessage(errorsArray, line, lineString,
    "usage for bitwise: <Rd>, <Rn>, <Rm>, <shift> #<imm>");
            return false;
        }
    } else if (strcmp(opcode, "tst") == 0) {
        if (matchFails(lineString, getDpPattern(DP_TST))) {
            addErrorMessage(errorsArray, line, lineString,
    "usage for tst: <Rn>, <Rm>{, <shift> #<imm>}");
            return false;
        }
    } else if (strcmp(opcode, "movk") == 0 || strcmp(opcode, "movn") == 0
               || strcmp(opcode, "movz") == 0) {
        if (matchFails(lineString, getDpPattern(DP_MOVX))) {
            addErrorMessage(errorsArray, line, lineString,
    "usage for tst: <Rd>, #<imm>{, lsl #<imm>}");
            return false;
        }
    } else if (strcmp(opcode, "mov") == 0) {
        if (matchFails(lineString, getDpPattern(DP_MOV))) {
            addErrorMessage(errorsArray, line, lineString,
    "usage for mov: <Rd>, <Rn>");
            return false;
        }
    } else if (strcmp(opcode, "mvn") == 0) {
        if (matchFails(lineString, getDpPattern(DP_MVN))) {
            addErrorMessage(errorsArray, line, lineString,
    "usage for mvn: <Rd>, <Rm>{, <shift> #<imm>}");
            return false;
        }
    } else if (strcmp(opcode, "madd") == 0 || strcmp(opcode, "msub") == 0) {
        if (matchFails(lineString, getDpPattern(DP_MADD))) {
            addErrorMessage(errorsArray, line, lineString,
    "usage for m(add/sub): <Rd>, <Rn>, <Rm>, <Ra>");
            return false;
        }
    } else if (strcmp(opcode, "mul") == 0 || strcmp(opcode, "mneg") == 0) {
        if (matchFails(lineString, getDpPattern(DP_MUL))) {
            addErrorMessage(errorsArray, line, lineString,
    "usage for (mul/mneg): <Rd>, <Rn>, <Rm>");
            return false;
        }
    } else if (strcmp(opcode, "ldr") == 0 || strcmp(opcode, "str") == 0) {
        if (matchFails(lineString, getDataTransferPattern(LDL))
        && matchFails(lineString, getDataTransferPattern(STR))) {
            addErrorMessage(errorsArray, line, lineString,
    "usage for ldr/str: \n        "
            "<Rt>, [<Xn|SP>], #<simm>\n        "
            "<Rt>, [<Xn|SP>, #<simm>]!\n        "
            "<Rt>, [<Xn|SP>, #<imm>]\n        "
            "<Rt>, [<Xn|SP>, <Xm>]\n     "
            "OR <Rt>, <literal>");
            return false;
        }
    } else if (strcmp(opcode, "b") == 0 || strstr(opcode, "b.")) {
        if (matchFails(lineString, getBranchPattern(B))) {
            addErrorMessage(errorsArray, line, lineString,
    "usage for b(.cond)?: <literal>");
            return false;
        }
    } else if (strcmp(opcode, "br") == 0) {
        if (matchFails(lineString, getBranchPattern(BR))) {
            addErrorMessage(errorsArray, line, lineString,
    "usage for br: <Xn>");
            return false;
        }
    } else {
        addErrorMessage(errorsArray, line, lineString, "opcode not recognised");
        return false;
    }
    return true;
}