#ifndef SCROLL_WORKER_H
#define SCROLL_WORKER_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include<QObject>
#include<QImage>
#include<QMutex>


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
    QImage ans_image;
    int SURF(cv::Mat imageL, cv::Mat imageR, cv::Mat& ans, int img_height);
    bool first_match(cv::Mat grayL, cv::Mat grayR, cv::Mat& ans);
    bool second_match(cv::Mat grayL, cv::Mat grayR, cv::Mat& ans);
    int process_width;
    int delta_width;
    cv::Mat maskImage;
    void initMask(int cols, int rows);
};

#endif // SCROLL_WORKER_H
