//
// Created by Faraz Malik on 09/06/2023.
//

#include <stdbool.h>
#include <stdint.h>

#ifndef ARMV8_27_FLAGS_H
    #define ARMV8_27_FLAGS_H

    bool overunderflow(long long val1, long long val2, long long result);
    bool carry(long long val1, long long val2, bool isPlus, bool is64Bit);
#endif //ARMV8_27_FLAGS_H
