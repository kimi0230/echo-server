#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

void* client_handler(void* arg) {
    int server_fd = *(int*)arg;
    char buffer[MAX_BUFFER_SIZE] = {0};
    char* message = "Hello from server";
    int n;
    struct sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof(client_addr);

    // 接收客戶端消息並回覆
    n = recvfrom(server_fd, buffer, MAX_BUFFER_SIZE, 0,
                 (struct sockaddr*)&client_addr, &client_addrlen);
    if (n < 0) {
        perror("recvfrom failed");
        close(server_fd);
        pthread_exit(NULL);
    }

    buffer[n] = '\0';
    printf("Received message: %s\n", buffer);

    sendto(server_fd, message, strlen(message), 0,
           (struct sockaddr*)&client_addr, client_addrlen);
    printf("Response sent\n");

    close(server_fd);
    pthread_exit(NULL);
}

int main(int argc, char const* argv[]) {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // 建立socket
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // 設定server address
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 綁定socket到server address
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d...\n", PORT);

    // 接收客戶端消息
    while (1) {
        pthread_t thread_id;

        // 使用pthread來處理客戶端消息
        if (pthread_create(&thread_id, NULL, client_handler,
                           (void*)&server_fd) < 0) {
            perror("pthread_create failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
