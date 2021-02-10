// Michael Reilly
// I pledge my honor that I have abided by the Stevens Honor System.

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include <sys/sysinfo.h>

#define MAX_COUNT 250000000
#define MAX_THREADS 8

typedef struct arg_struct {     
	int start;     
	int end; 
} thread_args;

int nextbase;
int total_count=0;
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
pthread_t threads[MAX_THREADS];
int prime[MAX_COUNT];
char prime_s[20];
thread_args targs[MAX_THREADS];
int retval;

void crossout(int k, int end){ 
	for (int i=3; i*k<=MAX_COUNT; i+=2){
		prime[i*k]=0;
	}
}

// each thread runs this routine
void* sieve(void *ptr){ 
	thread_args x = *(thread_args *)ptr;
	int limit;
	int base;
	limit=x.end;
	do{
		// lock
		if((retval=pthread_mutex_lock(&lock))!=0){
			fprintf(stderr, "Warning: Cannot lock mutex. %s.\n", strerror(retval));
		}
		base=nextbase;
		nextbase+=2;
		// unlock
		if((retval = pthread_mutex_unlock(&lock))!=0){
			fprintf(stderr, "Warning: Cannot unlock mutex. %s.\n", strerror(retval));
		}
		if (base<=limit){
			// don't bother crossing out if base known composite
			if(prime[base]){
				crossout(base, limit);
			}
		}
		else return ptr;
	}while(1);
}

int main(int argc, char **argv) {
	if(argc<2){
		printf("Usage: ./mtsieve -s <starting value> -e <ending value> -t <num threads>\n");
		return EXIT_FAILURE;
	}
	int opt;
	int start=0, end=0, thread_num=0;
	int s_init=0, e_init=0, t_init=0;
	long long long_long_i;
	while((opt=getopt(argc,argv, ":s:e:t:"))!=-1){
		switch(opt){
			case 's':
				s_init++;
				start=atoi(optarg);
				if(start==0){
					fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'.\n", optarg, 's');
					return EXIT_FAILURE;
				}
				if(sscanf(optarg,"%lld", &long_long_i)== 1){
					int value=(int)long_long_i;
					if(long_long_i!=(long long)value){
						fprintf(stderr, "Error: Integer overflow for parameter '-%c'.\n", 's');
						return EXIT_FAILURE;
					}
				}
				break;
			case 'e':
				e_init++;
				end=atoi(optarg);
				if(end==0){
					fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'.\n", optarg, 'e');
					return EXIT_FAILURE;
				}
				if(sscanf(optarg,"%lld", &long_long_i)== 1){
					int value=(int)long_long_i;
					if(long_long_i!=(long long)value){
						fprintf(stderr, "Error: Integer overflow for parameter '-%c'.\n", 'e');
						return EXIT_FAILURE;
					}
				}
				break;
			case 't':
				t_init++;
				thread_num=atoi(optarg);
				if(thread_num==0){
					fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'.\n", optarg, 't');
					return EXIT_FAILURE;
				}
				if(sscanf(optarg,"%lld", &long_long_i)== 1){
					int value=(int)long_long_i;
					if(long_long_i!=(long long)value){
						fprintf(stderr, "Error: Integer overflow for parameter '-%c'.\n", 't');
						return EXIT_FAILURE;
					}
				}
				break;
			case '?':
				if (optopt == 'e' || optopt == 's' || optopt == 't') {         
					fprintf(stderr, "Error: Option -%c requires an argument.\n", optopt);     
				} else if (isprint(optopt)) {       
					fprintf(stderr, "Error: Unknown option '-%c'.\n", optopt);     
				} else {         
					fprintf(stderr, "Error: Unknown option character '\\x%x'.\n",                 
					optopt);     
				}     
				return EXIT_FAILURE;
				break;
		}
	}
	for(int i=1; i<argc; i++){
		if((argv[i][0]!='-' && (argv[i][1]!='s' || argv[i][1]!='e' || argv[i][1]!='t') && atoi(argv[i])==0)){
			fprintf(stderr, "Error: Non-option argument '%s' supplied.\n" , argv[i]);
			return EXIT_FAILURE;
		}
	}
	if(s_init==0){
		fprintf(stderr, "Error: Required argument <starting value> is missing.\n");
		return EXIT_FAILURE;
	}
	if(start<2){
		fprintf(stderr, "Error: Starting value must be >= 2.\n");
		return EXIT_FAILURE;
	}
	if(e_init==0){
		fprintf(stderr, "Error: Required argument <ending value> is missing.\n");
		return EXIT_FAILURE;
	}
	if(end<2){
		fprintf(stderr, "Error: Ending value must be >= 2.\n");
		return EXIT_FAILURE;
	}
	if(end<start){
		fprintf(stderr, "Error: Ending value must be >= starting value.\n");
		return EXIT_FAILURE;
	}
	if(t_init==0){
		fprintf(stderr, "Error: Required argument <num threads> is missing.\n");
		return EXIT_FAILURE;
	}
	if(thread_num<1){
		fprintf(stderr, "Error: Numbers of threads cannot be less than 1.\n");
		return EXIT_FAILURE;
	}
	if(thread_num>2*get_nprocs()){
		fprintf(stderr, "Error: Numbers of threads cannot exceed twice the number of processors(%d).\n", get_nprocs());
		return EXIT_FAILURE;
	}
	printf("Finding all prime numbers between %d and %d.\n", start, end);
	printf("%d segments:\n", thread_num);
	int count=end-start+1;
	if(thread_num>count){
		thread_num=count;
	}
	for(int i=2; i<=end; i++){
		if(i%2==0){
			prime[i]=0;
		}
		else{
			prime[i]=1;
		}
	}
	int thread_range=count/thread_num;
	int modulus=count%thread_num;
	int mod_init=modulus;
	nextbase=3;
	for(int i=0; i<thread_num; i++){
		targs[i].start=start+thread_range*i;
    	targs[i].end=start+thread_range*(i+1);
		if(modulus!=0){
			targs[i].start=targs[i].start+i;
			targs[i].end=targs[i].end+(i);
			modulus--;
		}
		else{
			targs[i].start=targs[i].start+mod_init;
			targs[i].end=targs[i].end+mod_init-1;
		}
		printf("   [%d, %d]\n", targs[i].start, targs[i].end);
		
		if ((retval=pthread_create(&threads[i],NULL,sieve,&targs[i]))!=0){
			fprintf(stderr, "Error: Cannot create thread %d. %s.\n", i + 1, strerror(retval));
			return EXIT_FAILURE;
		}
	}
	// once all is done
	for(int i=0;i<thread_num;i++){
		if(pthread_join(threads[i], NULL)!=0){
            fprintf(stderr, "Warning: Thread %d did not join properly.\n",i+1);
        }
    }
    if((retval = pthread_mutex_destroy(&lock)) != 0){
        fprintf(stderr, "Error: Cannot destroy mutex. %s.\n", strerror(retval));
    }
	
	for(int i=start;i<=end;i++){
		int three_count=0;
		if(prime[i]){
			sprintf(prime_s,"%d",i);
			for(int j=0; j<20; j++){
				if(prime_s[j]=='3'){
					three_count++;
				}
			}
			if(three_count>=2){
				total_count++;
			}
		}
	}
	printf("Total primes between %d and %d with two or more '3' digits: %d\n", start, end, total_count);
	
	return EXIT_SUCCESS;
}