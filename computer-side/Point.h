#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED

#include <iostream>
#include <cmath>
using namespace std;

class Vector;

class Point {
    friend class Vector;
    friend ostream &operator<< (ostream &os, const Point &a)
    {
        os << a.x << '\t' << a.y;
        return os;
    }
    friend bool operator== (const Point &a, const Point &b)
    {
        return (a.x == b.x) && (a.y == b.y);
    }
private:
    double x, y;
public:
    Point(double a = 0, double b = 0) :x(a), y(b) {}
};

class Vector {
    friend ostream &operator<< (ostream &os, const Vector &x)
    {
        os << x.x << '\t' << x.y;
        return os;
    }
    friend double operator* (Vector a, Vector b)
    {
        a.modify();
        b.modify();
        return a.x * b.x + a.y * b.y;
    }
private:
    double x, y;
    double norm;

    void modify()
    {
        x /= norm;
        y /= norm;
        norm = 1;
    }


public:
    Vector(Point d = Point(0, 0), Point s = Point(0, 0))
    {
        x = d.x - s.x;
        y = d.y - s.y;
        norm = sqrt(x * x + y * y);
    }

};

#endif // POINT_H_INCLUDED
