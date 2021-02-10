// Michael Reilly and Travis Dengler
// I pledge my honor that I have abided by the Stevens Honor System.

#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include <sys/wait.h>

#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT    "\x1b[0m"
#define MAX_BUFFER 4096

sigjmp_buf jmpbuf;
volatile sig_atomic_t signal_val = 0;

void catch_signal(int sig){
	if(!signal_val){
		return;
	}
	/* Jump back to min, don't return from the function.
		Give 1 back to sigsetjmp to distinguish it from the first time sigsetjmp returned. */
	siglongjmp(jmpbuf,2);
}

char* read_input(void){
	int position=0;
	int buf_size=1024;
	char *input=(char *)malloc(sizeof(char) * 1024);
	char c;
	c=getchar();
	while(c!=EOF && c!='\n'){
		input[position]=c;
		if (position>=buf_size){
			buf_size+=64;
			input=realloc(input, buf_size);
		}
		position++;
		c=getchar();
	}
	return input;
}

int cd_command(char *arg) {
	for(int i=0; i<strlen(arg); i++){
		if(arg[i]==' '){
			fprintf(stderr, "Error: Too many arguments to cd.\n");
			return -1;
		}
	}
    if(arg[0]!= '/'){
		if(arg[0]=='~'){
			uid_t uid=getuid();
			struct passwd *password=getpwuid(uid);
			chdir(password->pw_dir);
			return 0;
		}
		else{
			char path[MAX_BUFFER];
			strcpy(path,arg);
	
			char cwd[MAX_BUFFER];
			
			if (getcwd(cwd, sizeof(cwd)) != NULL){
				strcat(cwd,"/");
				strcat(cwd,path);
				if(chdir(cwd)!=0){
					fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", cwd, strerror(errno));
					return -1;
				}
			}
			else{
				fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
				return -1;
			}
		}
    }else{
		if(chdir(arg)!=0){
			fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", arg, strerror(errno));
			return -1;
		}
    }

    return 0;
}

int exit_command(char **arg){
	return 0;
}

char** get_input(char *input) {
	char **command=malloc(8 * sizeof(char *));
    if (command==NULL) {
        fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
        exit(1);
    }

    char *separator=" ";
    char *parsed;
    int index=0;

    parsed=strtok(input, separator);
    while(parsed != NULL){
		command[index] = parsed;
        index++;

        parsed = strtok(NULL, separator);
    }

    command[index] = NULL;
    return command;
}

int main(int argc, char *argv[]){
	char **command;
    char *input;
    pid_t child_pid;
    int status;
	
	struct sigaction action;
	action.sa_handler=catch_signal;
	sigemptyset(&action.sa_mask);
	action.sa_flags=SA_RESTART;
	if(sigaction(SIGINT,&action,NULL)==-1){
		fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));
		return EXIT_FAILURE;
	}
	
	while(1){
		if(sigsetjmp(jmpbuf, 1)==2){
			if(input==NULL){
				printf("\n");
			}
			continue;
		}
		
		signal_val=1;
		
		char cwd[MAX_BUFFER];
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			printf("[");
			printf("%s%s", BRIGHTBLUE, cwd);
			printf("%s]$ ", DEFAULT);
		}
		else{
			fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
			return EXIT_FAILURE;
		}
			
		input=read_input();
        command=get_input(input);
		if (!command[0]) {      /* Handle empty commands */
            free(input);
            free(command);
            continue;
        }
		if(strcmp(command[0], "exit")==0){
			free(input);
			free(command);
			exit_command(command);
			return EXIT_SUCCESS;
		}
        if (strcmp(command[0], "cd") == 0) {
			if(command[2]!=NULL){
				fprintf(stderr, "Error: Too many arguments to cd.\n");
				return EXIT_FAILURE;
			}
			else if(command[1]==NULL){
				cd_command("~");
			}
            else if (cd_command(command[1]) < 0) {
				free(input);
				free(command);
                return EXIT_FAILURE;
            }
			free(input);
			free(command);
            /* Skip the fork */
            continue;
        }

        child_pid = fork();
        if (child_pid<0) {
            fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
            return EXIT_FAILURE;
        }
        if (child_pid==0) {
			struct sigaction act_child;
			act_child.sa_handler=catch_signal;
			sigemptyset(&act_child.sa_mask);
			act_child.sa_flags=SA_RESTART;
			if(sigaction(SIGINT,&act_child,NULL)==-1){
				fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));
				return EXIT_FAILURE;
			}
			
				/* Never returns if the call is successful */
			if (execvp(command[0], command)<0) {
				fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
				return EXIT_FAILURE;
			}	
        } 
		else {
			if(waitpid(child_pid, &status, WUNTRACED)==-1){
				fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));
				return EXIT_FAILURE;
			}
        }

        free(input);
        free(command);
    }
	return EXIT_SUCCESS;
}