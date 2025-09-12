#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h> // For rand()

#define PORT 8042
#define MAX_CLIENTS 10
#define FPS 60

float cubeX = 1.0f;
float cubeY = 1.0f;

void physics() {
    float dx = (((float) (rand() % 1000)) / 1000.0f - 0.5f) * 2.0f * 0.01f;
    float dy = (((float) (rand() % 1000)) / 1000.0f - 0.5f) * 2.0f * 0.01f;
    cubeX += dx;
    cubeY += dy;

    if (cubeX > 8.0f) cubeX = 8.0f;
    if (cubeY > 8.0f) cubeY = 8.0f;
    if (cubeX < 0.0f) cubeX = 0.0f;
    if (cubeY < 0.0f) cubeY = 0.0f;
}

int main() {
    int server_sock, client_sockets[MAX_CLIENTS] = {0};
    struct sockaddr_in addr;
    fd_set read_fds;
    int max_fd, num_clients = 0;

    srand(time(NULL)); // Seed rand for physics

    // Create server socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        return 1;
    }

    // Set non-blocking
    fcntl(server_sock, F_SETFL, O_NONBLOCK);

    // Bind to port
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    // Listen for connections
    if (listen(server_sock, 5) < 0) {
        perror("listen");
        return 1;
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(server_sock, &read_fds);
        max_fd = server_sock;

        // Add client sockets to set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &read_fds);
                if (client_sockets[i] > max_fd) max_fd = client_sockets[i];
            }
        }

        // Select with timeout for ~60 FPS (16ms)
        struct timeval tv = {0, 1000000 / FPS};
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &tv);

        if (activity < 0 && errno != EINTR) {
            perror("select");
        }

        // New connection
        if (FD_ISSET(server_sock, &read_fds)) {
            int new_sock = accept(server_sock, NULL, NULL);
            if (new_sock >= 0) {
                fcntl(new_sock, F_SETFL, O_NONBLOCK); // Non-blocking
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (client_sockets[i] == 0) {
                        client_sockets[i] = new_sock;
                        num_clients++;
                        printf("New client connected\n");
                        break;
                    }
                }
            }
        }

        // Update physics once per "frame"
        physics();

        // Handle clients
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sock = client_sockets[i];
            if (sock > 0) {
                // Send position
                float pos[2] = {cubeX, cubeY};
                if (send(sock, pos, sizeof(float) * 2, 0) != sizeof(float) * 2) {
                    if (errno != EWOULDBLOCK && errno != EAGAIN) {
                        close(sock);
                        client_sockets[i] = 0;
                        num_clients--;
                        printf("Client disconnected\n");
                    }
                } else {
                    // Receive buffer (ignore content for now)
                    int buffer;
                    int ret = recv(sock, &buffer, sizeof(int), 0);
                    if (ret == 0 || (ret < 0 && errno != EWOULDBLOCK && errno != EAGAIN)) {
                        close(sock);
                        client_sockets[i] = 0;
                        num_clients--;
                        printf("Client disconnected\n");
                    }
                }
            }
        }
    }

    close(server_sock);
    return 0;
}
