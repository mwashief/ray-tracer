#pragma once
#include <fstream>
#include <math.h>

class Vector
{
public:
    double x, y, z;
    Vector()
    {
    }
    Vector(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    double getLength()
    {
        return sqrt(x * x + y * y + z * z);
    }

    Vector getUnitAlong()
    {
        double len = getLength();
        return Vector(x / len, y / len, z / len);
    }

    Vector operator+(Vector const &rhs)
    {
        return Vector(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vector operator-(Vector const &rhs)
    {
        return Vector(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    double dot(Vector const &rhs)
    {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    Vector operator*(Vector const &rhs)
    {
        double xx = y * rhs.z - z * rhs.y;
        double yy = z * rhs.x - x * rhs.z;
        double zz = x * rhs.y - y * rhs.x;
        return Vector(xx, yy, zz);
    }

    Vector operator*(double scale)
    {
        return Vector(x * scale, y * scale, z * scale);
    }

    friend std::ostream &operator<<(std::ostream &os, const Vector &dt)
    {
        os << dt.x << " " << dt.y << " " << dt.z;
        return os;
    }
};
