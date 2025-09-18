#include "init.h"
#include "input.h"
#include "physics.h"
#include "draw.h"
#include <GL/glut.h>
#include <GL/glu.h>

// Ensure M_PI is defined if not included from game.h
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

void initGL(void)
{
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f); // Match client.c (dark blue background)
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
    glLoadIdentity();
    gluPerspective(45.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, RENDER_DIST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Load OBJ model
    loadOBJ("assets/eprod.obj");
}