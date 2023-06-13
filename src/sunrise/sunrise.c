//
// Created by Faraz Malik on 13/06/2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "sunrise.h"

bool getEditorExists(void) {
    FILE *file = fopen("editor.s", "r");
    if (file == NULL) {
        return false;
    } else {
        fclose(file);
        return true;
    }
}

void openAssemblyEditor(void) {
    if (getEditorExists()) {
        printf("'editor.s' found!\n");
    } else {
        printf("Creating file 'editor.s'...\n");
        system("touch editor.s");
    }

    printf("Opening file...\n");
    system("open editor.s");
}

bool assemble(void) {
    if (!getEditorExists()) {
        printf("'editor.s' not found.\n");
        return false;
    }

    system("./assemble editor.s output.bin");
    return true;
}

void emulate(void) {
    printf("----RESULT----\n\n");
    system("./emulate output.bin");
}

int main(void) {
    system("clear");
    printf("Welcome to sunriseIDE\n");
    while (true) {
        char *target = malloc(16 * sizeof(char));
        printf("\n----MENU----\n\n");
        printf("  1. Open editor\n");
        printf("  2. Build \u2692 \n");
        printf("  3. Run \u25b6 \n");
        printf("  4. Debug \n");
        printf("  5. Exit\n");
        printf("\nChoice: ");
        char *option = fgets(target, 16, stdin);
        printf("\n");

        switch (option[0]) {
            case '1':
                openAssemblyEditor();
                break;
            case '2':
                if (!assemble()) {
                    continue;
                }
                break;
            case '3':
                if (!assemble()) {
                    continue;
                }
                emulate();
                break;
            case '5':
                system("clear");
                return EXIT_SUCCESS;
            default:
                printf("%c not recognised\n", option[0]);
        }
    }
}