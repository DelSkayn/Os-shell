#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "shell.h"

const size_t SIZE_INCREASE = 40;
const size_t PATH_BUFFER_SIZE = 1024;
/*
 * A struct for containing the command
 */
struct CommandBuffer{
    char * command_buffer;
    size_t size;
};

//Create an initialized command buffer
struct CommandBuffer command_buffer_factory(){
    struct CommandBuffer cb;
    cb.command_buffer = NULL;
    cb.size = 0;
    return cb;
}

//Properly delete the command buffer
void delete_command_buffer(struct CommandBuffer * cmb){
    free(cmb->command_buffer);
    cmb->command_buffer = NULL;
    cmb->size = 0;
}

//Put a command from in into the give command buffer
bool get_command(struct CommandBuffer * cb,FILE * in){
    if(cb->command_buffer){//free remains
        free(cb->command_buffer);
        cb->command_buffer = NULL;
        cb->size = 0;
    }
    size_t itt = 0;
    for(;;){
        cb->command_buffer = realloc(cb->command_buffer,cb->size+sizeof(char)*SIZE_INCREASE);
        cb->size += SIZE_INCREASE;
        //init memory
        for(size_t i = SIZE_INCREASE*itt;i < cb->size;++i){
            cb->command_buffer[i] = fgetc(in);
            if(cb->command_buffer[i] == '\n'){
                cb->command_buffer[i] = '\0';
                return true;
            }else if(cb->command_buffer[i] == EOF){
                printf("Error: Eof during before end reading\n");
                return false;
            }
        }
        ++itt;
    }
}

//Remove a character from a string
void removeChar(char *str, char garbage) {

    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

//Parse a command into different token
char ** parse_command(char * src){
    size_t size = 0;
    char ** buff = NULL;
    for(;;){
        size_t old_size = size;
        size += SIZE_INCREASE;
        buff = realloc(buff,size*sizeof(char*));

        for(size_t i = old_size;i < size;i++){
            if(i == 0){
                buff[0] = src;
            }else{
                buff[i] = strstr(buff[i-1]," ");
                //test for end of string
                if(!buff[i])
                    return buff;
                //test for escape characters 
                while(*(buff[i]-1) == '\\'){
                    buff[i] = strstr(buff[i]+1," ");
                    //test for end of string
                    if(!buff[i]){
                        removeChar(buff[i-1],'\\');
                        return buff;
                    }
                }
                //replace the space with a endstring
                *buff[i] = '\0';
                //move over the endstring
                buff[i] += 1;
                //test if string is empty
                if(strcmp(buff[i-1],"") == 0){
                    buff[i-1] = buff[i];
                    i -= 1;
                }
                removeChar(buff[i-1],'\\');
            }
        }
    }
}

//Check if the command exists in the given path and returns the full path
char * command_full_path(char * buff,const char ** path,char * name){
    size_t itt = 0;
    while(path[itt]){
        //get full path
        strncpy(buff,path[itt],PATH_BUFFER_SIZE);
        strncat(buff,name,PATH_BUFFER_SIZE);
        //test if we can acces file
        struct stat sb;
        if(!stat(buff,&sb)){
            if(S_ISREG(sb.st_mode) && sb.st_mode & 0111){
                if(access(buff,X_OK) != -1){
                    return buff;
                }else if(!path[itt+1]){
                    printf("%s\n",strerror(errno));
                }
            }
        }
        ++itt;
    }
    return NULL;
}

//changes the dir to path if it exists.
//Also changes the current working directory
void change_dir(char * path,char ** cwd){
    int err = chdir(path);
    if(err == -1){
        printf("%s\n",strerror(errno));
        return;
    }
    (*cwd) = getcwd(*cwd,PATH_BUFFER_SIZE);
}

//executes command with name
bool execute(const char ** path,char * name,char ** args){
    char full_path_buff[PATH_BUFFER_SIZE];
    char * full_path = command_full_path(full_path_buff,path,name);
    if(!full_path){
        return true;
    }
    pid_t pid = fork();
    if(pid == 0){
        if(execv(full_path,args) == -1)
            printf("%s\n",strerror(errno));
        return false;
    }else{
        waitpid(pid,NULL,0);
        return true;
    }
}


//Executes the command with given name and pipes the output in to command with other name.
bool execute_pipe(const char ** path,char * name,char ** args,char * name_child,char ** args_child){
    char full_path_buff[PATH_BUFFER_SIZE];
    char * full_path = command_full_path(full_path_buff,path,name);
    if(!full_path){
        return true;
    }
    pid_t pid = fork();
    {
        if(pid == -1){
            printf("%s\n",strerror(errno));
        }
        if(pid == 0){//parent
            char sub_buff[PATH_BUFFER_SIZE];
            char * sub_full_path = command_full_path(sub_buff,path,name_child);
            if(!sub_full_path){
                exit(1);
            }
            int pipes[2];
            if(pipe(pipes) == -1){
                printf("%s\n",strerror(errno));
                exit(1);
            }
            pid_t pid2 = fork();
            {
                if(pid2 == 0){//CHILD
                    close(pipes[1]);
                    while ((dup2(pipes[0],0) == -1) && (errno == EINTR)) {}
                    close(pipes[0]);
                    if(execv(sub_full_path,args_child) == -1)
                        printf("%s\n",strerror(errno));
                    exit(0);
                }else{ //PARENT
                    close(pipes[0]);
                    while ((dup2(pipes[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
                    close(pipes[1]);
                    if(execv(full_path,args) == -1)
                        printf("%s\n",strerror(errno));
                    wait(NULL);
                    exit(0);
                }
            }
        }else{//grand parent
            wait(NULL);
            return true;
        }
    }
}

//matches a command executes it,
//returns whether the shell should continue running.
bool match_command(struct CommandBuffer * cb,char ** cwd,const char ** path){
    bool res = true;
    if(strcmp(cb->command_buffer,"exit") == 0){
        res = false;
    }else{
        char ** buff = parse_command(cb->command_buffer);
        if(strcmp(buff[0],"cd") == 0){
            change_dir(buff[0],cwd);
        }else{
            bool found_pipe = false;
            size_t pos = 0;
            while(buff[pos]){
                if(strcmp(buff[pos],"|") == 0){
                    found_pipe = true;
                    buff[pos] = NULL;
                    break;
                }
                ++pos;
            }
            if(found_pipe){
                res = execute_pipe(path,buff[0],&buff[0],buff[pos+1],&buff[pos+1]);
            }else{
                res = execute(path,buff[0],&buff[0]);
            }
        }
        free(buff);
    }
    return res;
}

/*
 * Runs the shell
 */
void start_shell(){
    const char *path[] = {
        "./",
        "/usr/bin/",
        "/bin/",
        NULL
    };
    char buff[PATH_BUFFER_SIZE];
    char * cwd;
    cwd = getcwd(buff,PATH_BUFFER_SIZE); 
    bool running = true;
    struct CommandBuffer cmb = command_buffer_factory();
    while(running){
        printf("%s > ",cwd);
        get_command(&cmb,stdin);
        running = match_command(&cmb,&cwd,path);
    }
    delete_command_buffer(&cmb);
    printf("Exiting shell\n");
}
