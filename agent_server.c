#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 8099
#define CLIENT_PORT 8098
#define CLIENT_IP "192.168.1.126"
#define GRID_SIZE 10.0
#define STEP_SIZE 0.5

typedef struct {
    float x;
    float y;
} Agent;

void initialize_agent(Agent *agent) {
    agent->x = GRID_SIZE / 2.0;
    agent->y = GRID_SIZE / 2.0;
}

void update_position(Agent *agent) {
    float dx = ((float)rand() / RAND_MAX - 0.5) * 2 * STEP_SIZE;
    float dy = ((float)rand() / RAND_MAX - 0.5) * 2 * STEP_SIZE;
    
    agent->x += dx;
    agent->y += dy;
    
    if (agent->x < 0) agent->x = 0;
    if (agent->x > GRID_SIZE) agent->x = GRID_SIZE;
    if (agent->y < 0) agent->y = 0;
    if (agent->y > GRID_SIZE) agent->y = GRID_SIZE;
}

int main() {
    srand(time(NULL));
    Agent agent;
    initialize_agent(&agent);
    
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(CLIENT_PORT);
    if (inet_pton(AF_INET, CLIENT_IP, &client_addr.sin_addr) <= 0) {
        perror("Invalid client address");
        exit(EXIT_FAILURE);
    }
    
    printf("Server sending to %s:%d...\n", CLIENT_IP, CLIENT_PORT);
    
    while (1) {
        update_position(&agent);
        sendto(sockfd, &agent, sizeof(Agent), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        printf("Sent position: (%.2f, %.2f)\n", agent.x, agent.y);
        sleep(1);
    }
    
    close(sockfd);
    return 0;
}