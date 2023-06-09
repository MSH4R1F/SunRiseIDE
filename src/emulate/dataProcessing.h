//
// Created by Faraz Malik on 09/06/2023.
//

#ifndef ARMV8_27_DATAPROCESSING_H
    #define ARMV8_27_DATAPROCESSING_H

    enum DpOpcArithmetic {
        ADD,
        ADDS,
        SUB,
        SUBS
    };

    enum DpOpcWideMove {
        MOVN,
        INVALID,
        MOVZ,
        MOVK
    };
#endif //ARMV8_27_DATAPROCESSING_H
