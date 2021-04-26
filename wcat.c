#include <stdio.h>
#include <stdlib.h>

void readfile(void *str){
    char *filestr = (char*) str;
    char * strpointer = NULL;
    size_t size;
    FILE *fp = fopen(filestr, "r");
    if (fp == NULL) {
        printf("Could not read file\n");
        exit(1);
    }
    while(getline(&strpointer,&size, fp) != -1){
        printf("%s", strpointer);
    }
    printf("\n");

    fclose(fp);


}


int main(int argc, char* argv[]){
    if (argc < 2) {
        exit(1);
    }else{
        for (int i = 0; i < argc; i++){
            readfile(argv[i]);
        }
    }

    return 0;
}