#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define MAX_WORKERS 3
#define BUFFER_SIZE 256

void error(const char *msg) {
    perror(msg);
    exit(1);
}

pthread_mutex_t mutex;

void* serve(void* arg) {
    int newsockfd = *(int*)arg;
    char buffer[BUFFER_SIZE];
    int n;

    memset(buffer, 0, BUFFER_SIZE);
    n = read(newsockfd, buffer, BUFFER_SIZE - 1);
    if (n < 0) error("ERROR reading from socket");

    int received = atoi(buffer);
    printf("Server received: %d\n", received);

    int response = received * 5;
    sprintf(buffer, "%d", response);
    n = write(newsockfd, buffer, strlen(buffer));
    if (n < 0) error("ERROR back to socket");

    close(newsockfd);
    return NULL;
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    pthread_mutex_init(&mutex, NULL);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening scket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        pthread_mutex_lock(&mutex);
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
        if (newsockfd < 0) {
            pthread_mutex_unlock(&mutex);
            error("ERROR on accept");
        }
        pthread_mutex_unlock(&mutex);

        pthread_t worker_thread;
        if (pthread_create(&worker_thread, NULL, serve, (void*)&newsockfd) != 0) {
            error("ERROR creating thread");
        }

        pthread_detach(worker_thread);
    }

    close(sockfd);
    pthread_mutex_destroy(&mutex);
    return 0;
}

