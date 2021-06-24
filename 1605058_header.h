#pragma once
#include <fstream>
#include <math.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <GL/glut.h>
#define PI 3.141592653589793238
using namespace std;
ofstream check;
double determinant(vector<vector<double>> &mat)
{

    double res = 0;
    for (int i = 0; i < 3; i++)
    {
        int j = (i + 1) % 3;
        int k = (i + 2) % 3;
        res += mat[0][i] * (mat[1][j] * mat[2][k] - mat[1][k] * mat[2][j]);
    }

    return res;
}

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
    Point color;
    double coEfficients[4];
    int shine;
    Object() {}
    virtual void draw() {}
    virtual double intersect(Ray *ray, Point *c, int level)
    {
        return -1;
    }
    virtual istream &takeInput(istream &is)
    {
        cout << "Object is taken." << endl;
        return is;
    }
    virtual ostream &printObject(ostream &os)
    {
        os << "This is an object." << endl;
        return os;
    }
    friend ostream &operator<<(ostream &os, Object obj)
    {
        return obj.printObject(os);
    }
    friend istream &operator>>(istream &is, Object &obj)
    {
        obj.takeInput(is);
        is >> obj.color;
        for (int i = 0; i < 4; i++)
            is >> obj.coEfficients[i];
        is >> obj.shine;
        return is;
    }
};

class Sphere : public Object
{
public:
    Point center;
    double radius;
    static const int stacks = 50, slices = 50;
    void draw()
    {
        double delta = 2.0 * radius / stacks;
        double angleUnit = 2.0 * PI / slices;
        vector<vector<Point>> sphere;
        for (double z = center.z + radius; z >= center.z - radius; z -= delta)
        {
            double change = abs(z - center.z);
            double r = radius * radius - change * change;
            r = sqrt(r);
            sphere.push_back(vector<Point>());
            for (double angle = 0; angle < 2.0 * PI; angle += angleUnit)
                sphere.back().push_back(Point(r * cos(angle) + center.x, r * sin(angle) + center.y, z));
        }
        for (int i = 0; i < sphere.size() - 1; i++)
        {
            for (int j = 0; j < sphere[i].size(); j++)
            {
                int k = (j + 1) % sphere[i].size();

                glColor3f(color.x, color.y, color.z);
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

    double intersect(Ray *ray, Point *c, int level)
    {
        c->x = color.x;
        c->y = color.y;
        c->z = color.z;
        auto r0 = center - ray->start;
        auto tp = r0.dot(ray->dir);
        auto r0Len = r0.getLength();
        if (tp < 0 || r0Len < tp)
            return -1.0;
        auto nearest = sqrt(r0Len * r0Len - tp * tp);
        if (nearest > radius)
            return -1.0;
        auto tprime = sqrt(radius * radius - nearest * nearest);
        return tp - tprime;
    }

    istream &takeInput(istream &is)
    {
        is >> center >> radius;
        cout << "A sphere added." << endl;
        return is;
    }
};

class Floor : public Object
{
public:
    double tileWidth;
    int tileCount;
    Floor()
    {
        tileCount = 25;
        tileWidth = 20.0;
    }
    void draw()
    {
        double range = tileWidth * tileCount;
        int start = 1;
        for (double y = range; y > -range; y -= tileWidth)
        {
            int now = start;
            for (double x = -range; x < range; x += tileWidth)
            {
                glColor3f(now, now, now);
                glBegin(GL_QUADS);
                {
                    glVertex3f(x, y, 0);
                    glVertex3f(x + tileWidth, y, 0);
                    glVertex3f(x + tileWidth, y - tileWidth, 0);
                    glVertex3f(x, y - tileWidth, 0);
                }
                glEnd();
                now = 1 - now;
            }
            start = 1 - start;
        }
    }

    double intersect(Ray *ray, Point *c, int level)
    {
        if (ray->dir.z == 0)
            return -1.0;
        double t = -ray->start.z / ray->dir.z;
        double x = ray->start.x + t * ray->dir.x;
        double y = ray->start.y + t * ray->dir.y;
        x += tileWidth * tileCount;
        y += tileWidth * tileCount;
        int row = floor(x / tileWidth);
        row = row & 1;
        int column = floor(y / tileWidth);
        column = column & 1;
        int color = row ^ column;
        c->x = color;
        c->y = color;
        c->z = color;
        return t;
    }
};

class Triangle : public Object
{
public:
    Point corners[3];
    void draw()
    {
        glColor3f(color.x, color.y, color.z);
        glBegin(GL_TRIANGLES);
        {
            for (int i = 0; i < 3; i++)
                glVertex3f(corners[i].x, corners[i].y, corners[i].z);
        }
        glEnd();
    }
    double intersect(Ray *ray, Point *c, int level)
    {

        vector<vector<double>> mat(3, vector<double>(3, 0.0));

        mat[0][0] = corners[0].x - corners[1].x;
        mat[0][1] = corners[0].x - corners[2].x;
        mat[0][2] = ray->dir.x;

        mat[1][0] = corners[0].y - corners[1].y;
        mat[1][1] = corners[0].y - corners[2].y;
        mat[1][2] = ray->dir.y;

        mat[2][0] = corners[0].z - corners[1].z;
        mat[2][1] = corners[0].z - corners[2].z;
        mat[2][2] = ray->dir.z;
        double A = determinant(mat);
        if (abs(A) < 1e-9)
            return -1.0;

        vector<double> column(3);
        column[0] = corners[0].x - ray->start.x;
        column[1] = corners[0].y - ray->start.y;
        column[2] = corners[0].z - ray->start.z;
        vector<double> res;
        for (int j = 0; j < 3; j++)
        {
            vector<double> temp(3);
            for (int i = 0; i < 3; i++)
            {
                temp[i] = mat[i][j];
                mat[i][j] = column[i];
            }
            res.push_back(determinant(mat) / A);

            for (int i = 0; i < 3; i++)
                mat[i][j] = temp[i];
        }
        if (res[0] + res[1] > 1.0 || res[0] < 0.0 || res[1] < 0.0)
            return -1.0;
        c->x = color.x;
        c->y = color.y;
        c->z = color.z;

        return res[2];
    }

    istream &takeInput(istream &is)
    {
        for (int i = 0; i < 3; i++)
            is >> corners[i];
        cout << "A triangle added" << endl;
        return is;
    }

    ostream &printObject(ostream &os)
    {
        os << "Triangle: " << endl;
        for (int i = 0; i < 3; i++)
            os << corners[i] << endl;

        return os;
    }
};

class Quadric : public Object
{
public:
    double polynomial[10];
    Point reference;
    Point dimension;
    istream &takeInput(istream &is)
    {
        for (int i = 0; i < 10; i++)
            is >> polynomial[i];
        is >> reference >> dimension;
        cout << "A quadric surface added" << endl;
        return is;
    }
};

class Light
{
public:
    Point position, color;
    friend istream &operator>>(istream &is, Light &light)
    {
        is >> light.position >> light.color;
        return is;
    }
};