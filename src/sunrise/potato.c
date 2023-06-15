//
// Created by Faraz Malik on 13/06/2023.
//

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <regex.h>

#define MAX_LINE_SIZE 256

typedef enum {
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
        case REG:
            sprintf(pattern, "(x|w)[0-9]{1,2}"); break;
        case XREG:
            sprintf(pattern, "x[0-9]{1,2}"); break;
        case SHIFT:
            sprintf(pattern, "((a|l)s(l|r))"); break;
        case IMM:
            sprintf(pattern, "(0x([0-9]|[a-z])+)|([0-9]+)"); break;
        case LITERAL:
            sprintf(pattern, "(.[a-z]+)|(%s)",
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

char *getBranchPattern(branchPatterns willy) {
    char *pattern = calloc(MAX_LINE_SIZE, sizeof(char));
    switch (willy) {
        case B:
            sprintf(pattern, " *b(.cond)? +%s *",
                    getKeyPattern(LITERAL)); break;
        case BR:
            sprintf(pattern, "^ *br +%s *$",
                    getKeyPattern(XREG)); break;
    }
    return pattern;
}

bool linePatternMatches(char *line) {
    regex_t regex;

    int value;
    char *pattern = ".\l+";
    printf("REGEX: '%s'\n", pattern);
    value = regcomp(&regex, pattern, 0);

    assert(value == 0);

    int res = regexec(&regex, line, 0, NULL, 0) == 0;

    if (res == REG_BADPAT) {
        printf("EE\n");
    }

    return res == 0;
}

void printb(bool b) {
    if (b) {
        printf("trew");
    } else {
        printf("fawse");
    }
}

int main(void) {
    printb(linePatternMatches("hello")); printf("\n");
}