#ifndef PARTICLE_H
#define PARTICLE_H

#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

extern double A;
extern double O;
extern double C;
extern double dt;
extern double timeSlow;
extern bool paused;
extern double sourceSize;
extern const double PI;

typedef struct Particle_s
{
  double x;
  double y;
  double z;
} Particle;

double magnitudeParticle(Particle p);
Particle scaleParticle(Particle p, double k);
Particle normalizeParticle(Particle p);
Particle sphereToParticle(double r, double theta, double phi);
Particle matrixTransformParticle(double matrix[9], Particle p);
Particle rotateParticleLongLatInverse(Particle p, double theta, double phi);
Particle rotateParticleToParticle(Particle p, Particle v);
void particleToSphere(Particle p, double* theta, double* phi);
void spawnParticle(Particle* p);
void stepParticle(Particle* p, Particle v);
void lorenzStepParticle(Particle* p);
Particle lorenzeVelocityFromPosition(Particle p);
#endif
