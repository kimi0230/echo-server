#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5000
#define BUFFER_SIZE 1024
#define MAX_RETRY 10
#define BASE_BACKOFF 500
#define MAX_BACKOFF 8000
#define BACKOFF_MULTIPLIER 2

/*
wait_interval = base * multiplier^n
* base is the initial interval, ie, wait for the first retry
* n is the number of failures that have occurred
* multiplier is an arbitrary multiplier that can be replaced with any suitable
value
*/
void exponential_backoff(int n, int *interval) {
    if (n == 0) {
        *interval = 0;
    } else {
        *interval = (BACKOFF_MULTIPLIER << (n - 1)) * BASE_BACKOFF;
        // printf("interval = %d\n", *interval);
        if (*interval > MAX_BACKOFF) {
            *interval = MAX_BACKOFF;
        }
    }
    printf("interval = %d\n", *interval);
}

int main(int argc, char *argv[]) {
    int sockfd, retry;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int buffer_len;
    int server_addr_len;
    int exit_code = 0;
    int wait_interval = BASE_BACKOFF;
    int max_wait_interval = MAX_BACKOFF;
    int backoff_multiplier = BACKOFF_MULTIPLIER;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(1);
    }

    buffer_len = sprintf(buffer, "%s", argv[1]);
    server_addr_len = sizeof(server_addr);
    int interval = 0;
    for (retry = 0; retry < MAX_RETRY; retry++) {
        if (sendto(sockfd, buffer, buffer_len, 0,
                   (struct sockaddr *)&server_addr, server_addr_len) < 0) {
            perror("sendto failed");
            exit_code = 1;
            break;
        }

        printf("Sent message: %s\n", buffer);

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1000;
        int sel_ret = select(sockfd + 1, &readfds, NULL, NULL, &tv);
        if (sel_ret == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        } else if (sel_ret == 0) {
            printf("No response from server, retrying...\n");
        } else {
            if (recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL) >= 0) {
                printf("Received message: %s\n", buffer);
                exit_code = 0;
                break;
            }
        }
        exponential_backoff(retry, &interval);
        sleep(interval / 1000);
    }

    close(sockfd);
    exit(exit_code);
}
