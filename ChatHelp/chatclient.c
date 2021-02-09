#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "util.h"


int client_socket = -1;
int* port;
char username[MAX_NAME_LEN + 1];
char inbuf[BUFLEN + 1];
char outbuf[MAX_MSG_LEN + 1];
int retval = EXIT_SUCCESS;
struct sockaddr_in server_addr;
socklen_t addrlen = sizeof(struct sockaddr_in);

/*
int handle_stdin() {
}
*/
int handle_client_socket() {

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error: Failed to create socket. %s.\n", strerror(errno));
        retval = EXIT_FAILURE;
        return -1;
    }
    server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(*port);
	
	memset(inbuf, 0, sizeof(inbuf) - 1);
	memset(outbuf, 0, sizeof(outbuf) - 1);
	
    if (connect(client_socket, (struct sockaddr *)&server_addr, addrlen) < 0) {
        fprintf(stderr, "Error: Failed to connect to server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        return -1;
    }
    
    recv(client_socket, inbuf, BUFLEN, 0);
    printf("%s\n", inbuf);
	
	return 0;
}




int main(int argc, char*argv[]){
	if (argc != 3){
		fprintf(stderr, "Usage: %s <server IP> <port>\n", argv[0]);
		retval = EXIT_FAILURE;
		goto EXIT;
	}
	
	port = malloc(sizeof(int));
    memset(&server_addr, 0, addrlen);
    
    
	if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) < 1){
		fprintf(stderr, "Error: Invalid IP address '%s'.\n", argv[1]);
		retval = EXIT_FAILURE;
		goto EXIT;
	}
	
	if (!parse_int(argv[2], port, "<port>")){
		retval = EXIT_FAILURE;
		goto EXIT;
	}
	
	if (*port > 65535 || *port < 1024){
		fprintf(stderr, "Error: Port must be in range [1024, 65535].\n");
		retval = EXIT_FAILURE;
		goto EXIT;
	}
	//username[0] = 'p';
	while(true){
		printf("Enter your username: ");
		scanf("%s", username);
		if(strlen(username) > MAX_NAME_LEN){
			printf("Sorry, limit your username to %d characters.\n", MAX_NAME_LEN);
		} else if (strlen(username) > 0){
			break;
		}
	}
	printf("Hello, %s. Let's try to connect to the server.\n", username);
	if(handle_client_socket() ==  -1){
		goto EXIT;
	}
	

EXIT:
	free(port);


}
