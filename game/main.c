#include "game.h"
#include "init.h"
#include "draw.h"
#include "network.h"
#include "input.h"
#include "physics.h"
#include "utils.h"

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
    .canClip = 0,
    .speedV = 50.0f,
    .dt = 0.01f,
    .cubeX = 5.0f,
    .cubeY = 5.0f,
    .message = 1};

int main(int argc, char **argv)
{
    initNetwork();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("Willohlandia");
    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();

    if (gameState.objList != 0)
        glDeleteLists(gameState.objList, 1);
    close(gameState.clientSock);
    close(gameState.sockFd);
    return 0;
}