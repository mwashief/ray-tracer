#pragma once
#include <fstream>
#include <math.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <GL/glut.h>
#define PI 3.141592653589793238
using namespace std;

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

    Vector operator+(const Vector &rhs)
    {
        return Vector(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vector operator-(const Vector &rhs)
    {
        return Vector(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    double dot(const Vector &rhs)
    {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    Vector operator*(const Vector &rhs)
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

    Vector operator-(Point const &rhs)
    {
        return Vector(x - rhs.x, y - rhs.y, z - rhs.z);
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

class Ray
{
public:
    Point start;
    Vector dir;
    Ray() {}
    Ray(Point s, Vector d) : start(s), dir(d) {}
};

class Object
{
public:
    Point reference_point;
    double height, width, length;
    Point color;
    double coEfficients[4];

    int shine;
    Object() {}
    virtual void draw() { cout << "Drawing object" << endl; }
    virtual istream &takeInput(istream &is)
    {
        cout << "Object is taken." << endl;
        return is;
    }
    virtual ostream &printObject(ostream &os)
    {
        os << "This is a object." << endl;
        return os;
    }
    friend ostream &operator<<(ostream &os, Object obj)
    {
        return obj.printObject(os);
    }
    friend istream &operator>>(istream &is, Object &obj)
    {
        return obj.takeInput(is);
    }
};

class Sphere : public Object
{
public:
    static const int stacks = 50, slices = 50;
    void draw()
    {
        double delta = 2.0 * length / stacks;
        double angleUnit = 2.0 * PI / slices;
        vector<vector<Point>> sphere;
        for (double z = reference_point.z + length; z >= reference_point.z - length; z -= delta)
        {
            double change = abs(z - reference_point.z);
            double r = length * length - change * change;
            r = sqrt(r);
            sphere.push_back(vector<Point>());
            for (double angle = 0; angle < 2.0 * PI; angle += angleUnit)
                sphere.back().push_back(Point(r * cos(angle) + reference_point.x, r * sin(angle) + reference_point.y, z));
        }
        for (int i = 0; i < sphere.size() - 1; i++)
        {
            for (int j = 0; j < sphere[i].size(); j++)
            {
                int k = (j + 1) % sphere[i].size();

                glColor3f(floor(color.x * 255), floor(color.y * 255), floor(color.z * 255));
                glBegin(GL_QUADS);
                {
                    glVertex3f(sphere[i][j].x, sphere[i][j].y, sphere[i][j].z);
                    glVertex3f(sphere[i][k].x, sphere[i][k].y, sphere[i][k].z);
                    glVertex3f(sphere[i + 1][k].x, sphere[i + 1][k].y, sphere[i + 1][k].z);
                    glVertex3f(sphere[i + 1][j].x, sphere[i + 1][j].y, sphere[i + 1][j].z);
                }
                glEnd();
            }
        }
    }
    virtual istream &takeInput(istream &is)
    {
        is >> reference_point >> length >> color;
        for (int i = 0; i < 4; i++)
            is >> coEfficients[i];
        is >> shine;
        cout << "A sphere added." << endl;
        return is;
    }
};
