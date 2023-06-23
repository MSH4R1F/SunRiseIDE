//
// Created by Faraz Malik on 13/06/2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <regex.h>

#include "../emulate/emulate.h"
#include "../assemble/assemble.h"
#include "../general/labelMap.h"
#include "../general/assembleFileUtils.h"
#include "../general/terminalColor.h"

#include "patternMatch.h"
#include "checkSyntax.h"

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

bool compile(void) {
    if (!getEditorExists()) {
        printf("'editor.s' not found.\n");
        return false;
    }

    if (!checkSyntax("editor.s")) {
        return false;
    }

    int argc = 3;
    char **argv = calloc(argc, sizeof(char *));
    argv[0] = "./assemble";
    argv[1] = "editor.s";
    argv[2] = "output.bin";

    assemble(argc, argv);
    free(argv);

    return true;
}

void run(void) {
    int argc = 2;
    char **argv = calloc(argc, sizeof(char *));
    argv[0] = "./emulate";
    argv[1] = "output.bin";

    printf("----RESULT----\n\n");
    emulate(argc, argv, false, NULL);
    free(argv);
}

void debug(void) {
    int argc = 2;
    char **argv = calloc(argc, sizeof(char *));
    argv[0] = "./emulate";
    argv[1] = "output.bin";

    char **assemblyArray = loadAssemblyFromFile("editor.s");

    emulate(argc, argv, true, assemblyArray);
    free(argv);
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
                if (!compile()) {
                    continue;
                } else {
                    setTerminalColour(GREEN);
                    printf("\nCOMPILE SUCCEEDED\n\n");
                    resetTerminalColour();
                }
                break;
            case '3':
                if (!compile()) {
                    continue;
                } else {
                    setTerminalColour(GREEN);
                    printf("\nCOMPILE SUCCEEDED\n\n");
                    resetTerminalColour();
                }
                run();
                break;
            case '4':
                if (!compile()) {
                    continue;
                }
                debug();
                break;
            case '5':
                system("clear");
                return EXIT_SUCCESS;
            default:
                printf("%c not recognised\n", option[0]);
        }
    }
}