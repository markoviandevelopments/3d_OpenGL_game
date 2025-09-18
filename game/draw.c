#include "draw.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

// Ensure M_PI is defined if not included from game.h
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

void drawCubeBase(float x, float y, float z, float r, float g, float b, float a, int drawEdges)
{
    glBegin(GL_QUADS);
    glColor4f(r, g, b, a);

    // Front face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);

    // Back face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);

    // Top face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);

    // Bottom face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);

    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
    glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);

    // Left face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
    glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
    glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);

    glEnd();

    if (drawEdges)
    {
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
        glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
        glVertex3f(0.5f + x, -0.5f + y, 0.5f + z);
        glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
        glVertex3f(0.5f + x, 0.5f + y, 0.5f + z);
        glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
        glVertex3f(-0.5f + x, 0.5f + y, 0.5f + z);
        glVertex3f(-0.5f + x, -0.5f + y, 0.5f + z);
        glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
        glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
        glVertex3f(0.5f + x, -0.5f + y, -0.5f + z);
        glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
        glVertex3f(0.5f + x, 0.5f + y, -0.5f + z);
        glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
        glVertex3f(-0.5f + x, 0.5f + y, -0.5f + z);
        glVertex3f(-0.5f + x, -0.5f + y, -0.5f + z);
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
}

void drawCubeBaseWidthSpec(float x, float y, float z, float r, float g, float b, float a, int drawEdges, float width)
{
    glBegin(GL_QUADS);
    glColor4f(r, g, b, a);

    // Front face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f - width + x, 0.0f - width + y, width + z);
    glVertex3f(width + x, 0.0f - width + y, width + z);
    glVertex3f(width + x, width + y, width + z);
    glVertex3f(0.0f - width + x, width + y, width + z);

    // Back face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.0f - width + x, 0.0f - width + y, 0.0f - width + z);
    glVertex3f(0.0f - width + x, width + y, 0.0f - width + z);
    glVertex3f(width + x, width + y, 0.0f - width + z);
    glVertex3f(width + x, 0.0f - width + y, 0.0f - width + z);

    // Top face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f - width + x, width + y, 0.0f - width + z);
    glVertex3f(0.0f - width + x, width + y, width + z);
    glVertex3f(width + x, width + y, width + z);
    glVertex3f(width + x, width + y, 0.0f - width + z);

    // Bottom face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f - width + x, 0.0f - width + y, 0.0f - width + z);
    glVertex3f(width + x, 0.0f - width + y, 0.0f - width + z);
    glVertex3f(width + x, 0.0f - width + y, width + z);
    glVertex3f(0.0f - width + x, 0.0f - width + y, width + z);

    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(width + x, 0.0f - width + y, 0.0f - width + z);
    glVertex3f(width + x, width + y, 0.0f - width + z);
    glVertex3f(width + x, width + y, width + z);
    glVertex3f(width + x, 0.0f - width + y, width + z);

    // Left face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f - width + x, 0.0f - width + y, 0.0f - width + z);
    glVertex3f(0.0f - width + x, 0.0f - width + y, width + z);
    glVertex3f(0.0f - width + x, width + y, width + z);
    glVertex3f(0.0f - width + x, width + y, 0.0f - width + z);

    glEnd();

    if (drawEdges)
    {
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f - width + x, 0.0f - width + y, width + z);
        glVertex3f(width + x, 0.0f - width + y, width + z);
        glVertex3f(width + x, 0.0f - width + y, width + z);
        glVertex3f(width + x, width + y, width + z);
        glVertex3f(width + x, width + y, width + z);
        glVertex3f(0.0f - width + x, width + y, width + z);
        glVertex3f(0.0f - width + x, width + y, width + z);
        glVertex3f(0.0f - width + x, 0.0f - width + y, width + z);
        glVertex3f(0.0f - width + x, 0.0f - width + y, 0.0f - width + z);
        glVertex3f(width + x, 0.0f - width + y, 0.0f - width + z);
        glVertex3f(width + x, 0.0f - width + y, 0.0f - width + z);
        glVertex3f(width + x, width + y, 0.0f - width + z);
        glVertex3f(width + x, width + y, 0.0f - width + z);
        glVertex3f(0.0f - width + x, width + y, 0.0f - width + z);
        glVertex3f(0.0f - width + x, width + y, 0.0f - width + z);
        glVertex3f(0.0f - width + x, 0.0f - width + y, 0.0f - width + z);
        glVertex3f(0.0f - width + x, 0.0f - width + y, width + z);
        glVertex3f(0.0f - width + x, 0.0f - width + y, 0.0f - width + z);
        glVertex3f(width + x, 0.0f - width + y, width + z);
        glVertex3f(width + x, 0.0f - width + y, 0.0f - width + z);
        glVertex3f(width + x, width + y, width + z);
        glVertex3f(width + x, width + y, 0.0f - width + z);
        glVertex3f(0.0f - width + x, width + y, width + z);
        glVertex3f(0.0f - width + x, width + y, 0.0f - width + z);
        glEnd();
        glEnable(GL_LIGHTING);
    }
}

void drawCube(float x, float y, float z, int textured)
{
    float r = textured ? 0.0f : 1.0f;
    float g = textured ? 0.0f : 0.30f;
    float b = 0.0f;
    drawCubeBase(x, y, z, r, g, b, 0.9f, 0);
}

void drawSolidCube(float x, float y, float z, float r, float g, float b, float a)
{
    drawCubeBase(x, y, z, r, g, b, a, 1);
}

void drawFood(float cx, float cy, float cz, float radius, int sectorCount, int stackCount, float type)
{
    glPushMatrix();
    glTranslatef(cx, cy, cz);

    GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    GLfloat shininess[] = {50.0f};
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;

    for (int i = 0; i < stackCount; ++i)
    {
        float stackAngle1 = M_PI / 2 - i * stackStep;
        float xy1 = radius * cosf(stackAngle1);
        float z1 = radius * sinf(stackAngle1);

        float stackAngle2 = M_PI / 2 - (i + 1) * stackStep;
        float xy2 = radius * cosf(stackAngle2);
        float z2 = radius * sinf(stackAngle2);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = j * sectorStep;

            float x1 = xy1 * cosf(sectorAngle);
            float y1 = xy1 * sinf(sectorAngle);
            float nx1 = x1 / radius;
            float ny1 = y1 / radius;
            float nz1 = z1 / radius;

            float x2 = xy2 * cosf(sectorAngle);
            float y2 = xy2 * sinf(sectorAngle);
            float nx2 = x2 / radius;
            float ny2 = y2 / radius;
            float nz2 = z2 / radius;

            float r, g, b;
            if (type == 0.1f)
            {
                r = 0.8f;
                g = 0.2f;
                b = 0.2f;
            }
            else if (type == 0.2f)
            {
                r = 0.3f;
                g = 0.1f;
                b = 0.6f;
            }
            else
            {
                r = 0.8f;
                g = 0.6f;
                b = 0.1f;
            }
            glColor3f(r, g, b);

            glNormal3f(nx1, ny1, nz1);
            glVertex3f(x1, y1, z1);
            glNormal3f(nx2, ny2, nz2);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }
    glPopMatrix();
}

void drawSkyDome(float radius, int slices, int stacks)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    float orange[3] = {1.0f, 0.4f, 0.2f};
    float purple[3] = {0.3f, 0.1f, 0.6f};
    float blue[3] = {0.0f, 0.0f, 0.3f};
    float max_theta = 2.0f * M_PI / 2.50f;

    for (int i = 0; i < stacks; ++i)
    {
        float theta1 = (float)i / stacks * max_theta;
        float theta2 = (float)(i + 1) / stacks * max_theta;
        float h1 = cosf(theta1);
        float h2 = cosf(theta2);
        float nh1 = fmaxf(0.0f, h1);
        float nh2 = fmaxf(0.0f, h2);
        nh1 = 1.0f - nh1;
        nh2 = 1.0f - nh2;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j)
        {
            float phi = (float)j / slices * 2.0f * M_PI;

            float x1 = radius * sinf(theta1) * cosf(phi);
            float y1 = radius * sinf(theta1) * sinf(phi);
            float z1 = radius * cosf(theta1);
            float r1, g1, b1;
            if (nh1 < 0.2f)
            {
                r1 = orange[0];
                g1 = orange[1];
                b1 = orange[2];
            }
            else if (nh1 < 0.6f)
            {
                float t = (nh1 - 0.2f) / 0.4f;
                r1 = orange[0] + t * (purple[0] - orange[0]);
                g1 = orange[1] + t * (purple[1] - orange[1]);
                b1 = orange[2] + t * (purple[2] - orange[2]);
            }
            else
            {
                float t = (nh1 - 0.6f) / 0.4f;
                r1 = purple[0] + t * (blue[0] - purple[0]);
                g1 = purple[1] + t * (blue[1] - purple[1]);
                b1 = purple[2] + t * (blue[2] - purple[2]);
            }
            glColor3f(r1, g1, b1);
            glVertex3f(x1, y1, z1);

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
    glEnable(GL_CULL_FACE);
}

void drawPatternedSphere(float cx, float cy, float cz, float radius, int sectorCount, int stackCount, float time, int type)
{
    glPushMatrix();
    glTranslatef(cx, cy, cz);

    GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    GLfloat shininess[] = {50.0f};
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;

    for (int i = 0; i < stackCount; ++i)
    {
        float stackAngle1 = M_PI / 2 - i * stackStep;
        float xy1 = radius * cosf(stackAngle1);
        float z1 = radius * sinf(stackAngle1);

        float stackAngle2 = M_PI / 2 - (i + 1) * stackStep;
        float xy2 = radius * cosf(stackAngle2);
        float z2 = radius * sinf(stackAngle2);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = j * sectorStep;

            float x1 = xy1 * cosf(sectorAngle);
            float y1 = xy1 * sinf(sectorAngle);
            float nx1 = x1 / radius;
            float ny1 = y1 / radius;
            float nz1 = z1 / radius;

            float x2 = xy2 * cosf(sectorAngle);
            float y2 = xy2 * sinf(sectorAngle);
            float nx2 = x2 / radius;
            float ny2 = y2 / radius;
            float nz2 = z2 / radius;

            float r, g, b;
            if (type == 0)
            {
                r = 0.8f;
                g = 0.2f;
                b = 0.2f;
            }
            else if (type == 1)
            {
                float swirl = sinf(stackAngle1 * 5.0f + sectorAngle * 2.0f + time * 2.0f);
                r = 0.3f + 0.2f * (cosf(sectorAngle + time * 1.5f) * 0.5f + 0.5f);
                g = 0.1f + 0.4f * (swirl * 0.5f + 0.5f);
                b = 0.6f + 0.3f * (sinf(stackAngle1 + time) * 0.5f + 0.5f);
            }
            else
            {
                float spot = sinf(stackAngle1 * 10.0f + sectorAngle * 10.0f + time * 0.5f);
                r = 0.8f + 0.2f * spot;
                g = 0.6f - 0.3f * spot;
                b = 0.1f + 0.2f * (cosf(sectorAngle + time * 0.8f) * 0.5f + 0.5f);
            }
            glColor3f(r, g, b);

            glNormal3f(nx1, ny1, nz1);
            glVertex3f(x1, y1, z1);
            glNormal3f(nx2, ny2, nz2);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }
    glPopMatrix();
}

void display(void)
{
    // FPS calculation
    gameState.currentTime = glutGet(GLUT_ELAPSED_TIME);
    int frameTime = gameState.currentTime - gameState.lastTime;
    gameState.avgFrameTime = (gameState.avgFrameTime * gameState.frameCount + frameTime) / (gameState.frameCount + 1);
    gameState.frameCount++;
    gameState.lastTime = gameState.currentTime;

    if (gameState.currentTime - gameState.lastFpsTime >= 1000)
    {
        gameState.fpsActual = 1000 / (int)gameState.avgFrameTime;
        gameState.avgFrameTime = 0.0f;
        gameState.frameCount = 0;
        gameState.lastFpsTime = gameState.currentTime;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Camera setup
    float yawRad = gameState.rotateY * (2.0f * M_PI / 360.0f);
    float pitchRad = gameState.rotateX * (2.0f * M_PI / 360.0f);
    float dx = cosf(yawRad) * cosf(pitchRad);
    float dy = sinf(yawRad) * cosf(pitchRad);
    float dz = -sinf(pitchRad);
    gluLookAt(gameState.posX, gameState.posY, gameState.posZ,
              gameState.posX + dx, gameState.posY + dy, gameState.posZ + dz,
              0.0f, 0.0f, 1.0f);

    // Light position
    GLfloat lightPos[] = {4.0f, 4.0f, 10.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Sky dome
    if (DRAW_DOME)
    {
        glPushMatrix();
        glTranslatef(gameState.posX, gameState.posY, gameState.posZ);
        drawSkyDome(100.0f, 20, 10);
        glPopMatrix();
    }

    // Checkerboard arena
    float z = -0.5f;
    int t = 0;
    for (float x = 0.0f; x <= 20.0f; x += 1.0f)
    {
        t = (int)x % 2;
        for (float y = 0.0f; y <= 20.0f; y += 1.0f)
        {
            double dx = x - 10.0;
            double dy = y - 10.0;
            if (sqrt(dx * dx + dy * dy) < 10.0)
            {
                drawCube(x, y, z, t);
            }
            t = 1 - t;
        }
    }

    // Arena border
    for (double a = 0; a < 2.0 * M_PI; a += 0.03)
    {
        float bx = 10.0f * cosf(a) + 10.0f;
        float by = 10.0f * sinf(a) + 10.0f;
        drawSolidCube(bx, by, 0.5f, 1.0f, 0.5f, 0.0f, 0.5f);
    }

    // Agent arena (food grid and agents)
    t = 0;
    for (float x = 0.0f; x <= 10.0f; x += 1.0f)
    {
        t = (int)x % 2;
        for (float y = 0.0f; y <= 10.0f; y += 1.0f)
        {
            int x1 = (int)x, y1 = (int)y;
            float r = gameState.agentServerInfo.foodgrid.food_col[x1].food_row[y1];
            drawSolidCube(x + 25.0f, y + 5.0f, -0.5f, r, 0.0f, 0.0f, 0.5f);
            t = 1 - t;
        }
    }

    gameState.agents = gameState.agentServerInfo.agents;
    for (int i = 0; i < 10; i++)
    {
        Agent agent = gameState.agents.agent[i];
        drawCubeBaseWidthSpec(agent.x + 25.0f, agent.y + 5.0f, 0.201f, agent.r, agent.g, agent.b, 1.0f,1,0.2f);
    }

    for (int i = 0; i < 25; i++)
    {
        Food food = gameState.foods.food[i];
        drawFood(food.x + 25.0f, food.y + 5.0f, 0.5f, 0.25f, 12, 12, food.food_type);
    }

    // Planets
    float animTime = gameState.currentTime / 1000.0f;
    drawPatternedSphere(gameState.planets[0].x, gameState.planets[0].y, gameState.planets[0].z, gameState.planets[0].radius, 20, 20, animTime, 0);
    drawPatternedSphere(gameState.planets[1].x, gameState.planets[1].y, gameState.planets[1].z, gameState.planets[1].radius, 30, 25, animTime, 1);
    drawPatternedSphere(gameState.planets[2].x, gameState.planets[2].y, gameState.planets[2].z, gameState.planets[2].radius, 40, 30, animTime, 2);

    // OBJ model
    if (gameState.objList != 0)
    {
        if (gameState.frameCount % 20 == 0)
        {
            gameState.enterpriseAngle += 10.0f;
        }
        glPushMatrix();
        glTranslatef(8.0f, 16.0f, 0.5f);
        glScalef(0.025f, 0.025f, 0.025f);
        glRotatef(90.0f, 0.0f, 1.0f, 1.0f);
        glRotatef(gameState.enterpriseAngle, 1.0f, 0.0f, 0.0f);
        glColor3f(0.0f, 0.0f, 0.5f);
        float spec_in = 0.0f;
        float shin_in = 0.0f;
        GLfloat obj_specular[] = {spec_in, spec_in, spec_in, spec_in};
        GLfloat obj_shininess[] = {shin_in};
        glMaterialfv(GL_FRONT, GL_SPECULAR, obj_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, obj_shininess);
        glCallList(gameState.objList);
        glPopMatrix();
    }

        if (gameState.objList2 != 0)
    {
        if (gameState.frameCount % 20 == 0)
        {
            gameState.enterpriseAngle += 10.0f;
        }
        glPushMatrix();
        glTranslatef(8.0f, 4.0f, 1.5f);
        glScalef(0.025f, 0.025f, 0.025f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(gameState.enterpriseAngle, 1.0f, 0.0f, 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        float spec_in = 0.0f;
        float shin_in = 0.0f;
        GLfloat obj_specular[] = {spec_in, spec_in, spec_in, spec_in};
        GLfloat obj_shininess[] = {shin_in};
        glMaterialfv(GL_FRONT, GL_SPECULAR, obj_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, obj_shininess);
        glCallList(gameState.objList2);
        glPopMatrix();
    }

    // Player cube
    drawSolidCube(gameState.cubeX, gameState.cubeY, 0.51f, 0.5f, 0.5f, 1.0f, 0.5f);

    // Other players
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (!gameState.serverMessage.playerinfo[i].exists)
            continue;
        float xp = gameState.serverMessage.playerinfo[i].position.x;
        float yp = gameState.serverMessage.playerinfo[i].position.y;
        float zp = gameState.serverMessage.playerinfo[i].position.z;
        float dx = xp - gameState.posX;
        float dy = yp - gameState.posY;
        float dz = zp - gameState.posZ;
        float dist = sqrtf(dx * dx + dy * dy + dz * dz);
        if (dist < 0.5f)
            continue;
        float r = (i == 0) ? 1.0f : 0.0f;
        float g = (i == 0) ? 0.2f : 1.0f;
        float b = (i == 0) ? 1.0f : 0.0f;
        drawSolidCube(xp, yp, zp, r, g, b, 0.5f);
    }

    // HUD
    char fpsText[256];
    sprintf(fpsText, "FPS: %d | Cube: (%.1f, %.1f) | Player: (%.3f, %.3f, %.3f) | Use Velocity: %d | Can Clip: %d",
            gameState.fpsActual, gameState.cubeX, gameState.cubeY, gameState.posX, gameState.posY, gameState.posZ,
            gameState.useVelocity, gameState.canClip);
    renderText(10, glutGet(GLUT_WINDOW_HEIGHT) - 30, GLUT_BITMAP_HELVETICA_18, fpsText);

    char fpsText2[256];
    sprintf(fpsText2, "Velocities: (%.3f, %.3f, %.3f) | Angles: (%.2f, %.2f)",
            gameState.velX, gameState.velY, gameState.velZ, gameState.rotateX, gameState.rotateY);
    renderText(10, glutGet(GLUT_WINDOW_HEIGHT) - 60, GLUT_BITMAP_HELVETICA_18, fpsText2);

    renderText(10, 30, GLUT_BITMAP_HELVETICA_12, "WASD: Move | Arrows: Look | N/J/I/L/K: Control Cube | SHOUTOUT PRESTON");

    glutSwapBuffers();
}