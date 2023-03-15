#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 10

volatile sig_atomic_t interrupted = 0;

void handle_interrupt(int signal) 
{
    interrupted = 1;
}

int main() 
{
    char command[MAX_COMMAND_LENGTH];

    // Set up the interrupt signal handler
    signal(SIGINT, handle_interrupt);

    DIR *dir;
    struct dirent *ent;

    while (1) {
        // Check if interrupt signal was received
        if (interrupted)
        {
            interrupted = 0;
            continue;
        }

        // Display the prompt and get user input
        printf("gshell> ");
        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            // End of input stream (Ctrl+D)
            printf("\n");
            return 0;
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

            int background = 0;
            if (i > 0 && strcmp(args[i-1], "&") == 0) {
                // Program should be run in the background
                background = 1;
                args[i-1] = NULL;
                i--;
            }


            int pid = fork();
            if (pid == 0){

                // int cArgs = 0;
                // while (args[cArgs] != NULL)
                // {
                //     printf("%s is at location: %d\n",args[cArgs],cArgs);
                //     cArgs++;
                // }

                if (command != NULL && i == 1)
                {
                    //printf("TEST COKMPLETE\n");

                    // Execute the command
                    execvp(args[0], args);
                    
                    system(command);
                    // If execvp returns, there was an error
                    //printf("Unknown command args 1 == null\n");
                    break;

                }
                else if (command != NULL && args[1] != NULL )
                {
                    // Check for input redirection
                    for (int j = 0; j < i; j++) 
                    {
                        if (strcmp(args[j], "<") == 0) 
                        {
                            int input_fd = open(args[j+1], O_RDONLY);
                            dup2(input_fd, STDIN_FILENO);
                            close(input_fd);
                            args[j] = NULL;
                            i -= 2; // Update the number of arguments
                        }
                    }

                    // Check for output redirection
                    for (int j = 0; j < i; j++) 
                    {
                        if (strcmp(args[j], ">") == 0) 
                        {
                            int output_fd = open(args[j+1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                            dup2(output_fd, STDOUT_FILENO);
                            close(output_fd);
                            args[j] = NULL;
                            i -= 2; // Update the number of arguments
                        }
                        
                    }

                    int cArgs = 0;
                    while (args[cArgs] != NULL)
                    {
                        printf("%s is at location: %d\n",args[cArgs],cArgs);
                        cArgs++;
                    }
                    
                    //Execute the command
                    execvp(args[0], args);


                        
                    // If execvp returns, there was an error
                    printf("Unknown command args1 != null\n");
                    break;

                }
                else
                {
                    printf("Unknown command args else\n");
                }
                exit(0);
            }else if (pid > 0){
                int status;
                waitpid(pid, &status, 0);
            }
        }
        

    }

    return 0;
}
