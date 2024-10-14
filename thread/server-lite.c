#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

#define N 3

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int sockfd;
struct sockaddr_in serv_addr, cli_addr;
socklen_t clilen;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void* serve() {
    int newsockfd;
    char buffer[256];
    int n;

    pthread_mutex_lock(&mutex);
    newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
    if (newsockfd < 0) error("ERROR on accept");

    pthread_mutex_unlock(&mutex);
    memset(buffer, 0, 256);
    n = read(newsockfd, buffer, 255);
    if (n < 0) error("ERROR reading from socket");

    int received = atoi(buffer);
    printf("Server received: %d\n", received);

    int response = received * 5;
    char response_str[256];
    sprintf(response_str, "%d", response);

    n = write(newsockfd, response_str, strlen(response_str));
    if (n < 0) error("ERROR writing to socket");

    close(newsockfd);

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    int port = atoi(argv[1]);
    pthread_t threads[N];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    memset((char*)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    for (int i = 0; i < N; i++) {
        if (pthread_create(&threads[i], NULL, serve, NULL) != 0)
            error("ERROR creating thread");
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    close(sockfd);

    return 0;
}
