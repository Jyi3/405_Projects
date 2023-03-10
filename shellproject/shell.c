#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 10

volatile sig_atomic_t interrupted = 0;

void handle_interrupt(int signal) {
    interrupted = 1;
}

int main() {
    char command[MAX_COMMAND_LENGTH];

    // Set up the interrupt signal handler
    signal(SIGINT, handle_interrupt);

    while (1) {
        // Check if interrupt signal was received
        if (interrupted) {
            interrupted = 0;
            continue;
        }

        // Display the prompt and get user input
        printf("gshell> ");
        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            // End of input stream (Ctrl+D)
            printf("\n");
            break;
        }
        //remove trailing new line
        command[strcspn(command, "\n")] = '\0'; 
        char *token;
        char *args[MAX_ARGS];
        int i = 0;
        token = strtok(command, " ");

        while (token != NULL && i < MAX_ARGS){
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        //for (int j = 0; j < i; j++){
          // printf("%s\n", args[j]);}
        if (i == 0 || args[0] == NULL) {
            // Empty command, or no arguments given
            continue;
        }else if (strcmp(args[0], "cd") == 0){
            chdir(args[1]);
            continue;
        }else if (strcmp(command, "exit") == 0) {
            // "exit" was entered
            break;
        }else {
            int pid = fork();
            if (pid == 0){
                execvp(args[0], args);
            }else if (pid > 0){
                int status;
                waitpid(pid, &status, 0);
            }
            
        }

        // Execute the command
        system(command);
    }

    return 0;
}
