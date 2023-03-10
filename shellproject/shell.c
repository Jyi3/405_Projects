#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
<<<<<<< Updated upstream
#include <dirent.h>
=======
#include <fcntl.h>
>>>>>>> Stashed changes

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 10

volatile sig_atomic_t interrupted = 0;
<<<<<<< Updated upstream
//volatile sig_atomic_t kill_interrupted = 0;
=======
>>>>>>> Stashed changes

void handle_interrupt(int signal) {
    interrupted = 1;
}

<<<<<<< Updated upstream
//void handle_kill(int signal) {
//    kill_interrupted = 1;
//}
=======
>>>>>>> Stashed changes

int main() {
    char command[MAX_COMMAND_LENGTH];

    // Set up the interrupt signal handler
    signal(SIGINT, handle_interrupt);

<<<<<<< Updated upstream
    // Set up the interrupt signal handler
    //signal(SIGKILL, handle_kill);

    DIR *dir;
    struct dirent *ent;

    while (1) {
        // Check if interrupt signal was received
        if (interrupted) {
            interrupted = 0;
            continue;
        }

//        if (kill_interrupted) {
//            kill_interrupted = 0;
//            printf("\n");
//            break;
//        }

=======
    while (1) {
>>>>>>> Stashed changes
        // Display the prompt and get user input
        printf("gshell> ");
        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            printf("\n");
            return 0;
<<<<<<< Updated upstream
            break;
=======
>>>>>>> Stashed changes
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
            continue;
        }else if (strcmp(args[0], "cd") == 0){
            chdir(args[1]);
            continue;
        }else if (strcmp(command, "exit") == 0) {
            // "exit" was entered
            break;
        }else if (strcmp(args[0], "cls") == 0) {
            dir = opendir("."); 
            while ((ent = readdir(dir)) != NULL){
                    printf("%s\n", ent->d_name);
            }
                closedir(dir);
                
        }else {
            int pid = fork();
            if (pid == 0){
                // Check for input redirection
                for (int j = 0; j < i; j++) {
                    if (strcmp(args[j], "<") == 0) {
                        int input_fd = open(args[j+1], O_RDONLY);
                        dup2(input_fd, STDIN_FILENO);
                        close(input_fd);
                        args[j] = NULL;
                        break;
                    }
                }

                // Check for output redirection
                for (int j = 0; j < i; j++) {
                    if (strcmp(args[j], ">") == 0) {
                        int output_fd = open(args[j+1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                        dup2(output_fd, STDOUT_FILENO);
                        close(output_fd);
                        args[j] = NULL;
                        break;
                    }
                }

                execvp(args[0], args);
                printf("Unknown command: %s\n", args[0]);
                exit(EXIT_FAILURE);
            }else if (pid > 0){
                int status;
                waitpid(pid, &status, 0);
            }
        }
    }

    return 0;
}
