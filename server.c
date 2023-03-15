#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main(void) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1024];
    int bytes_received, bytes_sent;
    socklen_t client_addr_len = sizeof(client_addr);

    // 設定socket以監聽UDP消息，端口為5000
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    /*
    htonl()--"Host to Network Long"
    ntohl()--"Network to Host Long"
    htons()--"Host to Network Short"
    ntohs()--"Network to Host Short"
    */
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(5000);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    // 循環接收並回傳消息
    while (1) {
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
    }

    return 0;
}
