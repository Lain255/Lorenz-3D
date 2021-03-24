#ifndef CAMERA_H
#define CAMERA_H

#include "particle.h"
#include <SDL2/SDL.h>
#include <stdio.h>
extern double screenDepth;

typedef struct Camera_s
{
  SDL_Window* window;
  SDL_Renderer* renderer;

  double screenWidth;
  double screenHeight;
  double screenDepth;

  Particle p;
  Particle v;

  double theta;
  double phi;

} Camera;

void drawParticle(Camera c, Particle p);
bool initCamera(Camera* c);
void resetCamera(Camera* c);
void closeCamera(Camera* c);

#endif
