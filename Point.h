#pragma once
#include <fstream>
#include "Vector.h"

class Point
{
public:
    double x, y, z;
    Point()
    {
    }
    Point(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    Point operator+(Vector const &rhs)
    {
        return Point(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Point operator-(Vector const &rhs)
    {
        return Point(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    friend std::ostream &operator<<(std::ostream &os, const Point &dt)
    {
        os << dt.x << " " << dt.y << " " << dt.z;
        return os;
    }

    friend std::istream &operator>>(std::istream &is, Point &dt)
    {
        is >> dt.x >> dt.y >> dt.z;
        return is;
    }
};
