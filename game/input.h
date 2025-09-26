#ifndef INPUT_H
#define INPUT_H

#include "game.h"
#include <stdbool.h>
#include <GL/glut.h>

extern bool keyStates[256];
extern bool specialKeyStates[256];
extern float speedModifier;
extern GameState gameState;

void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void updateMovement(void);
void specialKeys(int key, int x, int y);
void specialKeysUp(int key, int x, int y);
void updateRotation(void);
void updateMovement(void);
void updateSpeedModifier(void);

#endif