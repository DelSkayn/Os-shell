#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "shell.h"

const size_t SIZE_INCREASE = 40;
const size_t PATH_BUFFER_SIZE = 1024;
const size_t MAX_TOKENS = 128;

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

void removeChar(char *str, char garbage) {

    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

int parse_command(char ** buff, char * src, const char * delim){
        buff[0] = src;
        for(size_t i = 1;i < MAX_TOKENS;i++){
            buff[i] = strstr(buff[i-1],delim);
            //test for end of string
            if(!buff[i])
                return i;
            //test for escape characters 
            while(*(buff[i]-1) == '\\'){
                buff[i] = strstr(buff[i]+1,delim);
            //test for end of string
                if(!buff[i])
                    removeChar(buff[i-1],'\\');
                    return i;
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
        return MAX_TOKENS;
}


bool match_command(struct CommandBuffer * cb,char ** cwd,const char ** path){
	if(strcmp(cb->command_buffer,"exit") == 0){
		return false;
	}else{
		char * buff[MAX_TOKENS];
		int amount_tokens = parse_command(buff,cb->command_buffer," ");
                for(int i = 0;i < amount_tokens;i++){
                    printf("test: %s \n",buff[i]);
                }
                if(strcmp(buff[0],"cd") == 0){
			int err = chdir(buff[1]);
			if(err == -1){
				printf("%s\n",strerror(errno));
			}
			(*cwd) = getcwd(*cwd,PATH_BUFFER_SIZE);
		}else{
                        size_t itt = 0;
                        while(path[itt]){
                            //get full path
                            char full_path_buff[PATH_BUFFER_SIZE];
                            strncpy(full_path_buff,path[itt],PATH_BUFFER_SIZE);
                            strncat(full_path_buff,buff[0],PATH_BUFFER_SIZE);
                            //test if we can acces file
                            if(access(full_path_buff,X_OK) != -1){
                                pid_t pid = fork();
                                if(pid == 0){
                                    if(execv(full_path_buff,&buff[0]) == -1)
                                        printf("%s\n",strerror(errno));
                                    return false;
                                }else{
                                    waitpid(pid,NULL,0);
                                    return true;
                                }
                            }else if(!path[itt+1]){
                                printf("%s\n",strerror(errno));
                            }
                            itt += 1;
                        }
                }
                return true;
        }
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
