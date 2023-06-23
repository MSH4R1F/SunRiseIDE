//
// Created by Faraz Malik on 18/06/2023.
//

#include "terminalColor.h"

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