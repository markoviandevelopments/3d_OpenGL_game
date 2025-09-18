#include "input.h"
#include "physics.h"
#include "draw.h"

// Ensure M_PI is defined if not included from game.h
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

void keyboard(unsigned char key, int x, int y)
{
    float dx = 0.0f, dy = 0.0f, dz = 0.0f;
    double yaw = gameState.rotateY * M_PI / 180.0;
    double pitch = gameState.rotateX * M_PI / 180.0;
    float moveSpeed = gameState.playerSpeed * 0.1f;

    switch (key)
    {
    case 'w':
        dx = moveSpeed * cos(yaw) * cos(pitch);
        dy = moveSpeed * sin(yaw) * cos(pitch);
        dz = -moveSpeed * pitch;
        break;
    case 's':
        dx = -moveSpeed * cos(yaw) * cos(pitch);
        dy = -moveSpeed * sin(yaw) * cos(pitch);
        dz = moveSpeed * pitch;
        break;
    case 'a':
        dx = moveSpeed * -sin(yaw);
        dy = moveSpeed * cos(yaw);
        break;
    case 'd':
        dx = moveSpeed * sin(yaw);
        dy = moveSpeed * -cos(yaw);
        break;
    case 'q':
        dz = -moveSpeed;
        break;
    case 'e':
        dz = moveSpeed;
        break;
    case 'm':
        gameState.useVelocity = 1 - gameState.useVelocity;
        break;
    case 'c':
        gameState.canClip = 1 - gameState.canClip;
        break;
    case 'n':
        gameState.message = 2;
        break;
    case 'j':
        gameState.message = 3;
        break;
    case 'i':
        gameState.message = 4;
        break;
    case 'l':
        gameState.message = 5;
        break;
    case 'k':
        gameState.message = 6;
        break;
    case 27: // Escape
        exit(0);
        break;
    default:
        gameState.message = 1;
        break;
    }

    if (gameState.useVelocity)
    {
        gameState.velX += gameState.speedV * dx * gameState.dt;
        gameState.velY += gameState.speedV * dy * gameState.dt;
        gameState.velZ += gameState.speedV * dz * gameState.dt;
        dx = gameState.velX * gameState.dt;
        dy = gameState.velY * gameState.dt;
        dz = gameState.velZ * gameState.dt;
    }

    int collided = 1;
    if (dx != 0.0f || dy != 0.0f || dz != 0.0f)
    {
        for (double f = 0.0; f < 7.0; f += 1.0)
        {
            float factor = 1.0f / powf(2.0f, f);
            if (!checkCollision(gameState.posX + dx * factor, gameState.posY + dy * factor, gameState.posZ + dz * factor) || gameState.canClip)
            {
                gameState.posX += dx * factor;
                gameState.posY += dy * factor;
                gameState.posZ += dz * factor;
                collided = 0;
                break;
            }
        }
    }

    if (collided)
    {
        gameState.velX = gameState.velY = gameState.velZ = 0.0f;
    }

    display(); // Immediate redraw for responsiveness
}

void specialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        gameState.rotateX -= gameState.rotateSpeed;
        break;
    case GLUT_KEY_DOWN:
        gameState.rotateX += gameState.rotateSpeed;
        break;
    case GLUT_KEY_LEFT:
        gameState.rotateY += gameState.rotateSpeed;
        break;
    case GLUT_KEY_RIGHT:
        gameState.rotateY -= gameState.rotateSpeed;
        break;
    }
    if (gameState.rotateX > 400.0f)
        gameState.rotateX = 400.0f;
    if (gameState.rotateX < -400.0f)
        gameState.rotateX = -400.0f;
    glutPostRedisplay();
}