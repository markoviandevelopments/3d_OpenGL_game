#include <GL/glut.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#define AGENT_SERVER_PORT 8099
#define SERVER_IP "192.168.1.126"
#define HEARTBEAT_INTERVAL 300  // ~5 sec at 60 FPS
#define GRID_SIZE 10.0f
#define WINDOW_SIZE 800

typedef struct {
    float x;
    float y;
} Agent;

typedef struct {
    Agent agent[10];
} Agents;

int sockfd;
struct sockaddr_in agent_server_addr;
Agents agents;  // Current agent positions

void draw_checkered_grid() {
    int t = 0;
    for (float x = 0.0f; x < GRID_SIZE; x += 1.0f) {
        t = (int)x % 2;
        for (float y = 0.0f; y < GRID_SIZE; y += 1.0f) {
            if (t) {
                glColor3f(0.0f, 0.0f, 0.0f);  // Black
            } else {
                glColor3f(1.0f, 1.0f, 1.0f);  // White
            }
            glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + 1.0f, y);
            glVertex2f(x + 1.0f, y + 1.0f);
            glVertex2f(x, y + 1.0f);
            glEnd();
            t = 1 - t;
        }
    }
}

void draw_agents() {
    glColor3f(1.0f, 0.0f, 0.0f);  // Red
    for (int i = 0; i < 10; i++) {
        glBegin(GL_QUADS);
        glVertex2f(agents.agent[i].x - 0.2f, agents.agent[i].y - 0.2f);
        glVertex2f(agents.agent[i].x + 0.2f, agents.agent[i].y - 0.2f);
        glVertex2f(agents.agent[i].x + 0.2f, agents.agent[i].y + 0.2f);
        glVertex2f(agents.agent[i].x - 0.2f, agents.agent[i].y + 0.2f);
        glEnd();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    draw_checkered_grid();
    draw_agents();
    glutSwapBuffers();
}

void timer(int value) {
    // Heartbeat
    static int heartbeat_counter = 0;
    heartbeat_counter++;
    if (heartbeat_counter >= HEARTBEAT_INTERVAL) {
        int dummy = 1;
        if (sendto(sockfd, &dummy, sizeof(int), 0, (struct sockaddr *)&agent_server_addr, sizeof(agent_server_addr)) < 0) {
            perror("Heartbeat send failed");
        }
        heartbeat_counter = 0;
    }

    // Receive agents
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
    ssize_t bytes_received = recvfrom(sockfd, &agents, sizeof(Agents), 0, (struct sockaddr *)&sender_addr, &addr_len);
    if (bytes_received < 0) {
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            perror("recvfrom");
        }
    } else if (bytes_received != sizeof(Agents)) {
        printf("Incomplete data received\n");
    }

    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);  // ~60 FPS
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) {  // Esc to quit
        exit(0);
    }
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0f, GRID_SIZE, 0.0f, GRID_SIZE);  // 2D view [0,10]x[0,10]
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
    // Initialize agents to center (fallback)
    for (int i = 0; i < 10; i++) {
        agents.agent[i].x = GRID_SIZE / 2.0f;
        agents.agent[i].y = GRID_SIZE / 2.0f;
    }

    // Socket setup
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    agent_server_addr.sin_family = AF_INET;
    agent_server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    agent_server_addr.sin_port = htons(AGENT_SERVER_PORT);

    // Bind to dynamic port
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(0);
    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Register with server
    int dummy = 1;
    if (sendto(sockfd, &dummy, sizeof(int), 0, (struct sockaddr *)&agent_server_addr, sizeof(agent_server_addr)) < 0) {
        perror("Registration send failed");
        exit(EXIT_FAILURE);
    }
    printf("Registered with agent server\n");

    // GLUT setup
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    glutCreateWindow("Agent Viewer");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();

    close(sockfd);
    return 0;
}
