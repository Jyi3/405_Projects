#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
int main () {

    DIR *dir;
    struct dirent *ent;
    dir = opendir("."); 
            while ((ent = readdir(dir)) != NULL){
                printf("%s\n", ent->d_name);
            }
                closedir(dir);







}
