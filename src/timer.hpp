#pragma once

class Timer
{
private:
    float previous_time;

public:
    Timer();

    void reset();
    float delta();

    static float time();
};
