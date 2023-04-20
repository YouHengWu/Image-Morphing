#include <iostream>
#include <string>
#include <cstring>
#include <cctype>
#include <vector>
#include <algorithm>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// Parameters
int a = 1;
int b = 2;
int p = 0;
int frames;

// Key 
int Keyboard;
int Drawing_Flag = 0;

// Image Shape
int Image_Row;
int Image_Column;

int First, Second;

const int Thickness = 2;
const float PI = 3.141592653589732384626433;

float Interpolation;

string First_Image, Second_Image;

IplImage *img1, *img2;
IplImage *img1_Save, *img2_Save;
IplImage *Original_img1, *Original_img2;

/*
VideoCapture capture(0);
VideoWriter writer("VideoTest.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25.0, Size(640, 480));
*/

// Feature Line Color
Vec3f Color(0, 255, 255);

class Line 
{
public:

    Point2f Begin;
    Point2f Middle;
    Point2f End;

    float Length;
    float Angle;

    Line();
    ~Line();

    float GetLength(Point2f p1, Point2f p2);
    float GetAngle(Point2f p1, Point2f p2);

    Point2f GetMiddle(Point2f p1, Point2f p2);
};

Line::Line()
{

}
Line::~Line() 
{

}

float Line::GetLength(Point2f p1, Point2f p2) 
{
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

float Line::GetAngle(Point2f p1, Point2f p2) 
{
    return atan2(p2.y - p1.y, p2.x - p1.x);
}

Point2f Line::GetMiddle(Point2f p1, Point2f p2)
{
    Point2f p((p2.x + p1.x) / 2, (p2.y + p1.y) / 2);

    return p;
}

class PairLine
{
public:
    Line Left_Line;
    Line Right_Line;

    vector<Line> Warp;
};

PairLine Two_lines;

string Integer_To_String(int x) {
    string tmp = "", result = "";
    while (x) {
        tmp += (char)(x % 10 + '0');
        x /= 10;
    }
    for (int i = tmp.size() - 1; i >= 0; --i) {
        result += tmp[i];
    }
    return result;
}

vector<PairLine> Pairs;

void Store_Warp_Line()
{
    while (Two_lines.Left_Line.Angle - Two_lines.Right_Line.Angle > PI) {
        Two_lines.Right_Line.Angle += PI;
    }
    while (Two_lines.Right_Line.Angle - Two_lines.Left_Line.Angle > PI) {
        Two_lines.Left_Line.Angle += PI;
    }

    for (int i = 0; i < frames; ++i) {
        Interpolation = (float)((i + 1.0f) / (frames + 1.0f));

        Line Two_Lines_Warp;

        Two_Lines_Warp.Middle = (1.0f - Interpolation) * Two_lines.Left_Line.Middle + Interpolation * Two_lines.Right_Line.Middle;
        Two_Lines_Warp.Length = (1.0f - Interpolation) * Two_lines.Left_Line.GetLength(Two_lines.Left_Line.Begin, Two_lines.Left_Line.End) + Interpolation * Two_lines.Right_Line.GetLength(Two_lines.Right_Line.Begin, Two_lines.Right_Line.End);
        Two_Lines_Warp.Angle = (1.0f - Interpolation) * Two_lines.Left_Line.Angle + Interpolation * Two_lines.Right_Line.Angle;
        //cout << Two_Lines_Warp.Length << endl;
        Point2f offset(0.5f * Two_Lines_Warp.Length * cos(Two_Lines_Warp.Angle), 0.5f * Two_Lines_Warp.Length * sin(Two_Lines_Warp.Angle));

        Two_Lines_Warp.Begin = Two_Lines_Warp.Middle - offset;
        Two_Lines_Warp.End = Two_Lines_Warp.Middle + offset;
        
        Two_lines.Warp.push_back(Two_Lines_Warp);
    }
}

void onMouse1 (int Event, int x, int y, int flags, void* param)
{
    Two_lines.Warp.clear();

    if (Drawing_Flag % 2 == 0 && Drawing_Flag > 0) {
        if (Event == CV_EVENT_LBUTTONDOWN) {
            printf("Left Line Start Point¡G( %d, %d) \n", x, y);

            Two_lines.Left_Line.Begin.x = x;
            Two_lines.Left_Line.Begin.y = y;
        }
        if (Event == CV_EVENT_LBUTTONUP) {
            printf("Left Line End Point¡G( %d, %d) \n", x, y);

            Two_lines.Left_Line.End.x = x;
            Two_lines.Left_Line.End.y = y;
            Two_lines.Left_Line.Middle = Two_lines.Left_Line.GetMiddle(Two_lines.Left_Line.Begin, Two_lines.Left_Line.End);
            Two_lines.Left_Line.Length = Two_lines.Left_Line.GetLength(Two_lines.Left_Line.Begin, Two_lines.Left_Line.End);             
            Two_lines.Left_Line.Angle = Two_lines.Left_Line.GetAngle(Two_lines.Left_Line.Begin, Two_lines.Left_Line.End);

            cvLine(img1, Two_lines.Left_Line.Begin, Two_lines.Left_Line.End, Color, Thickness);
            img1_Save = cvCloneImage(img1);

            --Drawing_Flag;
        }
        if (flags == CV_EVENT_FLAG_LBUTTON) {
            Two_lines.Left_Line.End.x = x;
            Two_lines.Left_Line.End.y = y;

            img1 = cvCloneImage(img1_Save);
            cvLine(img1, Two_lines.Left_Line.Begin, Two_lines.Left_Line.End, Color, Thickness);

            cvShowImage("Left Image", img1);
        }
    }
}
void onMouse2 (int Event, int x, int y, int flags, void* param)
{
    if (Drawing_Flag % 2 == 1 && Drawing_Flag > 0) {
        if (Event == CV_EVENT_LBUTTONDOWN) {
            printf("Right Line Start Point¡G( %d, %d) \n", x, y);

            Two_lines.Right_Line.Begin.x = x;
            Two_lines.Right_Line.Begin.y = y;
        }
        if (Event == CV_EVENT_LBUTTONUP) {
            printf("Right Line End Point¡G( %d, %d) \n", x, y);

            Two_lines.Right_Line.End.x = x;
            Two_lines.Right_Line.End.y = y;
            Two_lines.Right_Line.Middle = Two_lines.Right_Line.GetMiddle(Two_lines.Right_Line.Begin, Two_lines.Right_Line.End);
            Two_lines.Right_Line.Length = Two_lines.Right_Line.GetLength(Two_lines.Right_Line.Begin, Two_lines.Right_Line.End);
            Two_lines.Right_Line.Angle = Two_lines.Right_Line.GetAngle(Two_lines.Right_Line.Begin, Two_lines.Right_Line.End);

            cvLine(img2, Two_lines.Right_Line.Begin, Two_lines.Right_Line.End, Color, Thickness);
            img2_Save = cvCloneImage(img2);

            Store_Warp_Line();

            Pairs.push_back(Two_lines);

            --Drawing_Flag;
        }
        if (flags == CV_EVENT_FLAG_LBUTTON) {           
            Two_lines.Right_Line.End.x = x;
            Two_lines.Right_Line.End.y = y;

            img2 = cvCloneImage(img2_Save);
            cvLine(img2, Two_lines.Right_Line.Begin, Two_lines.Right_Line.End, Color, Thickness);

            cvShowImage("Right Image", img2);
        }
    }
}

Point2f Perpendicular(Point2f p)
{
    Point2f tmp;

    tmp.x = -p.y;
    tmp.y = p.x;

    return tmp;
}

float Compute_Weight(float u, float v, Line Destination, Point2f Pixel)
{
    float distance;

    if (u < 0) {
        distance = sqrt(pow(Pixel.x - Destination.Begin.x, 2) + pow(Pixel.y - Destination.Begin.y, 2));
    }
    else if (u > 1) {
        distance = sqrt(pow(Pixel.x - Destination.End.x, 2) + pow(Pixel.y - Destination.End.y, 2));
    }
    else {
        distance = abs(v);
    }

    return pow(pow(Destination.Length, p) / (a + distance), b);
}

Point2f In_Image(Point2f p)
{
    if (p.x < 0) {
        p.x = 0;
    }
    if (p.y < 0) {
        p.y = 0;
    }
    if (p.x >= Image_Column) {
        p.x = Image_Column - 1;
    }
    if (p.y >= Image_Row) {
        p.y = Image_Row - 1;
    }

    return p;
}

CvScalar GetColor(IplImage *img, Point2f p)
{
    int Max_x = (int)p.x + 1;
    int Max_y = (int)p.y + 1;
    int Min_x = (int)p.x;
    int Min_y = (int)p.y;

    float t1 = p.x - Min_x;
    float t2 = p.y - Min_y;
    
    if (Max_x >= Image_Column) {
        Max_x = Image_Column - 1;
    }
    if (Max_y >= Image_Row) {
        Max_y = Image_Row - 1;
    }
    if (Min_x < 0) {
        Min_x = 0;
    }
    if (Min_y < 0) {
        Min_y = 0;
    }
    
    CvScalar Left_Down = cvGet2D(img, Min_y, Min_x);
    CvScalar Left_Up = cvGet2D(img, Min_y, Max_x);
    CvScalar Right_Down = cvGet2D(img, Max_y, Min_x);
    CvScalar Right_Up = cvGet2D(img, Max_y, Max_x);
    CvScalar Interpolation_Color;

    for (int i = 0; i < 4; ++i) {
        Interpolation_Color.val[i] = (1.0f - t1) * (1.0f - t2) * Left_Down.val[i] + (1.0f - t1) * t2 * Left_Up.val[i] + t1 * (1.0f - t2) * Right_Down.val[i] + t1 * t2 * Right_Up.val[i];
    }
    
    return Interpolation_Color;
}

void Warping() 
{    
    for (int j = 0; j < frames; ++j) {

        Interpolation = (float)(j + 1.0f) / (frames + 1.0f);
        IplImage* Morphing_Result = cvCreateImage(cvSize(Image_Column, Image_Row), IPL_DEPTH_8U, 3);

        Original_img1 = cvLoadImage((First_Image + ".jpg").c_str());
        Original_img2 = cvLoadImage((Second_Image + ".jpg").c_str());
        
        for (int x = 0; x < Image_Column; ++x) {
            for (int y = 0; y < Image_Row; ++y) {
                
                Point2f X;
                X.x = x;
                X.y = y;

                Point2f DSUM_Left(0.0f, 0.0f);
                Point2f DSUM_Right(0.0f, 0.0f);

                float weightsum_Left = 0.0f;
                float weightsum_Right = 0.0f;
                
                for (int i = 0; i < Pairs.size(); ++i) {

                    // Left
                    Line Source_Left_Line = Pairs[i].Left_Line;
                    Line Destination_Left_Line = Pairs[i].Warp[j];
                   //if(x + y == 0)
                   //cout << Destination_Left_Line.Begin << " " << Destination_Left_Line.End << " " << Destination_Left_Line.Length << endl;
                    float Left_u = (X - Destination_Left_Line.Begin).dot(Destination_Left_Line.End - Destination_Left_Line.Begin) / pow(Destination_Left_Line.GetLength(Destination_Left_Line.Begin, Destination_Left_Line.End), 2);
                    float Left_v = (X - Destination_Left_Line.Begin).dot(Perpendicular(Destination_Left_Line.End - Destination_Left_Line.Begin)) / Destination_Left_Line.GetLength(Destination_Left_Line.Begin, Destination_Left_Line.End);

                    Point2f Left_Source_Point = Source_Left_Line.Begin + Left_u * (Source_Left_Line.End - Source_Left_Line.Begin) + Left_v * Perpendicular(Source_Left_Line.End - Source_Left_Line.Begin) / (Source_Left_Line.Length);
                    Point2f Left_D = Left_Source_Point - X;

                    float Left_Source_Weight = Compute_Weight(Left_u, Left_v, Destination_Left_Line, X);
                    
                    DSUM_Left += Left_D * Left_Source_Weight;
                    weightsum_Left += Left_Source_Weight;

                    // Right
                    Line Source_Right_Line = Pairs[i].Right_Line;
                    Line Destination_Right_Line = Pairs[i].Warp[j];

                    float Right_u = (X - Destination_Right_Line.Begin).dot(Destination_Right_Line.End - Destination_Right_Line.Begin) / pow(Destination_Right_Line.GetLength(Destination_Right_Line.Begin, Destination_Right_Line.End), 2);
                    float Right_v = (X - Destination_Right_Line.Begin).dot(Perpendicular(Destination_Right_Line.End - Destination_Right_Line.Begin)) / Destination_Right_Line.GetLength(Destination_Right_Line.Begin, Destination_Right_Line.End);

                    Point2f Right_Source_Point = Source_Right_Line.Begin + Left_u * (Source_Right_Line.End - Source_Right_Line.Begin) + Left_v * Perpendicular(Source_Right_Line.End - Source_Right_Line.Begin) / (Source_Right_Line.Length);
                    Point2f Right_D = Right_Source_Point - X;

                    float Right_Source_Weight = Compute_Weight(Right_u, Right_v, Destination_Right_Line, X);

                    DSUM_Right += Right_D * Right_Source_Weight;
                    weightsum_Right += Right_Source_Weight;
                    //cout << Left_u << " " << Left_v << " " << Right_u << " " << Right_v << endl;
                }
                
                Point2f Left_Point = X + DSUM_Left / weightsum_Left;
                Point2f Right_Point = X + DSUM_Right / weightsum_Right;
                //cout << Left_Point.x << " " << Left_Point.y << " " << Right_Point.x << " " << Right_Point.y << endl;
                Left_Point = In_Image(Left_Point);
                Right_Point = In_Image(Right_Point);

                CvScalar Left_Color_Vector = GetColor(Original_img1, Left_Point);
                CvScalar Right_Color_Vector = GetColor(Original_img2, Right_Point);
                CvScalar Cross_Dissolving;
                
                for (int i = 0; i < 4; ++i) {
                    Cross_Dissolving.val[i] = (1.0f - Interpolation) * Left_Color_Vector.val[i] + Interpolation * Right_Color_Vector.val[i];
                }

                cvSet2D(Morphing_Result, y, x, Cross_Dissolving);
                
            }
        }

        cvShowImage(("Morping" + Integer_To_String(j)).c_str(), Morphing_Result);

        if (First == 9 && Second == 10){
            Mat Vegetto = imread("Vegetto.jpg");
            imshow("Vegetto", Vegetto);
        }
        if (First == 13 && Second == 14) {
            Mat Result2 = imread("Result2.jpg");
            imshow("Result", Result2);
        }

        Mat Result = cvarrToMat(Morphing_Result);
        /*
        capture >> Result;
        writer << Result;
        imshow("video", Result);
        */
        if (j < 10) {
            imwrite("Morphing_000" + Integer_To_String(j) + ".png", Result);
        }
        else {
            imwrite("Morphing_00" + Integer_To_String(j) + ".png", Result);
        }
    }
}

int main() {
   
    cout << "Choose First image (Input an odd integer number between 1 to 14)¡G";
    cin >> First;
    while (First % 2 == 0 || First < 1 || First > 14) {
        cout << "Hey¡I What are you doing¡H Please follow the instruction. :(\n";
        cout << "Choose First image (Input an odd integer number between 1 to 14)¡G";
        cin >> First;
    }

    cout << "Choose Second image (Input an even integer number which is the number of first image + 1)¡G";
    cin >> Second;
    while (Second != First + 1) {
        cout << "Hey¡I What are you doing¡H Please follow the instruction. :(\n";
        cout << "Choose Second image (Input an even integer number which is the number of first image + 1)¡G";
        cin >> Second;
    }

    cout << "Input Frames (Input an integer number greater than 0)¡G";
    cin >> frames;
    while (frames <= 0) {
        cout << "Hey¡I What are you doing¡H Please follow the instruction. :(\n";
        cout << "Choose Second image (Input an even integer number which is the number of first image + 1)¡G";
        cin >> frames;
    }
    /*
    cout << "Choose constant a (Input an integer number between 0 to 2)¡G";
    cin >> a;
    while (a < 0 || a > 2) {
        cout << "Hey¡I What are you doing¡H Please follow the instruction. :(\n";
        cout << "Choose Second image (Input an even integer number which is the number of first image + 1)¡G";
        cin >> a;
    }
    cout << "Choose constant b (Input an integer number between 0 to 2)¡G";
    cin >> b;
    while (b < 0 || b > 2) {
        cout << "Hey¡I What are you doing¡H Please follow the instruction. :(\n";
        cout << "Choose Second image (Input an even integer number which is the number of first image + 1)¡G";
        cin >> b;
    }
    cout << "Choose constant p (Input an integer number between 0 to 2)¡G";
    cin >> p;
    while (p < 0 || p > 2) {
        cout << "Hey¡I What are you doing¡H Please follow the instruction. :(\n";
        cout << "Choose Second image (Input an even integer number which is the number of first image + 1)¡G";
        cin >> p;
    }
    */
    First_Image = Integer_To_String(First);
    Second_Image = Integer_To_String(Second);

    img1 = cvLoadImage(("image" + First_Image + ".jpg").c_str());
    img2 = cvLoadImage(("image" + Second_Image + ".jpg").c_str());

    img1_Save = img1;
    img2_Save = img2;

    Image_Column = img1->width;
    Image_Row = img1->height;

    namedWindow("Left Image", 1);
    namedWindow("Right Image", 1);
    moveWindow("Left Image", 50, 200);
    moveWindow("Right Image", 680, 200);

    setMouseCallback("Left Image", onMouse1, NULL);
    setMouseCallback("Right Image", onMouse2, NULL);
   
    cvShowImage("Left Image", img1);
    cvShowImage("Right Image", img2);
    
    while (true)
    {
        Keyboard = waitKey(0);
        if (Keyboard == 'd' || Keyboard == 'D') {
            if (!Drawing_Flag % 2) {
                Drawing_Flag = 2;
            }
            else {
                Drawing_Flag = 1;
            }
        }
        else if (Keyboard == 'w' || Keyboard == 'W') {
            Warping();
        }
        else if (Keyboard == 'q' || Keyboard == 'Q') {
            break;
        }
    }

    waitKey(0);
    destroyAllWindows;
    
    return 0;
}