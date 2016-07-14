#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
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

Point pa[3000];
int pcounter = 0;
const int DIS = 20;
const double COS_Boundary = 0.5;
const int SKIP = 40;
int ccounter = 0;

int main()
{
    ifstream fin("path.out");
    ofstream fout2("critical.out");
    stringstream stream2;
    fin >> pcounter;
    int x, y;
    for (int i = 0; i != pcounter; ++i) {
        fin >> x >> y;
        pa[i] = Point(x, y);
    }

    for (int i = 80; i + DIS * 2 < pcounter; ++i) {
        double cos_theta = Vector(pa[i + DIS], pa[i]) * Vector(pa[i + 2 * DIS], pa[i + DIS]);
        if (cos_theta < COS_Boundary && cos_theta > -1 * COS_Boundary) {
            stream2 << pa[i + DIS] << endl;
            ++ccounter;
            i += SKIP;
        }
    }

    fout2 << (ccounter + 1) << endl;
    int yfinal;
    while (stream2 >> x >> y) {
        yfinal = y;
        fout2 << x << ' ' << y << endl;
    }
    fout2 << 440 << ' ' << yfinal << endl;

    fin.close();
    fout2.close();
}
