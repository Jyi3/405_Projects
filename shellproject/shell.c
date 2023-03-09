#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 100

int main() {
    char command[MAX_COMMAND_LENGTH];

    while (1) {
        // Display the prompt and get user input
        printf("gshell> ");
        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            // End of file (Ctrl-D) was entered
            break;
        }

        // Remove trailing newline
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0) {
            // "exit" was entered
            break;
        }

        // Execute the command
        system(command);
    }

    return 0;
}

