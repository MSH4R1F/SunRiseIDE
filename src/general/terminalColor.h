//
// Created by Faraz Malik on 18/06/2023.
//

#include <stdio.h>

#ifndef ARMV8_27_TERMINALCOLOR_H
    #define ARMV8_27_TERMINALCOLOR_H

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

    void setTerminalColour(TerminalColour colour);
    void resetTerminalColour(void);
#endif //ARMV8_27_TERMINALCOLOR_H
