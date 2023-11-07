#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include <windows.h>
#define PI 3.14159265358979323846 // Define π for Windows
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <vector>
#include <iostream>
#include <cmath>

using namespace std;

// Struct to represent a 3D particle
struct Particle
{
    float x;      // X-coordinate of the particle
    float y;      // Y-coordinate of the particle
    float z;      // Z-coordinate of the particle (not used)
    float life;   // Remaining life of the particle
};

const int MaxParticles = 500; // Maximum number of particles
vector<Particle> particles;   // Vector to store particles

bool isDrawing = false;         // Indicates whether the user is currently drawing particles
bool gravitySimulation = false; // Indicates whether gravity simulation is currently active
bool blackHoleSimulation = false;   // Indicates whether black hole simulation is currently active
float timeStep = 0.005;      // Time step for simulation

// Function to display all the particles
void displayParticles()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0, 0.0, 0.0); // Set the particle color to red
    glPointSize(5.0);         // Set the size of the particles

    glBegin(GL_POINTS); // Start drawing points

    for (int i = 0; i < particles.size(); i++)
    {
        if (particles[i].life > 0)
        {
            glVertex3f(particles[i].x, particles[i].y, particles[i].z);
            particles[i].life -= timeStep;
        }
    }

    glEnd();           // End drawing points
    glutSwapBuffers(); // Swap the display buffer
}

void initialize()
{
    glEnable(GL_DEPTH_TEST); // Enable depth testing

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the orthographic projection (2D drawing in a 3D space)
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
}

// Callback function for mouse motion
void motionCallback(int x, int y)
{
    if (isDrawing)
    {
        // Create a new particle based on the mouse position
        Particle newParticle;
        newParticle.x = (float)x / glutGet(GLUT_WINDOW_WIDTH) * 2 - 1;
        newParticle.y = (float)(glutGet(GLUT_WINDOW_HEIGHT) - y) / glutGet(GLUT_WINDOW_HEIGHT) * 2 - 1;
        newParticle.z = 0.0;
        newParticle.life = 1.0; // Set initial life

        if (particles.size() < MaxParticles)
        {
            particles.push_back(newParticle);
        }
        else
        {
            particles.erase(particles.begin());
            particles.push_back(newParticle);
        }

        glutPostRedisplay(); // Request a redisplay
    }
}

// Check if the mouse button is being pressed for drawing
void mouseCallback(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            isDrawing = true;
        }
        else if (state == GLUT_UP)
        {
            isDrawing = false;
        }
    }
}

// Function to update the particles for the gravity simulation
void gravitySimulationStep()
{
    for (int i = 0; i < particles.size(); i++)
    {
        if (particles[i].life > 0)
        {
            // Apply downward gravity
            particles[i].y -= 0.005;
            particles[i].life -= timeStep;
        }
    }
}

// Function to update the particles for the black hole simulation
void blackHoleSimulationStep()
{
    float bhX = 0.0;
    float bhY = 0.0;

    for (int i = 0; i < particles.size(); i++)
    {
        if (particles[i].life > 0)
        {
            // Calculate the direction towards the black hole
            float dx = bhX - particles[i].x;
            float dy = bhY - particles[i].y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance > 0.01)
            {
                // Apply a force towards the black hole (physics)
                float physics = 0.01 / (distance * distance);
                particles[i].x += physics * dx;
                particles[i].y += physics * dy;
            }

            particles[i].life -= timeStep;
        }
    }
}

// Function to handle key presses
void keyPressCallback(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 's': // Switch to stationary mode
    case 'S':
        gravitySimulation = false;
        blackHoleSimulation = false;
        break;

    case 'g': // Switch to gravity simulation mode
    case 'G':
        gravitySimulation = true;
        blackHoleSimulation = false;
        break;

    case 'b': // Switch to black hole simulation mode
    case 'B':
        gravitySimulation = false;
        blackHoleSimulation = true;
        break;

    case 'o': // Clear all particles
        particles.clear();
        break;

    case 't': // Increase time step
    case 'T':
        timeStep *= 2.0;
        break;

    case 'l': // Decrease time step
    case 'L':
        timeStep /= 2.0;
        break;
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv); // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    int windowHandle = glutCreateWindow("Particle Simulation");

    glutSetWindow(windowHandle);

    glutPositionWindow(100, 100);
    glutReshapeWindow(500, 500);

    glutDisplayFunc(displayParticles); // Register display callback function

    glutMotionFunc(motionCallback); // Register mouse motion callback
    glutMouseFunc(mouseCallback);   // Register mouse button callback
    glutKeyboardFunc(keyPressCallback); // Register keyboard callback

    initialize(); // Initialize some OpenGL settings

    glutIdleFunc([]() {
        if (gravitySimulation)
            gravitySimulationStep();
        else if (blackHoleSimulation)
            blackHoleSimulationStep();

        glutPostRedisplay(); // Request a redisplay
    });

    glutMainLoop(); // Start the main loop for rendering

    return 0;
}