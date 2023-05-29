//
// Created by Faraz Malik on 29/05/2023.
//

#include "utils.h"
#include <stdio.h>

u_int32_t fetchInstruction(long long programCounter, char *filename) {
    FILE *file = fopen(filename, "rb");  // Open the binary file in read mode

    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    uint32_t value;  // Variable to store the read integer

    if (fseek(file, programCounter, SEEK_SET) != 0) {
        printf("Failed to seek to the desired position.\n");
        fclose(file);
        return 1;
    }

    // Read 4 bytes from the file into the variable 'value'
    if (fread(&value, sizeof(value), 1, file) != 1) {
        printf("Failed to read from the file.\n");
        fclose(file);
        return 1;
    }

    // Convert the value from little-endian to the system's endianness if necessary
    value = ntohl(value);  // Use ntohl() for network-to-host conversion

    printf("Read value: %d\n", value);

    fclose(file);  // Close the file

    return value;
}