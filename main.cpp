// Maze Runner - Refactored Architecture
// C++ + OpenGL + FreeGLUT

#include <GL/freeglut.h>
#include "core/game.h"
#include "core/config.h"

// Global game instance
Game* game = nullptr;

// Window dimensions
int windowWidth = Config::DEFAULT_WINDOW_WIDTH;
int windowHeight = Config::DEFAULT_WINDOW_HEIGHT;

// GLUT callback functions
void display()
{
    if (game)
    {
        game->render(windowWidth, windowHeight);
        glutSwapBuffers();
    }
}

void reshape(int width, int height)
{
    if (height == 0)
    {
        height = 1;
    }

    windowWidth = width;
    windowHeight = height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width, 0.0, height, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboardDown(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    if (game)
    {
        game->handleKeyDown(key);
        glutPostRedisplay();
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    if (game)
    {
        game->handleKeyUp(key);
        glutPostRedisplay();
    }
}

void specialDown(int key, int x, int y)
{
    (void)x;
    (void)y;

    if (game)
    {
        game->handleSpecialDown(key);
        glutPostRedisplay();
    }
}

void specialUp(int key, int x, int y)
{
    (void)x;
    (void)y;

    if (game)
    {
        game->handleSpecialUp(key);
        glutPostRedisplay();
    }
}

void update()
{
    if (game)
    {
        game->update();
        glutPostRedisplay();
    }
}

int main(int argc, char** argv)
{
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Maze Runner");

    // OpenGL setup
    glClearColor(0.06f, 0.08f, 0.11f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);
    glutIdleFunc(update);

    // Initialize game
    game = new Game();
    game->init();

    reshape(windowWidth, windowHeight);

    // Start main loop
    glutMainLoop();

    // Cleanup
    delete game;

    return 0;
}
