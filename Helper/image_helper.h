#ifndef IMAGE_HELPER_H
#define IMAGE_HELPER_H
#include<QImage>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/types_c.h"

#define WIN32_LEAN_AND_MEAN
#ifdef Q_OS_WIN
#include<windows.h>
#endif
class Image_helper
{
public:
    Image_helper();
    static QImage Mat2QImage(cv::Mat const& src);
    static cv::Mat QImage2Mat(QImage const& src);
#ifdef Q_OS_WIN
    static cv::Mat HBitmapToMat(HBITMAP& _hBmp);
#endif
    static bool is_equal(const cv::Mat& data1, const cv::Mat& data2);
};

#endif // IMAGE_HELPER_H
