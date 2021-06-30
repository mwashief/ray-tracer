#pragma once
#include <fstream>
#include <math.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <GL/glut.h>
#define PI 3.141592653589793238
#define HALFPI 1.570796327
#define BUFFER_LENGTH 50
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
        this->x = 0.0;
        this->y = 0.0;
        this->z = 0.0;
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

extern vector<Object *> objects;

class Light
{
public:
    Point position, color;
    vector<vector<double>> buffer;

    friend istream &operator>>(istream &is, Light &light)
    {
        is >> light.position >> light.color;
        return is;
    }

    bool nearestObject(const Point &p)
    {
        Object *res = 0;
        double d = 2000;
        Vector dir(p.x - position.x, p.y - position.y, p.z - position.z);
        dir = dir.getUnitAlong();
        Ray ray(position, dir);
        Point dummy;
        for (Object *object : objects)
        {
            double dist = object->intersect(&ray, &dummy, 0);
            if (dist < 0)
                continue;
            if (dist < d)
            {
                res = object;
                d = dist;
            }
        }
        double actual = sqrt(pow(p.x - position.x, 2) + pow(p.y - position.y, 2) + pow(p.z - position.z, 2));
        if (actual <= d + 1e-2)
            return true;

        return false;
    }
};
extern vector<Light> lights;

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

    Vector getRefracted(Vector N, Vector I, double miu)
    {
        N = N.getUnitAlong();
        I = I.getUnitAlong();
        double ctheta = N.dot(I);
        double stheta = sqrt(1.0 - ctheta * ctheta);
        double sdelta = stheta * miu;
        if (sdelta > 1)
            return Vector(100, 100, 100);
        double delta = asin(sdelta);
        Vector NI = I - N * ctheta;
        NI = NI.getUnitAlong();
        double NR = cos(delta) / ctheta * stheta;
        Vector res = N - NI * (NR);
        //cout << stheta << " " << sdelta << " " << stheta / sdelta << endl;
        res = res.getUnitAlong();

        return res;
    }

    double intersect(Ray *ray, Point *c, int level)
    {
        auto r0 = center - ray->start;
        auto tp = r0.dot(ray->dir);
        auto r0Len = r0.getLength();
        if (tp < 0 || r0Len < tp)
            return -1.0;
        auto nearest = sqrt(r0Len * r0Len - tp * tp);
        if (nearest > radius)
            return -1.0;
        auto tprime = sqrt(radius * radius - nearest * nearest);
        if (tprime > tp)
            return -1.0;
        if (level == 0)
            return tp - tprime;
        c->x = color.x * coEfficients[0];
        c->y = color.y * coEfficients[0];
        c->z = color.z * coEfficients[0];
        auto Q = ray->start + ray->dir * (tp - tprime);
        auto N = Q - center;
        N = N.getUnitAlong();

        auto componentAlongN = -N.dot(ray->dir);
        auto NR = ray->dir + N * componentAlongN;
        auto R = N + NR;
        R = R.getUnitAlong();
        auto V = ray->dir * (-1);

        for (Light &light : lights)
        {
            Vector L = light.position - Q;
            L = L.getUnitAlong();
            if (L.dot(N) <= 0.0)
                continue;
            if (!light.nearestObject(Q))
                continue;
            c->x += light.color.x * coEfficients[1] * L.dot(N) + light.color.x * coEfficients[2] * pow(R.dot(V), shine);

            c->y += light.color.y * coEfficients[1] * L.dot(N) + light.color.y * coEfficients[2] * pow(R.dot(V), shine);

            c->z += light.color.z * coEfficients[1] * L.dot(N) + light.color.z * coEfficients[2] * pow(R.dot(V), shine);
        }

        if (level > 1)
        {
            int nearestObject = -1;
            double distance = 1000;
            Ray r(Q, R);
            for (int i = 0; i < objects.size(); i++)
            {
                Object *object = objects[i];
                if (object == this)
                    continue;
                double dis = object->intersect(&r, &Q, 0);
                if (dis < 0.0)
                    continue;
                if (dis < distance)
                {
                    distance = dis;
                    nearestObject = i;
                }
            }
            if (nearestObject != -1)
            {
                Point reflectedColor;
                objects[nearestObject]->intersect(&r, &reflectedColor, level - 1);
                c->x += coEfficients[3] * reflectedColor.x;
                c->y += coEfficients[3] * reflectedColor.y;
                c->z += coEfficients[3] * reflectedColor.z;
            }

            double miu = .8;
            auto refracted1 = getRefracted(N, ray->dir * (-1), miu) * (-1);
            if (refracted1.x <= 1.0)
            {
                auto outside = Q + refracted1 * (2.1 * radius);
                Ray complimentary(outside, refracted1 * (-1));
                auto extra = this->intersect(&complimentary, 0, 0);
                auto QQ = Q + refracted1 * (2.1 * radius - extra);
                auto refracted2 = getRefracted(QQ - center, refracted1, 1.0 / miu);

                nearestObject = -1;
                distance = 1000;
                r = Ray(QQ, refracted2);

                for (int i = 0; i < objects.size(); i++)
                {
                    Object *object = objects[i];
                    if (object == this)
                        continue;
                    double dis = object->intersect(&r, &Q, 0);
                    if (dis < 0.0)
                        continue;
                    if (dis < distance)
                    {
                        distance = dis;
                        nearestObject = i;
                    }
                }
                if (nearestObject != -1)
                {
                    Point refractedColor;
                    objects[nearestObject]->intersect(&r, &refractedColor, level - 1);
                    c->x += (1.0 - coEfficients[3]) * refractedColor.x;
                    c->y += (1.0 - coEfficients[3]) * refractedColor.y;
                    c->z += (1.0 - coEfficients[3]) * refractedColor.z;
                }
            }
        }

        c->x = min(1.0, c->x);
        c->y = min(1.0, c->y);
        c->z = min(1.0, c->z);

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
        double t = -ray->start.z / ray->dir.z;
        if (t < 0)
            return -1.0;
        if (level == 0)
            return t;

        double x = ray->start.x + t * ray->dir.x;
        double y = ray->start.y + t * ray->dir.y;
        if (x < -tileWidth * tileCount || x > tileWidth * tileCount || y < -tileWidth * tileCount || y > tileWidth * tileCount)
            return -1.0;

        x += tileWidth * tileCount;
        y += tileWidth * tileCount;
        int row = floor(x / tileWidth);
        row = row & 1;
        int column = floor(y / tileWidth);
        column = column & 1;
        int color = row ^ column;
        x -= tileWidth * tileCount;
        y -= tileWidth * tileCount;

        c->x = color * coEfficients[0];
        c->y = color * coEfficients[0];
        c->z = color * coEfficients[0];
        Point Q(x, y, 0);
        Vector N(0, 0, 1);
        if (ray->start.z < 0)
            N.z = -1;
        N = N.getUnitAlong();
        auto componentAlongN = -N.dot(ray->dir);
        auto NR = ray->dir + N * componentAlongN;
        auto R = N + NR;
        R = R.getUnitAlong();
        auto V = ray->dir * (-1);

        for (Light &light : lights)
        {
            Vector L = light.position - Q;
            L = L.getUnitAlong();
            if (L.dot(N) <= 0.0)
                continue;
            if (!light.nearestObject(Q))
                continue;
            c->x += light.color.x * coEfficients[1] * L.dot(N) + light.color.x * coEfficients[2] * pow(R.dot(V), shine);

            c->y += light.color.y * coEfficients[1] * L.dot(N) + light.color.y * coEfficients[2] * pow(R.dot(V), shine);

            c->z += light.color.z * coEfficients[1] * L.dot(N) + light.color.z * coEfficients[2] * pow(R.dot(V), shine);
        }
        if (level > 1)
        {
            int nearestObject = -1;
            double distance = 1000;
            Ray r(Q, R);
            for (int i = 0; i < objects.size(); i++)
            {
                Object *object = objects[i];
                if (object == this)
                    continue;
                double dis = object->intersect(&r, &Q, 0);
                if (dis < 0.0)
                    continue;
                if (dis < distance)
                {
                    distance = dis;
                    nearestObject = i;
                }
            }
            if (nearestObject != -1)
            {
                Point reflectedColor;
                objects[nearestObject]->intersect(&r, &reflectedColor, level - 1);
                c->x += coEfficients[3] * reflectedColor.x;
                c->y += coEfficients[3] * reflectedColor.y;
                c->z += coEfficients[3] * reflectedColor.z;
            }
        }

        c->x = min(1.0, c->x);
        c->y = min(1.0, c->y);
        c->z = min(1.0, c->z);

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
        if (res[0] + res[1] > 1.0 || res[0] < 0.0 || res[1] < 0.0 || res[2] < 0)
            return -1.0;
        if (level == 0)
            return res[2];

        c->x = color.x * coEfficients[0];
        c->y = color.y * coEfficients[0];
        c->z = color.z * coEfficients[0];

        Point Q = ray->start + ray->dir * res[2];
        Vector X = corners[2] - corners[0];
        Vector Y = corners[1] - corners[0];
        Vector N = X * Y;
        N = N.getUnitAlong();
        if (N.dot(ray->dir) > 0)
            N = N * (-1);
        auto componentAlongN = -N.dot(ray->dir);
        auto NR = ray->dir + N * componentAlongN;
        auto R = N + NR;
        R = R.getUnitAlong();
        auto V = ray->dir * (-1);

        for (Light &light : lights)
        {
            Vector L = light.position - Q;
            L = L.getUnitAlong();
            if (L.dot(N) <= 0.0)
                continue;
            if (!light.nearestObject(Q))
                continue;
            c->x += light.color.x * coEfficients[1] * L.dot(N) + light.color.x * coEfficients[2] * pow(R.dot(V), shine);

            c->y += light.color.y * coEfficients[1] * L.dot(N) + light.color.y * coEfficients[2] * pow(R.dot(V), shine);

            c->z += light.color.z * coEfficients[1] * L.dot(N) + light.color.z * coEfficients[2] * pow(R.dot(V), shine);
        }
        if (level > 1)
        {
            int nearestObject = -1;
            double distance = 1000;
            Ray r(Q, R);
            for (int i = 0; i < objects.size(); i++)
            {
                Object *object = objects[i];
                if (object == this)
                    continue;
                double dis = object->intersect(&r, &Q, 0);
                if (dis < 0.0)
                    continue;
                if (dis < distance)
                {
                    distance = dis;
                    nearestObject = i;
                }
            }
            if (nearestObject != -1)
            {
                Point reflectedColor;
                objects[nearestObject]->intersect(&r, &reflectedColor, level - 1);
                c->x += coEfficients[3] * reflectedColor.x;
                c->y += coEfficients[3] * reflectedColor.y;
                c->z += coEfficients[3] * reflectedColor.z;
            }
        }

        c->x = min(1.0, c->x);
        c->y = min(1.0, c->y);
        c->z = min(1.0, c->z);

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
    Vector getNormal(double x, double y, double z)
    {
        double X = 2 * polynomial[0] * x +
                   polynomial[3] * y +
                   polynomial[5] * z +
                   polynomial[6];

        double Y = 2 * polynomial[1] * y +
                   polynomial[3] * x +
                   polynomial[4] * z +
                   polynomial[7];

        double Z = 2 * polynomial[2] * z +
                   polynomial[4] * y +
                   polynomial[5] * x +
                   polynomial[8];
        return Vector(X, Y, Z);
    }
    double intersect(Ray *ray, Point *c, int level)
    {
        double a = polynomial[0] * ray->dir.x * ray->dir.x +
                   polynomial[1] * ray->dir.y * ray->dir.y +
                   polynomial[2] * ray->dir.z * ray->dir.z +
                   polynomial[3] * ray->dir.x * ray->dir.y +
                   polynomial[4] * ray->dir.y * ray->dir.z +
                   polynomial[5] * ray->dir.x * ray->dir.z;

        double b = 2.0 * polynomial[0] * ray->dir.x * ray->start.x +
                   2.0 * polynomial[1] * ray->dir.y * ray->start.y +
                   2.0 * polynomial[2] * ray->dir.z * ray->start.z +
                   polynomial[3] * (ray->dir.x * ray->start.y + ray->dir.y * ray->start.x) +
                   polynomial[4] * (ray->dir.y * ray->start.z + ray->dir.z * ray->start.y) +
                   polynomial[5] * (ray->dir.x * ray->start.z + ray->dir.z * ray->start.x) +
                   polynomial[6] * ray->dir.x +
                   polynomial[7] * ray->dir.y +
                   polynomial[8] * ray->dir.z;

        double cons = polynomial[0] * ray->start.x * ray->start.x +
                      polynomial[1] * ray->start.y * ray->start.y +
                      polynomial[2] * ray->start.z * ray->start.z +
                      polynomial[3] * ray->start.x * ray->start.y +
                      polynomial[4] * ray->start.y * ray->start.z +
                      polynomial[5] * ray->start.x * ray->start.z +
                      polynomial[6] * ray->start.x +
                      polynomial[7] * ray->start.y +
                      polynomial[8] * ray->start.z +
                      polynomial[9];
        double bSquare = b * b;
        double ac4 = 4.0 * a * cons;
        if (abs(a) < 1e-9 || ac4 > bSquare)
            return -1.0;
        bSquare = sqrt(bSquare - ac4);
        vector<double> tt;
        tt.push_back((-b + bSquare) / (2.0 * a));
        tt.push_back((-b - bSquare) / (2.0 * a));
        if (tt[0] > tt[1])
            swap(tt[0], tt[1]);

        double t = -1.0;
        for (int i = 0; i < 2; i++)
        {
            if (tt[i] < 0)
                continue;
            Point Q = ray->start + ray->dir * tt[i];
            if (abs(dimension.x) > 1e-6 && reference.x + dimension.x < Q.x)
                continue;
            if (abs(dimension.y) > 1e-6 && reference.y + dimension.y < Q.y)
                continue;
            if (abs(dimension.z) > 1e-6 && reference.z + dimension.z < Q.z)
                continue;
            if (abs(dimension.x) > 1e-6 && reference.x > Q.x)
                continue;
            if (abs(dimension.y) > 1e-6 && reference.y > Q.y)
                continue;
            if (abs(dimension.z) > 1e-6 && reference.z > Q.z)
                continue;
            t = tt[i];
            break;
        }

        if (level == 0 || t < 0)
            return t;

        Point Q = ray->start + ray->dir * t;
        c->x = color.x * coEfficients[0];
        c->y = color.y * coEfficients[0];
        c->z = color.z * coEfficients[0];
        Vector N = getNormal(Q.x, Q.y, Q.z);
        N = N.getUnitAlong();
        if (N.dot(ray->dir) > 0)
            N = N * (-1);
        auto componentAlongN = -N.dot(ray->dir);
        auto NR = ray->dir + N * componentAlongN;
        auto R = N + NR;
        R = R.getUnitAlong();
        auto V = ray->dir * (-1);

        for (Light &light : lights)
        {
            Vector L = light.position - Q;
            L = L.getUnitAlong();
            if (L.dot(N) <= 0.0)
                continue;
            if (!light.nearestObject(Q))
                continue;
            c->x += light.color.x * coEfficients[1] * L.dot(N) + light.color.x * coEfficients[2] * pow(R.dot(V), shine);

            c->y += light.color.y * coEfficients[1] * L.dot(N) + light.color.y * coEfficients[2] * pow(R.dot(V), shine);

            c->z += light.color.z * coEfficients[1] * L.dot(N) + light.color.z * coEfficients[2] * pow(R.dot(V), shine);
        }

        if (level > 1)
        {
            int nearestObject = -1;
            double distance = 1000;
            Ray r(Q, R);
            for (int i = 0; i < objects.size(); i++)
            {
                Object *object = objects[i];
                if (object == this)
                    continue;
                double dis = object->intersect(&r, &Q, 0);
                if (dis < 0.0)
                    continue;
                if (dis < distance)
                {
                    distance = dis;
                    nearestObject = i;
                }
            }
            if (nearestObject != -1)
            {
                Point reflectedColor;
                objects[nearestObject]->intersect(&r, &reflectedColor, level - 1);
                c->x += coEfficients[3] * reflectedColor.x;
                c->y += coEfficients[3] * reflectedColor.y;
                c->z += coEfficients[3] * reflectedColor.z;
            }
        }

        c->x = min(1.0, c->x);
        c->y = min(1.0, c->y);
        c->z = min(1.0, c->z);

        return t;
    }

    istream &takeInput(istream &is)
    {
        for (int i = 0; i < 10; i++)
            is >> polynomial[i];
        swap(polynomial[4], polynomial[5]);
        is >> reference >> dimension;
        cout << "A quadric surface added" << endl;
        return is;
    }
};
