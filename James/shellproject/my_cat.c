#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *fp;
    char ch;

    if(argc != 2) {
        printf("Usage: custom_cat <filename>\n");
        exit(1);
    }

    fp = fopen(argv[1], "r");
    if(fp == NULL) {
        printf("Unable to open file '%s'\n", argv[1]);
        exit(1);
    }

    while((ch = fgetc(fp)) != EOF) {
        printf("%c", ch);
    }

    printf(" TEST ");

    fclose(fp);
    return 0;
}

