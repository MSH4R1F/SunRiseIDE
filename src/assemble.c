#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char **argv) {
    bool hello = 0x3 & 0x4;
    if (hello) {
        printf("hello");
    }
    return EXIT_SUCCESS;
}
