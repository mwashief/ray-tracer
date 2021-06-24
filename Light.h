#pragma once
#include <fstream>
#include "Point.h"

class Light
{
public:
    Point position, color;
    friend std::ostream &operator<<(std::ostream &os, const Light &dt)
    {
        os << dt.position << std::endl
           << dt.color << std::endl;
        return os;
    }

    friend std::istream &operator>>(std::istream &is, Light &dt)
    {
        is >> dt.position >> dt.color;
        return is;
    }
};
