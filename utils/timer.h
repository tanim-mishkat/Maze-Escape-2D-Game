#ifndef TIMER_H
#define TIMER_H

class Timer
{
public:
    Timer();

    void start();
    void stop();
    void reset();
    void resume();

    int getElapsedMs() const;
    float getElapsedSeconds() const;
    float getAnimationTime() const;

    void updateAnimationTime();

private:
    int startTimeMs;
    int elapsedTimeMs;
    float animationTime;
    bool running;
};

#endif // TIMER_H
