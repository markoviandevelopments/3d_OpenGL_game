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
#include <time.h> // For rand() and time()

#define PORT 8042
#define MAX_CLIENTS 10
#define FPS 3
#define CLIENT_TIMEOUT_SEC 5 // Timeout inactive clients after 5 seconds

float cubeX = 1.0f;
float cubeY = 1.0f;
float speed = 0.05f;

int message = 1;
int prev_message = 1;


// Client tracking struct
struct Client {
    struct sockaddr_in addr;
    time_t last_seen;
};

struct Client clients[MAX_CLIENTS];
int num_clients = 0;



void physics() {
    

    if (message == 2) {
        speed = 0.5f;
    } else {
        speed = 0.01f;
    }

    float dx = (((float) (rand() % 1000)) / 1000.0f - 0.5f) * 2.0f * speed;
    float dy = (((float) (rand() % 1000)) / 1000.0f - 0.5f) * 2.0f * speed;

    if (message == 3) {
        dx = -0.1f;
    }
    if (message == 4) {
        dy = 0.1f;
    }
    if (message == 5) {
        dx = 0.1f;
    }
    if (message == 6) {
        dy = -0.1f;
    }
    
    cubeX += dx;
    cubeY += dy;

    if (cubeX > 8.0f) cubeX = 8.0f;
    if (cubeY > 8.0f) cubeY = 8.0f;
    if (cubeX < 0.0f) cubeX = 0.0f;
    if (cubeY < 0.0f) cubeY = 0.0f;
}

// Check if client address already exists
int client_exists(struct sockaddr_in *new_addr) {
    for (int i = 0; i < num_clients; i++) {
        if (memcmp(&clients[i].addr, new_addr, sizeof(struct sockaddr_in)) == 0) {
            return i; // Return index if found
        }
    }
    return -1;
}

int main() {
    int server_sock;
    struct sockaddr_in addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    fd_set read_fds;
    int max_fd;

    srand(time(NULL)); // Seed rand for physics

    // Create UDP socket
    server_sock = socket(AF_INET, SOCK_DGRAM, 0);
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

    printf("UDP Server listening on port %d\n", PORT);

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(server_sock, &read_fds);
        max_fd = server_sock;

        // Select with timeout for ~60 FPS (16ms)
        struct timeval tv = {0, 1000000 / FPS};
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &tv);

        if (activity < 0 && errno != EINTR) {
            perror("select");
        }

        // Receive from clients (to discover/update them)
        if (FD_ISSET(server_sock, &read_fds)) {
            int buffer;
            message = buffer;
            int ret = recvfrom(server_sock, &buffer, sizeof(int), 0, (struct sockaddr*)&client_addr, &addr_len);
            if (ret == sizeof(int)) {
                int idx = client_exists(&client_addr);
                if (idx == -1 && num_clients < MAX_CLIENTS) {
                    // New client
                    clients[num_clients].addr = client_addr;
                    clients[num_clients].last_seen = time(NULL);
                    num_clients++;
                    printf("New client from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                } else if (idx != -1) {
                    // Update existing
                    clients[idx].last_seen = time(NULL);
                }
            }
        }

        // Update physics once per "frame"
        physics();

        // Send position to all known clients
        float pos[2] = {cubeX, cubeY};
        for (int i = 0; i < num_clients; i++) {
            sendto(server_sock, pos, sizeof(float) * 2, 0, (struct sockaddr*)&clients[i].addr, sizeof(struct sockaddr_in));
        }

        // Timeout inactive clients
        time_t now = time(NULL);
        for (int i = 0; i < num_clients; i++) {
            if (now - clients[i].last_seen > CLIENT_TIMEOUT_SEC) {
                printf("Timed out client %s:%d\n", inet_ntoa(clients[i].addr.sin_addr), ntohs(clients[i].addr.sin_port));
                // Remove by shifting
                for (int j = i; j < num_clients - 1; j++) {
                    clients[j] = clients[j + 1];
                }
                num_clients--;
                i--; // Re-check index
            }
        }
    }

    close(server_sock);
    return 0;
}