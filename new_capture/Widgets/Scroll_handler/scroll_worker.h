#ifndef SCROLL_WORKER_H
#define SCROLL_WORKER_H
#include<windows.h>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/xfeatures2d.hpp"
#include<QObject>
#include<QImage>


class Scroll_worker : public QObject
{
    Q_OBJECT
public:
    Scroll_worker(QObject* object=nullptr);
    ~Scroll_worker();
public slots:
    void  begin_work(QImage scroll_image1, QImage scroll_image2, int img_height);
signals:
    void work_finish(QImage image);
public:
    struct four_corners_t
    {
        cv::Point2f left_top;
        cv::Point2f left_bottom;
        cv::Point2f right_top;
        cv::Point2f right_bottom;
    };
    four_corners_t corners; //配准图的四个顶点坐标
    void CalcCorners(const cv::Mat& H, const cv::Mat src);
    //优化两图的连接处，使得拼接自然
    void OptimizeSeam(cv::Mat& imageL, cv::Mat& imageTransformR, cv::Mat& dst);
    inline void ImShow(std::string name, cv::Mat img) {
        int n = 4;
        cv::namedWindow(name, cv::WINDOW_NORMAL);
        cv::resizeWindow(name, cv::Size(img.cols / n, img.rows / n));
        cv::imshow(name, img);
    }

    int SURF(cv::Mat imageL, cv::Mat imageR, cv::Mat& ans);
};

#endif // SCROLL_WORKER_H
