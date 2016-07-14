#ifndef MAZE_H_INCLUDED
#define MAZE_H_INCLUDED

#include <queue>
#include <vector>
#include <iostream>
#include <fstream>
#include "Point.h"
using namespace std;

const int INTERVAL = 1;
extern int counter;
extern Point pa[3000];
extern int pcounter;

class Maze {
private:
    enum Flag{
        UNVISITED, DONE, PROCESSING
    };

    struct node {
        int x, y;
        double cost;
        int distance;
        double priority;
        node *prev;

        node(int x_pos = 0, int y_pos = 0, double oldCost = 0, node *p = NULL)
            :x(x_pos), y(y_pos), cost(oldCost + 1), distance(0), priority(0.0), prev(p)
        {
            int temp = x - x_end;
            temp = temp > 0 ? temp : -temp;
            distance += temp;
            temp = y - y_end;
            temp = temp > 0 ? temp : -temp;
            distance += temp;
            priority += cost + distance;
        }
    };

    struct MPoint {
        Flag flag;
        node *point;

        MPoint(Flag f = UNVISITED, node *p = NULL) : flag(f), point(p) {}
    };

    struct Cmp {
        bool operator() (node *p, node *q)
        {
            return p -> priority > q -> priority;
        }
    };

public:
    Maze(istream &is);
    void findPath(ostream &os);

    void printMaze()
    {
        for (int i = 0; i <= length + 1; ++i) {
            for (int j = 0; j <= width + 1; ++j) {
                cout << maze[i][j];
            }
            cout << endl;
        }
    }
    void extractPath();

private:
    priority_queue<node *, vector<node *>, Cmp> open;
    MPoint **seal;
    int length, width;
    char **maze;

    static int x_start, y_start;
    static int x_end, y_end;
    static const char ROAD, WALL, PATH;

    bool inMaze(int a, int b)
    {
        return (a > 0) && (b > 0) && (a <= length) & (b <= width);
    }



    void print(ostream &os, node *p)
    {
        static int i = 0;
        if (p) {
            print(os, p -> prev);
            ++i;
            if (i % INTERVAL == 0) {
                ++counter;
                os << p -> x << '\t' << p -> y << endl;
                pa[pcounter++] = Point(p -> x, p -> y);
            }
        }
    }

};


#endif // MAZE_H_INCLUDED
