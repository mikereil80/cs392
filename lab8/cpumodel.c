#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool starts_with(const char *str, const char *prefix) {
    /* TODO:
       Return true if the string starts with prefix, false otherwise.
       Note that prefix might be longer than the string itself.
     */
	 if(strlen(prefix)>strlen(str)){
		 return false;
	 }
	 else if(strncmp(str, prefix, strlen(prefix))==0){
		 return true;
	 }
	 else{
		 return false;
	 }

}

int main() {
    /* TODO:
       Open "cat /proc/cpuinfo" for reading.
       If it fails, print the string "Error: popen() failed. %s.\n", where
       %s is strerror(errno) and return EXIT_FAILURE.
     */
	 FILE *pipe;
	 int status;
	 if((pipe=popen("cat /proc/cpuinfo", "r"))==NULL){
		 fprintf(stderr, "Error: popen() failed. %s.\n", strerror(errno));
		 return EXIT_FAILURE;
	 }


    /* TODO:
       Allocate an array of 256 characters on the stack.
       Use fgets to read line by line.
       If the line begins with "model name", print everything that comes after
       ": ".
       For example, with the line:
       model name      : AMD Ryzen 9 3900X 12-Core Processor
       print
       AMD Ryzen 9 3900X 12-Core Processor
       including the new line character.
       After you've printed it once, break the loop.
     */
	 char buf[256];
	 while(fgets(buf,256,pipe)!=NULL){
		 if(starts_with(buf, "model name")==true){
			 for(int i=0; i<256; i++){
				if(buf[i]==':' && buf[i+1]==' '){
					for(int j=i+2; j<256; j++){
						printf("%c",buf[j]);
						if(buf[j]=='\n'){
							break;
						}
					}
					break;
				}
			 }
			 break;
		 }
	 }

    /* TODO:
       Close the file descriptor and check the status.
       If closing the descriptor fails, print the string
       "Error: pclose() failed. %s.\n", where %s is strerror(errno) and return
       EXIT_FAILURE.
     */
	 if((status=pclose(pipe))<0){
		 fprintf(stderr, "Error: pclose() failed. %s.\n", strerror(errno));
		 return EXIT_FAILURE;
	 }
	 else{
		 return EXIT_SUCCESS;
	 }
}
