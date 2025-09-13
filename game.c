#include <GL/glut.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <math.h>
#include <time.h>

#define NUM_PARTICLES 150
//#define M_PI 3.14159265358979

// Rotation angles for the cube
float rotateX = 920.0f;
float rotateY = -620.0f;
float rotateSpeed = 5.0f; // Degrees per key press
const int FPS = 30; // Target frames per second
const int FRAME_INTERVAL_MS = 1000 / FPS; // 50ms for 20 FPS

float posX = -5.8f;
float posY = -7.6f;
float posZ = 3.6f;

float cubeX = 1.0f;
float cubeY = 1.0f;

int client_sock; // Global client socket
struct sockaddr_in server_addr; // Server address for sendto
socklen_t addr_len = sizeof(server_addr);

int message = 1;

// Frame timing variables
int frame_count = 0;
int last_time = 0;
int current_time = 0;
float avg_frame_time = 0.0f;
int fps_counter = 0;
int last_fps_time = 0;

typedef struct
{
    float x, y, z;    // Position
    float vx, vy, vz; // Velocity
    float life;       // Lifetime (0.0 to 1.0)
} Particle;

Particle particles[NUM_PARTICLES];

void drawCube(float x, float y, float z, int t) {

    float a = 0.0f;

    if (t) {
        a = 1.0f - a;
    }
    glBegin(GL_QUADS);
    // Front face (red)
    glColor3f(1.0f, 0.0f, 0.0f + a);
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    // Back face (green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    // Top face (blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    // Bottom face (yellow)
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    // Right face (cyan)
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    // Left face (magenta)
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    glEnd();
}

void drawSolidCube(float x, float y, float z, float r, float g, float b, float a)
{

    glBegin(GL_QUADS);
    glColor4f(r,g,b,a);
    // Front face
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    // Back face
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    // Top face
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    // Bottom face
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    // Right face
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    // Left face
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    glEnd();


    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f); // Black edges for contrast
    // Front face edges
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    
    // Back face edges
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    
    // Connecting edges (front to back)
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    glEnd();
}




void display() {

    current_time = glutGet(GLUT_ELAPSED_TIME);
    int frame_time = current_time - last_time;
    avg_frame_time = (avg_frame_time * frame_count + frame_time) / (frame_count + 1);
    frame_count++;
    last_time = current_time;

    // Print FPS and frame time every second
    if (current_time - last_fps_time >= 1000) {
        int actual_fps = 1000 / avg_frame_time;
        printf("FPS: %d | Avg Frame Time: %.2f ms | Cube Pos: (%.2f, %.2f)\n", actual_fps, avg_frame_time, cubeX, cubeY);
        avg_frame_time = 0.0f;
        frame_count = 0;
        last_fps_time = current_time;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    // Set camera position
    float dx1 = (float) cos(((double) rotateY) / 360.0 * M_PI);
    float dy1 = (float) sin(((double) rotateY) / 360.0 * M_PI);
    float dz1 = (float) cos(((double) rotateX) / 360.0 * M_PI);
    gluLookAt(posX,posY, posZ,// Eye position
              posX + dx1, posY + dy1, posZ + dz1,  // Look at origin
              0.0f, 0.0f, 1.0f); // Up vector
    // Apply rotation
    //glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
    //glRotatef(rotateY, 0.0f, 1.0f, 0.0f);

    float z=0.0f;
    int t=0;

    

    for (float x=0.0f;x<=8.0f;x+=1.0f) {
        for (float y=0.0f;y<=8.0f;y+=1.0f){
            drawCube(x, y, z, t);
            t = 1 - t;
            
        }
        //t = 1 - t;
    }

    glDepthMask(GL_FALSE);
    glEnable(GL_POINT_SMOOTH); // Smooth points
    glPointSize(3.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    drawSolidCube(cubeX,cubeY,1.05f,0.5f,0.5f,1.0f, 0.5f);
    glDepthMask(GL_TRUE); // Re-enable depth writing

    glutSwapBuffers();
    
}

void timer(int value) {
    // Receive position from server (non-blocking)
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    float pos[2];
    int ret = recvfrom(client_sock, pos, sizeof(float) * 2, 0, (struct sockaddr*)&from_addr, &from_len);
    if (ret == sizeof(float) * 2) {
        cubeX = pos[0];
        cubeY = pos[1];
    } else if (ret < 0 && errno != EWOULDBLOCK && errno != EAGAIN) {
        perror("recvfrom");
    }

    // Send buffer back to server
    int buffer = message;
    sendto(client_sock, &buffer, sizeof(int), 0, (struct sockaddr*)&server_addr, addr_len);
    message = 1;

    // Update particles
    for (int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].x += particles[i].vx;
        particles[i].y += particles[i].vy;
        particles[i].z += particles[i].vz;
        particles[i].life -= 0.005f;  // Fade out slowly

        // Respawn if dead or out of bounds
        if (particles[i].life <= 0.0f || particles[i].x < 0 || particles[i].x > 8 || particles[i].y < 0 || particles[i].y > 8 || particles[i].z < 0.5f || particles[i].z > 3.0f) {
            particles[i].x = (rand() % 80) / 10.0f;
            particles[i].y = (rand() % 80) / 10.0f;
            particles[i].z = (rand() % 20) / 10.0f + 0.5f;
            particles[i].vx = ((rand() % 10) - 5) / 50.0f;
            particles[i].vy = ((rand() % 10) - 5) / 50.0f;
            particles[i].vz = ((rand() % 5) - 2.5f) / 100.0f;
            particles[i].life = 1.0f;
        }
    }

    glutPostRedisplay(); // Request redraw
    glutTimerFunc(FRAME_INTERVAL_MS, timer, 0); // Schedule next timer
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:    // Rotate up
            rotateX += rotateSpeed;
            break;
        case GLUT_KEY_DOWN:  // Rotate down
            rotateX -= rotateSpeed;
            break;
        case GLUT_KEY_LEFT:  // Rotate left
            rotateY += rotateSpeed;
            break;
        case GLUT_KEY_RIGHT: // Rotate right
            rotateY -= rotateSpeed;
            break;
        
    }
    glutPostRedisplay(); // Request redraw
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'w') {
        posX += 0.1f;
        display();
    }
    if (key == 's') {
        posX -= 0.1f;
        display();
    }
    if (key == 'a') {
        posY -= 0.1f;
        display();
    }
    if (key == 'd') {
        posY += 0.1f;
        display();
    }
    if (key == 'q') {
        posZ -= 0.1f;
        display();
    }
    if (key == 'e') {
        posZ += 0.1f;
        display();
    }
    if (key == 'm') {
        printf("Rotate X: %f\tRotate Y: %f\tX: %f\tY: %f\tZ: %f\n",rotateX,rotateY,posX,posY,posZ);
    }
    if (key == 'n') {
        message = 2;
    } else if(key == 'j') {
        message = 3;
    } else if(key == 'i') {
        message = 4;
    } else if(key == 'l') {
        message = 5;
    } else if(key == 'k') {
        message = 6;
    } else {
        message = 1;
    }
    if (key == 27) { // Escape key to exit
        exit(0);
    }
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glEnable(GL_BLEND); // Enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set blend function
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0f, 1.0f, 0.1f, 100.0f); // FOV, aspect, near, far
    glMatrixMode(GL_MODELVIEW);
    srand(time(NULL)); // Seed random
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        particles[i].x = (rand() % 80) / 10.0f;        // Random x in 0-8
        particles[i].y = (rand() % 80) / 10.0f;        // Random y in 0-8
        particles[i].z = (rand() % 20) / 10.0f + 0.5f; // Slightly above ground
        particles[i].vx = ((rand() % 10) - 5) / 50.0f; // Small random velocity
        particles[i].vy = ((rand() % 10) - 5) / 50.0f;
        particles[i].vz = ((rand() % 5) - 2.5f) / 100.0f; // Gentle up/down
        particles[i].life = (rand() % 100) / 100.0f;      // Random start life
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w/h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    // Create UDP socket
    client_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_sock < 0) {
        perror("socket");
        return 1;
    }

    // Set non-blocking
    fcntl(client_sock, F_SETFL, O_NONBLOCK);

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8042);
    server_addr.sin_addr.s_addr = inet_addr("192.168.1.126"); // Your server IP

    // Send initial buffer to register with server
    int buffer = 1;
    sendto(client_sock, &buffer, sizeof(int), 0, (struct sockaddr*)&server_addr, addr_len);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Willohlandia");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();

    close(client_sock);
    return 0;
}
