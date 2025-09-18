#ifndef GAME_H
#define GAME_H

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

#define M_PI 3.14159265358979323846f
#define MAX_CLIENTS 10
#define DRAW_DOME 1
#define RENDER_DIST 5000.0f
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000
#define AGENT_SERVER_PORT 8099
#define HEARTBEAT_INTERVAL 300
#define FPS 60
#define FRAME_INTERVAL_MS (1000 / FPS)

typedef struct
{
    float x, y;
} Agent;

typedef struct
{
    Agent agent[10];
} Agents;

typedef struct
{
    float x, y, food_type;
} Food;

typedef struct
{
    Food food[25];
} Foods;

typedef struct
{
    float food_row[10];
} FoodRow;

typedef struct
{
    FoodRow food_col[10];
} FoodGrid;

typedef struct
{
    Agents agents;
    FoodGrid foodgrid;
} AgentServerInfo;

typedef struct
{
    float x, y, z;
} Position;

typedef struct
{
    int command;
    Position position;
} ClientMessage;

typedef struct
{
    int exists;
    Position position;
} PlayerInfo;

typedef struct
{
    float browniancube[2];
    PlayerInfo playerinfo[MAX_CLIENTS];
} ServerMessage;

typedef struct
{
    float x, y, z;
    float radius, mass;
} Planet;

typedef struct
{
    float posX, posY, posZ;
    float velX, velY, velZ;
    float rotateX, rotateY;
    float rotateSpeed;
    float playerSpeed;
    float dt, speedV, gravC;
    int useVelocity, canClip;
    GLuint objList;
    float enterpriseAngle;
    float cubeX, cubeY;
    int fpsActual;
    int frameCount, lastTime, currentTime;
    float avgFrameTime;
    int lastFpsTime;
    int clientSock;
    struct sockaddr_in serverAddr;
    socklen_t addrLen;
    int sockFd;
    struct sockaddr_in agentServerAddr;
    int message;
    ClientMessage clientMessage;
    ServerMessage serverMessage;
    AgentServerInfo agentServerInfo;
    Agents agents;
    Foods foods;
    FoodGrid foodGrid;
    Planet planets[3];
    GLuint objList2;
} GameState;

extern GameState gameState;

#endif