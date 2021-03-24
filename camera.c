#include "camera.h"

//this function draws a particle to the global renderer
void drawParticle(Camera c, Particle p)
{
    //r is the reletive distance vector
    Particle r;
    r.x = p.x - c.p.x;
    r.y = p.y - c.p.y;
    r.z = p.z - c.p.z;
    
    //this rotates the point with respect to the cemeras direction
    r = rotateParticleLongLatInverse(r, c.theta, c.phi);

    //if the particle is in front of the camera draw it
    if(r.z > 0)
    {
        SDL_RenderDrawPointF(c.renderer, c.screenWidth / 2 * (1 + c.screenDepth * r.x / r.z), c.screenHeight / 2 * (1 + c.screenDepth * -r.y / r.z));
    }
    
}

//this initiates SDL, the window, and the renderer
bool initCamera(Camera *c)
{
    bool success = true;
    c->window = NULL;
    c->renderer = NULL;
    c->screenWidth = 0;
    c->screenHeight = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("failed to initialize sdl\n");
        success = false;
    }
    else
    {
        c->window = SDL_CreateWindow("Lorenz Attractor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, c->screenWidth, c->screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
        if (c->window == NULL)
        {
            printf("failed to make window\n");
            success = false;
        }
        else
        {
            c->renderer = SDL_CreateRenderer(c->window, -1, SDL_RENDERER_ACCELERATED);
            if (c->renderer == NULL)
            {
                printf("failed to make renderer\n");
                success = false;
            }
            else
            {
                SDL_SetRenderDrawColor(c->renderer, 0x00, 0x00, 0x00, 0xFF);
                int w,h;
                SDL_GetRendererOutputSize(c->renderer, &w, &h);
                c->screenWidth = w;
                c->screenHeight = h;
                c->screenDepth = 1;
            }
        }
    }

    //initializing values
    c->p.x = 0;
    c->p.y = 0;
    c->p.z = -1;
    c->v.x = 0;
    c->v.y = 0;
    c->v.z = 0;
    c->theta = 0;
    c->phi = 0;

    return success;
}

//this function resets the cameras member funtions
void resetCamera(Camera *c)
{
    c->screenDepth = 1;
    c->p.x = 0;
    c->p.y = 0;
    c->p.z = 0;
    c->v.x = 0;
    c->v.y = 0;
    c->v.z = 0;
    c->theta = 0;
    c->phi = 0;
}

//this function deinitializes SDL, the window, and the renderer
void closeCamera(Camera *c)
{
    SDL_DestroyRenderer(c->renderer);
    SDL_DestroyWindow(c->window);
    c->window = NULL;
    c->renderer = NULL;
    SDL_Quit();
}
