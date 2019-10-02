#ifndef SINGLECIRCLE_H
#define SINGLECIRCLE_H
#include <opencv2/opencv.hpp>
#include <cmath>
#include "watershedSegmentation.h"
#include <QDebug>
#define PI 3.14159265359

using namespace std;
using namespace cv;

typedef vector<Point> Contour;

struct Parameter {
    double focal_length,
    diameter, rot_diameter, pixel_size;
};

struct Origin {
    double x,y,z, // initialized when
    ax,ay,az;
};

struct CircleFit {
    float radius;
    Point2f center;
};

class SingleCircle
{
    // variable
public:
    Parameter para;
    Origin origin;
    deque<double> X,Y,Z,AX,AY,AZ;
    size_t queSize;
    double rate_shape, rate_area;

private:
    WatershedSegmenter segmenter;
    vector<Contour> segmentContours(Mat &maskedImg);

    // graphic variable
public:
    Contour model, theCircle;
    CircleFit theFit;
    CircleFit left_fit, right_fit, up_fit, down_fit;
    Point2f prev_position, curr_position;
    Point2f img_center;
    int unit_len; // radius of theCircle on image
    

    // interface
public:
    SingleCircle();
    
    bool update(Mat &img);
    void updateOthers(Mat &img, bool &left_right, bool &up_down);

    bool start(Mat &img, Rect &roi);
    void setFrameCnt(size_t cnt);
    void setOrigin();
    bool retrack(Mat &img, Rect &roi);
    void ending();

    double x();
    double y();
    double z();

    double ax();
    double ay();
    double az();
    
    // toolBox
public:
    bool findCircleInROI(const Mat& img, Rect &roi, Contour& output);

    bool updateCircle(const Mat &img);
    bool updateCircleM(const Mat &img);
    double areaMatch(const Contour &c1, const Contour &c2, double shrinkC1 =1);
    bool unContain(const Rect &big, const Rect &small);

    double calc_x(CircleFit &fit);
    double calc_y(CircleFit &fit);
    double calc_z(CircleFit &fit);

    double mean(deque<double> &que);

    Mat creatMaskedImg(const Mat &img);
    vector<Contour> findOtherCircles(Mat &masked);
    double calc_ax(CircleFit &upFit, CircleFit &downFit);
    double calc_ay(CircleFit &leftFit, CircleFit &rightFit);
    double calc_az(CircleFit &leftFit, CircleFit &rightFit);

};


#endif // SINGLECIRCLE_H
