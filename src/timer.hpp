#pragma once

class Timer
{
private:
    float previous_time;

public:
    Timer();

    float delta();
};
