//
// Created by Faraz Malik on 18/06/2023.
//

#include "assembleFileUtils.h"

// private functions declaration
static char **loadAssemblyFromLines(char **data, int lines);

char **loadAssemblyLinesFromFile(char *filename) {
    FILE *fp = fopen(filename, "r");

    if( !fp ) {
        fprintf(stderr, "Opened file: %s\n", filename);
    }

    // Count Lines
    char cr = '\0';
    size_t lines = 0;

    while( cr != EOF ) {
        if ( cr == '\n' ) {
            lines++;
        }
        cr = getc(fp);
    }
    rewind(fp);

    char **data = calloc(lines + 1, sizeof(char *));
    size_t n;

    for (size_t i = 0; i < lines; i++) {
        data[i] = NULL;
        n = 0;

        getline(&data[i], &n, fp);

        if (ferror( fp )) {
            fprintf(stderr, "Error reading from file\n");
        }
        data[i][strlen(data[i]) - 1] = '\0';

        for (int j = 0; data[i][j] != '\0'; j++) {
            data[i][j] = tolower(data[i][j]);
        }
    }
    fclose(fp);

    return data;
}

char **loadAssemblyFromFile(char *filename) {
    char **data = loadAssemblyLinesFromFile(filename);
    int lines = 0;

    while (data[lines] != NULL) {
        lines++;
    }

    return loadAssemblyFromLines(data, lines);;
}

void writeMachineToFile(uint8_t *memPointer, char *filename, int numInstructions) {
    FILE *fp;
    fp = fopen(filename, "wb");
    fwrite(memPointer, sizeof(uint32_t), numInstructions, fp); //count
    fclose(fp); //Don't forget to close the file when finished
}

static char **loadAssemblyFromLines(char **data, int lines) {
    char **newArray = calloc(lines + 1, sizeof(char *));
    int next = 0;

    for (int i = 0; i < lines; i++) {
        while (data[i][0] == ' ') {
            data[i]++;
        }

        if (strlen(data[i]) != 0) {
            newArray[next] = data[i];

            int endIndex = strlen(newArray[next]) - 1;
            while (newArray[next][endIndex] == ' ') {
                newArray[next][endIndex] = '\0';
                endIndex--;
            }
            next++;
        }

    }
    return newArray;
}