#ifndef IMAGE_HELPER_H
#define IMAGE_HELPER_H
#include<QImage>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/types_c.h"
#define WIN32_LEAN_AND_MEAN
#include<windows.h>
#include <QScreen>
class ImageHelper {
public:
    static QImage Mat2QImage(cv::Mat const& src);
    static cv::Mat QImage2Mat(QImage const& src);
    static cv::Mat HBitmapToMat(HBITMAP& _hBmp);
    static bool is_equal(const cv::Mat& data1, const cv::Mat& data2);
    static QScreen* getCurrentScreen();
    static int getCurrentIndex();
    static QRect getCurrentGeometry();
    static QPixmap grabScreen();
    static QPixmap grabScreen(int index);
    static QPixmap grabScreen(int index, WId wid);
    static QPixmap grabScreen(int x, int y, int width, int height);
    static QPixmap grabScreen(int index, int x, int y, int width, int height);
    static QIcon getIcon(QString name, int pt_w=24, int pt_h=24);
    static QPixmap getPixmap(QString name, int pt_w=24, int pt_h=24);
};

#endif // IMAGE_HELPER_H
