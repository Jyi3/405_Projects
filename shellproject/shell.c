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

void handle_interrupt(int signal) {
    interrupted = 1;
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
<<<<<<< HEAD
        int n = sizeof(args);
        //for (int j = 0; j < i; j++){
          // printf("%s\n", args[j]);}
=======

>>>>>>> bfb8fc9ab79b7bcc38d0da539b1cfb72cc51a6ad
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
<<<<<<< HEAD
                closedir(dir);
                
        }else if (strcmp(args[n-1], "&") == 0){
                int pid = fork();
                if (pid == 0){
                   execvp(args[0], args);
                }

                }else {

=======
            closedir(dir);
        }else {
>>>>>>> bfb8fc9ab79b7bcc38d0da539b1cfb72cc51a6ad
            int pid = fork();
            if (pid == 0){
                
                if (command != NULL && args[0] != NULL)
                {
                    execvp(args[0], args);
                    // Execute the command
                    system(command);
                        
                    int p = 0;
                    while (args[p] != NULL)
                    {
                        printf("%s is in args pos: %d \n",args[p], p);
                        args[p] = NULL;
                        p++;
                    }

                }
                else if(command != NULL && args[0] != NULL)
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
                            printf("%s is in args pos: %d \n",args[j], j);
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
                            printf("%s is in args pos: %d \n",args[j], j);
                            args[j] = NULL;
                        }
                    }

                    
                    int p = 0;
                    while (args[p] != NULL)
                    {
                        printf("%s is in args pos: %d \n",args[p], p);
                        args[p] = NULL;
                        p++;
                    }

                    
                }
                else
                {
                    int p = 0;
                    while (args[p] != NULL)
                    {
                        printf("%s is in args pos: %d \n",args[p], p);
                        args[p] = NULL;
                        p++;
                    }
                    printf("Unknown command\n");
                }
                exit(0);
            }else if (pid > 0){
                int status;
                waitpid(pid, &status, 0);
            }
        }
<<<<<<< HEAD
        // Execute the command
        system(command);
=======
        

>>>>>>> bfb8fc9ab79b7bcc38d0da539b1cfb72cc51a6ad
    }

    return 0;
}
