#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>

//vector structure and functions
typedef struct V3_s {
    double x;
    double y;
    double z;
} V3;

V3 add(V3 a, V3 b) {
    V3 r = {a.x + b.x, a.y + b.y, a.z + b.z}; 
    return r;
}

V3 mult(double c, V3 a) {
    V3 r = {c* a.x, c * a.y, c * a.z}; 
    return r;
}

V3 randV3() {
    V3 r = {
        (rand() % 1000000)/100000000.0, 
        (rand() % 1000000)/100000000.0,
        (rand() % 1000000)/100000000.0
    };
    return r;
}

int is_outbound(V3 a)
{
    return isinf(a.x) || isinf(a.y) || isinf(a.z) || 
           isnan(a.x) || isnan(a.y) || isnan(a.z);
}

const double PI = 3.1415926;
const int numParticles = 100000;

//lorenz constants
double A = 10.0;
double B = 8.0 / 3.0;
double C = 28.0;

double dt = 1.0 / 60.0;

int main(int argc, char **argv)
{
    //initialization
    SDL_Init(SDL_INIT_VIDEO);

    int w = 800;
    int h = 600;
    double FOV = 1;

    SDL_Window *window = SDL_CreateWindow(
        "Lorenze", 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        w, 
        h,
        SDL_WINDOW_MAXIMIZED
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC
    );

    SDL_GetRendererOutputSize(renderer, &w, &h);

    V3* particals = malloc(sizeof(V3)*numParticles);
    SDL_FPoint* points = malloc(sizeof(SDL_FPoint)*numParticles);
    V3 c = {0,0,0};
    double theta = 0;
    double phi = 0;
    int dTheta, dPhi;
    double cosTheta = cos(theta);
    double sinTheta = sin(theta);
    double cosPhi = cos(phi);
    double sinPhi = sin(phi);

    int stopTime = 1;
    int turbo;

    //initalizing all particles to random spots on source cube
    for (int i = 0; i < numParticles; i++)
    {
        particals[i] = randV3();
    }
    
    SDL_bool quit = SDL_FALSE;
    SDL_Event e;
    SDL_SetRelativeMouseMode(SDL_TRUE);
    const Uint8* keyboard = SDL_GetKeyboardState(NULL);

    //main loop
    while (!quit) 
    {
        //input handling
        while (SDL_PollEvent(&e)) {
            switch(e.type) {
            
            case SDL_QUIT:
                quit = SDL_TRUE;    
                break;
                
            case SDL_KEYDOWN:
                if (e.key.keysym.scancode == SDL_SCANCODE_R) {
                    A = 10.0;
                    B = 8.0 / 3.0;
                    C = 28.0;
                }
                
                if (e.key.keysym.scancode == SDL_SCANCODE_F) {
                    stopTime = !stopTime;
                }
                break;
            
            case SDL_MOUSEMOTION:
                SDL_GetRelativeMouseState(&dTheta, &dPhi);
                theta += dTheta / (100 * PI);
                phi += dPhi / (100 * PI);

                if (phi > PI / 2) {
                    phi = PI / 2;
                }
                if (phi < -PI / 2) {
                    phi = -PI / 2;
                }
                
                cosTheta = cos(theta);
                sinTheta = sin(theta);
                cosPhi = cos(phi);
                sinPhi = sin(phi);

                break;
            }
        }

        quit = quit || keyboard[SDL_SCANCODE_ESCAPE];
        turbo = 1 + 4*keyboard[SDL_SCANCODE_LSHIFT];
        
        double right   = 0.1*turbo*(keyboard[SDL_SCANCODE_D] - keyboard[SDL_SCANCODE_A]);
        double up      = 0.1*turbo*(keyboard[SDL_SCANCODE_E] - keyboard[SDL_SCANCODE_Q]);
        double forward = 0.1*turbo*(keyboard[SDL_SCANCODE_W] - keyboard[SDL_SCANCODE_S]);

        c.x +=  (forward*cosPhi*sinTheta) + (up*sinPhi*sinTheta) +  (right*cosTheta);    
        c.y += (-forward*sinPhi)          + (up*cosPhi);
        c.z +=  (forward*cosPhi*cosTheta) + (up*sinPhi*cosTheta) + (-right*sinTheta);
        
        A += 0.1*turbo*(keyboard[SDL_SCANCODE_U] - keyboard[SDL_SCANCODE_J]);
        B += 0.1*turbo*(keyboard[SDL_SCANCODE_I] - keyboard[SDL_SCANCODE_K]);
        C += 0.1*turbo*(keyboard[SDL_SCANCODE_O] - keyboard[SDL_SCANCODE_L]);

        dt *= 1 + turbo*0.01*(keyboard[SDL_SCANCODE_T] - keyboard[SDL_SCANCODE_G]);

        FOV *= 1 + turbo*0.01*(keyboard[SDL_SCANCODE_X] - keyboard[SDL_SCANCODE_Z]);

        
        //particle processor
        for (int i = 0; i < numParticles; i++)
        {
            //temp variable
            V3 p = particals[i];

            //the Lorenz system is dictated by this differential equation
            V3 v = {
                A * (p.y - p.x),
                p.x * (C - p.z) - p.y,
                p.x * p.y - B * p.z
            };

            //approximating differential equasion with differentials
            p = add(p,mult(dt*stopTime,v));
            
            //update particle list
            particals[i] = p;

            //move particle to camera
            p = add(p, mult(-1,c));

            //rotate particle to camera's facing
            V3 rp = {
                p.x*cosTheta                     - p.z*sinTheta,
                p.x*sinPhi*sinTheta + p.y*cosPhi + p.z*sinPhi*cosTheta,
                p.x*cosPhi*sinTheta - p.y*sinPhi + p.z*cosPhi*cosTheta
            };

            //check if paricle is out of bounds
            if (is_outbound(rp))
            {
                particals[i] = randV3();
            }
            //put particle with perspective projection into point buffer
            else if (rp.z > FOV) {
                rp.x = FOV*rp.x / rp.z;
                rp.y = FOV*rp.y / rp.z;

                points[i].x = (w/2.0)*(rp.x+1);
                points[i].y = (h/2.0)*(-rp.y+1);
            }
            //put junk in point buffer
            else
            {
                points[i].x = -1;
                points[i].y = -1;
            }
        }
        
        //drawing point buffer to screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawPointsF(renderer, points, numParticles);
        SDL_RenderPresent(renderer);
    }

    //freeing memory
    free(particals);
    free(points);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
