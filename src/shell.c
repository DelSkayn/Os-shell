#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

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

struct CommandBuffer command_buffer_factory(){
    struct CommandBuffer cb;
    cb.command_buffer = NULL;
    cb.size = 0;
    return cb;
}

void delete_command_buffer(struct CommandBuffer * cmb){
    free(cmb->command_buffer);
}

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

bool match_command(struct CommandBuffer * cb,char ** cwd,const char ** path){
    if(strcmp(cb->command_buffer,"exit") == 0){
        return false;
    }else{
        char * next = strtok(cb->command_buffer," ");
        char * current = cb->command_buffer;
        if(strcmp(current,"cd") == 0){
            current = next;
            next = strtok(next," ");
        }
    }
    return true;
}

/*
 * Runs the shell
 */
void start_shell(){
    const char *path[] = {
        "./",
        "/usr/bin",
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
}
