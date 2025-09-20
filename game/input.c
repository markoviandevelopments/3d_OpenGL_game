#include <stdio.h>
#include <GL/glut.h>
#include "input.h"
#include "physics.h"
#include "draw.h"
#include "game.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

bool keyStates[256] = {false};
bool specialKeyStates[256] = {false};
float speedModifier;

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
    case 's':
    case 'a':
    case 'd':
    case 'q':
    case 'e':
        keyStates[key] = true;
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
    case 'shift':
        speedModifier = 0.005f;
        break;
    case 27: // Escape
        exit(0);
        break;
    default:
        gameState.message = 1;
        speedModifier = 0.008f;
        break;
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    if (key < 256)
    {
        keyStates[key] = false;
    }
}

void specialKeys(int key, int x, int y)
{
    if (key < 256)
    {
        specialKeyStates[key] = true;
    }
    glutPostRedisplay();
}

void specialKeysUp(int key, int x, int y)
{
    if (key < 256)
    {
        specialKeyStates[key] = false;
    }
}

void updateRotation()
{
    float rotateSpeed = gameState.rotateSpeed;
    float dt = gameState.dt;

    if (specialKeyStates[GLUT_KEY_UP])
    {
        gameState.rotateX -= rotateSpeed * dt / 0.1f;
    }
    if (specialKeyStates[GLUT_KEY_DOWN])
    {
        gameState.rotateX += rotateSpeed * dt / 0.1f;
    }
    if (specialKeyStates[GLUT_KEY_LEFT])
    {
        gameState.rotateY += rotateSpeed * dt / 0.1f;
    }
    if (specialKeyStates[GLUT_KEY_RIGHT])
    {
        gameState.rotateY -= rotateSpeed * dt / 0.1f;
    }

    if (gameState.rotateX > 400.0f)
        gameState.rotateX = 400.0f;
    if (gameState.rotateX < -400.0f)
        gameState.rotateX = -400.0f;
}

void updateMovement()
{
    float dx = 0.0f, dy = 0.0f, dz = 0.0f;
    double yaw = gameState.rotateY * M_PI / 180.0;
    double pitch = gameState.rotateX * M_PI / 180.0;
    float moveSpeed = gameState.playerSpeed * 0.1f;

    float forward = (keyStates['w'] ? 1.0f : 0.0f) - (keyStates['s'] ? 1.0f : 0.0f);
    float strafe = (keyStates['d'] ? 1.0f : 0.0f) - (keyStates['a'] ? 1.0f : 0.0f);
    float vertical = (keyStates['e'] ? 1.0f : 0.0f) - (keyStates['q'] ? 1.0f : 0.0f);

    float horizLen = sqrtf(forward * forward + strafe * strafe);
    if (horizLen > 0.0f)
    {
        forward /= horizLen;
        strafe /= horizLen;
    }

    dx += forward * moveSpeed * cos(yaw) * cos(pitch);
    dy += forward * moveSpeed * sin(yaw) * cos(pitch);
    dz += forward * moveSpeed * -sin(pitch); // Revert to -pitch if needed

    dx += strafe * moveSpeed * sin(yaw);
    dy += strafe * moveSpeed * -cos(yaw);
    dz += vertical * moveSpeed;

    if (gameState.useVelocity)
    {
        gameState.velX += gameState.speedV * dx * gameState.dt / 0.0055f;
        gameState.velY += gameState.speedV * dy * gameState.dt / 0.0055f;
        gameState.velZ += gameState.speedV * dz * gameState.dt / 0.0055f;
        dx = gameState.velX * gameState.dt;
        dy = gameState.velY * gameState.dt;
        dz = gameState.velZ * gameState.dt;
    }
    else
    {
        dx *= gameState.dt / 0.0055f;
        dy *= gameState.dt / 0.0055f;
        dz *= gameState.dt / 0.0055f;
    }

    int collided = 1;
    if (dx != 0.0f || dy != 0.0f || dz != 0.0f)
    {
        //printf("Move attempt: pos=(%.2f, %.2f, %.2f), dx=%.4f, dy=%.4f, dz=%.4f, dt=%.4f\n",gameState.posX, gameState.posY, gameState.posZ, dx, dy, dz, gameState.dt);
        for (double f = 0.0; f < 7.0; f += 1.0)
        {
            float factor = 1.0f / powf(2.0f, f);
            float testX = gameState.posX + dx * factor;
            float testY = gameState.posY + dy * factor;
            float testZ = gameState.posZ + dz * factor;
            int isCollision = checkCollision(testX, testY, testZ);
            // printf("  Step %.4f: test=(%.2f, %.2f, %.2f), collision=%d, canClip=%d\n",factor, testX, testY, testZ, isCollision, gameState.canClip);
            if (!isCollision || gameState.canClip)
            {
                gameState.posX = testX;
                gameState.posY = testY;
                gameState.posZ = testZ;
                collided = 0;
                break;
            }
        }
        if (collided)
        {
            // printf("Collision stopped movement!\n");
        }
    }

    if (collided)
    {
        gameState.velX = gameState.velY = gameState.velZ = 0.0f;
    }

    // Add gravity (moved from updatePhysics)
    if (gameState.useVelocity)
    {
        for (int i = 0; i < 3; i++)
        {
            float dx1 = gameState.posX - gameState.planets[i].x;
            float dy1 = gameState.posY - gameState.planets[i].y;
            float dz1 = gameState.posZ - gameState.planets[i].z;
            float dist1 = sqrtf(dx1 * dx1 + dy1 * dy1 + dz1 * dz1);
            if (dist1 > 0.001f)
            {
                float force = gameState.dt * gameState.gravC * (gameState.planets[i].mass / (dist1 * dist1 * dist1));
                gameState.velX -= force * dx1;
                gameState.velY -= force * dy1;
                gameState.velZ -= force * dz1;
            }
        }
    }
}