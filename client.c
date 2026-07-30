#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define MAX_FIELD 30
#define MAX_MSG 128

int main(void) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket failed"); return 1; }

    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server.sin_addr) <= 0) {
        fprintf(stderr, "Invalid server address\n");
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed - is the server running?");
        close(sock);
        return 1;
    }

    char user[MAX_FIELD], pass[MAX_FIELD];
    printf("Username: ");
    if (scanf("%29s", user) != 1) { close(sock); return 1; }
    printf("Password: ");
    if (scanf("%29s", pass) != 1) { close(sock); return 1; }

    char msg[MAX_MSG];
    snprintf(msg, sizeof(msg), "AUTH %s %s", user, pass);

    if (send(sock, msg, strlen(msg), 0) < 0) {
        perror("send failed");
        close(sock);
        return 1;
    }

    char reply[MAX_MSG];
    ssize_t n = recv(sock, reply, sizeof(reply) - 1, 0);
    if (n < 0) {
        perror("recv failed");
        close(sock);
        return 1;
    }
    reply[n] = '\0'; 

    printf("Server says: %s", reply);

    close(sock);
    return 0;
}
