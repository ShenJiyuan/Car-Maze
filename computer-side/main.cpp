#include <iostream>
#include <fstream>
#include "highgui.h"
#include "cv.h"
#include "Control.h"
#include "Maze.h"
#include <sstream>
using namespace std;

template<class T>
class Image {
public:
    IplImage* imgp;
    Image(IplImage* img=0) :imgp(img) {}
    ~Image()
    {
        imgp=0;
    }

    inline T* operator[] (const int rowIndx)
    {
        return ((T *)(imgp->imageData + rowIndx*imgp -> widthStep));
    }
};
typedef struct{
unsigned char b,g,r;
} RgbPixel;
typedef Image<RgbPixel> RgbImage;
typedef Image<unsigned char> BwImage;


int firstX, firstY, width, height;
void mouse(int mouseevent, int x, int y, int flags, void* param);
void goAhead();
void turnLeft();
void turnRight();
void adjust(double headX, double headY, double tailX, double tailY, double destX, double destY);
int cnt = 0;
CvPoint2D32f originPoints[4];
CvPoint2D32f destiPoints[4];
CvMat* transmat = cvCreateMat(3, 3, CV_32FC1);
CvCapture* cam;

void solveMaze(ofstream& fout);
void cvThin (IplImage* src, IplImage* dst, int iterations);

ComPort port("\\\\.\\COM3");
OVERLAPPED Wol;

int direction[8][2] = { {0, 1}, {-1, 0}, {0, -1}, {1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
int counter = 0;
Point pa[3000];
int pcounter = 0;
const int DIS = 20;
const double COS_Boundary = 0.3;
const int SKIP = 20;
int ccounter = 0;


int main()
{
    try {
    CvPoint trace[100], carHead, carTail;
    int n;
    ofstream fout("maze.out");
    destiPoints[0].x = 0;
    destiPoints[0].y = 0;
    destiPoints[1].x = 639;
    destiPoints[1].y = 0;
    destiPoints[2].x = 0;
    destiPoints[2].y = 479;
    destiPoints[3].x = 639;
    destiPoints[3].y = 479;

    IplImage* destimage;
    IplImage* capG = cvLoadImage("C:\\object6.jpg");
    IplImage* caphsvG = cvCreateImage(cvGetSize(capG), 8, 3);
    IplImage* caphueG = cvCreateImage(cvGetSize(capG), 8, 1);
    cvCvtColor(capG, caphsvG, CV_BGR2HSV);
    cvSplit(caphsvG, caphueG, NULL, NULL, NULL);
    int bins = 30;
    float range[] = {0, 180};
    float* ranges[] = {range};
    CvHistogram* histG = cvCreateHist(1, &bins, CV_HIST_ARRAY, ranges);
    cvCalcHist(&caphueG, histG);

    IplImage* capR = cvLoadImage("C:\\object7.jpg");
    IplImage* caphsvR = cvCreateImage(cvGetSize(capR), 8, 3);
    IplImage* caphueR = cvCreateImage(cvGetSize(capR), 8, 1);
    cvCvtColor(capR, caphsvR, CV_BGR2HSV);
    cvSplit(caphsvR, caphueR, NULL, NULL, NULL);
    CvHistogram* histR = cvCreateHist(1, &bins, CV_HIST_ARRAY, ranges);
    cvCalcHist(&caphueR, histR);

    IplImage* camera;
    IplImage* hsvCamG;
    IplImage* hueCamG;
    IplImage* destiG;
    IplImage* hsvCamR;
    IplImage* hueCamR;
    IplImage* destiR;
    IplImage* cameradG;
    IplImage* cameradR;

    cvNamedWindow("win1");
    cvNamedWindow("win2");

    cvSetMouseCallback("win1", mouse);

    CvCapture* cam = cvCreateCameraCapture(0);

    if(!cam)
    {
        return 0;
    }

    camera = cvQueryFrame(cam);
    destimage = cvQueryFrame(cam);
    cameradG = cvCreateImage(cvGetSize(camera), 8, 1);
    cameradR = cvCreateImage(cvGetSize(camera), 8, 1);
    while(true)
    {
        camera = cvQueryFrame(cam);
        cvShowImage("win1", camera);
        if(cvWaitKey(1) >= 0) break;
        if(cnt >= 4) break;
        //cout << 2 << endl;
    }
    solveMaze(fout);
    RgbImage maze(camera);
    cvShowImage("win1", camera);
    cvGetPerspectiveTransform(originPoints, destiPoints, transmat);
    //cout << camera -> type() << ' ' << destimage -> type() << ' ' << endl;
    while(true)
    {
        camera = cvQueryFrame(cam);
        cvWarpPerspective(camera, destimage, transmat);
        cvShowImage("win1", destimage);
        if(cvWaitKey(1) >= 0) break;
        if(cnt >= 6) break;
    }

    CvRect trackerG = cvRect(firstX, firstY, width, height);

    while(true)
    {
        camera = cvQueryFrame(cam);
        cvWarpPerspective(camera, destimage, transmat);
        cvShowImage("win1", destimage);
        if(cvWaitKey(1) >= 0) break;
        if(cnt >= 8) break;
    }

    CvRect trackerR = cvRect(firstX, firstY, width, height);
    fout.close();
    cout << "step1" << endl;

    ifstream fin("maze.out");
    ofstream fout3("path.out");
    fstream fout2("critical.out");
    stringstream stream, stream2;
    Maze m(fin);
    m.findPath(stream);
    int x, y;
    fout3 << (counter) << endl;

    while (stream >> x >> y) {
        fout3 << x << ' ' << y << endl;
    }

    for (int i = 80; i + DIS * 2 < pcounter; ++i) {
        double cos_theta = Vector(pa[i + DIS], pa[i]) * Vector(pa[i + 2 * DIS], pa[i + DIS]);
        if (cos_theta < COS_Boundary && cos_theta > -1 * COS_Boundary) {
            stream2 << pa[i + DIS] << endl;
            ++ccounter;
            i += SKIP;
        }
    }
    if (!ccounter) {
        throw 1;
    }

    fout2 << (ccounter + 1) << endl;
    int yfinal;
    while (stream2 >> x >> y) {
        //yfinal = y;
        fout2 << x << ' ' << y << endl;
    }
    fout2 << 440 << ' ' << 40 << endl;

    fin.close();
    fout.close();
    fout2.close();

    fout2.open("critical.out");

    cout << "step2" << endl;

    cout << "Enter the points:" << endl;
    cout << "Enter the number of the points:" << endl;
    fout2 >> n;
    for(int i = 0; i < n; ++i)
    {
        fout2 >> (trace[i].y) >> (trace[i].x);
    }

    DWORD  BytesToSend = 0;
    unsigned char* temp;
    temp = new unsigned char[7];
    temp[0] = '$';
    temp[1] = '0';
    temp[2] = '0';
    temp[3] = '0';
    temp[4] = '7';
    temp[5] = '4';
    temp[6] = '#';
    Wol.Internal = 0;
    Wol.InternalHigh = 0;
    Wol.Offset = 0;
    Wol.OffsetHigh = 0;
    Wol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    long unsigned int len = 7;
    WriteFile(port.hCom, temp, len, NULL, &Wol);
    int i = 0;

    while(i != n)
    {
        camera = cvQueryFrame(cam);
        cvWarpPerspective(camera, destimage, transmat);
        cvShowImage("win1", destimage);
        cvShowImage("win1", camera);
        if(cvWaitKey(1) >= 0)
        {
            break;
        }
        cvInRangeS(destimage, CV_RGB(40, 90, 40), CV_RGB(120, 200, 100), cameradG);
        cvInRangeS(destimage, CV_RGB(130, 20, 20), CV_RGB(255, 100, 100), cameradR);

        hsvCamG = cvCreateImage(cvGetSize(destimage), 8, 3);
        hueCamG = cvCreateImage(cvGetSize(destimage), 8, 1);
        destiG = cvCreateImage(cvGetSize(destimage), 8, 1);
        cvCvtColor(destimage, hsvCamG, CV_BGR2HSV);
        cvSplit(hsvCamG, hueCamG, NULL, NULL, NULL);

        hsvCamR = cvCreateImage(cvGetSize(destimage), 8, 3);
        hueCamR = cvCreateImage(cvGetSize(destimage), 8, 1);
        destiR = cvCreateImage(cvGetSize(destimage), 8, 1);
        cvCvtColor(destimage, hsvCamR, CV_BGR2HSV);
        cvSplit(hsvCamR, hueCamR, NULL, NULL, NULL);


        cvCalcBackProject(&hueCamR, destiR, histR);
        cvCalcBackProject(&hueCamG, destiG, histG);

        cvAnd(destiG, cameradG, destiG);
        cvAnd(destiR, cameradR, destiR);

        cvShowImage("win2", destiG);
        cvShowImage("win2", destiR);

         CvConnectedComp compG;
        cvCamShift(destiG, trackerG, cvTermCriteria(3, 10, 1), &compG);
        trackerG = compG.rect;

        CvConnectedComp compR;
        cvCamShift(destiR, trackerR, cvTermCriteria(3, 10, 1), &compR);
        trackerR = compR.rect;

        double tailX = compG.rect.x + compG.rect.width / 2, tailY = compG.rect.y + compG.rect.height / 2;
        double headX = compR.rect.x + compR.rect.width / 2, headY = compR.rect.y + compR.rect.height / 2;
        double dis = sqrt(((headX + tailX) / 2 - trace[i].x) * ((headX + tailX) / 2 - trace[i].x) + ((headY + tailY) / 2 - trace[i].y) * ((headY + tailY) / 2 - trace[i].y));
        adjust(headX, headY, tailX, tailY, trace[i].x, trace[i].y);
        if(sqrt(((headX + tailX) / 2 - trace[i].x) * ((headX + tailX) / 2 - trace[i].x) + ((headY + tailY) / 2 - trace[i].y) * ((headY + tailY) / 2 - trace[i].y)) < 50)
        {
            ++i;
        }
        if(cvWaitKey(1) >= 0)
        {
            CloseHandle(port.hCom);
            break;
        }
    }

    CloseHandle(port.hCom);
    return 0;
    } catch(...) {
        CloseHandle(port.hCom);
        return 0;
    }
}

void solveMaze(ofstream& fout)
{
    cout << 2 << endl;
    IplImage *img, *destimage1;
    IplImage* finalimage;
    IplImage* finalnalimage;
    cam = cvCreateCameraCapture(0);
        img = cvQueryFrame(cam);
        destimage1 = cvQueryFrame(cam);
    finalimage = cvQueryFrame(cam);
        destimage1 = cvCreateImage(cvGetSize(destimage1), 8, 3);
    finalimage = cvCreateImage(cvGetSize(destimage1), 8, 1);
    finalnalimage = cvCreateImage(cvGetSize(destimage1), 8, 1);
    cvGetPerspectiveTransform(originPoints, destiPoints, transmat);
        cvWarpPerspective(img, destimage1, transmat);
        cvShowImage("win2", destimage1);
        cvInRangeS(destimage1, CV_RGB(100, 100, 100), CV_RGB(255, 255, 255), finalimage);
        //cvShowImage("win1", finalimage);
        cvThin(finalimage, finalnalimage, 52);
        //cvShowImage("win2", finalnalimage);
        cvNamedWindow("win3");
        cvShowImage("win3", destimage1);
        if(cvWaitKey(1000) >= 0)
        {
            return;
        }
        RgbImage maze(finalnalimage);
        cvNamedWindow("win4");
        cvShowImage("win4", maze.imgp);
        fout << maze.imgp ->height << ' ' << maze.imgp -> width << endl;
        for(int i = 0; i < img -> height; ++i)
        {
            for(int j = 0; j < img -> width; ++j)
            {
                fout << (unsigned(finalnalimage -> imageData[i * img -> width + j]) > 120) << ' ';
            }
            fout << endl;
        }
        cout << 3 << endl;
}

void cvThin (IplImage* src, IplImage* dst, int iterations)
{
    cvCopyImage(src, dst);
    BwImage dstdat(dst);
    IplImage* t_image = cvCloneImage(src);
    BwImage t_dat(t_image);
    for (int n = 0; n < iterations; n++)
    for (int s = 0; s <= 1; s++)
    {
        cvCopyImage(dst, t_image);
        for (int i = 0; i < src->height; i++)
        for (int j = 0; j < src->width; j++)
        if (t_dat[i][j])
        {
            int a = 0, b = 0;
            int d[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1},{1, 0}, {1, -1}, {0, -1}, {-1, -1}};
            int p[8];
            p[0] = (i == 0) ? 0 : t_dat[i-1][j];
            for (int k = 1; k <= 8; k++)
            {
                if (i+d[k%8][0] < 0 || i+d[k%8][0] >= src->height || j+d[k%8][1] < 0 || j+d[k%8][1] >= src->width)
                p[k%8] = 0;
                else p[k%8] = t_dat[ i+d[k%8][0] ][ j+d[k%8][1] ];
                if (p[k%8])
                {
                    b++;
                    if (!p[k-1]) a++;
                }
            }
            if (b >= 2 && b <= 6 && a == 1)
                if (!s && !(p[2] && p[4] && (p[0] || p[6])))
                dstdat[i][j] = 0;
            else if (s && !(p[0] && p[6] && (p[2] || p[4])))
                dstdat[i][j] = 0;
        }
    }
    cvReleaseImage(&t_image);
}
