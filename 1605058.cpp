#pragma GCC optimize("Ofast")
#pragma GCC optimization("unroll-loops, no-stack-protector")
#pragma GCC target("avx,avx2,fma")

#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <GL/glut.h>
#include "1605058_header.h"
#include "bitmap_image.hpp"

using namespace std;

#define pb push_back
#define mp make_pair
#define fr first
#define sc second
#define fastio ios_base::sync_with_stdio(0)
#define untie cin.tie(0)
#define rep(i, n) for (int i = 0; i < n; i++)
#define repe(i, n) for (int i = 1; i <= n; i++)
#define rrep(i, n) for (int i = n - 1; i >= 0; i--)
#define rrepe(i, n) for (int i = n; i > 0; i--)
#define ms(a, b) memset(a, b, sizeof a)
#define MOD 1000000007

typedef long long ll;
typedef unsigned long long ull;

using pii = pair<int, int>;
using pll = pair<ll, ll>;
using vb = vector<bool>;
using vi = vector<int>;
using vl = vector<ll>;
using vvb = vector<vector<bool>>;
using vvi = vector<vector<int>>;
using vvl = vector<vector<ll>>;
using vpii = vector<pair<int, int>>;
using mii = map<int, int>;
using tddd = tuple<double, double, double>;

ifstream scene;
int recursionLevel, dimension, drawaxes, height, width;
int toggleOutline = 0;
int iniX, iniY;
bool fullScreen = 0;
Point cameraPosition(-100, 100, 50);
Vector gaze(1, -1, 0), tempGaze, head(0, 0, 1), tempHead;
vector<Object *> objects;
vector<Light> lights;

Vector getRight()
{
    return gaze * head;
}

Vector getLeft()
{
    return head * gaze;
}

void drawAxes()
{
    if (drawaxes == 1)
    {
        glColor3f(255, 255, 255);
        glBegin(GL_LINES);
        {
            glVertex3f(500, 0, 0);
            glVertex3f(-500, 0, 0);

            glVertex3f(0, -500, 0);
            glVertex3f(0, 500, 0);

            glVertex3f(0, 0, 500);
            glVertex3f(0, 0, -500);
        }
        glEnd();
    }
}

void drawSS()
{
    if (toggleOutline)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (auto object : objects)
        object->draw();

    for (Light &light : lights)
    {
        glColor3f(floor(light.color.x * 255), floor(light.color.y * 255), floor(light.color.z * 255));
        glBegin(GL_QUADS);
        {
            glVertex3f(light.position.x - .5, light.position.y + .5, light.position.z);
            glVertex3f(light.position.x + .5, light.position.y + .5, light.position.z);
            glVertex3f(light.position.x + .5, light.position.y - .5, light.position.z);
            glVertex3f(light.position.x - .5, light.position.y - .5, light.position.z);
        }

        glEnd();
    }
}

void capture()
{
    double planeDistance = dimension / 2.0 / tan(PI * 35 / 180);
    auto left = getLeft();
    auto right = getRight();
    auto topLeft = cameraPosition + gaze * planeDistance + left * (dimension / 2.0) + head * (dimension / 2.0);
    double du = 1.0, dv = 1.0;
    topLeft = topLeft + right * (.5 * du) - head * (.5 * dv);

    bitmap_image image(dimension, dimension);
    for (int i = 0; i < dimension; i++)
    {
        auto current = topLeft;
        for (int j = 0; j < dimension; j++)
        {
            auto dir = current - cameraPosition;
            dir = dir.getUnitAlong();
            Ray ray(cameraPosition, dir);

            double t = 1e9;
            Point color(0, 0, 0);
            for (auto obj : objects)
            {
                Point c;
                double temp = obj->intersect(&ray, &c, 3);
                if (temp <= 0.0 || temp >= 1000.0)
                    continue;
                if (temp < t)
                {
                    t = temp;
                    color = c;
                }
            }
            color.x = min(1.0, color.x);
            color.y = min(1.0, color.y);
            color.z = min(1.0, color.z);
            image.set_pixel(j, i, floor(255 * color.x), floor(255 * color.y), floor(255 * color.z));
            current = current + right * du;
        }
        topLeft = topLeft - head * dv;
    }
    image.save_image("scene.bmp");
    cout << "Image generation done" << endl;
}

void keyboardListener(unsigned char key, int x, int y)
{
    switch (key)
    {

    case '1':
    {
        gaze = gaze + getLeft() * .005;
        gaze = gaze.getUnitAlong();
        break;
    }
    case '2':
    {
        gaze = gaze + getRight() * .005;
        gaze = gaze.getUnitAlong();
        break;
    }
    case '3':
    {
        Vector temp = gaze;
        gaze = gaze + head * .005;
        head = head - temp * .005;
        head = head.getUnitAlong();
        gaze = gaze.getUnitAlong();
        break;
    }
    case '4':
    {
        Vector temp = gaze;
        gaze = gaze - head * .005;
        head = head + temp * .005;
        head = head.getUnitAlong();
        gaze = gaze.getUnitAlong();
        break;
    }
    case '5':
    {
        head = head + getLeft() * .005;
        head = head.getUnitAlong();
        break;
    }
    case '6':
    {
        head = head + getRight() * .005;
        head = head.getUnitAlong();
        break;
    }
    case '0':
    {
        capture();
        break;
    }
    case ']':
    {
        toggleOutline = 1 - toggleOutline;
        break;
    }
    case 13:
    {
        if (fullScreen)
        {
            glutReshapeWindow(700, 600);
            glutPositionWindow(300, 50);

            fullScreen = 0;
        }
        else
        {
            glutFullScreen();
            fullScreen = 1;
        }
        break;
    }
    case ' ':
    {
        break;
    }
    default:
        break;
    }
}

void specialKeyListener(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_DOWN: //down arrow key
    {
        cameraPosition = cameraPosition - gaze;
        break;
    }
    case GLUT_KEY_UP: // up arrow key
    {
        cameraPosition = cameraPosition + gaze;
        break;
    }
    case GLUT_KEY_RIGHT:
    {
        Vector right = getRight();
        cameraPosition = cameraPosition + right;
        break;
    }
    case GLUT_KEY_LEFT:
    {
        Vector left = getLeft();
        cameraPosition = cameraPosition + left;
        break;
    }
    case GLUT_KEY_PAGE_UP:
    {
        cameraPosition = cameraPosition + head;
        break;
    }
    case GLUT_KEY_PAGE_DOWN:
    {
        cameraPosition = cameraPosition - head;
        break;
    }

    case GLUT_KEY_INSERT:
        break;

    case GLUT_KEY_HOME:
        break;
    case GLUT_KEY_END:
        break;

    default:
        break;
    }
}

void mouseListener(int button, int state, int x, int y)
{
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN)
        {
            iniX = x;
            iniY = y;
            tempGaze = gaze;
            tempHead = head;
        }
        break;

    case GLUT_RIGHT_BUTTON:
    {
        if (state == GLUT_DOWN)
            drawaxes = 1 - drawaxes;
        break;
    }

    case GLUT_MIDDLE_BUTTON:
        //........
        break;

    default:
        break;
    }
}

void mouseDrag(int x, int y)
{

    double xChange = iniX - x;
    double yChange = iniY - y;
    Vector right = getRight();
    Vector gazeHorizontal = tempGaze + right * xChange * .001;
    gazeHorizontal = gazeHorizontal.getUnitAlong();

    Vector headShifted = tempHead + gazeHorizontal * yChange * .001;
    gazeHorizontal = gazeHorizontal + tempHead * (-yChange) * .001;
    head = headShifted.getUnitAlong();
    gaze = gazeHorizontal.getUnitAlong();
    //cout << iniX << " " << iniY << " " << x << " " << y << " " << gaze << " " << gaze.dot(head) << endl;
}

void display()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Point reference = cameraPosition + gaze;
    gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z, reference.x, reference.y, reference.z, head.x, head.y, head.z);

    glMatrixMode(GL_MODELVIEW);
    drawAxes();
    drawSS();
    glutSwapBuffers();
}

void animate()
{
    glutPostRedisplay();
}

void init()
{
    drawaxes = 0;
    glClearColor(0, 0, 0, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 1, 1, 1000.0);

    gaze = gaze.getUnitAlong();
    head = head.getUnitAlong();
}

void reshape(int w, int h)
{
    if (h == 0)
        h = 1;
    float ratio = 1.0 * w / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(70, ratio, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
}

void loadData()
{
    scene >> recursionLevel >> dimension;
    int objectCount, lightSources;
    scene >> objectCount;
    Object *obj;
    for (int object = 0; object < objectCount; object++)
    {
        string objectType;
        scene >> objectType;
        if (objectType == "sphere")
            obj = new Sphere();
        else if (objectType == "triangle")
            obj = new Triangle();
        else
            obj = new Quadric();
        scene >> *obj;
        objects.push_back(obj);
    }
    scene >> lightSources;
    while (lightSources--)
    {
        Light light;
        scene >> light;
        lights.push_back(light);
    }

    obj = new Floor();
    obj->coEfficients[0] = 0.4;
    obj->coEfficients[1] = 0.2;
    obj->coEfficients[2] = 0.2;
    obj->coEfficients[3] = 0.2;
    obj->shine = 10;
    objects.push_back(obj);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    scene.open("scene.txt");
    loadData();
    scene.close();
    glutInitWindowSize(dimension, dimension);
    glutInitWindowPosition(300, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Ray Tracing");
    init();
    //glLineWidth(2.0f);
    glEnable(GL_DEPTH_TEST);
    glutDisplayFunc(display);
    glutIdleFunc(animate);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);
    glutMotionFunc(mouseDrag);

    glutMainLoop();

    return 0;
}
