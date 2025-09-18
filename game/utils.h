#ifndef UTILS_H
#define UTILS_H

#include "game.h"

void loadOBJ(const char *filename);
void loadOBJ2(const char *filename);
void renderText(float x, float y, void *font, const char *text);
void reshape(int w, int h);

#endif