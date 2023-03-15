#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXBUF 1024

void *handle_client(void *arg) {
    int sockfd = *(int *)arg;
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    char buffer[MAXBUF];
    int recv_len;
    int bytes_sent;

    memset(buffer, 0, MAXBUF);

    recv_len = recvfrom(sockfd, buffer, MAXBUF, 0,
                        (struct sockaddr *)&client_addr, &addrlen);

    if (recv_len < 0) {
        perror("recvfrom() failed");
        pthread_exit(NULL);
        exit(EXIT_FAILURE);
    }

    printf("Received message '%s' from %s:%d\n", buffer,
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    bytes_sent = sendto(sockfd, buffer, recv_len, 0,
                        (struct sockaddr *)&client_addr, addrlen);
    if (bytes_sent < 0) {
        perror("sendto() failed");
        pthread_exit(NULL);
        exit(EXIT_FAILURE);
    }

    printf("Sent message '%s' back to %s:%d\n", buffer,
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    pthread_exit(NULL);
    close(sockfd);
    return NULL;
}

int main(void) {
    pthread_t tid;

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAXBUF];
    int bytes_received, bytes_sent;
    socklen_t client_addr_len = sizeof(client_addr);

    // 設定socket以監聽UDP消息，端口為5000
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(5000);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    printf("Listening on %u:%d\n", server_addr.sin_addr.s_addr,
           server_addr.sin_port);

    // 循環接收並回傳消息
    while (1) {
        /*
        bytes_received =
            recvfrom(sockfd, buffer, sizeof(buffer), 0,
                     (struct sockaddr *)&client_addr, &client_addr_len);
        if (bytes_received < 0) {
            perror("recvfrom() failed");
            exit(EXIT_FAILURE);
        }

        printf("Received message '%s' from %s:%d\n", buffer,
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        bytes_sent = sendto(sockfd, buffer, bytes_received, 0,
                            (struct sockaddr *)&client_addr, client_addr_len);
        if (bytes_sent < 0) {
            perror("sendto() failed");
            exit(EXIT_FAILURE);
        }

        printf("Sent message '%s' back to %s:%d\n", buffer,
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        */

        // pthread_create handle client

        if ((recvfrom(sockfd, buffer, sizeof(buffer), 0,
                      (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("recvfrom() failed");
            continue;
        }

        if (pthread_create(&tid, NULL, handle_client, (void *)&sockfd) != 0) {
            perror("pthread_create() failed");
        }
    }

    close(sockfd);
    return 0;
}
