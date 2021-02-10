#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 128

bool is_integer(char *input){
	int start=0, len=strlen(input);
	if(len>=1 && input[0]=="-"{
		if(len<2){
			return false;
		}
		start=1;
	}
	for(int i=start; i<len; i++){
		if(!isdigit(input[i])){
			return false;
		}
	}
	return true;
}

bool get_integer(char *input, int *value){
	long long long_long_i;
	if(sscanf(input,"%lld", &long_long_i)== 1){
		*value=(int)long_long_i;
		if(long_long_i!=(long long)*value){
			fprintf(stderr, "Warning: Integer overflow with '%s'.\n", input);
			return false;
		}
		return true;
	}
	else{
		return false;
	}
}

int main(int argc, char *argv[]) {
	if(argc!=2){
		return EXIT_FAILURE;
	}
	char buf[BUFSIZE];
	
	FILE *fp=fopen(argv[1], "r");
	if(fp==NULL){
		fprintf(stderr, "Error: Cannot open file '%s'.\n", argv[1]);
		return EXIT_FAILURE;
	}
	
	while(fgets(buf, BUFSIZE-1, fp)){
		buf[strlen(buf)-1]='\0';
		if(is_integer(buf)){
			int val;
			if(get_integer(nuf, &val)){
				printf("%s\n", buf);
			}
		}
	}
	
	fclose(fp);
	return EXIT_SUCCESS;
}