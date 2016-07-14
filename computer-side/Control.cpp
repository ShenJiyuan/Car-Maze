#include "highgui.h"
#include "cv.h"
#include "Control.h"
extern int cnt;
extern int firstX, firstY, width, height;
extern CvPoint2D32f originPoints[4];
extern CvPoint2D32f destiPoints[4];
extern ComPort port;
extern OVERLAPPED Wol;

void mouse(int mouseevent, int x, int y, int flags, void* param);
void goAhead();
void turnLeft();
void turnRight();
void adjust(double headX, double headY, double tailX, double tailY, double destX, double destY);

void mouse(int mouseevent, int x, int y, int flags, void* param)
{
    if(mouseevent == CV_EVENT_LBUTTONUP && cnt < 4)
    {
        originPoints[cnt] = cvPoint2D32f(x, y);
        ++cnt;
        return;
    }
    if(mouseevent == CV_EVENT_LBUTTONUP && (cnt == 4 || cnt == 6))
    {
        firstX = x;
        firstY = y;
        ++cnt;
        return;
    }
    if(mouseevent == CV_EVENT_LBUTTONUP && (cnt == 5 || cnt == 7))
    {
        width = x - firstX;
        height = y - firstY;
        ++cnt;
        return;
    }
}

void adjust(double headX, double headY, double tailX, double tailY, double destX, double destY)
{
    double cenX = (headX + tailX) / 2, cenY = (headY + tailY) / 2;
    double cross = ((destX - cenX) * (headY - tailY) - (destY - cenY) * (headX - tailX))
    /(sqrt((destX - cenX) * (destX - cenX) + (destY - cenY) * (destY - cenY)) * sqrt(((headY - tailY) * (headY - tailY)) + ((headX - tailX)) * (headX - tailX)));
    if(abs(cross) < 0.15)
    {
        goAhead();
        cout << "Go ahead." << endl;
    }
    else
    {
        if(cross > 0)
        {
            turnLeft();
            cout << "Turn left." << endl;
        }
        else
        {
            turnRight();
            cout << "Turn right." << endl;
        }
    }
}

void goAhead()
{
    DWORD  BytesToSend = 0;
    //ComPort port("\\\\.\\COM13");
    unsigned char* temp;
    temp = new unsigned char[7];
    temp[0] = '$';
    temp[1] = '0';
    temp[2] = '0';
    temp[3] = '0';
    temp[4] = '0';
    temp[5] = '1';
    temp[6] = '#';
    Wol.Internal = 0;
    Wol.InternalHigh = 0;
    Wol.Offset = 0;
    Wol.OffsetHigh = 0;
    Wol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    long unsigned int len = 7;
    WriteFile(port.hCom, temp, len, NULL, &Wol);
    temp[0] = '$';
    temp[1] = '0';
    temp[2] = '0';
    temp[3] = '0';
    temp[4] = '7';
    temp[5] = '4';
    temp[6] = '#';
    /*Wol.Internal = 0;
    Wol.InternalHigh = 0;
    Wol.Offset = 0;
    Wol.OffsetHigh = 0;*/
    Wol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    //len = 7;
    WriteFile(port.hCom, temp, len, NULL, &Wol);
}

void turnLeft()
{
    DWORD  BytesToSend = 0;
    //ComPort port("\\\\.\\COM13");
    unsigned char* temp;
    temp = new unsigned char[7];
    temp[0] = '$';
    temp[1] = '0';
    temp[2] = '1';
    temp[3] = '6';
    temp[4] = '4';
    temp[5] = '2';
    temp[6] = '#';
    Wol.Internal = 0;
    Wol.InternalHigh = 0;
    Wol.Offset = 0;
    Wol.OffsetHigh = 0;
    Wol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    long unsigned int len = 7;
    WriteFile(port.hCom, temp, len, NULL, &Wol);
}

void turnRight()
{
    DWORD  BytesToSend = 0;
    //ComPort port("\\\\.\\COM13");
    unsigned char* temp;
    temp = new unsigned char[7];
    temp[0] = '$';
    temp[1] = '0';
    temp[2] = '0';
    temp[3] = '6';
    temp[4] = '4';
    temp[5] = '2';
    temp[6] = '#';
    Wol.Internal = 0;
    Wol.InternalHigh = 0;
    Wol.Offset = 0;
    Wol.OffsetHigh = 0;
    Wol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    long unsigned int len = 7;
    WriteFile(port.hCom, temp, len, NULL, &Wol);
}
