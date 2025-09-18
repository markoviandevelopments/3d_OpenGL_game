#include "physics.h"
#include <math.h>

int checkCollision(float x, float y, float z)
{
    double dist = sqrt((x - 10.0) * (x - 10.0) + (y - 10.0) * (y - 10.0));
    if (dist < 10.0 && z < 0.5 && z > -0.5)
        return 1;
    if (dist > 9.5 && dist < 10.5 && z < 1.0 && z > 0.0)
        return 1;

    for (int i = 0; i < 3; i++)
    {
        double dx_p = x - gameState.planets[i].x;
        double dy_p = y - gameState.planets[i].y;
        double dz_p = z - gameState.planets[i].z;
        double dist_p = sqrt(dx_p * dx_p + dy_p * dy_p + dz_p * dz_p);
        if (dist_p < gameState.planets[i].radius + 0.5)
            return 1;
    }
    return 0;
}

void updatePhysics(void)
{
    float dx = 0.0f, dy = 0.0f, dz = 0.0f;

    if (gameState.useVelocity)
    {
        dx = gameState.velX * gameState.dt;
        dy = gameState.velY * gameState.dt;
        dz = gameState.velZ * gameState.dt;

        float dx1 = gameState.posX - gameState.planets[0].x;
        float dy1 = gameState.posY - gameState.planets[0].y;
        float dz1 = gameState.posZ - gameState.planets[0].z;
        float dist1 = sqrtf(dx1 * dx1 + dy1 * dy1 + dz1 * dz1);
        if (dist1 > 0.001f)
        {
            gameState.velX -= gameState.dt * gameState.gravC * dx1 / (dist1 * dist1);
            gameState.velY -= gameState.dt * gameState.gravC * dy1 / (dist1 * dist1);
            gameState.velZ -= gameState.dt * gameState.gravC * dz1 / (dist1 * dist1);
        }
    }
    else
    {
        gameState.velX = gameState.velY = gameState.velZ = 0.0f;
    }

    int collided = 1;
    if (dx != 0.0f || dy != 0.0f || dz != 0.0f || gameState.useVelocity)
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