// Michael Reilly
// I pledge my honor that I have abided by the Stevens Honor System.

#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>

void set_read(int* p1){
	dup2(p1[0], STDIN_FILENO);
	close(p1[0]);
	close(p1[1]);
}

void set_write(int* p2){
	dup2(p2[1], STDOUT_FILENO);
	close(p2[0]);
	close(p2[1]);
}

bool is_perm_string(char argv[]){
	if(strlen(argv)!=9){
		return false;
	}
	else{
		for(int i=0; i<9; i+=3){
			if(argv[i]!='r'){
				if(argv[i]!='-'){
					return false;
				}
			}
			if(argv[i+1]!='w'){
				if(argv[i+1]!='-'){
					return false;
				}
			}
			if(argv[i+2]!='x'){
				if(argv[i+2]!='-'){
					return false;
				}
			}
		}
	}
	return true;
}

int main(int argc, char *argv[]) {
	if(argc==1){
		printf("Usage: ./spfind -d <directory> -p <permissions string> [-h]\n");
		return EXIT_FAILURE;
	}
	bool permb=is_perm_string(argv[4]);
	if(permb==false){
		fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", argv[4]);
		return EXIT_FAILURE;
	}
	
	pid_t pidpfind;
	pid_t pidsort;
	int pipepfind[2];
	int pipesort[2];
	
	if((pidpfind=fork())<0){
		perror("Error: pfind failed.\n");
		close(pipepfind[0]);
		close(pipepfind[1]);
		close(pipesort[0]);
		close(pipesort[1]);
		return EXIT_FAILURE;
	}
	if((pidsort=fork())<0){
		perror("Error: sort failed.\n");
		close(pipepfind[0]);
		close(pipepfind[1]);
		close(pipesort[0]);
		close(pipesort[1]);
		return EXIT_FAILURE;
	}
	
	if((pidsort=fork())>0){
		pipe(pipesort);
		set_read(pipesort);
		int n=read(0, pipesort, sizeof(pipesort));
		for(int i=0; i<n; i++){
			printf("%d\n",pipesort[i]);
		}
	}
	
	if((pidpfind=fork())==0){
		set_write(pipepfind);
		pipesort[0]=pipepfind[0];
		pipesort[1]=pipepfind[1];
		if(execv("/home/cs392/shared/cs392/pfindbinary/pfind", argv)==-1){
			fprintf(stderr, "Error: pfind failed.\n");
			return EXIT_FAILURE;
		}
	}
	if((pidsort=fork())==0){
		pipe(pipepfind);
		set_read(pipesort);
		set_write(pipepfind);
		execlp("sort", "sort", pipepfind[0], pipesort[1], NULL);
		close(pipesort[0]);
		close(pipesort[1]);
		pipesort[0]=pipepfind[0];
		pipesort[1]=pipepfind[1];
		set_write(pipesort);
		close(pipesort[0]);
		close(pipesort[1]);
	}
	
	
	return EXIT_SUCCESS;
}