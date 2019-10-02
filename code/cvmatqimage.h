#ifndef CVMATQIMAGE_H
#define CVMATQIMAGE_H

#include <QImage>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


QImage cvMat2QImage(const cv::Mat& mat);
cv::Mat QImage2cvMat(QImage image);

#endif // CVMATQIMAGE_H
