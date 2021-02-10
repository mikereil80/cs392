// Travis Dengler and Michael Reilly
// I pledge my honor that I have abided by the Stevens Honor System.

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

int numEOL(char *string){
	//counts the number of EOL characters to count total number of matches
	int count = 0;
	for (int i = 0; i <=sizeof(string); i++){
		if(string[i] == '\n'){
			count++;
		}
	}
	return count;
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
    if (argc != 5 && argc != 6) {
		printf("Usage: ./spfind -d <directory> -p <permissions string> [-h]\n");
		return EXIT_FAILURE;
	}
	bool permb=is_perm_string(argv[4]);
	if(permb==false){
		fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", argv[4]);
		return EXIT_FAILURE;
	}
    //int grep_to_wc[2], wc_to_parent[2];
    int pfind_to_sort[2], sort_to_parent[2];
    /*pipe(ps_to_grep);
    pipe(grep_to_wc);
    pipe(wc_to_parent);
    */
    pipe(pfind_to_sort);
    pipe(sort_to_parent);

    pid_t pid[2];
    
    if ((pid[0] = fork()) == 0) {
        // pfind, formerly ps
        close(pfind_to_sort[0]);
        dup2(pfind_to_sort[1], STDOUT_FILENO);

        // Close all unrelated file descriptors.
        close(sort_to_parent[0]);
        close(sort_to_parent[1]);
		argv[0] = "./pfind";
        if((execvp(argv[0], argv))==-1){
			fprintf(stderr, "Error: pfind failed.\n");
			return EXIT_FAILURE;
		}
    }
    
    if ((pid[1] = fork()) == 0) {
        // grep, but now sort
        close(pfind_to_sort[1]);
        dup2(pfind_to_sort[0], STDIN_FILENO);
        close(sort_to_parent[0]);
        dup2(sort_to_parent[1], STDOUT_FILENO);

        // Close all unrelated file descriptors.
        /*
        close(wc_to_parent[0]);
        close(wc_to_parent[1]);
		*/
        if((execlp("sort", "sort", NULL))==-1){
			fprintf(stderr, "Error: sort failed.\n");
			return EXIT_FAILURE;
		}
    }
    /*
    if ((pid[2] = fork()) == 0) {
        // wc
        close(grep_to_wc[1]);
        dup2(grep_to_wc[0], STDIN_FILENO);
        close(wc_to_parent[0]);
        dup2(wc_to_parent[1], STDOUT_FILENO);

        // Close all unrelated file descriptors.
        close(ps_to_grep[0]);
        close(ps_to_grep[1]);

        execlp("wc", "wc", "-l", NULL);
    }*/
	
	if((pid[0]=fork())>0 && (pid[1]=fork())>0){
	
    close(sort_to_parent[1]);
    dup2(sort_to_parent[0], STDIN_FILENO);
    // Close all unrelated file descriptors.
    close(pfind_to_sort[0]);
    close(pfind_to_sort[1]);
    /*
    close(grep_to_wc[0]);
    close(grep_to_wc[1]);
	*/
    char buffer[1];
    		int n = 0;
			while (1) {
			ssize_t count = read(sort_to_parent[0], buffer, sizeof(char));
			if(buffer[0] == '\n') n++;
			
			if (count == -1){
				if(errno == EINTR){                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
					continue;
				}
				else{
					perror("read()");
					exit(EXIT_FAILURE);
				}
			}
			else if (count == 0){
				break;
			}
			else{
				write(STDOUT_FILENO, buffer, count);
				}
			}
			if(n>0) n--;
			char numString [128];
			sprintf(numString, "%d", n);
			write(STDOUT_FILENO, "Total matches: ", strlen("Total matches: "));
			write(STDOUT_FILENO, numString, strlen(numString));
			write(STDOUT_FILENO,"\n", strlen("\n"));


    wait(NULL);
    wait(NULL);
    //wait(NULL);
    return EXIT_SUCCESS;
	}
}
