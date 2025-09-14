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
#include <string.h>
#include <float.h>

#define MAX_CLIENTS 10
#define DRAW_DOME 0

// #define M_PI 3.14159265358979
GLuint objList = 0;

// Rotation angles for the cube
float rotateX = 0.0f;
float rotateY = -620.0f;
float rotateSpeed = 5.0f; // Degrees per key press
const int FPS = 60; // Target frames per second
const int FRAME_INTERVAL_MS = 1000 / FPS;
int fps_actual = 60;
float player_speed = 1.0f;

float posX = 5.8f;
float posY = 7.6f;
float posZ = 3.6f;

float velX = 0.0f;
float velY = 0.0f;
float velZ = 0.0f;

float dt = 0.01f;

int usevelocity = 0;
float speedV = 10.0f;
float grav_c = 100.0f;

float cubeX = 5.0f;
float cubeY = 5.0f;

int client_sock;                // Global client socket
struct sockaddr_in server_addr; // Server address for sendto
socklen_t addr_len = sizeof(server_addr);

float enterprise_angle = 90.0f;

    struct Position
{
    float x;
    float y;
    float z;
};

struct ClientMessage {
    int command;
    struct Position position;
};

struct PlayerInfo {
    int exists;
    struct Position position;
};

struct ServerMessage {
    float browniancube[2]; // x and y positions of cube on checkerboard
    struct PlayerInfo playerinfo[MAX_CLIENTS];
};

int message = 1;

struct ClientMessage clientmessage;
struct ServerMessage servermessage;

// Frame timing variables
int frame_count = 0;
int last_time = 0;
int current_time = 0;
float avg_frame_time = 0.0f;
int fps_counter = 0;
int last_fps_time = 0;

void drawCube(float x, float y, float z, int t)
{
    // Alternate between yellow and cyan based on t
    float r = t ? 0.0f : 1.0f;
    float g = t ? 0.0f : 0.30f;
    float b = 0.0f;

    glBegin(GL_QUADS);
    glColor4f(r, g, b, 0.9f);

    // Front face (normal: 0, 0, 1)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);

    // Back face (normal: 0, 0, -1)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);

    // Top face (normal: 0, 1, 0)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);

    // Bottom face (normal: 0, -1, 0)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);

    // Right face (normal: 1, 0, 0)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);

    // Left face (normal: -1, 0, 0)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);

    glEnd();
}

void drawSolidCube(float x, float y, float z, float r, float g, float b, float a)
{
    glBegin(GL_QUADS);
    glColor4f(r, g, b, a);

    // Front face (normal: 0, 0, 1)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);

    // Back face (normal: 0, 0, -1)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);

    // Top face (normal: 0, 1, 0)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);

    // Bottom face (normal: 0, -1, 0)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);

    // Right face (normal: 1, 0, 0)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);

    // Left face (normal: -1, 0, 0)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);

    glEnd();

    // Draw black edges (lighting doesn’t affect lines, so disable temporarily)
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
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
    // Connecting edges
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawSkyDome(float radius, int slices, int stacks)
{
    glDisable(GL_LIGHTING);   // Flat colors
    glDisable(GL_DEPTH_TEST); // Ensure behind everything
    glDepthMask(GL_FALSE);    // No depth writes
    glDisable(GL_BLEND);      // Solid colors
    glDisable(GL_CULL_FACE);  // Avoid winding issues

    // Colors for gradient
    float orange[3] = {1.0f, 0.4f, 0.2f}; // Horizon
    float purple[3] = {0.3f, 0.1f, 0.6f}; // Mid-sky
    float blue[3] = {0.0f, 0.0f, 0.3f};   // Zenith

    float max_theta = 2.0f * M_PI / 2.50f;

    for (int i = 0; i < stacks; ++i)
    {
        float theta1 = (float)i / stacks * max_theta; // From 0 (zenith) to max_theta (below horizon)
        float theta2 = (float)(i + 1) / stacks * max_theta;
        float h1 = cosf(theta1); // Normalized height: 1 at zenith, negative below horizon
        float h2 = cosf(theta2);

        // Normalize h to [0,1] for gradient (clamp if below 0)
        float nh1 = fmaxf(0.0f, h1); // Clip below horizon to 0
        float nh2 = fmaxf(0.0f, h2);
        nh1 = 1.0f - nh1; // Invert: 0 at zenith, 1 at/below horizon (for easier low-to-high mapping)
        nh2 = 1.0f - nh2;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j)
        { // <= for seam closure
            float phi = (float)j / slices * 2.0f * M_PI;

            // Vertex 1 (current stack)
            float x1 = radius * sinf(theta1) * cosf(phi);
            float y1 = radius * sinf(theta1) * sinf(phi);
            float z1 = radius * cosf(theta1);
            // Interp color based on normalized height (quicker transition: orange low, fast to purple/blue)
            float r1, g1, b1;
            if (nh1 < 0.2f)
            { // Orange: bottom 20%
                r1 = orange[0];
                g1 = orange[1];
                b1 = orange[2];
            }
            else if (nh1 < 0.6f)
            { // Purple: next 40%
                float t = (nh1 - 0.2f) / 0.4f;
                r1 = orange[0] + t * (purple[0] - orange[0]);
                g1 = orange[1] + t * (purple[1] - orange[1]);
                b1 = orange[2] + t * (purple[2] - orange[2]);
            }
            else
            { // Blue: top 40%
                float t = (nh1 - 0.6f) / 0.4f;
                r1 = purple[0] + t * (blue[0] - purple[0]);
                g1 = purple[1] + t * (blue[1] - purple[1]);
                b1 = purple[2] + t * (blue[2] - purple[2]);
            }
            glColor3f(r1, g1, b1);
            glVertex3f(x1, y1, z1);

            // Vertex 2 (next stack)
            float x2 = radius * sinf(theta2) * cosf(phi);
            float y2 = radius * sinf(theta2) * sinf(phi);
            float z2 = radius * cosf(theta2);
            float r2, g2, b2;
            if (nh2 < 0.2f)
            {
                r2 = orange[0];
                g2 = orange[1];
                b2 = orange[2];
            }
            else if (nh2 < 0.6f)
            {
                float t = (nh2 - 0.2f) / 0.4f;
                r2 = orange[0] + t * (purple[0] - orange[0]);
                g2 = orange[1] + t * (purple[1] - orange[1]);
                b2 = orange[2] + t * (purple[2] - orange[2]);
            }
            else
            {
                float t = (nh2 - 0.6f) / 0.4f;
                r2 = purple[0] + t * (blue[0] - purple[0]);
                g2 = purple[1] + t * (blue[1] - purple[1]);
                b2 = purple[2] + t * (blue[2] - purple[2]);
            }
            glColor3f(r2, g2, b2);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE); // Re-enable if needed elsewhere
}

void loadOBJ(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error: Cannot open OBJ file %s\n", filename);
        return;
    }

    float vertices[10000][3]; // Up to 10000 vertices
    float normals[10000][3];  // Up to 10000 normals
    int numVertices = 0;
    int numNormals = 0;
    int faces[10000][6]; // Up to 10000 faces (v1/n1, v2/n2, v3/n3)
    int numFaces = 0;

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        // Remove trailing newline
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "v ", 2) == 0)
        {
            if (numVertices >= 10000)
            {
                printf("Error: Too many vertices (max 10000)\n");
                fclose(file);
                return;
            }
            if (sscanf(line + 2, "%f %f %f", &vertices[numVertices][0], &vertices[numVertices][1], &vertices[numVertices][2]) == 3)
            {
                numVertices++;
            }
            else
            {
                printf("Error: Invalid vertex format: %s\n", line);
            }
        }
        else if (strncmp(line, "vn ", 3) == 0)
        {
            if (numNormals >= 10000)
            {
                printf("Error: Too many normals (max 10000)\n");
                fclose(file);
                return;
            }
            if (sscanf(line + 3, "%f %f %f", &normals[numNormals][0], &normals[numNormals][1], &normals[numNormals][2]) == 3)
            {
                numNormals++;
            }
            else
            {
                printf("Error: Invalid normal format: %s\n", line);
            }
        }
        else if (strncmp(line, "f ", 2) == 0)
        {
            if (numFaces >= 10000)
            {
                printf("Error: Too many faces (max 10000)\n");
                fclose(file);
                return;
            }
            int v1, n1, v2, n2, v3, n3;
            // Try parsing with vertex/normal format (f v1//n1 v2//n2 v3//n3)
            if (sscanf(line + 2, "%d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3) == 6)
            {
                // Validate indices
                if (v1 < 1 || v1 > numVertices || n1 < 1 || n1 > numNormals ||
                    v2 < 1 || v2 > numVertices || n2 < 1 || n2 > numNormals ||
                    v3 < 1 || v3 > numVertices || n3 < 1 || n3 > numNormals)
                {
                    printf("Error: Invalid face indices: %s\n", line);
                    continue;
                }
                faces[numFaces][0] = v1 - 1; // Convert to 0-based
                faces[numFaces][1] = n1 - 1;
                faces[numFaces][2] = v2 - 1;
                faces[numFaces][3] = n2 - 1;
                faces[numFaces][4] = v3 - 1;
                faces[numFaces][5] = n3 - 1;
                numFaces++;
            }
            else if (sscanf(line + 2, "%d %d %d", &v1, &v2, &v3) == 3)
            {
                // Handle faces without normals (f v1 v2 v3)
                if (v1 < 1 || v1 > numVertices || v2 < 1 || v2 > numVertices || v3 < 1 || v3 > numVertices)
                {
                    printf("Error: Invalid face indices (no normals): %s\n", line);
                    continue;
                }
                faces[numFaces][0] = v1 - 1;
                faces[numFaces][1] = 0; // Dummy normal index
                faces[numFaces][2] = v2 - 1;
                faces[numFaces][3] = 0;
                faces[numFaces][4] = v3 - 1;
                faces[numFaces][5] = 0;
                numFaces++;
            }
            else
            {
                printf("Error: Unsupported face format: %s\n", line);
            }
        }
    }
    fclose(file);

    if (numVertices == 0 || numFaces == 0)
    {
        printf("Error: Invalid OBJ file %s (no vertices or faces)\n", filename);
        return;
    }

    // Create display list
    objList = glGenLists(1);
    if (objList == 0)
    {
        printf("Error: Failed to create display list\n");
        return;
    }
    glNewList(objList, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < numFaces; i++)
    {
        int v1 = faces[i][0], n1 = faces[i][1];
        int v2 = faces[i][2], n2 = faces[i][3];
        int v3 = faces[i][4], n3 = faces[i][5];

        // If normals are present, use them; otherwise, skip (for faces without normals)
        if (n1 > 0 && n2 > 0 && n3 > 0)
        {
            glNormal3f(normals[n1][0], normals[n1][1], normals[n1][2]);
        }
        glVertex3f(vertices[v1][0], vertices[v1][1], vertices[v1][2]);

        if (n2 > 0)
        {
            glNormal3f(normals[n2][0], normals[n2][1], normals[n2][2]);
        }
        glVertex3f(vertices[v2][0], vertices[v2][1], vertices[v2][2]);

        if (n3 > 0)
        {
            glNormal3f(normals[n3][0], normals[n3][1], normals[n3][2]);
        }
        glVertex3f(vertices[v3][0], vertices[v3][1], vertices[v3][2]);
    }
    glEnd();
    glEndList();

    printf("Loaded OBJ: %d vertices, %d normals, %d faces\n", numVertices, numNormals, numFaces);
}

void renderText(float x, float y, void *font, const char *text) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));  // Pixel coords: bottom-left origin
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);  // Ignore 3D depth for HUD
    glDisable(GL_LIGHTING);    // No lighting on text
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    glutBitmapString(font, (unsigned char*)text);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

int checkCollision(float x_in, float y_in, float z_in) {
    double x = (double) x_in;
    double y = (double) y_in;
    double z = (double) z_in;

    //check collision with platform
    float dist = sqrt((x - 10.0) * (x - 10.0) + (y - 10.0) * (y - 10.0));
    if (dist < 10.0 && z < 0.5 && z > -0.5) {
        return 1;
    }
    if (dist > 9.5 && dist < 10.5 && z < 1.0 && z > 0.0) {
        return 1;
    }
    return 0;
}

void physics() {


    float dx;
    float dy;
    float dz;

    if (usevelocity) {
        dx = velX * dt;
        dy = velY * dt;
        dz = velZ * dt;

        float dx1 = posX - 10.0f;
        float dy1 = posY = 10.0f;
        float dz1 = posZ - (-1000.0f);

        float dist1 = (float) sqrt(((double) dx1) * ((double) dx1) + ((double) dy1) * ((double) dy1) + ((double) dz1) * ((double) dz1));
        
        velX -= dt * grav_c * dx1 / dist1 / dist1;
        velY -= dt * grav_c * dy1 / dist1 / dist1;
        velZ -= dt * grav_c * dz1 / dist1 / dist1;


    } else {
        dx = 0.0f;
        dy = 0.0f;
        dz = 0.0f;
        velX = 0.0f;
        velY = 0.0f;
        velZ = 0.0f;
    }

    
    //velZ -= grav_c * dt;

    int has_collided = 1;
    if (dx != 0.0f || dy != 0.0f || dz != 0.0f || usevelocity) {
        for (double f=0.0;f<7.0;f+=1.0) {
            float factor = (float) (1.0 / pow(2.0 , f));
            if (!checkCollision(posX + dx * factor, posY + dy * factor,posZ + dz * factor)) {
            posX += dx * factor;
            posY += dy * factor;
            posZ += dz * factor;
            has_collided = 0;
            break;
        }
        }
    }
    if (has_collided) {
        velX = 0.0f;
        velY = 0.0f;
        velZ = 0.0f;
    }
}

void display()
{
    current_time = glutGet(GLUT_ELAPSED_TIME);
    int frame_time = current_time - last_time;
    avg_frame_time = (avg_frame_time * frame_count + frame_time) / (frame_count + 1);
    frame_count++;
    last_time = current_time;

    if (current_time - last_fps_time >= 1000)
    {
        fps_actual = 1000 / avg_frame_time;
        //("FPS: %d | Avg Frame Time: %.2f ms | Cube Pos: (%.2f, %.2f)\n", fps_actual, avg_frame_time, cubeX, cubeY);
        avg_frame_time = 0.0f;
        frame_count = 0;
        last_fps_time = current_time;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw sky dome
    if (DRAW_DOME) {
        glPushMatrix();
        glTranslatef(posX, posY, posZ); // Center on camera
        drawSkyDome(100.0f, 20, 10);    // Radius, slices (horizontal detail), stacks (vertical detail)
        glPopMatrix();
    }

    glLoadIdentity();
    // Set camera position
    float dx1 = (float)cos(((double)rotateY) / 360.0 * 2.0 * M_PI);
    float dy1 = (float)sin(((double)rotateY) / 360.0 * 2.0 * M_PI);
    //float dz1 = (float)cos(((double)rotateX) / 360.0 * 2.0f * M_PI);
    float dz1 = -1 * rotateX / 360.0f * 2.0f * M_PI;
    gluLookAt(posX, posY, posZ,
              posX + dx1, posY + dy1, posZ + dz1,
              0.0f, 0.0f, 1.0f);

    // Set light position
    GLfloat light_position[] = {4.0f, 4.0f, 10.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Draw checkerboard
    float z = -0.5f;
    int t = 0;
    for (float x = 0.0f; x <= 20.0f; x += 1.0f)
    {
        t = (int)x % 2;
        for (float y = 0.0f; y <= 20.0f; y += 1.0f)
        {
            double dx = (double) (x - 10.0f);
            double dy = (double) (y - 10.0f);
            double dist = sqrt(dx*dx + dy*dy);
            if (dist < 10.0) {
                drawCube(x, y, z, t);
            }
            
            t = 1 - t;
        }
    }

    for (double a=0;a<2.0 * M_PI;a+=.03) {
        float x = 10.0f * ((float) cos(a)) + 10.0f;
        float y = 10.0f * ((float) sin(a)) + 10.0f;
        drawSolidCube(x,y,0.5f,1.0f,0.5f,0.0f,0.5f);
    }

    

    // Draw OBJ model
    if (objList != 0)
    {
        if(frame_count % 20 == 0) {
            enterprise_angle += 10.0f;
        }
        glPushMatrix();
        glTranslatef(12.0f, 8.0f, 1.0f);
        glScalef(0.025f, 0.025f, 0.025f);
        glRotatef(enterprise_angle, 1.0f, 0.0f, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        GLfloat obj_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat obj_shininess[] = {100.0f};
        glMaterialfv(GL_FRONT, GL_SPECULAR, obj_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, obj_shininess);
        glCallList(objList);
        glPopMatrix();
    }

    // Draw player cube
    drawSolidCube(cubeX, cubeY, 0.51f, 0.5f, 0.5f, 1.0f, 0.5f);

    for (int i=0; i<MAX_CLIENTS; i++) {
        
        float xp = servermessage.playerinfo[i].position.x;
        float yp = servermessage.playerinfo[i].position.y;
        float zp = servermessage.playerinfo[i].position.z;
        float dx = xp - posX;
        float dy = yp - posY;
        float dz = zp - posZ;
        float dist = (float) sqrt((double) (dx*dx + dy*dy + dz*dz));
        if (!servermessage.playerinfo[i].exists || dist < 0.5f) {
            continue;
        }
        //printf("dist %f\n",dist);
        if (i == 0) {
            drawSolidCube(xp, yp, zp, 1.0f, 0.2f, 1.0f, 0.5f);
        } else if (i > 0) {
            drawSolidCube(xp, yp, zp, 0.0f, 1.0f, 0.0f, 0.5f);
        }
        
    }

    // Render HUD text
    char fpsText[256];
    sprintf(fpsText, "FPS: %d | Cube: (%.1f, %.1f) | Player: (%.3f, %.3f, %.3f) | Use Velocity: %d", fps_actual, cubeX, cubeY,posX,posY,posZ,usevelocity);
    renderText(10, glutGet(GLUT_WINDOW_HEIGHT) - 30, GLUT_BITMAP_HELVETICA_18, fpsText);  // Top-left
    char fpsText2[256];
    sprintf(fpsText2,"Velocities: (%.3f, %.3f, %.3f) | Angles: (%.2f, %.2f)", velX,velY,velZ,rotateX,rotateY);
    renderText(10, glutGet(GLUT_WINDOW_HEIGHT) - 60, GLUT_BITMAP_HELVETICA_18, fpsText2);
    // Example: Static instructions
    renderText(10, 30, GLUT_BITMAP_HELVETICA_12, "WASD: Move | Arrows: Look | N/J/I/L/K: Control Cube");

    glutSwapBuffers();
    physics();
}



void timer(int value)
{
    // Receive position from server (non-blocking)
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    int ret = recvfrom(client_sock, &servermessage, sizeof(float) * 2 + (sizeof(int) + 3UL * sizeof(float)) * ((unsigned long) MAX_CLIENTS), 0, (struct sockaddr *)&from_addr, &from_len);
    if (ret < 0 && errno != EWOULDBLOCK && errno != EAGAIN)
    {
        perror("recvfrom");
    } else {
        cubeX = servermessage.browniancube[0];
        cubeY = servermessage.browniancube[1];
    }

    // Send buffer back to server
    clientmessage.command = message;
    clientmessage.position.x = posX;
    clientmessage.position.y = posY;
    clientmessage.position.z = posZ;
    sendto(client_sock, &clientmessage, sizeof(int) + 3UL * sizeof(float), 0, (struct sockaddr *)&server_addr, addr_len);
    message = 1;

    glutPostRedisplay();
    glutTimerFunc(FRAME_INTERVAL_MS, timer, 0);
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:    // Rotate up
            rotateX -= rotateSpeed;
            break;
        case GLUT_KEY_DOWN:  // Rotate down
            rotateX += rotateSpeed;
            break;
        case GLUT_KEY_LEFT:  // Rotate left
            rotateY += rotateSpeed;
            break;
        case GLUT_KEY_RIGHT: // Rotate right
            rotateY -= rotateSpeed;
            break;
        
    }

    if (rotateX > 400.0f) {rotateX = 400.0f;}
    if (rotateX < -400.0f) {rotateX = -400.0f;}
    glutPostRedisplay(); // Request redraw
}



void keyboard(unsigned char key, int x, int y)
{
    float dx = 0;
    float dy = 0;
    float dz = 0;
    if (key == 'w')
    {
        dx = player_speed * (float) 0.1 * cos(((double) rotateY) / 360.0 * M_PI);
        dy = player_speed * (float) 0.1 * sin(((double) rotateY) / 360.0 * M_PI);
        dz = player_speed * (float) 0.1 * cos(((double) rotateX) / 360.0 * M_PI);
    }
    if (key == 's')
    {
        dx = -1.0f * player_speed * (float) 0.1 * cos(((double) rotateY) / 360.0 * M_PI);
        dy = -1.0f * player_speed * (float) 0.1 * sin(((double) rotateY) / 360.0 * M_PI);
        dz = -1.0f * player_speed * (float) 0.1 * cos(((double) rotateX) / 360.0 * M_PI);
    }
    if (key == 'a')
    {
        dx = player_speed * (float) 0.1 * cos(((double) rotateY + 180.0) / 360.0 * M_PI);
        dy = player_speed * (float) 0.1 * sin(((double) rotateY + 180.0) / 360.0 * M_PI);
        dz = player_speed * (float) 0.0 * cos(((double) rotateX + 180.0) / 360.0 * M_PI);
    }
    if (key == 'd')
    {
        dx = -1.0f * player_speed * (float) 0.1 * cos(((double) rotateY + 180.0) / 360.0 * M_PI);
        dy = -1.0f * player_speed * (float) 0.1 * sin(((double) rotateY + 180.0) / 360.0 * M_PI);
        dz = -1.0f * player_speed * (float) 0.0 * cos(((double) rotateX + 180.0) / 360.0 * M_PI);
    }
    if (key == 'q')
    {
        dz = -1.0f * player_speed * 0.1f;
    }
    if (key == 'e')
    {
        dz = player_speed * 0.1f;
    }

    if (usevelocity) {
        velX += speedV * dx * dt;
        velY += speedV * dy * dt;
        velZ += speedV * dz * dt;
        dx = velX * dt;
        dy = velY * dt;
        dz = velZ * dt;
    }

    int has_collided = 1;
    if (dx != 0.0f || dy != 0.0f || dz != 0.0f) {
        for (double f=0.0;f<7.0;f+=1.0) {
            float factor = (float) (1.0 / pow(2.0 , f));
            if (!checkCollision(posX + dx * factor, posY + dy * factor,posZ + dz * factor)) {
            posX += dx * factor;
            posY += dy * factor;
            posZ += dz * factor;
            display();
            has_collided = 0;
            break;
        }
        }
    }
    if (has_collided) {
        velX = 0.0f;
        velY = 0.0f;
        velZ = 0.0f;
    }


    if (key == 'm')
    {
        usevelocity = 1 - usevelocity;
    }
    if (key == 'n')
    {
        message = 2;
    }
    else if (key == 'j')
    {
        message = 3;
    }
    else if (key == 'i')
    {
        message = 4;
    }
    else if (key == 'l')
    {
        message = 5;
    }
    else if (key == 'k')
    {
        message = 6;
    }
    else
    {
        message = 1;
    }
    if (key == 27)
    { // Escape key to exit
        exit(0);
    }
}

void init()
{
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    GLfloat material_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat material_shininess[] = {50.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0f, 1.0f, 0.1f, 500.0f);
    glMatrixMode(GL_MODELVIEW);

    // Load OBJ model after OpenGL setup
    loadOBJ("assets/eprod.obj");
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / h, 0.1f, 500.0f);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    // Create UDP socket
    client_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_sock < 0)
    {
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
    clientmessage.command = 1;
    clientmessage.position.x = posX;
    clientmessage.position.y = posY;
    clientmessage.position.z = posZ;
    sendto(client_sock, &clientmessage, sizeof(int) + 3UL * sizeof(float), 0, (struct sockaddr *)&server_addr, addr_len);

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

    if (objList != 0)
    {
        glDeleteLists(objList, 1);
    }

    close(client_sock);
    return 0;
}