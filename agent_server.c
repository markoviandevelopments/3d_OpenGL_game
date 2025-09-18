#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define SERVER_PORT 8099
#define MAX_CLIENTS 10
#define CLIENT_TIMEOUT_SEC 5
#define GRID_SIZE 10.0
#define STEP_SIZE 0.5

typedef struct {
    float x;
    float y;
    float r;
    float g;
    float b;
} Agent;

typedef struct {
    Agent agent[10];
} Agents;

typedef struct {
    float food_row[10];
} FoodRow;

typedef struct {
    FoodRow food_col[10];
} FoodGrid;

typedef struct {
    float x;
    float y;
    float food_type;
} Food;

typedef struct {
    Food food[25];
} Foods;

typedef struct {
    Agents agents;
    FoodGrid foodgrid;
} AgentServerInfo;

// Client tracking struct
struct Client {
    struct sockaddr_in addr;
    time_t last_seen;
};

void initialize_row(FoodRow *foodrow) {
    for (int i=0;i<10;i++) {
        (*foodrow).food_row[i] = ((float) (rand() % 100)) / 100.0f;
    }
}

void initialize_grid(FoodGrid *foodgrid) {
    for (int i=0;i<10;i++) {
        FoodRow foodrow;
        initialize_row(&foodrow);
        (*foodgrid).food_col[i] = foodrow;
    }
}

struct Client clients[MAX_CLIENTS];
int num_clients = 0;

void initialize_agent(Agent *agent) {
    (*agent).x = ((float) (rand() % 1000)) / 1000.0f * GRID_SIZE;
    (*agent).y = ((float) (rand() % 1000)) / 1000.0f * GRID_SIZE;
    (*agent).r = ((float) (rand() % 1000)) / 1000.0f;
    (*agent).g = ((float) (rand() % 1000)) / 1000.0f;
    (*agent).b = ((float) (rand() % 1000)) / 1000.0f;
}

void initialize_food(Food *food) {
    (*food).x = GRID_SIZE / 2.0;
    (*food).y = GRID_SIZE / 2.0;
}

void update_position(Agent *agent) {
    float dx = ((float)rand() / RAND_MAX - 0.5) * 2 * STEP_SIZE;
    float dy = ((float)rand() / RAND_MAX - 0.5) * 2 * STEP_SIZE;
    
    (*agent).x += dx;
    (*agent).y += dy;
    
    if ((*agent).x < 0) (*agent).x = 0;
    if ((*agent).x > GRID_SIZE) (*agent).x = GRID_SIZE;
    if ((*agent).y < 0) (*agent).y = 0;
    if ((*agent).y > GRID_SIZE) (*agent).y = GRID_SIZE;
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
    srand(time(NULL));
    Agent agent;
    Agents agents;
    Food food;
    Foods foods;
    FoodRow foodrow;
    FoodGrid foodgrid;
    AgentServerInfo agentserverinfo;

    initialize_grid(&foodgrid);
    agentserverinfo.foodgrid = foodgrid;

    for (int i=0; i<10;i++) {
        initialize_agent(&agent);
        agents.agent[i] = agent;
    }
    initialize_agent(&agent);
    for (int i=0; i<25; i++) {
        initialize_food(&food);
        foods.food[i] = food;
    }
    initialize_food(&agent);
    
    int sockfd;
    struct sockaddr_in server_addr;
    fd_set read_fds;
    int max_fd;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Set non-blocking
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Agent server listening on port %d...\n", SERVER_PORT);
    
    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        max_fd = sockfd;
        
        // Select with 1-second timeout for updates
        struct timeval tv = {1, 0};
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &tv);
        
        if (activity < 0 && errno != EINTR) {
            perror("select");
        }
        
        // Handle incoming registrations/heartbeats
        if (FD_ISSET(sockfd, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            char buffer[1024]; // Dummy buffer for any message
            ssize_t bytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
            if (bytes >= 0) {
                int idx = client_exists(&client_addr);
                if (idx == -1 && num_clients < MAX_CLIENTS) {
                    // New client
                    clients[num_clients].addr = client_addr;
                    clients[num_clients].last_seen = time(NULL);
                    num_clients++;
                    printf("New client registered: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                } else if (idx != -1) {
                    // Update existing
                    clients[idx].last_seen = time(NULL);
                }
            }
        }
        
        // Update and broadcast agent position (every ~1 sec due to timeout)
        for (int i=0;i<10;i++) {
            update_position(&agents.agent[i]);
        }

        agentserverinfo.agents = agents;
        
        printf("Sent position\n");
        for (int i = 0; i < num_clients; i++) {
            sendto(sockfd, &agentserverinfo, sizeof(AgentServerInfo), 0, (struct sockaddr *)&clients[i].addr, sizeof(struct sockaddr_in));
        }
        
        // Timeout inactive clients
        time_t now = time(NULL);
        for (int i = 0; i < num_clients; i++) {
            if (now - clients[i].last_seen > CLIENT_TIMEOUT_SEC) {
                printf("Timed out client: %s:%d\n", inet_ntoa(clients[i].addr.sin_addr), ntohs(clients[i].addr.sin_port));
                // Remove by shifting
                for (int j = i; j < num_clients - 1; j++) {
                    clients[j] = clients[j + 1];
                }
                num_clients--;
                i--; // Re-check index
            }
        }
    }
    
    close(sockfd);
    return 0;
}