#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <GL/glut.h>
#include "Vector.h"
#include "Point.h"
#include "Triangle.h"
#include "Light.h"
using namespace std;

#define PI (3.1415926)
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

int recursionLevel, dimension, drawaxes;
int toggleOutline = 0;
int iniX, iniY;
bool fullScreen = 0;
Point cameraPosition(-200, -100, 0);
Vector gaze(3, 1, 0), tempGaze, head(0, 0, 1), tempHead;
vector<Triangle> mesh;
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

void drawHalfSphere(double radius, int slices, int stacks, int side)
{
    Point points[100][100];
    int i, j;
    double h, r;
    //generate points
    for (i = 0; i <= stacks; i++)
    {
        h = radius * sin(((double)i / (double)stacks) * (PI / 2));
        r = radius * cos(((double)i / (double)stacks) * (PI / 2));
        for (j = 0; j <= slices; j++)
        {
            points[i][j].z = r * cos(((double)j / (double)slices) * 2 * PI);
            points[i][j].y = r * sin(((double)j / (double)slices) * 2 * PI);
            points[i][j].x = side * h;
        }
    }
    //draw quads using generated points
    for (i = 0; i < stacks; i++)
    {
        for (j = 0; j < slices; j++)
        {
            glColor3f(255, 255, 255);
            glBegin(GL_QUADS);
            {
                //upper hemisphere
                glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
            }
            glEnd();
        }
    }
}

void drawInvertedHalfSphere(double radius, int slices, int stacks, int side)
{
    Point points[100][100];
    int i, j;
    double h, r;
    //generate points
    for (i = 0; i <= stacks; i++)
    {
        h = radius * sin(((double)i / (double)stacks) * (PI / 2));
        r = radius + radius * (1 - cos(((double)i / (double)stacks) * (PI / 2)));
        for (j = 0; j <= slices; j++)
        {
            points[i][j].z = r * cos(((double)j / (double)slices) * 2 * PI);
            points[i][j].y = r * sin(((double)j / (double)slices) * 2 * PI);
            points[i][j].x = side * h;
        }
    }
    //draw quads using generated points
    for (i = 0; i < stacks; i++)
    {
        for (j = 0; j < slices; j++)
        {
            glColor3f(255, 255, 255);
            glBegin(GL_QUADS);
            {
                //upper hemisphere
                glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
            }
            glEnd();
        }
    }
}

void drawPipe(double r, int slices)
{
    Point points[100];
    int i, j;
    for (j = 0; j <= slices; j++)
    {
        points[j].z = r * cos(((double)j / (double)slices) * 2 * PI);
        points[j].y = r * sin(((double)j / (double)slices) * 2 * PI);
        points[j].x = 0;
    }
    //draw quads using generated points
    for (j = 0; j < slices; j++)
    {
        glColor3f(255, 255, 255);
        glBegin(GL_QUADS);
        {
            //upper hemisphere
            glVertex3f(0, points[j].y, points[j].z);
            glVertex3f(100, points[j].y, points[j].z);
            glVertex3f(100, points[j + 1].y, points[j + 1].z);
            glVertex3f(0, points[j + 1].y, points[j + 1].z);
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

    for (Triangle &triangle : mesh)
    {
        glColor3f(floor(triangle.color.x * 255), floor(triangle.color.y * 255), floor(triangle.color.z * 255));
        glBegin(GL_TRIANGLES);
        {
            for (int i = 0; i < 3; i++)
                glVertex3f(triangle.corner[i].x, triangle.corner[i].y, triangle.corner[i].z);
        }
        glEnd();
    }

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

void addToMesh(Point a, Point b, Point c, Point color, double ambient, double diffuse, double specular, double recur, double shininess)
{
    Triangle triangle;
    triangle.corner[0] = a;
    triangle.corner[1] = b;
    triangle.corner[2] = c;
    triangle.color = color;
    triangle.ambient = ambient;
    triangle.diffuse = diffuse;
    triangle.specular = specular;
    triangle.recur = recur;
    triangle.shininess = shininess;
    mesh.push_back(triangle);
}

void addRectangle(Point a, Point b, Point c, Point d, Point color, double ambient, double diffuse, double specular, double recur, double shininess)
{
    addToMesh(a, b, c, color, ambient, diffuse, specular, recur, shininess);
    addToMesh(d, b, c, color, ambient, diffuse, specular, recur, shininess);
}

void addTriangle()
{
    Point a, b, c, color;
    double ambient, diffuse, specular, recur, shininess;
    cin >> a >> b >> c;
    cin >> color;
    cin >> ambient >> diffuse >> specular >> recur >> shininess;
    addToMesh(a, b, c, color, ambient, diffuse, specular, recur, shininess);
    cout << "triangle added" << endl;
}

void addSphere(int stacks, int slices)
{
    Point center, color;
    double radius, ambient, diffuse, specular, recur, shininess;
    cin >> center;
    cin >> radius;
    cin >> color;
    cin >> ambient >> diffuse >> specular >> recur >> shininess;
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
            addRectangle(sphere[i][j], sphere[i][k], sphere[i + 1][j], sphere[i + 1][k], color, ambient, diffuse, specular, recur, shininess);
        }
    }
    cout << "sphere added" << endl;
}

void addGeneral()
{
    double params[10];
    double ref[6];
    Point color;
    double ambient, diffuse, specular, recur, shininess;

    for (int i = 0; i < 10; i++)
        cin >> params[i];
    for (int i = 0; i < 6; i++)
        cin >> ref[i];
    cin >> color;
    cin >> ambient >> diffuse >> specular >> recur >> shininess;
    cout << "general added" << endl;
}

void addFloor()
{
    for (int y = 500; y > -500; y -= 20)
    {
        int col = y + 1000;
        col /= 20;
        col %= 2;
        for (int x = -500; x < 500; x += 20)
        {
            addRectangle(Point(x, y, 0), Point(x + 20, y, 0), Point(x, y - 20, 0), Point(x + 20, y - 20, 0), Point(col, col, col), 1, 1, 1, 1, 3);
            col = 1 - col;
        }
    }
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

    addFloor();
    int objects, lightSources;
    cin >> objects;
    for (int object = 0; object < objects; object++)
    {
        string objectType;
        cin >> objectType;
        if (objectType == "sphere")
            addSphere(10, 10);
        else if (objectType == "triangle")
            addTriangle();
        else
            addGeneral();
    }
    cin >> lightSources;
    for (int light = 0; light < lightSources; light++)
    {
        Light l;
        cin >> l;
        lights.push_back(l);
    }
    cout << mesh.size() << endl;

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
    gluPerspective(45, ratio, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    cin >> recursionLevel >> dimension;
    glutInitWindowSize(dimension, dimension);
    glutInitWindowPosition(300, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Ray Tracing");
    init();
    glLineWidth(2.0f);
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
