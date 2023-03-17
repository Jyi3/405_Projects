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
#define MIN_ARGS 3

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
        char **sub_args1;
        char **sub_args2;
        int i = 0;
        token = strtok(command, " ");
        
        int resetter = 0;
        while (resetter < MAX_ARGS )
        {
            args[resetter] = NULL;
            resetter++;
        }

        while (token != NULL && i < MAX_ARGS){
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
       
        }
        //printf("%d\n", i);
        //for (int j = 0; j < i; j++){
          // printf("%s\n", args[j]);}
        int pipe_used = 0; //used to check if pipe was used
        int pipe_dex; //saves the index of the pipe symbol to split args into 2 arrays
        for (int j = 0; j < i; j++){
            //printf("james: %p\n", args[j]);
            if (strcmp(args[j], "|") == 0)
            {
                pipe_used = 1;
                pipe_dex = j;
            }
            // printf("james\n");
        }

            // pipe dex = 1

            int subcount1 = 0;
            int subcount2 = 0;
            if (pipe_used == 1){
            sub_args1 = malloc(sizeof(char *) *pipe_dex);
            sub_args2 = malloc(sizeof(char *) *(i-pipe_dex));
            for (int k = 0; k < pipe_dex; k++)
            {
                // printf("k1 counter: %d\n",k);
                // printf("%s\n", args[k]);
                sub_args1[k] = args[k];
                // printf("%s\n", sub_args1[k]);
                // printf("size of sub1: %d\n",subcount1);
            }

            // printf("split the lines here\n");

            int pipeCounterJ = 0;
            for (int k = pipe_dex+1; k < i; k++)
            {
                // printf("k1 counter: %d\n",k);
                // printf("%s\n", args[k]);
                sub_args2[pipeCounterJ] = args[k];
                // printf("%s\n", sub_args2[pipeCounterJ]);
                // printf("size of sub1: %d\n",subcount2);
                pipeCounterJ++;
            } 

            int p[2];
            pipe(p);

            // this is in parent

            //left child
            if (fork() == 0) 
            { 
                dup2(p[1],1); //dupe pipe write end on top of stdout
                close(p[0]); //close pipe fd's
                close(p[1]);
                execvp(sub_args1[0], sub_args1);
                system(command);
            }
            else
            {
                if (fork() == 0) 
                {
                    // right child
                    dup2(p[0], 0);// dup pipe read end on top of stdin
                    close(p[0]);//close pipe fd's
                    close(p[1]);
                    execvp(sub_args2[0], sub_args2);
                    system(command);
                }
                else 
                {
                    //parent
                    close(p[0]);
                    close(p[1]);
                    wait(NULL);
                    wait(NULL);
                }
            }

        }else if (i == 0 || args[0] == NULL) {
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
                
        }else if (strcmp(args[i-1], "&") == 0){
            args[i-1] = '\0';
            int pid = fork();
            if (pid == 0)
            {
                execvp(args[0], args);
            }


        }else {

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

                    // int cArgs = 0;
                    // while (args[cArgs] != NULL)
                    // {
                    //     printf("%s is at location: %d\n",args[cArgs],cArgs);
                    //     cArgs++;
                    // }
                    
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
        // Execute the command
        //system(command);
        

    }

    return 0;
}