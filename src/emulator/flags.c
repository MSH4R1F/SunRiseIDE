#include "flags.h"

/// Checks for an overflow or an underflow
bool overunderflow(long long val1, long long val2, long long result) {
    bool sign1 = val1 < 0;
    bool sign2 = val2 < 0;
    bool signResult = result < 0;
    return sign1 == sign2 && sign1 != signResult;
}

/// Checks if the unsigned value's result is outside the range of UINT**_MAX, indicating a carry
bool carry(long long val1, long long val2, bool isPlus, bool is64Bit) {
    uint64_t max = is64Bit ? UINT64_MAX : UINT32_MAX;
    if (isPlus) {
        return max - (uint64_t) val1 < (uint64_t) val2;
    } else {
        return (uint64_t) val2 <= (uint64_t) val1;
    }
}