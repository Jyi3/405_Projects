#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/wait.h> // Add this header

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 10

pid_t child_pid = -1;
volatile sig_atomic_t interrupted = 0;

void handle_interrupt(int signal) {
    if (child_pid > 0) {
        kill(child_pid, SIGINT);
    } else {
        printf("\n");
        exit(0);
    }
    interrupted = 1;
}

void cat_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    int c;
    while ((c = fgetc(file)) != EOF) {
        putchar(c);
    }

    fclose(file);
}

int main() {
    char command[MAX_COMMAND_LENGTH];

    // Set up the interrupt signal handler
    signal(SIGINT, handle_interrupt);

    DIR *dir;
    struct dirent *ent;

    while (1) {
        // Check if interrupt signal was received
        if (interrupted) {
            interrupted = 0;
            continue;
        }

        // Display the prompt and get user input
        printf("gshell> ");
        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            // End of input stream (Ctrl-D)
            printf("\n");
            return 0;
        }
        command[strcspn(command, "\n")] = '\0';

        char *token;
        char *args[MAX_ARGS];
        int i = 0;
        int pipe_position = -1;
        token = strtok(command, " ");

        while (token != NULL && i < MAX_ARGS) {
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
        }

        if (i == 0 || args[0] == NULL) {
            continue;
        } else if (strcmp(args[0], "cd") == 0) {
            chdir(args[1]);
            continue;
        } else if (strcmp(args[0], "pwd") == 0) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            } else {
                perror("getcwd() error");
            }
            continue;
        } else if (strcmp(command, "exit") == 0) {
            // "exit" was entered
            break;
        } else if (strcmp(args[0], "ls") == 0) {
            dir = opendir(".");
            while ((ent = readdir(dir)) != NULL) {
                printf("%s\n", ent->d_name);
            }
            closedir(dir);
        } else if (strcmp(args[0], "cat") == 0) {
            if (args[1] != NULL) {
                cat_file(args[1]);
            } else {
                printf("Usage: cat <filename>\n");
            }
            continue;
        } else {
            int background = 0;
            if (i > 0 && strcmp(args[i - 1], "&") == 0) {
                // Program should be run in the background
                background = 1;
                args[i - 1] = NULL;
                i--;
            }

            int pid = fork();
            child_pid = pid;
            if (pid == 0) {
                // TODO: Implement piping here
                // Check for pipes in
                // Check for pipes in the command and split the command into two parts
                // Then, create another process and set up the pipe between the two processes

                // Check for input redirection
                for (int j = 0; j < i; j++) {
                    if (strcmp(args[j], "<") == 0) {
                        int input_fd = open(args[j + 1], O_RDONLY);
                        dup2(input_fd, STDIN_FILENO);
                        close(input_fd);
                        args[j] = NULL;
                        i -= 2; // Update the number of arguments
                    }
                }

                // Check for output redirection
                for (int j = 0; j < i; j++) {
                    if (strcmp(args[j], ">") == 0) {
                        int output_fd = open(args[j + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                        dup2(output_fd, STDOUT_FILENO);
                        close(output_fd);
                        args[j] = NULL;
                        i -= 2; // Update the number of arguments
                    }
                }

                if (pipe_position != -1) {
                    int pipe_fd[2];
                    if (pipe(pipe_fd) == -1) {
                        perror("pipe");
                        exit(1);
                    }

                    int pipe_pid = fork();
                    if (pipe_pid == -1) {
                        perror("fork");
                        exit(1);
                    }

                    if (pipe_pid == 0) {
                        close(pipe_fd[0]); // Close unused read end
                        dup2(pipe_fd[1], STDOUT_FILENO);
                        close(pipe_fd[1]);

                        args[pipe_position] = NULL;
                        execvp(args[0], args);

                        perror("execvp");
                        exit(1);
                    } else {
                        close(pipe_fd[1]); // Close unused write end
                        dup2(pipe_fd[0], STDIN_FILENO);
                        close(pipe_fd[0]);

                        char **new_args = args + pipe_position + 1;
                        i -= (pipe_position + 1);
                    }
                }

                execvp(args[0], args);
                perror("execvp");
                exit(1);
            } else {
                if (!background) {
                    waitpid(pid, NULL, 0);
                }
                child_pid = -1;
            }
        }
    }
}
