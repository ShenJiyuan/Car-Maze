#include "Maze.h"
extern int direction[8][2];
extern int counter;
int Maze::x_start = 0, Maze::y_start = 0, Maze::x_end = 0, Maze::y_end = 0;
const char Maze::ROAD = '1', Maze::WALL = '0', Maze::PATH = '1';
const double TIME1 = 0.000001, TIME2 = 0.000000000001;
ofstream fmaze("fmaze.out");


Maze::Maze(istream &is)
{
    is >> length >> width;
    //cout << length << width << endl;

    maze = new char * [length + 2];
    seal = new MPoint * [length + 2];
    for (int i = 0; i <= length + 1; ++i) {
        maze[i] = new char [width + 2];
        seal[i] = new MPoint [width + 2];
    }

    for (int i = 1; i <= length; ++i) {
        for (int j = 1; j <= width; ++j) {
            is >> maze[i][j];
            if (i >= length * 5 / 6 && i <= length && j >= 1 && j <= width / 6) {
                maze[i][j] = PATH;
            }
            if (i >= 1 && i <= length / 6 && j >= width * 3 / 4 && j <= width) {
                maze[i][j] = PATH;
            }
            fmaze << maze[i][j];
        }
        fmaze << endl;
    }

    for (int i = 0; i <= length + 1; ++i) {
        maze[i][0] = maze[i][width + 1] = WALL;
    }

    for (int j = 0; j <= width + 1; ++j) {
        maze[0][j] = maze[length + 1][j] = WALL;
    }

    //is >> x_start >> y_start >> x_end >> y_end;
    x_start = 40;
    y_start = 600;
    x_end = 440;
    y_end = 40;

}

void Maze::findPath(ostream &os)
{
    if (!inMaze(x_start, y_start) || maze[x_start][y_start] == WALL) {
        cout << "Invalid start point!" << endl;
        return;
    }

    if (!inMaze(x_end, y_end) || maze[x_end][y_end] == WALL) {
        cout << "Invalid end point!" << endl;
        return;
    }
    node *p = new node(x_start, y_start, -1);
    open.push(p);
    seal[x_start][y_start].flag = PROCESSING;
    seal[x_start][y_start].point = p;
    double time = 0.0;

    while(!open.empty()) {
        p = open.top();
        open.pop();
        int x = p -> x, y = p -> y;
        seal[x][y].flag = DONE;

        for (int i = 0; i != 4; ++i) {
            int x_temp = x + direction[i][0], y_temp = y + direction[i][1];

            if (!inMaze(x_temp, y_temp) || maze[x_temp][y_temp] != PATH || seal[x_temp][y_temp].flag == DONE) {
                continue;
            }

            if (seal[x_temp][y_temp].flag == UNVISITED) {
                if (x_temp == x_end && y_temp == y_end) {
                    print(os, p);
                    os << x_end << '\t' << y_end << endl;
                    return;
                }

                node *temp = new node(x_temp, y_temp, p -> cost - TIME1, p);
                seal[x_temp][y_temp].flag = PROCESSING;
                seal[x_temp][y_temp].point = temp;
                open.push(temp);
            }
            else {
                node *temp = seal[x_temp][y_temp].point;
                if (p -> cost + 1 < temp -> cost) {
                    temp -> cost = p -> cost + 1;
                    temp -> prev = p;
                    temp -> priority = temp -> cost + temp -> distance;
                }
            }
        }
    }

    cout << "Not existed!" << endl;
    return;

}
