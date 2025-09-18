#include "utils.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h> // Changed from <GL/glut.h>
#include <stdio.h>
#include <string.h>

void loadOBJ(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Error: Cannot open OBJ file %s\n", filename);
        return;
    }

    float vertices[10000][3];
    float normals[10000][3];
    int numVertices = 0;
    int numNormals = 0;
    int faces[10000][6];
    int numFaces = 0;

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "v ", 2) == 0)
        {
            if (numVertices >= 10000)
            {
                fprintf(stderr, "Error: Too many vertices (max 10000)\n");
                fclose(file);
                return;
            }
            if (sscanf(line + 2, "%f %f %f", &vertices[numVertices][0], &vertices[numVertices][1], &vertices[numVertices][2]) == 3)
            {
                numVertices++;
            }
            else
            {
                fprintf(stderr, "Error: Invalid vertex format: %s\n", line);
            }
        }
        else if (strncmp(line, "vn ", 3) == 0)
        {
            if (numNormals >= 10000)
            {
                fprintf(stderr, "Error: Too many normals (max 10000)\n");
                fclose(file);
                return;
            }
            if (sscanf(line + 3, "%f %f %f", &normals[numNormals][0], &normals[numNormals][1], &normals[numNormals][2]) == 3)
            {
                numNormals++;
            }
            else
            {
                fprintf(stderr, "Error: Invalid normal format: %s\n", line);
            }
        }
        else if (strncmp(line, "f ", 2) == 0)
        {
            if (numFaces >= 10000)
            {
                fprintf(stderr, "Error: Too many faces (max 10000)\n");
                fclose(file);
                return;
            }
            int v1, n1, v2, n2, v3, n3;
            if (sscanf(line + 2, "%d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3) == 6)
            {
                if (v1 < 1 || v1 > numVertices || n1 < 1 || n1 > numNormals ||
                    v2 < 1 || v2 > numVertices || n2 < 1 || n2 > numNormals ||
                    v3 < 1 || v3 > numVertices || n3 < 1 || n3 > numNormals)
                {
                    fprintf(stderr, "Error: Invalid face indices: %s\n", line);
                    continue;
                }
                faces[numFaces][0] = v1 - 1;
                faces[numFaces][1] = n1 - 1;
                faces[numFaces][2] = v2 - 1;
                faces[numFaces][3] = n2 - 1;
                faces[numFaces][4] = v3 - 1;
                faces[numFaces][5] = n3 - 1;
                numFaces++;
            }
            else if (sscanf(line + 2, "%d %d %d", &v1, &v2, &v3) == 3)
            {
                if (v1 < 1 || v1 > numVertices || v2 < 1 || v2 > numVertices || v3 < 1 || v3 > numVertices)
                {
                    fprintf(stderr, "Error: Invalid face indices (no normals): %s\n", line);
                    continue;
                }
                faces[numFaces][0] = v1 - 1;
                faces[numFaces][1] = 0;
                faces[numFaces][2] = v2 - 1;
                faces[numFaces][3] = 0;
                faces[numFaces][4] = v3 - 1;
                faces[numFaces][5] = 0;
                numFaces++;
            }
            else
            {
                fprintf(stderr, "Error: Unsupported face format: %s\n", line);
            }
        }
    }
    fclose(file);

    if (numVertices == 0 || numFaces == 0)
    {
        fprintf(stderr, "Error: Invalid OBJ file %s (no vertices or faces)\n", filename);
        return;
    }

    gameState.objList = glGenLists(1);
    if (gameState.objList == 0)
    {
        fprintf(stderr, "Error: Failed to create display list\n");
        return;
    }
    glNewList(gameState.objList, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < numFaces; i++)
    {
        int v1 = faces[i][0], n1 = faces[i][1];
        int v2 = faces[i][2], n2 = faces[i][3];
        int v3 = faces[i][4], n3 = faces[i][5];
        if (n1 > 0)
            glNormal3f(normals[n1][0], normals[n1][1], normals[n1][2]);
        glVertex3f(vertices[v1][0], vertices[v1][1], vertices[v1][2]);
        if (n2 > 0)
            glNormal3f(normals[n2][0], normals[n2][1], normals[n2][2]);
        glVertex3f(vertices[v2][0], vertices[v2][1], vertices[v2][2]);
        if (n3 > 0)
            glNormal3f(normals[n3][0], normals[n3][1], normals[n3][2]);
        glVertex3f(vertices[v3][0], vertices[v3][1], vertices[v3][2]);
    }
    glEnd();
    glEndList();

    printf("Loaded OBJ: %d vertices, %d normals, %d faces\n", numVertices, numNormals, numFaces);
}

void renderText(float x, float y, void *font, const char *text)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    glutBitmapString(font, (unsigned char *)text);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / h, 0.1f, RENDER_DIST);
    glMatrixMode(GL_MODELVIEW);
}