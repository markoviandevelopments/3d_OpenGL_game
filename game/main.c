#include "game.h"
#include "init.h"
#include "draw.h"
#include "network.h"
#include "input.h"
#include "physics.h"
#include "utils.h"
#include <unistd.h> // For close()

GameState gameState = {
    .posX = 5.8f,
    .posY = 7.6f,
    .posZ = 3.6f,
    .playerSpeed = 3.0f,
    .rotateX = 0.0f,
    .rotateY = -620.0f,
    .rotateSpeed = 5.0f,
    .velX = 0.0f,
    .velY = 0.0f,
    .velZ = 0.0f,
    .useVelocity = 0,
    .canClip = 1,
    .speedV = 50.0f,
    .dt = 0.01f,
    .cubeX = 5.0f,
    .cubeY = 5.0f,
    .message = 1,
    .objList = 0,
    .objList2 = 0,
    .avatarList = 0,
    .avatarTexture = 0,
    .clientSock = -1,
    .sockFd = -1,
    .enterpriseAngle = 0.0f,
    .fpsActual = 0,
    .frameCount = 0,
    .lastTime = 0,
    .currentTime = 0,
    .avgFrameTime = 0.0f,
    .lastFpsTime = 0,
    .serverAddr = {0},
    .agentServerAddr = {0},
    .addrLen = 0,
    .clientMessage = {0},
    .serverMessage = {
        .browniancube = {0.0f, 0.0f},
        .playerinfo = {{0}} // Initializes all MAX_CLIENTS PlayerInfo structs
    },
    .agentServerInfo = {.agents = {
                            .agent = {{0}} // Initializes all 10 Agent structs
                        },
                        .foodgrid = {.food_col = {{0}, {0}, {0}, {0}, {0}, // Explicitly initialize each FoodRow
                                                  {0},
                                                  {0},
                                                  {0},
                                                  {0},
                                                  {0}}}},
    .agents = {
        .agent = {{0}} // Initializes all 10 Agent structs
    },
    .foods = {
        .food = {{0}} // Initializes all 25 Food structs
    },
    .foodGrid = {.food_col = {{0}, {0}, {0}, {0}, {0}, // Explicitly initialize each FoodRow
                              {0},
                              {0},
                              {0},
                              {0},
                              {0}}},
    .planets = {{0}, {0}, {0}},
    .gravC = 1.0f};

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("Willohlandia");
    initGL();
    initNetwork();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();

    // Cleanup
    if (gameState.objList != 0)
        glDeleteLists(gameState.objList, 1);
    if (gameState.objList2 != 0)
        glDeleteLists(gameState.objList2, 1);
    if (gameState.avatarList != 0)
        glDeleteLists(gameState.avatarList, 1);
    if (gameState.avatarTexture != 0)
        glDeleteTextures(1, &gameState.avatarTexture);
    if (gameState.clientSock >= 0)
        close(gameState.clientSock);
    if (gameState.sockFd >= 0)
        close(gameState.sockFd);
    return 0;
}