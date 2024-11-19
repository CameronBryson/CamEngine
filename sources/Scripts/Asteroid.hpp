#pragma once
struct Asteroid
{
    Asteroid(unsigned short target, float speed) : target(target), speed(speed)
    {
    }
    unsigned short target;
    float speed;
};