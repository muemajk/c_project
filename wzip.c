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
    int len = strlen(filestr);
    char *encoding = (char*)malloc(sizeof(char) * (len * 2 + 1));
    char *map;

    size_t i,k;
    int size;
    int file_read = open(filestr, O_RDONLY, 0);

    if (file_read== -1){
        perror("file couldnt be opened");
        exit(EXIT_FAILURE);
    }

    fstat (file_read, & s);
    size = s.st_size;
    int sz = getpagesize();
    map = mmap(0, sz, PROT_READ, MAP_SHARED, file_read, 0);
    if (map == MAP_FAILED){
        close(file_read);
        perror("map failed");
        exit(EXIT_FAILURE);
    }

    for (i = 0; map[i] != '\0'; i++)
    {   
        
        int count=1;
        char line = map[i];
        char *newval;
        while (line == map[i+1]){
            count++, i++;
        }
        char countnum[3];
        sprintf(countnum,"%d", count);
        if (&line != NULL || &line != ""){
            newval = concat(countnum,&line);
        }
        
        encoding = strcat(encoding,newval);

    }
    printf("%s\n", encoding);
    if (munmap(map, size) == -1) {
            close(file_read);
            perror("Error un-mmapping the file");
            exit(1);
        }
    char *fileloaded = filestr;
    char *delimeter = ".";
    char *token = strtok(fileloaded, delimeter);
    char* newtoken = concat(token,".z");  // fileloaded.substr(0, fileloaded.find(delimeter));
    const char *zipfile =  newtoken;
    int file_write = open(zipfile, O_CREAT | O_RDWR, S_IRWXU); 

    if (file_write == -1){
        perror(" file couldnt be opened");
        exit(EXIT_FAILURE);
    } 
    size_t textsize = strlen(encoding)+1;
    if (lseek(file_write, textsize+1, SEEK_SET) == -1)
    {
        close(file_write);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    } 
    write(file_write, "", 1);
    char* writemap = mmap(0, textsize, PROT_WRITE, MAP_SHARED, file_write, 0);
    // for (size_t i=0; i < textsize; i++){
    //     writemap[i] = encoding[i];
    // }
    memcpy(writemap,encoding, textsize+1);
    msync(writemap, textsize+1, MS_SYNC);
    if (munmap(writemap, textsize+1) == -1) {
        close(file_write);
        perror("Error un-mmapping the file");
        exit(1);
    }
    encoding = "";
    close(file_write);

    return NULL;  
}

int main(int argc, char *argv[]){
    for (int i=1; i<argc; i++){
        encode(argv[i]);
    }
    return 0;
}