LORENZ SYSTEM
v.x = A * (p.y - p.x);
v.y = p.x * (O - p.z) - p.y;
v.z = p.x * p.y - C * p.z;

IMPORTANT
The program reads the number of particles from a file called numParticles.txt
the number of particles effects preformance

CONTROLS
escape key - quit
control key - amplifies the effects of most controls

mouse movement - controls cameras direction
scroll wheel - moves camera in camera direction
left and right mouse buttons - moves camera in camera direction
scroll button - set camera velocity to 0

S - move absolutly backwards
W - move absolutly forewards
A - move absolutly left
D - move absolutly right
E - move absolutly up
Q - move absolutly down

U - increases A in lorenz equasion
J - decreases A in lorenz equasion
I - increases O in lorenz equasion
K - decreases O in lorenz equasion
O - increases C in lorenz equasion
L - decreases C in lorenz equasion

T - slow down time
G - speed up time
P - pause time

R - reset camera and lorenz system
R+CTRL - reset all points to source cube

F - toggle free fly mode (WARNING EXPERIMENTAL)
