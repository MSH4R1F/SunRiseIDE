#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char **argv) {
    long long word = 0xFFFFFFFF;
    if (word & 0x80000000) {
        word = word | (0xFFFFFFFF << 32);
    }
    printf("%lld\n", word);
    return EXIT_SUCCESS;
}
