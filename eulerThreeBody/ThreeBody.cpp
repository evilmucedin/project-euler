#include "lib/header.h"

#include <cmath>

#include <iostream>
#include <vector>

#include "GL/freeglut.h"
#include "GL/gl.h"

using namespace std;

struct Point {
  double x{};
  double y{};
  double vx{};
  double vy{};
  double fx{};
  double fy{};
  double m{1.0};
  char r = 0;
  char g = 0;
  char b = 0;
  char a = 255;
  double rx{};
  double ry{};

  void step(double dt) {
    double ax = fx / m;
    double ay = fy / m;
    vx += ax * dt;
    vy += ay * dt;
    x += vx * dt;
    y += vy * dt;
  }
};

ostream &operator<<(ostream &s, const Point &p) {
  s << "{" << p.x << ", " << p.y << "; " << p.vx << ", " << p.vy
    << ", m = " << p.m << "}";
  return s;
}

template <typename T> T sqr(T x) { return x * x; }

double dist2(const Point &a, const Point &b) {
  return sqr(a.x - b.x) + sqr(a.y - b.y);
}

double dist(const Point &a, const Point &b) { return sqrt(dist2(a, b)); }

static const double WX = 1000;
static const double WY = 1000;

void draw() {
  double maxX = -1000;
  double minX = 1000;
  double maxY = -1000;
  double minY = 1000;

  vector<Point> points(3);
  for (auto &p : points) {
    p.x = rand() % 100;
    p.y = rand() % 100;
  }
  points[0].r = 255;
  points[1].g = 255;
  points[2].b = 255;
  while (true) {
    /*
    for (auto &p : points) {
      cout << p << endl;
    }
    */
    for (auto &p : points) {
      p.fx = 0;
      p.fy = 0;
    }
    for (size_t i = 0; i < points.size(); ++i) {
      auto &pi = points[i];
      for (size_t j = i + 1; j < points.size(); ++j) {
        auto &pj = points[j];
        double d = dist(pi, pj);
        double force = pi.m * pj.m / sqr(d);
        double dx = pi.x - pj.x;
        double dy = pi.y - pj.y;
        double fx = dx * force / d;
        double fy = dy * force / d;
        pi.fx -= fx;
        pi.fy -= fy;
        pj.fx += fx;
        pj.fy += fy;
      }
    }
    for (auto &p : points) {
      p.step(0.001);
    }

    for (const auto &p : points) {
      maxX = max(maxX, p.x);
      minX = min(minX, p.x);
      maxY = max(maxY, p.y);
      minY = min(minY, p.y);
    }

    for (auto& p : points) {
      p.rx = (p.x - minX)*WX/(maxX - minX);
      p.ry = (p.y - minY)*WY/(maxY - minY);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WX, 0, WY, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3ub(255, 255, 255);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(2, GL_DOUBLE, sizeof(Point), &points[0].rx);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Point), &points[0].r);
    glPointSize(10.0);
    glDrawArrays(GL_POINTS, 0, points.size());
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glFlush();
    glutSwapBuffers();
  }
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE);
  glutInitWindowSize(WX, WY);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("OpenGL - three body problem");
  glutDisplayFunc(draw);
  glutMainLoop();
  return 0;
}
