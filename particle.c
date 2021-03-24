#include "particle.h"

//returns magnitude of particle
double magnitudeParticle(Particle p)
{
  return sqrt(p.x*p.x+p.y*p.y+p.z*p.z);
}

//returns scaled particle
Particle scaleParticle(Particle p, double k)
{
  Particle r;
  r.x = k * p.x;
  r.y = k * p.y;
  r.z = k * p.z;
  return r;
}

//returns vector in same direction with magnitude equal to 1
Particle normalizeParticle(Particle p)
{
  Particle r;
  double invsqrt = 1/magnitudeParticle(p);
  r.x = p.x * invsqrt;
  r.y = p.y * invsqrt;
  r.z = p.z * invsqrt;
  return r;
}

//turns spherical coordinate to particle
Particle sphereToParticle(double radius, double theta, double phi)
{
  Particle r;
  r.x = radius*cos(phi)*sin(theta);
  r.y = radius*sin(phi);
  r.z = radius*cos(phi)*cos(theta);
  return r;
}

//turns particle into spherical coordinates
void particleToSphere(Particle p, double* theta, double* phi)
{
  *theta = atan2(p.x, p.z);
  *phi = asin(p.y / magnitudeParticle(p));
}

//matrix-vector multiplication, used as a change of base
Particle matrixTransformParticle(double matrix[9], Particle p)
{
  Particle r;
  r.x = p.x*matrix[0] + p.y*matrix[1] + p.z*matrix[2];
  r.y = p.x*matrix[3] + p.y*matrix[4] + p.z*matrix[5];
  r.z = p.x*matrix[6] + p.y*matrix[7] + p.z*matrix[8];
  return r;
}



//rotates particle with respect to sherical coordinate
Particle rotateParticleLongLatInverse(Particle p, double theta, double phi)
{
  //saving the cos and sin of theta and phi saves calculations
  //NOTE: it may or not be faster to calculate sin(x) as sqrt(1-cos(x)^2)
  double cosTheta = cos(theta);
  double sinTheta = sin(theta);
  double cosPhi = cos(phi);
  double sinPhi = sin(phi);

  //Z, foreward facing vector
  //cos(phi)*sin(theta)
  //sin(phi)
  //cos(phi)*cos(theta)

  //theta = arctan(x/z)
  //phi = arcsin(y/r)

  double matrix[9] = {
     cosTheta,        0,      -sinTheta,
    -sinPhi*sinTheta, cosPhi, -sinPhi*cosTheta,
     cosPhi*sinTheta, sinPhi,  cosPhi*cosTheta
     };

  return matrixTransformParticle(matrix, p);
}

//this function spawns a particle in the source cube
void spawnParticle(Particle* p)
{
  int e = 1000;
  p->x = sourceSize*(rand() % e - e/2) / (double) e;
  p->y = sourceSize*(rand() % e - e/2) / (double) e;
  p->z = sourceSize*(rand() % e - e/2) / (double) e;
}

//this function steps a particle based off velocity
void stepParticle(Particle* p, Particle v)
{
  //dt is an external variable defined in main.c
  //dt represents the change in time between each frame.
  p->x += v.x * dt;
  p->y += v.y * dt;
  p->z += v.z * dt;

  if (isinf(p->x) || isinf(p->y) || isinf(p->z) || isnan(p->x) || isnan(p->y) || isnan(p->z))
  {
      spawnParticle(p);
  }
}

//this function steps particles through the lorenz system
void lorenzStepParticle(Particle* p)
{
  Particle v = lorenzeVelocityFromPosition(*p);
  v = scaleParticle(v, !paused * timeSlow);
  stepParticle(p, v);
}

Particle lorenzeVelocityFromPosition(Particle p)
{
  Particle v;
  //velocities defined by the lorenze system,
  //a, b, and c are extern doubles defined in main
  v.x = A * (p.y - p.x);
  v.y = p.x * (O - p.z) - p.y;
  v.z = p.x * p.y - C * p.z;
  return v;
}


