/*******************************************************************************
 * Name        : sort.c
 * Author      : Michael Reilly
 * Date        : 2/21/20
 * Description : Uses quicksort to sort a file of either ints, doubles, or
 *               strings.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quicksort.h"

#define MAX_STRLEN     64 // Not including '\0'
#define MAX_ELEMENTS 1024
#define BUFSIZE	128

typedef enum {
    STRING,
    INT,
    DOUBLE
} elem_t;

/**
 * Basic structure of sort.c:
 *
 * Parses args with getopt.
 * Opens input file for reading.
 * Allocates space in a char** for at least MAX_ELEMENTS strings to be stored,
 * where MAX_ELEMENTS is 1024.
 * Reads in the file
 * - For each line, allocates space in each index of the char** to store the
 *   line.
 * Closes the file, after reading in all the lines.
 * Calls quicksort based on type (int, double, string) supplied on the command
 * line.
 * Frees all data.
 * Ensures there are no memory leaks with valgrind. 
 */
int main(int argc, char **argv) {
	if(argc==1 || argc>3){
		printf("Usage: ./sort [-i|-d] [filename]\n");
		printf("\t-i: Specifies the file contains ints.\n");
		printf("\t-d: Specifies the file contains doubles.\n");
		printf("\tfilename: The file to sort.\n");
		printf("\tNo flags defaults to sorting strings.\n");
		return EXIT_FAILURE;
	}
	FILE *fp;
	int opt;
	while((opt=getopt(argc,argv, ":id"))!=-1){
		switch(opt){
			case 'i':
				fp=fopen(argv[2],"r");
				if(fp==NULL){
					fprintf(stderr, "Error: Cannot open file '%s'. %s.\n", argv[2], strerror(errno));
					fclose(fp);
					return EXIT_FAILURE;
				}
				else{
					int i=0;
					int n=0;
					int *iarray=(int *)malloc(sizeof(int) * MAX_ELEMENTS);
					while(fscanf(fp, "%d", &iarray[i])==1){
						n++;
						i++;
					}
					fclose(fp);
					int (*comparatori)(const void*, const void*)=&int_cmp;
					quicksort(iarray,n,sizeof(int),comparatori);
					for(int j=0;j<n;j++){
						printf("%d\n", iarray[j]);
					}
					free(iarray);
					return EXIT_SUCCESS;
				}
			case 'd':
				fp=fopen(argv[2],"r");
				if(fp==NULL){
					fprintf(stderr, "Error: Cannot open file '%s'. %s.\n", argv[2], strerror(errno));
					fclose(fp);
					return EXIT_FAILURE;
				}
				else{
					int i=0;
					int n=0;
					double *darray=(double *)malloc(sizeof(double) * MAX_ELEMENTS);
					while(fscanf(fp, "%lf", &darray[i])==1){
						n++;
						i++;
					}
					fclose(fp);
					int (*comparatord)(const void*, const void*)=&dbl_cmp;
					quicksort(darray,n,sizeof(double),comparatord);
					for(int j=0;j<n;j++){
						printf("%lf\n", darray[j]);
					}
					free(darray);
					return EXIT_SUCCESS;
				}
			case '?':
				printf("Error: Unkown option '%c' received.\n", optopt);
				printf("Usage: ./sort [-i|-d] [filename]\n");
				printf("\t-i: Specifies the file contains ints.\n");
				printf("\t-d: Specifies the file contains doubles.\n");
				printf("\tfilename: The file to sort.\n");
				printf("\tNo flags defaults to sorting strings.\n");
				return EXIT_FAILURE;
				
		}
	}
	fp=fopen(argv[1],"r");
	if(fp==NULL){
		fprintf(stderr, "Error: Cannot open file '%s'. %s.\n", argv[1], strerror(errno));
		fclose(fp);
		return EXIT_FAILURE;
	}
	else{
		int n=0;
		char **sarray=(char **)malloc(sizeof(char *) * MAX_ELEMENTS);
		for(int k=0;k<MAX_ELEMENTS;k++){
			sarray[k]=(char *)malloc(sizeof(char *) * MAX_STRLEN);
		}
		while(fgets(sarray[n], MAX_STRLEN, fp)!=NULL){
			n++;
		}
		fclose(fp);
		int (*comparators)(const void*, const void*)=&str_cmp;
		quicksort(sarray,n,sizeof(sarray[0]),comparators);
		for(int i=0;i<n;i++){
			printf("%s", sarray[i]);
		}
		for(int j=0;j<MAX_ELEMENTS;j++){
			free(sarray[j]);
		}
		free(sarray);
		return EXIT_SUCCESS;
	}
			
	return EXIT_SUCCESS;
	
}