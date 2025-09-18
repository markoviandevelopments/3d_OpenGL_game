#ifndef DRAW_H
#define DRAW_H

#include "game.h"

void drawCubeBase(float x, float y, float z, float r, float g, float b, float a, int drawEdges);
void drawCube(float x, float y, float z, int textured);
void drawSolidCube(float x, float y, float z, float r, float g, float b, float a);
void drawFood(float cx, float cy, float cz, float radius, int sectorCount, int stackCount, float type);
void drawSkyDome(float radius, int slices, int stacks);
void drawPatternedSphere(float cx, float cy, float cz, float radius, int sectorCount, int stackCount, float time, int type);
void display(void);

#endif