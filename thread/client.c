#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>

void error (char *m){
	perror(m);
	exit(0);
}

int main(int argc, char *argv[]) {

	int sockfd, port, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];
	if (argc < 3) error("usage client [hostname] [port]\n");
	port = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) error ("ERROR opening scoket");
	server = gethostbyname(argv[1]);
	if (server == NULL) error ("ERROR, no such host\n");
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;

	bcopy((char *) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);

	if (connect(sockfd, (const struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error ("ERROR conencting");
	printf("Please enter the message : ");
	fgets(buffer, 255, stdin);
	n = write(sockfd, buffer, strlen(buffer));
	
	n = read(sockfd, buffer, 255);
	if (n < 0) error("ERROR reading from scoket");
	int received = atoi(buffer);

	printf("Client received: %d\n", received);
	if (n < 0) error ("ERROR reading from socket");
	return 0;
}
