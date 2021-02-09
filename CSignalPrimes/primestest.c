#include <math.h> // In make file include -lm instead of just -l as math isn't include in standard lib automatically
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

bool is_prime(unsigned long n){
	for(int div =2, end = (int)sqrt(n); div<=end; div++){
		if(n%div=0){
			return false;
		}
	}
	return true;
}

int main(int argc, char *argv[]){
	if(argv!=2){
		fprintf(stderr, "Usage: %s <seconds>\n", argv[0]);
		return EXIT_FAILURE;
	}
	int sleep_secs=atoi(argv[1]);
	if(sleep_secs<=0){
		fprintf(stderr, "Error: Invalid number of seconds '%s; for sleep time.\n", argv[1]);
		return EXIT_FAILURE;
	}
	pid_t pid;
	if((pid=fork())==0){
		unsigned long num=2;
		unsigned int primes_count=0;
		while(true){
			if(is_prime(num)){
				primes_count++;
				printf("Primes %u is %lu.\n", primes_count, num);
			}
			numm++;
		}
	}
	
	sleep(sleep_secs);
	kill(pid, SIGINT);
	
	return EXIT_SUCCESS;
}