#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void readfile(void *substring, void *str){
    char strpointer[600];
    char *filestr = (char*) str;
    char *searchstring = (char*) substring;
    size_t len = 100;
    
    if (filestr == NULL){
        for( char * inputkey; scanf("%s", inputkey);){
            printf("%s\n",inputkey);
        }
        exit(0);
    }
    char  *line = NULL;
    FILE *fp = fopen(filestr, "r");
    if (fp == NULL) {
        printf("wgrep: cannot open file\n");
        exit(1);
    }
    char * strngfile = strpointer;
    while( getline(&line, &len, fp) != -1){
        char * readline = line;
        char *found = strstr(readline,searchstring);
        if (found != NULL){
            printf("%s\n",line);
        }
    }

    fclose(fp);


}

int main(int argc, char *argv[]){

    if (argc == 1){
        printf("wgrep: searchterm [file ...] ]\n");
        exit(1);
    }
    if (argc == 2){
        size_t len = 100;
        char lines[len];
        fgets(lines, len, stdin);
        char * cmdline = lines;
        char * found = strstr(cmdline,argv[1]);
        if (found != NULL){
            printf("Line with search key is :%s\n", lines);
        }

        exit(0);
    }
    
    readfile(argv[1], argv[2]);
    return 0;


}