#include <sys/sysinfo.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>


char* concat(const char *s1, const char *s2){
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1);
    strcpy(result,s1);
    strcat(result,s2);
    return result;
}


int counter;
void *encode(void *str){
    struct stat s;
    char *filestr =(char*) str;  
    counter += 1;
    char * encoding = "";
    char *map;
    
    int i;
    int file_read = open(filestr, O_RDONLY, 0);
    
    if (file_read== -1){
        perror("file couldnt be opened");
        exit(EXIT_FAILURE);
    }

    fstat (file_read, & s);
    int sz = getpagesize();
    map = mmap(0, sz, PROT_READ, MAP_SHARED, file_read, 0);
    if (map == MAP_FAILED){
        close(file_read);
        perror("map failed");
        exit(EXIT_FAILURE);
    }
    char * decompressed=(char*)malloc(sizeof(char) * (strlen(map) * 2 + 1));
    char *digits=(char*)malloc(sizeof(char) * (strlen(map) * 2 + 1));
    for (i = 0; map[i] != '\0'; i++)
    {   
        
        char line = map[i];
        char * characters =(char*)malloc(sizeof(char) * (strlen(map) * 2 + 1));
        //printf("%s\n", &line);
        if(isdigit(line)){
            digits = concat(digits,&line);
            //printf("%s\n", digits);
        }else{
            //printf("%s\n", &line);
            int num = 0;
            sscanf(digits, "%d", &num); 
            digits = "";
            //printf("%d\n", num);
            if (num != 0){
                for (int y= 0; y < num; ){
                    characters = concat(characters, &line);
                    y++;
                }

            }
        }
        //printf("%s\n", characters);
        decompressed = concat(decompressed,characters);
        //printf("%s\n", decompressed);
        //encoding += to_string(count) + map[i];
    }

    printf("%s\n",decompressed);
    //encoding = "";

    return NULL;  
}

int main(int argc, char *argv[]){
    for (int i=1; i<argc; i++){
        encode(argv[i]);
    }
    return 0;
}