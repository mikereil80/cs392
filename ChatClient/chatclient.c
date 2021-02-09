// Michael Reilly and Travis Dengler
// I pledge my honor that I have abided by the Stevens Honor System.

#include <arpa/inet.h> 
#include <errno.h> 
#include <fcntl.h> 
#include <stdbool.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include "util.h" 
 
int client_socket = -1; 
char username[MAX_NAME_LEN + 1]; 
char inbuf[BUFLEN + 1]; 
char outbuf[MAX_MSG_LEN + 1]; 

struct sockaddr_in server_addr;
socklen_t addrlen=sizeof(struct sockaddr_in);
int ip_conversion;
int port_n;
int retval;

int handle_stdin() { 
	int string=get_string(inbuf, MAX_MSG_LEN);
	if(strcmp(inbuf, "bye")==0){
		printf("Goodbye.\n");
		return -1;
	}
	if(string==TOO_LONG){
		fprintf(stderr, "Sorry, limit your message to %d characters.\n", MAX_MSG_LEN);
		return -1;
	}
	else{
		send(client_socket, inbuf, strlen(inbuf), 0);
		return 0;
	}
} 
 
int handle_client_socket() { 
	int received;
	if((received=recv(client_socket, inbuf, BUFLEN, 0))<0){
		fprintf(stderr, "Warning: Failed to receive incoming message. %s.\n", strerror(errno));
		return -1;
	}
	else if(received==0){
		fprintf(stderr, "\nConnection to server has been lost.\n");
		return -1;
	}
	inbuf[BUFLEN]='\0';
	if(strcmp(inbuf, "bye")==0){
		printf("\nServer initiated shutdown.\n");
		return -1;
	}
	printf("%s\n", inbuf);
	return 0;
} 
 
int main(int argc, char *argv[]) { 
	if(argc!=3){
		fprintf(stderr, "Usage: %s <server IP> <port>\n", "./chatclient");
		return EXIT_FAILURE;
	}
	retval=EXIT_SUCCESS;
	
	memset(&server_addr, 0, addrlen);
	memset(inbuf, 0, sizeof(inbuf) - 1);
	memset(outbuf, 0, sizeof(outbuf) - 1);
	
	ip_conversion=inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
	if(ip_conversion<=0){
		fprintf(stderr, "Error: Invalid IP address '%s'.\n", argv[1]);
		return EXIT_FAILURE;
	}
	bool valid=false;
	for(int i=1024; i<=65535; i++){
		if((parse_int(argv[2], (int *)&i, "port number"))==true){
			port_n=i;
			valid=true;
			break;
		}
	}
	if(valid==false){
		fprintf(stderr, "Port must be in range [1024, 65535].\n");
		retval=EXIT_FAILURE;
		goto EXIT;
	}
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(port_n);
	

	while(1){
		memset(outbuf, 0, sizeof(outbuf) - 1);
		printf("Please enter a user name: ");
		scanf("%s", username);
		if(strlen(username) > MAX_NAME_LEN){
			printf("Sorry, limit your username to %d characters.\n", MAX_NAME_LEN);
		}
		else{
			break;
		}
	}
	for(int i=1; i<=MAX_NAME_LEN; i++){
		if(username[i]=='\n'){
			username[i]='\0';
		}
	}
	printf("Hello, %s. Let's try to connect to the server.\n", username);
	
	if((client_socket=socket(AF_INET, SOCK_STREAM,0))<0){
		fprintf(stderr, "Error: Failed to create socket. %s.\n", strerror(errno));
		retval=EXIT_FAILURE;
		goto EXIT;
	}
	if(connect(client_socket, (struct sockaddr *)&server_addr, addrlen)<0){
		fprintf(stderr, "Error: Failed to connect to server. %s.\n", strerror(errno));
		retval=EXIT_FAILURE;
		goto EXIT;
	}
	int bytes_recvd;
	if((bytes_recvd=recv(client_socket, inbuf, BUFLEN, 0))<0){
		fprintf(stderr, "Error: Failed to receive message from server. %s.\n", strerror(errno));
		retval=EXIT_FAILURE;
		goto EXIT;
	}
	else if(bytes_recvd==0){
		fprintf(stderr, "All connections are busy. Try again later.\n");
		retval=EXIT_FAILURE;
		goto EXIT;
	}
	inbuf[bytes_recvd]='\0';
	printf("%s", inbuf);
	if(send(client_socket, username, strlen(username), 0)<0){
		fprintf(stderr, "Error: Failed to send username to server. %s.\n", strerror(errno));
		retval=EXIT_FAILURE;
		goto EXIT;
	}
	fd_set read_fds;
	fd_set write_fds;
	fd_set except_fds;
	
	while (1) {
		FD_ZERO(&read_fds);
		FD_SET(STDIN_FILENO, &read_fds);
		FD_SET(client_socket, &read_fds);
		
		FD_ZERO(&write_fds);
		if (strlen(outbuf) > 0){
			FD_SET(client_socket, &write_fds);
		}
		FD_ZERO(&except_fds);
		FD_SET(STDIN_FILENO, &except_fds);
		FD_SET(client_socket, &except_fds);
		
		int activity = select(client_socket, &read_fds, &write_fds, &except_fds, NULL);
		if(activity<=0){
			retval=EXIT_FAILURE;
			goto EXIT;
		}
		else{
			if (FD_ISSET(STDIN_FILENO, &read_fds)) {
				if (handle_stdin() < 0) {
					retval=EXIT_FAILURE;
					goto EXIT;
				}
			}
			if (FD_ISSET(STDIN_FILENO, &except_fds)) {
				retval=EXIT_FAILURE;
				goto EXIT;
			}
			if (FD_ISSET(client_socket, &read_fds)) {
				if (handle_client_socket() < 0) {
					retval=EXIT_FAILURE;
					goto EXIT;
				}
			}
			if (FD_ISSET(client_socket, &write_fds)) {
				if (handle_client_socket() < 0){
					retval=EXIT_FAILURE;
					goto EXIT;
				}
			}
			if (FD_ISSET(client_socket, &except_fds)) {
				retval=EXIT_FAILURE;
				goto EXIT;
			}
        }
    }
EXIT:
	if(fcntl(client_socket, F_GETFD)>=0){
		close(client_socket);
	}
	return retval;
} 