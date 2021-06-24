#pragma once
#include "Point.h"
#include <fstream>
#include <math.h>

class Triangle
{
public:
    Point corner[3];
    Point color;
    double ambient, diffuse, specular, recur, shininess;
    Triangle()
    {
    }
    friend std::ostream &operator<<(std::ostream &os, const Triangle &t)
    {
        os << "Points:" << std::endl;
        for (int i = 0; i < 3; i++)
            os << t.corner[i] << std::endl;
        os << "Color:" << std::endl;
        os << t.color << std::endl;
        return os;
    }
};
