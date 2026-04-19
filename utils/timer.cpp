#include "timer.h"
#include <GL/freeglut.h>

Timer::Timer()
    : startTimeMs(0), elapsedTimeMs(0), animationTime(0.0f), running(false)
{
}

void Timer::start()
{
    startTimeMs = glutGet(GLUT_ELAPSED_TIME);
    running = true;
}

void Timer::stop()
{
    if (running)
    {
        elapsedTimeMs = glutGet(GLUT_ELAPSED_TIME) - startTimeMs;
        running = false;
    }
}

void Timer::reset()
{
    startTimeMs = 0;
    elapsedTimeMs = 0;
    running = false;
}

void Timer::resume()
{
    if (!running && elapsedTimeMs > 0)
    {
        startTimeMs = glutGet(GLUT_ELAPSED_TIME) - elapsedTimeMs;
        running = true;
    }
}

int Timer::getElapsedMs() const
{
    if (running)
    {
        return glutGet(GLUT_ELAPSED_TIME) - startTimeMs;
    }
    return elapsedTimeMs;
}

float Timer::getElapsedSeconds() const
{
    return getElapsedMs() / 1000.0f;
}

float Timer::getAnimationTime() const
{
    return animationTime;
}

void Timer::updateAnimationTime()
{
    animationTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}
