#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include<stdbool.h>  
#include <sys/stat.h>
#include <fcntl.h>
#define ARGC 2
#define ARRLEN (30)



/**
 * Hey June the error is a segmentation Fault(core dumped)
 * the error is caused by the if statement code on line 202, getcommands function
 * 
 * 
 * To run the application you need to set the path as follows "path /bin" or "path /usr/bin" or both "path /bin /usr/bin"
 *
 *  **/


char* concat(const char *s1, const char *s2){
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = (char*)malloc(len1 + len2 + 1);
    strcpy(result,s1);
    strcat(result,s2);
    return result;
}
char * file;
char * externalcoms = (char*)'\0';
char Path_list[4][10];
int initz(char path[4][10]){
    for(int i=0;i<4;i++){
        for(int j=0;j<10;j++){
            Path_list[i][j] = '\0';
        }
    } 
    return 0;
}


int checkArray(char path[4][10]){
   int i,j,count;
   j=0;
   count=0;
   for(i=0; i<4; i++) {
    if (path[i][10] == '\0'){
            count = count + 1;
    }else{
        j=j+1;
    }
   }
   if(count > 4){
       return -1;
   }
    
   return j;    
}

struct arguements{
    char * command;
    char argv[1][10];
};

typedef struct arguements Struct;
extern int n;
Struct getparameters(char * command){
    Struct s;
    //s.argv = malloc(s_size+(sizeof(char)*n));
    char *checkcommand,*found;
    const int n = strlen(command);
    char comand_array[n]; 
    strncpy(comand_array, command,sizeof(comand_array));
    checkcommand = strdup(comand_array);
    // add values(inputs for user) to array
    int count = 0;
    
    if(strchr(command, ' ') != NULL){
        while((found = strsep(&checkcommand," ")) != NULL){
            if (count == 0){
                s.command = found;
            }
            strcpy(s.argv[count],found);
            count++;
        }
    }else{
        s.command = command;
        strcpy(s.argv[count],command);
        count++;
    }
    size_t cn = count;
    memset((void*)s.argv[count], '\0', cn);
    return s;
}

void externalCommssaver(char * command){
    int n = 0;
    char *p = strtok(command, ">");
    char *arr[2];
    while(p != NULL){
        arr[n++]=p;
        p = strtok(NULL,"/");
    }
    file = arr[1];
    externalcoms = arr[0];
}
//function to excute the commands
void runComms(Struct commands){
    int arrSize =checkArray(Path_list);
    
    if (arrSize == -1){
        printf("Error: Set Path\n");
        exit(-1);
    }
    for (int u = 0; u < arrSize+1; u++){
        int post;

        char * path = concat(&Path_list[u][10], "/");
        path = concat(path, commands.command);
        int result = access(path, X_OK);
        int status;
        if(result == 0){
            pid_t pid;
            pid=fork();
            if(pid == -1){
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message)); 
            }else if (pid == 0){
                char *coms[]={commands.command};
                int count = 0;
                int n = sizeof(commands.argv) / sizeof(commands.argv[0]);
                for (int i = 0; i<n; i++){
                    strcpy(coms[count],commands.argv[i]);
                    count = count + 1;
                }
                coms[count]=NULL;
                if (*file != '\0'){
                    int fd = open(file, O_RDWR | O_CREAT);
                    dup2(fd,STDOUT_FILENO);
                    dup2(fd,STDERR_FILENO);
                    close(fd);
                    execv(path,  coms);
                    file=(char*)'\0';
                    if (post != -1){
                        break;
                    }

                }else{
                    execv(path, coms);
                    if(post != -1){
                        break;
                    }
                }
            }else{
                wait(&status);
                if(WEXITSTATUS(status) ==-1){
                    char error_message[30] = "An error has occurred\n";
                    write(STDERR_FILENO, error_message, strlen(error_message)); 
                }else{
                    break;
                }
                
            }
        }
        printf("\n");
    }
}
void getcommands(char * command){
    command[strcspn(command,"\n")]=0;
    char *checkcommand,*found;
    int n = strlen(command);
    char comand_array[n+1];
    strcpy(comand_array, command);
    checkcommand = strdup(comand_array);
    const char *coms_list[n];
    // add values(inputs for user) to array
    int count = 0;
    
    while((found = strsep(&checkcommand," ")) != NULL){
        coms_list[count] = found;
        count++;
    }
    // this if statement is for CD command
    if (strcmp(coms_list[0],"cd") == 0){
        if (count > 2 || count == 0){
            printf("Error too many arguements for a cd operation\n");
            exit(1);
        }else{
            int moved = chdir(coms_list[1]);
            
  
            if (moved != 0){
                printf("failed to change directories\n");
                exit(1);
            }
        }
    }else if (strcmp(coms_list[0],"path") == 0){
        //this code sets the paths 
        printf("found\n");
        initz(Path_list);
        if (count > 1){
            int i;
            for(i=1; i<count; i++) {
                strcpy(&Path_list[i][10], coms_list[i]);
            }
        }else if(count == 1){
            strcpy(&Path_list[count][10], coms_list[count]);
            
        }
    }else{
        //the error is found here, when you try to run a single command like "pwd"
        Struct results = getparameters(command);
        runComms(results);
    }
}

void assessCommands(char * command){
    char *checkcommand,*found;
    int n = strlen(command);
    char comand_array[n+1];
    strcpy(comand_array, command);
    checkcommand = strdup(comand_array);
    // add values(inputs for user) to array
    found = strstr(command,"&");
    if(found != NULL){
        while((found = strsep(&checkcommand,"&")) != NULL){
            getcommands(found);
        }
    }else{
        getcommands(command);
    }
}

void readfile(char * str){
    int n = strlen(str);
    char filestr[n+1];
    strcpy(filestr, str);
    size_t len = 30;
    char  *line = NULL;
    FILE *fp = fopen(filestr, "r");
    
    if (fp == NULL) {
        printf("wish: cannot open file\n");
        exit(1);
    }
    while( getline(&line, &len, fp) != -1){
        assessCommands(line);
        printf("\n");
    }
    printf("\n");

    fclose(fp);


}


int main(int argc, char *argv[]) {
        bool prompt = true;
        while (prompt == true) {
            char * command;
            size_t bufsize = 10;
            printf("\n%s","wish>");
            //get user input
            int gotten =getline(&command,&bufsize,stdin);
            
            if (gotten < 0){
                printf("No input received\n");
            }
            if (strcmp(command,"exit()\n") == 0 || strcmp(command,"exit\n") == 0){
                prompt = false;
                exit(0);
                break;
            }
            char * found = strstr(command,".");
            if(strchr(command, '>') != NULL){
                externalCommssaver(command);
                // file = outarr[1];
                assessCommands(externalcoms);
            }
            else if(found != NULL && strchr(command,'>') == NULL){
                readfile(command);
            }
            else{
                assessCommands(command);
            }

            printf("\n");
        }
    



    return 0;
}