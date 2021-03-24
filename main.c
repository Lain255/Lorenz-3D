#include "camera.h"

const double PI = 3.1415926;
const int fps = 60;
double dt = 1. / 60; //1/fps
double timeSlow = 1;
double A = 10;
double O = 28;
double C = 8 / 3.;
double sourceSize = 0.5;
int numParticles = 30000;
int ctrlDown = 1;
bool paused = false;
bool flyFree = false;

int main(int argc, char *argv[])
{
  //loads the number of points
  FILE* numParticlesFile = fopen("numParticles.txt", "r");
  fscanf(numParticlesFile, "%i", &numParticles);
  fclose(numParticlesFile);

  Camera c;
  if (initCamera(&c))
  {
    //Main loop flag
    bool quit = false;

    //create and initialize particles
    Particle *particleList = malloc(numParticles * sizeof(Particle));
    for (int i = 0; i < numParticles; i++)
    {
      spawnParticle(&particleList[i]);
    }
    
    //Event handler
    SDL_Event e;
    SDL_SetRelativeMouseMode(SDL_TRUE);

    int startOfFrame = SDL_GetTicks();
    while (!quit)
    {
      //handles events on queue
      while (SDL_PollEvent(&e) != 0)
      {
        if (e.type == SDL_QUIT)
        {
          quit = true;
        }
        else if (e.type == SDL_KEYDOWN)
        {
          switch (e.key.keysym.sym)
          {
          //quiting
          case SDLK_ESCAPE:
            quit = true;
            break;

          //absolute movements
          case SDLK_s:
            c.v.z = -10 * ctrlDown;
            break;
          case SDLK_w:
            c.v.z = 10 * ctrlDown;
            break;
          case SDLK_a:
            c.v.x = -10 * ctrlDown;
            break;
          case SDLK_d:
            c.v.x = 10 * ctrlDown;
            break;
          case SDLK_q:
            c.v.y = -10 * ctrlDown;
            break;
          case SDLK_e:
            c.v.y = 10 * ctrlDown;
            break;

          //editing a,o,c coeffecients
          case SDLK_u:
            A += 0.1 * ctrlDown;
            break;
          case SDLK_j:
            A += -0.1 * ctrlDown;
            break;
          case SDLK_i:
            O += 0.1 * ctrlDown;
            break;
          case SDLK_k:
            O += -0.1 * ctrlDown;
            break;
          case SDLK_o:
            C += 0.1 * ctrlDown;
            break;
          case SDLK_l:
            C += -0.1 * ctrlDown;
            break;

          //edit time
          case SDLK_t:
            timeSlow *= 0.95;
            break;
          case SDLK_g:
            timeSlow *= 1.05;
            break;
          case SDLK_p:
            paused = !paused;
            break;

          //fly free mode
          case SDLK_f:
            flyFree = !flyFree;
            break;

          //reset variables
          case SDLK_r:
            if (ctrlDown != 1)
            {
              for (int i = 0; i < numParticles; i++)
              {
                spawnParticle(&particleList[i]);
              }
            }
            else
            {
              timeSlow = 1;
              A = 10;
              O = 28;
              C = 8 / 3.;
              resetCamera(&c);
            }
            break;

          //holding down control
          case SDLK_LCTRL:
          case SDLK_RCTRL:
            ctrlDown = 5;
            break;
          }
        }
        if (e.type == SDL_KEYUP)
        {
          switch (e.key.keysym.sym)
          {
          //stop absolute movement
          case SDLK_s:
          case SDLK_w:
            c.v.z = 0;
            break;
          case SDLK_a:
          case SDLK_d:
            c.v.x = 0;
            break;
          case SDLK_q:
          case SDLK_e:
            c.v.y = 0;
            break;

          //letting go of control
          case SDLK_LCTRL:
          case SDLK_RCTRL:
            ctrlDown = 1;
            break;
          }
        }
        else if (e.type == SDL_MOUSEWHEEL)
        {
          c.v = sphereToParticle(ctrlDown * 10 * e.wheel.y, c.theta, c.phi);
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN)
        {
          if (e.button.button == SDL_BUTTON_LEFT)
          {
            c.v = sphereToParticle(ctrlDown * 10, c.theta, c.phi);
          }
          if (e.button.button == SDL_BUTTON_RIGHT)
          {
            c.v = sphereToParticle(-ctrlDown * 10, c.theta, c.phi);
          }

          if (e.button.button == SDL_BUTTON_MIDDLE)
          {
            c.v.x = 0;
            c.v.y = 0;
            c.v.z = 0;
          }
        }
        else if (e.type == SDL_MOUSEBUTTONUP)
        {
          if (e.button.button == SDL_BUTTON_LEFT)
          {
            c.v.x = 0;
            c.v.y = 0;
            c.v.z = 0;
          }
          if (e.button.button == SDL_BUTTON_RIGHT)
          {
            c.v.x = 0;
            c.v.y = 0;
            c.v.z = 0;
          }
        }
        else if (e.type == SDL_MOUSEMOTION)
        {
          int dTheta, dPhi;
          SDL_GetRelativeMouseState(&dTheta, &dPhi);
          c.theta += dTheta / (100 * PI);
          c.phi -= dPhi / (100 * PI);

          if (c.phi > PI / 2)
          {
            c.phi = PI / 2;
          }
          if (c.phi < -PI / 2)
          {
            c.phi = -PI / 2;
          }
        }
      }

      //move camera
      if (flyFree)
      {
        c.v = scaleParticle(lorenzeVelocityFromPosition(c.p), !paused * timeSlow);
        particleToSphere(c.v, &c.theta, &c.phi);
        stepParticle(&c.p, c.v);
      }
      else
      {
        stepParticle(&c.p, c.v);
      }
      
      //move particles
      for (int i = 0; i < numParticles; i++)
      {
        lorenzStepParticle(&particleList[i]);
      }

      //clear screen
      SDL_SetRenderDrawColor(c.renderer, 0x00, 0x00, 0x00, 0xFF);
      SDL_RenderClear(c.renderer);

      //draw particles
      SDL_SetRenderDrawColor(c.renderer, 255, 255, 255, 255);
      for (int i = 0; i < numParticles; i++)
      {
        drawParticle(c, particleList[i]);
      }

      //Update screen
      SDL_RenderPresent(c.renderer);

      //frame cap
      if (SDL_GetTicks() - startOfFrame < dt * 1000)
      {
        SDL_Delay(dt * 1000 - (SDL_GetTicks() - startOfFrame));
        startOfFrame = SDL_GetTicks();
      }
    }
    free(particleList);
  }
  closeCamera(&c);
  return 0;
}
