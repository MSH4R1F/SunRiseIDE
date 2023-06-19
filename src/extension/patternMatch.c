//
// Created by Faraz Malik on 18/06/2023.
//

#include "patternMatch.h"

#include "../general/constants.h"

char *getKeyPattern(keyPattern key) {
    char *pattern = calloc(MAX_LINE_LENGTH, sizeof(char));
    switch (key) {
        case REG:
            sprintf(pattern, "(x|w)[0-9]{1,2}"); break;
        case XREG:
            sprintf(pattern, "x[0-9]{1,2}"); break;
        case SHIFT:
            sprintf(pattern, "((a|l)s(l|r))"); break;
        case IMM:
            sprintf(pattern, "((0x([0-9]|[a-z])+)|(([0-9])+))"); break;
        case LITERAL:
            sprintf(pattern, "((.[a-z]+)|%s)",
                    getKeyPattern(IMM)); break;
        case SIMM:
            sprintf(pattern, "-?%s",
                    getKeyPattern(IMM)); break;
    }
    return pattern;
}

char *getDpPattern(dpPattern key) {
    char *pattern = calloc(MAX_LINE_LENGTH, sizeof(char));
    switch (key) {
        case LS_12:
            sprintf(pattern, " *, *lsl +#(0)|(12)"); break;
        case SHIFT_IMM:
            sprintf(pattern, " *, *%s +#%s",
                    getKeyPattern(SHIFT), getKeyPattern(IMM)); break;
        case DP_ADD:
            sprintf(pattern, "^ *((add)|(sub))s? +%s *, *%s *, *((#%s(%s)?)|(%s(%s)?)) *$",
                    getKeyPattern(REG), getKeyPattern(REG),
                    getKeyPattern(IMM), getDpPattern(LS_12),
                    getKeyPattern(REG), getDpPattern(SHIFT_IMM)); break;
        case DP_CMP:
            sprintf(pattern, "^ *(cmp|n) +%s *, *((#%s(%s)?)|(%s(%s)?)) *$",
                    getKeyPattern(REG), getKeyPattern(IMM),
                    getDpPattern(LS_12), getKeyPattern(REG),
                    getDpPattern(SHIFT_IMM)); break;
        case DP_NEGS:
            sprintf(pattern, "^ *negs? +%s *, *((#%s(%s)?)|(%s(%s)?)) *$",
                    getKeyPattern(REG), getKeyPattern(IMM),
                    getDpPattern(LS_12), getKeyPattern(REG),
                    getDpPattern(SHIFT_IMM)); break;
        case DP_BITWISE:
            sprintf(pattern, "^( *((ands?)|(bics?)|(eor)|(orr)|(eon)|(orn)) +%s *, *%s *, *%s%s *)$",
                    getKeyPattern(REG), getKeyPattern(REG),
                    getKeyPattern(REG), getDpPattern(SHIFT_IMM)); break;
        case DP_TST:
            sprintf(pattern, "^( *tst +%s *, *%s(%s)? *)$",
                    getKeyPattern(REG), getKeyPattern(REG),
                    getDpPattern(SHIFT_IMM)); break;
        case DP_MOVX:
            sprintf(pattern, "^( *mov(k|n|z) +%s *, *#%s( *, *lsl +#%s)? *)$",
                    getKeyPattern(REG), getKeyPattern(IMM),
                    getKeyPattern(IMM)); break;
        case DP_MOV:
            sprintf(pattern, "^( *mov +%s *, *%s *)$",
                    getKeyPattern(REG), getKeyPattern(REG)); break;
        case DP_MVN:
            sprintf(pattern, "^( *mvn +%s *, *%s(%s)? *)$",
                    getKeyPattern(REG), getKeyPattern(REG),
                    getDpPattern(SHIFT_IMM)); break;
        case DP_MADD:
            sprintf(pattern, "^( *m((add)|(sub)) +%s *, *%s *, *%s *, *%s *)$",
                    getKeyPattern(REG), getKeyPattern(REG),
                    getKeyPattern(REG), getKeyPattern(REG)); break;
        case DP_MUL:
            sprintf(pattern, "^( *m((ul)|(neg)) +%s *, *%s *, *%s *)$",
                    getKeyPattern(REG), getKeyPattern(REG),
                    getKeyPattern(REG)); break;
    }
    return pattern;
}

char *getDataTransferPattern(dataTransferPattern key) {
    char *pattern = calloc(MAX_LINE_LENGTH, sizeof(char));
    switch (key) {
        case STR:
            sprintf(pattern, "^( *((str)|(ldr)) +%s *, *\\[ *%s(( *\\]( *, *#%s)?)|( *, *((#%s *\\]!)|(#%s *\\])|(%s *\\])))) *)$",
                    getKeyPattern(REG), getKeyPattern(REG),
                    getKeyPattern(SIMM), getKeyPattern(SIMM),
                    getKeyPattern(IMM), getKeyPattern(XREG)); break;
        case LDL:
            sprintf(pattern, "^( *ldr +%s *, *%s *)$",
                    getKeyPattern(REG), getKeyPattern(LITERAL)); break;
    }
    return pattern;
}

char *getBranchPattern(branchPattern key) {
    char *pattern = calloc(MAX_LINE_LENGTH, sizeof(char));
    switch (key) {
        case COND:
            sprintf(pattern, "((eq)|(ne)|(ge)|(lt)|(gt)|(le)|(al))"); break;
        case B:
            sprintf(pattern, "^( *b(.%s)? +%s *)$",
                    getBranchPattern(COND), getKeyPattern(LITERAL)); break;
        case BR:
            sprintf(pattern, "^( *br +%s *)$",
                    getKeyPattern(XREG)); break;
    }
    return pattern;
}