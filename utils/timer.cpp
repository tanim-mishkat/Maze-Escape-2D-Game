#include "timer.h"
#include <GL/freeglut.h>

Timer::Timer()
    : startTimeMs(0), elapsedTimeMs(0), animationTime(0.0f), running(false),
      animationPauseOffset(0), lastPauseStartMs(0)
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
        int now = glutGet(GLUT_ELAPSED_TIME);
        elapsedTimeMs = now - startTimeMs;
        lastPauseStartMs = now;
        running = false;
    }
}

void Timer::reset()
{
    startTimeMs = 0;
    elapsedTimeMs = 0;
    running = false;
    animationPauseOffset = 0;
    lastPauseStartMs = 0;
}

void Timer::resume()
{
    if (!running && elapsedTimeMs > 0)
    {
        int now = glutGet(GLUT_ELAPSED_TIME);
        animationPauseOffset += now - lastPauseStartMs;
        startTimeMs = now - elapsedTimeMs;
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
    animationTime = (glutGet(GLUT_ELAPSED_TIME) - animationPauseOffset) / 1000.0f;
}
