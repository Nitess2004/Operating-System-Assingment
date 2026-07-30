
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#define PORT 8080
#define USERS_FILE "users.db"
#define MAX_MSG 128
#define MAX_FIELD 30

void reap_children(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int check_credentials(const char *user, const char *pass) {
    FILE *f = fopen(USERS_FILE, "r");
    if (!f) return 0;
    char du[MAX_FIELD], dp[MAX_FIELD];
    int ok = 0;
    while (fscanf(f, "%29s %29s", du, dp) == 2) {
        if (strcmp(user, du) == 0 && strcmp(pass, dp) == 0) { ok = 1; break; }
    }
    fclose(f);
    return ok;
}

void handle_client(int fd) {
    char buf[MAX_MSG];
    ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
    if (n <= 0) { close(fd); return; }
    buf[n] = '\0'; 

    char cmd[8], user[MAX_FIELD], pass[MAX_FIELD];
    const char *reply;

    if (sscanf(buf, "%7s %29s %29s", cmd, user, pass) != 3 || strcmp(cmd, "AUTH") != 0)
        reply = "ERR MALFORMED_REQUEST\n";
    else if (check_credentials(user, pass)) {
        reply = "OK LOGIN_SUCCESS\n";
        printf("[+] %s authenticated (pid %d)\n", user, getpid());
    } else {
        reply = "FAIL LOGIN_FAILED\n";
        printf("[-] failed login for '%s'\n", user);
    }

    send(fd, reply, strlen(reply), 0);
    close(fd);
}

int main(void) {
    setvbuf(stdout, NULL, _IOLBF, 0); 
    signal(SIGCHLD, reap_children);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(1); }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) { perror("bind"); exit(1); }
    if (listen(server_fd, 10) < 0) { perror("listen"); exit(1); }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); close(client_fd); }
        else if (pid == 0) { close(server_fd); handle_client(client_fd); exit(0); }
        else close(client_fd);
    }
    return 0;
}
