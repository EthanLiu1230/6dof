#include "singlecircle.h"

vector<Contour> SingleCircle::segmentContours(Mat &maskedImg)
{
    Mat bina = maskedImg.clone();
    cvtColor(bina,bina,CV_BGR2GRAY);
    threshold(bina,bina,0,255,CV_THRESH_BINARY | CV_THRESH_OTSU);
    Mat marker = segmenter.createMarkerImage(bina);
    segmenter.setMarkers(marker);
    segmenter.process(maskedImg);
    Mat imgWater = segmenter.getSegmentation();
    cv::threshold(imgWater,imgWater,128,255,THRESH_BINARY);

    vector<Contour> ret_val;
    findContours(imgWater,ret_val,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
    return ret_val;
}

SingleCircle::SingleCircle()
{
    para.focal_length = 35.0;

    // para.pixel_size = 0.002917834;// emmmmmm
    para.pixel_size = 0.005835668;

    //------mode

//    para.diameter = 40.0;
//    para.rot_diameter = 200.0;

    para.diameter = 20.0; // for small ball
    para.rot_diameter = 100.0;

    //------------
    queSize = 30;

    origin.x = 0; origin.y = 0; origin.z = 0;
    origin.ax= 0; origin.ay= 0; origin.az= 0;

    rate_area = 1.3; rate_shape = 0.3;
}

bool SingleCircle::update(Mat &img)
{
    //qDebug() << origin.z;
    if(updateCircleM(img))
    {
        // -------------mode
        X.push_back(calc_x(theFit)*2.0 - origin.x);
        Y.push_back(calc_y(theFit)*2.0 - origin.y);
        Z.push_back(calc_z(theFit)*2.0 - origin.z);// big center circle

//        X.push_back(calc_x(theFit) - origin.x);
//        Y.push_back(calc_y(theFit)- origin.y);
//        Z.push_back(calc_z(theFit) - origin.z);
        //------------

        while(X.size() > queSize) X.pop_front();
        while(Y.size() > queSize) Y.pop_front();
        while(Z.size() > queSize) Z.pop_front();

        // draw circle_center
        circle(img, theFit.center, 2, Scalar(0,0,255), 2);

        // draw fit
        circle(img,theFit.center,theFit.radius,Scalar(0,0,255),2);

        return true;
    }
    return false;
}

void SingleCircle::updateOthers(Mat &img, bool &left_right, bool &up_down)
{
    Mat masked = creatMaskedImg(img);

   // imshow("ob2",masked);

    vector<Contour> circles = findOtherCircles(masked);
    vector<CircleFit> fits;
    for(auto &c : circles)
    {
        // fits.push_back(minAreaRect(c));
        CircleFit fit;
        minEnclosingCircle(c,fit.center,fit.radius);
        fits.push_back(fit);
    }

    // test if left&right, up&down circles exist

    bool left=false, right=false, up=false, down=false;
    for(CircleFit &f : fits)
    {
        Point2f v = f.center - theFit.center;
        if(v.x < 0 && fabs(v.x) > fabs(v.y) )
        {
            left_fit = f;
            left = true;
        }
        if(v.x > 0 && fabs(v.x) > fabs(v.y) )
        {
            right_fit = f;
            right = true;
        }
        if(v.y > 0 && fabs(v.x) < fabs(v.y) )
        {
            up_fit = f;
            up = true;
        }
        if(v.y < 0 && fabs(v.x) < fabs(v.y) )
        {
            down_fit = f;
            down = true;
        }
    }
    up_down = up && down;
    left_right = left && right;

    if(up_down)
    {
        double ax = calc_ax(up_fit, down_fit);
        AX.push_back(ax - origin.ax);
        while (AX.size()>queSize)
            AX.pop_front();

        // draw circle_center
        circle(img, up_fit.center, 2, Scalar(255,0,0), 2);
        // draw fit
        circle(img,up_fit.center,up_fit.radius,Scalar(255,0,0),2);

        // draw circle_center
        circle(img, down_fit.center, 2, Scalar(255,0,0), 2);
        // draw fit
        circle(img,down_fit.center,down_fit.radius,Scalar(255,0,0),2);
    }

    if(left_right)
    {
        double ay = calc_ay(left_fit, right_fit);
        AY.push_back(ay - origin.ay);
        while (AY.size()>queSize)
            AY.pop_front();

        double az = calc_az(left_fit, right_fit);
        AZ.push_back(az - origin.az);
        while (AZ.size()>queSize)
            AZ.pop_front();

        // draw circle_center
        circle(img, left_fit.center, 2, Scalar(0,255,0), 2);
        // draw fit
        circle(img,left_fit.center,left_fit.radius,Scalar(0,255,0),2);

        // draw circle_center
        circle(img, right_fit.center, 2, Scalar(0,255,0), 2);
        // draw fit
        circle(img,right_fit.center,right_fit.radius, Scalar(0,255,0), 2);
    }
}

double SingleCircle::mean(deque<double> &que)
{
    if(que.empty())
        return 0;
    double result = 0;
    for(double q : que) result += q;
    return result / que.size();
}

vector<Contour> SingleCircle::findOtherCircles(Mat &masked)
{

     vector<Contour> contours = segmentContours(masked);

    vector<Contour> ret_val;
    for(auto &c : contours)
    {
        //-----------mode

        if(matchShapes(model,c,CV_CONTOURS_MATCH_I3,0) < rate_shape &&
                areaMatch(theCircle,c,4) < rate_area)              // big center circle
            ret_val.push_back(c);

//        if(matchShapes(model,c,CV_CONTOURS_MATCH_I3,0) < rate_shape &&
//                areaMatch(theCircle,c) < rate_area)
//            ret_val.push_back(c);

        //----------------
    }
    return ret_val;
}

double SingleCircle::calc_ax(CircleFit &upFit, CircleFit &downFit)
{

    double delta_z = calc_z(upFit) - calc_z(downFit);
    return asin(delta_z/para.rot_diameter) * 180 / PI;

}

double SingleCircle::calc_ay(CircleFit &leftFit, CircleFit &rightFit)
{
    double delta_z = calc_z(leftFit) - calc_z(rightFit);
    return asin(delta_z/para.rot_diameter) * 180 / PI;
}

double SingleCircle::calc_az(CircleFit &leftFit, CircleFit &rightFit)
{
    double delta_x = rightFit.center.x - leftFit.center.x;
    double delta_y = rightFit.center.y - leftFit.center.y;
    double ratio = delta_y / delta_x;
    return atan(ratio) * 180 / PI;
}

bool SingleCircle::start(Mat &img, Rect &roi)
{
    // Mat img_clone = img.clone();
    if(findCircleInROI(img, roi, theCircle))
    {
        model = theCircle;
        // qDebug() << model.size();
        minEnclosingCircle(theCircle,theFit.center, theFit.radius);
        unit_len = cvRound(theFit.radius*2);
        prev_position = theFit.center;
        curr_position = theFit.center;

        img_center = Point2f(0.5*img.cols, 0.5*img.rows);

        // set origin for ax,ay,az
        Mat masked = creatMaskedImg(img);
        vector<Contour> circles = findOtherCircles(masked);
        vector<CircleFit> fits;
        for(auto &c : circles)
        {
            CircleFit fit;
            minEnclosingCircle(c,fit.center,fit.radius);
            fits.push_back(fit);
        }

        // test if left&right, up&down circles exist
        CircleFit left_fit, right_fit, up_fit, down_fit;
        bool left=false, right=false, up=false, down=false;
        for(CircleFit &f : fits)
        {
            Point2f v = f.center - theFit.center;
            if(v.x < 0 && fabs(v.x) > fabs(v.y) )
            {
                left_fit = f;
                left = true;
            }
            if(v.x > 0 && fabs(v.x) > fabs(v.y) )
            {
                right_fit = f;
                right = true;
            }
            if(v.y > 0 && fabs(v.x) < fabs(v.y) )
            {
                up_fit = f;
                up = true;
            }
            if(v.y < 0 && fabs(v.x) < fabs(v.y) )
            {
                down_fit = f;
                down = true;
            }
        }

        if(left && right && up && down) {

            return true;
        }
    }
    return false;
}

void SingleCircle::setFrameCnt(size_t cnt)
{
    if(cnt == queSize) return;
    queSize = cnt;
}

void SingleCircle::setOrigin()
{
    if(!X.empty())
        origin.x += mean(X);
    if(!Y.empty())
        origin.y += mean(Y);
    if(!Z.empty())
        origin.z += mean(Z);
    if(!AX.empty())
        origin.ax += mean(AX);
    if(!AY.empty())
        origin.ay += mean(AY);
    if(!AZ.empty())
        origin.az += mean(AZ);
}

bool SingleCircle::retrack(Mat &img, Rect &roi)
{
    bool ret_val = findCircleInROI(img, roi, theCircle);
    if(!ret_val) return ret_val;

    model = theCircle;
    // theFit = fitEllipse(theCircle);
    minEnclosingCircle(theCircle,theFit.center,theFit.radius);
    unit_len = cvRound(theFit.radius*2);
    prev_position = theFit.center;
    curr_position = theFit.center;
}

void SingleCircle::ending()
{
    X.clear(); Y.clear(); Z.clear();
    AX.clear(); AY.clear(); AZ.clear();

    origin.x=0; origin.y=0; origin.z=0;
    origin.ax=0; origin.ay=0; origin.az=0;
}

double SingleCircle::x()
{
    if(X.size() == queSize)
        return mean(X);
    else
        return X.size();
}

double SingleCircle::y()
{
    if(Y.size() == queSize)
        return mean(Y);
    else
        return Y.size();
}

double SingleCircle::z()
{
    if(Z.size() == queSize)
        return mean(Z);
    else
        return Z.size();
}

double SingleCircle::ax()
{
    if(AX.size() == queSize)
        return mean(AX);
    else
        return AX.size();
}

double SingleCircle::ay()
{
    if(AY.size() == queSize)
        return mean(AY);
    else
        return AY.size();
}

double SingleCircle::az()
{
    if(AZ.size() == queSize)
        return mean(AZ);
    else
        return AZ.size();
}

/**/bool areaGreater(Contour &a, Contour &b){
    return contourArea(a) > contourArea(b);
}
bool SingleCircle::findCircleInROI(const Mat &img, Rect &roi, Contour &output)
{
    Mat ROI(img, roi);
    if(ROI.channels() != 1)
        cvtColor(ROI, ROI, CV_BGR2GRAY);
    threshold(ROI, ROI, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    vector<Contour> contours;
    findContours(ROI, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE,  roi.tl());
    // if failed
    if(contours.size() < 1)
        return false;

    std::sort(contours.begin(),contours.end(), areaGreater);
    output = contours[0];
    return true;
}

bool SingleCircle::updateCircle(const Mat &img)
{
    // update positions, theFit, theCircle, unit_len
    // qDebug() << unit_len;
    Point2i pt = curr_position - prev_position + curr_position;
    Rect imgBound(0,0,img.cols-1,img.rows-1);
    // Point pt(search_position);
    Rect search_roi(pt-Point(unit_len,unit_len)
                    , pt+Point(unit_len,unit_len));
    if(unContain(imgBound, search_roi))
        return false;
    Mat ROI(img, search_roi);

    cvtColor(ROI, ROI, CV_BGR2GRAY);
    threshold(ROI, ROI, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    vector<Contour> cons;
    findContours(ROI, cons, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, search_roi.tl());
    // qDebug()<<cons.size();
    for(size_t i=0; i!=cons.size(); ++i)
    {
        Contour &c = cons[i];

        if(areaMatch(theCircle, c) < rate_area
                && matchShapes(model, c, CV_CONTOURS_MATCH_I3, 0) < rate_shape)
        {
            theCircle = c;
            // theFit = fitEllipse(theCircle);
            minEnclosingCircle(theCircle,theFit.center,theFit.radius);
            prev_position = curr_position;
            curr_position = theFit.center;
            unit_len = cvRound(theFit.radius*2);
            return true;
        }
    }
    return false;
}

bool SingleCircle::updateCircleM(const Mat &img)
{
    Point2i pt = curr_position - prev_position + curr_position;
    // M
    Mat masked;
    Mat mask = Mat::zeros(img.size(), CV_8UC1);
    cv::circle(mask,pt,theFit.radius*2,Scalar::all(255),-1);
    img.copyTo(masked,mask);

    //imshow("ob1", masked);

//    cvtColor(masked, masked, CV_BGR2GRAY);
//    threshold(masked,masked,0,255,CV_THRESH_BINARY | CV_THRESH_OTSU);
//    vector<Contour> contours;
//    findContours(masked,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
    vector<Contour> contours = segmentContours(masked);

    for(Contour &c : contours)
    {
        if(areaMatch(model, c) < rate_area &&
                matchShapes(model,c,CV_CONTOURS_MATCH_I3,0) < rate_shape)
        {
            theCircle = c;
            minEnclosingCircle(theCircle,theFit.center,theFit.radius);
            prev_position = curr_position;
            curr_position = theFit.center;
            return true;
        }
    }
    return false;
}

double SingleCircle::areaMatch(const Contour &c1, const Contour &c2, double shrinkC1)
{
    double a1 = contourArea(c1);
    a1 /= shrinkC1;
    double a2 = contourArea(c2);
    return a1 > a2 ? (a1/a2) : (a2/a1);
}

bool SingleCircle::unContain(const Rect &big, const Rect &small)
{
    return !big.contains(small.tl()) || !big.contains(small.br());
}

double SingleCircle::calc_x(CircleFit &fit)
{
    double xi = fit.center.x - img_center.x;
    double amp =  para.diameter / (fit.radius*2.0);//2.0
    //double amp = para.diameter / (fit.radius*2.0);//2.0

    // double amp =
    return xi * amp;
}

double SingleCircle::calc_y(CircleFit &fit)
{
    double yi = fit.center.y - img_center.y;
    double amp = para.diameter / (fit.radius*2.0);
    //double amp = para.diameter / (fit.radius*2.0);
    return yi * amp;
}

double SingleCircle::calc_z(CircleFit &fit)
{
     //original
//    double hi = para.pixel_size * fit.radius * 2.0;
//    return para.diameter * para.focal_length / hi;

    // modified
    double hi = para.pixel_size * fit.radius * 2.0 * 100.0;
    double ret_100 = para.diameter * para.focal_length / hi;
    return ret_100 * 100.0;

}

Mat SingleCircle::creatMaskedImg(const Mat &img)
{
    // -----mode

     double scale = 3.2; // big center circle
     // double scale = 7;

    //------------

    double r1 = theFit.radius*1.5;
    double r2 = theFit.radius*scale;

    Mat ret_val;

    Mat mask = Mat::zeros(img.size(), CV_8UC1);

    cv::circle(mask,theFit.center,r2,Scalar::all(255),-1);

    cv::circle(mask,theFit.center,r1,Scalar::all(0),-1);

    img.copyTo(ret_val,mask);

    return ret_val;
}
