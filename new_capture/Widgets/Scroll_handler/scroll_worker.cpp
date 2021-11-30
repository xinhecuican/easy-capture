#include "scroll_worker.h"
#include<QImage>
#include<QPainter>
#include<QDebug>
#include<QDateTime>
#include<QThread>
#include "Helper/image_helper.h"
#include "Helper/debug.h"
#include "Scroll_handler_global.h"

Scroll_worker::Scroll_worker(QObject* object) : QObject(object)
{

}

Scroll_worker::~Scroll_worker()
{
}

void Scroll_worker::begin_work(QImage image1, QImage image2, int img_height)
{

    cv::Mat img1 = Image_helper::QImage2Mat(image1);
    cv::Mat img2 = Image_helper::QImage2Mat(image2);

    cv::Mat temp_img1 = img1(cv::Rect(0, image1.height()-img_height, image1.width(), img_height));
    cv::Mat temp_img2 = img2(cv::Rect(0, 0, image2.width(), img_height));
    cv::Mat scroll_image1, scroll_image2;
    cv::rotate(temp_img1, scroll_image1, cv::ROTATE_90_COUNTERCLOCKWISE);
    cv::rotate(temp_img2, scroll_image2, cv::ROTATE_90_COUNTERCLOCKWISE);
    if (temp_img1.empty() || temp_img2.empty())
    {
        qDebug() << "Read image failed, please check again!" << endl;
        return ;
    }
    cv::Mat image;
    if(SURF(scroll_image1, scroll_image2, image, img_height) == -1)
    {
        image = cv::Mat(scroll_image1.rows, scroll_image1.cols + scroll_image2.cols - img_height/2, CV_8UC3);
        cv::cvtColor(scroll_image1, scroll_image1, cv::COLOR_RGBA2RGB);
        cv::cvtColor(scroll_image2, scroll_image2, cv::COLOR_RGBA2RGB);
        scroll_image2.copyTo(image(cv::Rect(scroll_image1.cols - img_height/2, 0,
                                            scroll_image2.cols, scroll_image2.rows)));
        scroll_image1.copyTo(image(cv::Rect(0, 0, scroll_image1.cols, scroll_image1.rows)));
    }


    cv::rotate(image, image, cv::ROTATE_90_CLOCKWISE);
    cv::cvtColor(img1, img1, cv::COLOR_RGBA2RGB);
    cv::cvtColor(img2, img2, cv::COLOR_RGBA2RGB);
//    ans_image = QImage(image1.width(),
//                       image1.height()+image2.height() - 2 * img_height + temp_image.height(),
//                       QImage::Format_ARGB32_Premultiplied);
    cv::Mat ans_image1 = cv::Mat(image1.height()+image2.height() - 2 * img_height + image.rows,
                                image1.width(),
                                CV_8UC3);
    if(img1.rows - img_height  != 0)
    {
        img1(cv::Rect(0, 0, img1.cols, img1.rows-img_height))
            .copyTo(ans_image1(cv::Rect(0, 0, img1.cols, img1.rows-img_height)));
    }
    image.copyTo(ans_image1(cv::Rect(0, img1.rows-img_height, image.cols, image.rows)));
    if(img2.rows - img_height != 0)
    {
        img2(cv::Rect(0, img_height, img2.cols, img2.rows-img_height))
            .copyTo(ans_image1(cv::Rect(0, img1.rows+image.rows - img_height, img2.cols, img2.rows-img_height)));
    }
    QImage ans_image = Image_helper::Mat2QImage(ans_image1);
//    QImage bind_image_copy = image2.copy(0, img_height, image2.width(), image2.height()-img_height);
//    QPainter painter(&ans_image);
//    painter.drawImage(QPoint(0, 0), image1);
//    painter.drawImage(QPoint(0, image1.height()-img_height), temp_image);
//    painter.drawImage(QPoint(0, image1.height()+temp_image.height() - img_height), bind_image_copy);
//    painter.end();
//    QImage ans_image(image1.width(), image1.height() + image2.height(), QImage::Format_RGB32);
//    QPainter painter(&ans_image);
//    painter.drawImage(0, 0, image1);
//    painter.drawImage(0, image1.height(), image2);
//    painter.end();
    emit work_finish(ans_image);
}

void Scroll_worker::OptimizeSeam(cv::Mat& imageL, cv::Mat& imageTransformR, cv::Mat& dst)
{
    int left_border_col = std::min(corners.left_top.x, corners.left_bottom.x);//开始位置，即重叠区域的左边界

        double processWidth = imageL.cols - left_border_col;//重叠区域的宽度
        double alpha = 1;//imageL中像素的权重

        CV_Assert(dst.channels() == 1 || 3);
        if (dst.channels() == 1)
        {
            for (int i = 0; i < imageL.rows; i++)//从上到下从左到右扫描
            {
                uchar* pImgL = imageL.ptr<uchar>(i);  //获取第i行的首地址
                uchar* pTran = imageTransformR.ptr<uchar>(i);
                uchar* pDst = dst.ptr<uchar>(i);
                for (int j = left_border_col; j < imageL.cols; j++)
                {
                    //如果遇到imageTransformR中无像素的黑点，则完全拷贝imageL中的数据
                    if (pTran[j] == 0)
                    {
                        alpha = 1;
                    }
                    else
                    {
                        //imageL中像素的权重，与当前处理点距重叠区域左边界的距离成正比(距离越远alpha越小)
                        alpha = (processWidth - (j - left_border_col)) / processWidth;
                    }
                    pDst[j] = pImgL[j] * alpha + pTran[j] * (1 - alpha);
                }
            }
        }
        else if (dst.channels() == 3)
        {
            for (int i = 0; i < imageL.rows; i++)
            {
                uchar* pImgL = imageL.ptr<uchar>(i);  //获取第i行的首地址
                uchar* pTran = imageTransformR.ptr<uchar>(i);
                uchar* pDst = dst.ptr<uchar>(i);
                for (int j = left_border_col; j < imageL.cols; j++)
                {
                    //如果遇到imageTransformR中无像素的黑点，则完全拷贝imageL中的数据
                    if (pTran[j * 3] == 0 && pTran[j * 3 + 1] == 0 && pTran[j * 3 + 2] == 0)
                    {
                        alpha = 1;
                    }
                    else
                    {
                        //imageL中像素的权重，与当前处理点距重叠区域左边界的距离成正比(距离越远alpha越小)
                        alpha = (processWidth - (j - left_border_col)) / processWidth;
                    }

                    pDst[j * 3] = pImgL[j * 3] * alpha + pTran[j * 3] * (1 - alpha);
                    pDst[j * 3 + 1] = pImgL[j * 3 + 1] * alpha + pTran[j * 3 + 1] * (1 - alpha);
                    pDst[j * 3 + 2] = pImgL[j * 3 + 2] * alpha + pTran[j * 3 + 2] * (1 - alpha);
                }
            }
        }
}

void Scroll_worker::CalcCorners(const cv::Mat& H, const cv::Mat src)
{
    //左上角(0,0,1)
        double v2[] = { 0, 0, 1 };
        double v1[3];//变换后的坐标值
        cv::Mat V2 = cv::Mat(3, 1, CV_64FC1, v2);  //列向量
        cv::Mat V1 = cv::Mat(3, 1, CV_64FC1, v1);  //列向量
        V1 = H * V2;
        corners.left_top.x = v1[0] / v1[2];
        corners.left_top.y = v1[1] / v1[2];

        //左下角(0,src.rows,1)
        v2[0] = 0;
        v2[1] = src.rows;
        v2[2] = 1;
        V2 = cv::Mat(3, 1, CV_64FC1, v2);  //列向量
        V1 = cv::Mat(3, 1, CV_64FC1, v1);  //列向量
        V1 = H * V2;
        corners.left_bottom.x = v1[0] / v1[2];
        corners.left_bottom.y = v1[1] / v1[2];

        //右上角(src.cols,0,1)
        v2[0] = src.cols;
        v2[1] = 0;
        v2[2] = 1;
        V2 = cv::Mat(3, 1, CV_64FC1, v2);  //列向量
        V1 = cv::Mat(3, 1, CV_64FC1, v1);  //列向量
        V1 = H * V2;
        corners.right_top.x = v1[0] / v1[2];
        corners.right_top.y = v1[1] / v1[2];

        //右下角(src.cols,src.rows,1)
        v2[0] = src.cols;
        v2[1] = src.rows;
        v2[2] = 1;
        V2 = cv::Mat(3, 1, CV_64FC1, v2);  //列向量
        V1 = cv::Mat(3, 1, CV_64FC1, v1);  //列向量
        V1 = H * V2;
        corners.right_bottom.x = v1[0] / v1[2];
        corners.right_bottom.y = v1[1] / v1[2];
}


int Scroll_worker::SURF(cv::Mat imageL, cv::Mat imageR, cv::Mat& ans, int img_height)
{
    if (imageL.empty() || imageR.empty())
    {
        qDebug() << "imread error!\n" << endl;
        return -1;
    }
    //转为灰度图
    cv::Mat grayL, grayR;
    cvtColor(imageL, imageL, cv::COLOR_RGBA2RGB);
    cvtColor(imageR, imageR, cv::COLOR_RGBA2RGB);
    cvtColor(imageL, grayL, cv::COLOR_RGB2GRAY);
    cvtColor(imageR, grayR, cv::COLOR_RGB2GRAY);

    //提取特征点

    cv::Mat imageDescR, imageDescL;
    //cv::Ptr<cv::xfeatures2d::SurfFeatureDetector> Detector;


    //double hessianThreshold = 1000;//阀值越大，得到的特征点的鲁棒性越好。
    //在处理场景简单的图像时，其阀值可以适当的调低。在复杂的图像中，
    //图像经旋转或者模糊后特征点变化的数量较大，测试需要适当提高阀值。
    //int nOctaves = 4;
//    int nOctaveLayers = 4;
//    bool isExtended = false;
//    bool isUpright = false;
//    cv::Ptr<cv::xfeatures2d::SURF> m_surf_det = Detector->create(hessianThreshold, nOctaves,
//        nOctaveLayers, isExtended, isUpright);

//    m_surf_det->detectAndCompute(grayL, cv::Mat(), keyPointL, imageDescL);
//    m_surf_det->detectAndCompute(grayR, cv::Mat(), keyPointR, imageDescR);
//    grayL.release();
//    grayR.release();
//    m_surf_det.release();
//    Detector.release();

//    cv::Ptr<cv::FastFeatureDetector> detector;

//    //detector = cv::ORB::create(5000, 1.2, 8, 31, 0, 2, cv::ORB::FAST_SCORE);
//    detector = cv::FastFeatureDetector::create();
//    std::vector<cv::KeyPoint> keyPointR, keyPointL;
//    detector->detectAndCompute(grayL, cv::noArray(), keyPointL, imageDescL);
//    detector->detectAndCompute(grayR, cv::noArray(), keyPointR, imageDescR);
    // Define features detector
    cv::Ptr<cv::FastFeatureDetector> detector = cv::FastFeatureDetector::create(20);
    // Detect the keypoints
    std::vector<cv::KeyPoint> keyPointR, keyPointL;
    detector->detect(grayL, keyPointL);
    detector->detect(grayR, keyPointR);
    //cv::Ptr<cv::xfeatures2d::BriefDescriptorExtractor> Descriptor =
    //        cv::xfeatures2d::BriefDescriptorExtractor::create(32);
    cv::Ptr<cv::ORB> Descriptor = cv::ORB::create();
    Descriptor->compute(grayL, keyPointL, imageDescL); //找出图一的关键点描述符 SiftDescriptorExtractor
    Descriptor->compute(grayR, keyPointR, imageDescR); //找出图二的关键点描述符 SiftDescriptorExtractor



    //cv::BFMatcher matcher;
    //cv::FlannBasedMatcher matcher;//基于Flann的描述符匹配器
    cv::FlannBasedMatcher matcher = cv::FlannBasedMatcher(cv::makePtr<cv::flann::LshIndexParams>(12, 20, 2));
    //cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");
    std::vector<std::vector<cv::DMatch>> matchePoints;//DMatch用于保存匹配结果
    std::vector<cv::DMatch> GoodMatchePoints;


    std::vector<cv::Mat> train_desc(1, imageDescR);
    matcher.add(train_desc); //添加描述符以训练CPU或GPU描述符集合
    matcher.train();//训练描述符匹配器
    matcher.knnMatch(imageDescL, matchePoints, 2);//K最近邻匹配-对每个匹配返回两个最佳匹配
    //matcher->knnMatch(imageDescR, imageDescL, matchePoints, 2);

    int process_width = 0;
    int process_sum = 0;
    int delta_width = 0;
    bool is_success = true;
TEST_AGAIN:;
    // Lowe's algorithm,获取优秀匹配点
    for (int i = 0; i < matchePoints.size(); i++)
    {
        if(matchePoints[i].size() >= 2)
        {
            int i1 = matchePoints[i][0].queryIdx;
            int i2 = matchePoints[i][0].trainIdx;
            if(i1 < 0 || i2 < 0 || i1>=static_cast<int>(keyPointL.size()) || i2>=static_cast<int>(keyPointR.size()))
            {
                continue;
            }
            cv::Point2f point1 = keyPointL[i1].pt;
            cv::Point2f point2 = keyPointR[i2].pt;
            bool history_influence = true;
            if(!is_success && std::abs(point1.x - Scroll_handler_global::instance()->middle_width) > img_height * 2 / 10)
            {
                history_influence = false;
            }
            //排除相同位置的干扰
            float dis = std::abs(point1.x - point2.x);
            if (dis >= 10 && dis && std::abs(point1.y-point2.y) <= 5
                    && dis > (Scroll_handler_global::instance()->delta_width >> 1) &&
                    matchePoints[i][0].distance < 0.33 * matchePoints[i][1].distance && history_influence)
            {
                delta_width += dis;
                process_width += keyPointL[i1].pt.x;
                process_sum++;
                GoodMatchePoints.push_back(matchePoints[i][0]);
            }
        }
    }
    if(process_sum == 0)
    {
        if(is_success)
        {
            is_success = false;
            goto TEST_AGAIN;
        }
//        cv::Mat first_match;
//        cv::drawMatches(imageL, keyPointL, imageR, keyPointR, GoodMatchePoints, first_match,
//                       cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
//                        cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
//        cv::imwrite("F:/dinfo/" + std::to_string(QDateTime::currentMSecsSinceEpoch()) + ".png", first_match);
        return -1;
    }
    process_width /= process_sum;
    delta_width /= process_sum;

    //画出匹配图
//    cv::Mat first_match;
//    cv::drawMatches(imageL, keyPointL, imageR, keyPointR, GoodMatchePoints, first_match,
//                   cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
//                    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
//    cv::imwrite("F:/dinfo/" + std::to_string(QDateTime::currentMSecsSinceEpoch()) + ".png", first_match);

    //优秀匹配点
    std::vector<cv::Point2f> imagePointsL, imagePointsR;
    for (int i = 0; i < GoodMatchePoints.size(); i++)
    {
        imagePointsR.push_back(keyPointL[GoodMatchePoints[i].queryIdx].pt);
        imagePointsL.push_back(keyPointR[GoodMatchePoints[i].trainIdx].pt);
    }

    //计算单应矩阵H，并细化匹配结果
    //射影变换:获取图像1到图像2的投影映射矩阵 尺寸为3*3
    if(imagePointsL.size() < 4 || imagePointsR.size() < 4)
    {
        qDebug() << "no match points";
        return -1;
    }
    cv::Mat H = cv::findHomography(imagePointsL, imagePointsR, cv::RANSAC);


    if (H.empty())
    {
        qDebug() << "findHomography error!\n" << endl;
        return -1;
    }

    //计算配准图的四个顶点坐标
    CalcCorners(H, imageR);
//    qDebug() << corners.right_top.x << corners.left_top.x << corners.right_bottom.x << corners.left_bottom.x
//             << imageR.cols;

    //将透视变换应用于图像

//    cv::Mat imageTransformR;
//    cv::warpPerspective(imageR, imageTransformR, H,
//                        cv::Size(MAX(corners.right_top.x + (corners.left_top.x < 0 ? -corners.left_top.x : 0)
//                                     , corners.right_bottom.x + (corners.left_bottom.x < 0 ? -corners.left_bottom.x : 0)),
//                                 imageL.rows),
//                        cv::INTER_LINEAR, cv::BORDER_REPLICATE);

//    //homography_warp(imageR, H, imageTransformR);
//    //图像拼接
//    qDebug() << process_width << imageTransformR.cols;
    int dst_width = MAX(corners.right_top.x + (corners.left_top.x < 0 ? -corners.left_top.x : 0)
                        , corners.right_bottom.x + (corners.left_bottom.x < 0 ? -corners.left_bottom.x : 0));  //取最右点的长度为拼接图的长度
    int dst_height = imageL.rows;
    if(dst_width <= imageR.cols)
    {
        if(is_success)
        {
            is_success = false;
            goto TEST_AGAIN;
        }
//        cv::Mat first_match;
//        cv::drawMatches(imageL, keyPointL, imageR, keyPointR, GoodMatchePoints, first_match,
//                       cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
//                        cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
//        cv::imwrite("F:/dinfo/" + std::to_string(QDateTime::currentMSecsSinceEpoch()) + ".png", first_match);
        return -1;
    }
    cv::Mat dst(dst_height, dst_width, CV_8UC3);
    dst.setTo(0);
    //cvtColor(imageTransformR, imageTransformR, cv::COLOR_RGBA2RGB);
    //cvtColor(imageL, imageL, cv::COLOR_RGBA2RGB);

    imageR.copyTo(dst(cv::Rect(dst_width-imageR.cols, 0, imageR.cols, imageR.rows)));
    imageL(cv::Rect(0, 0, process_width, imageL.rows))
            .copyTo(dst(cv::Rect(0, 0, process_width, imageL.rows)));
    //OptimizeSeam(imageL, imageTransformR, dst);	//优化拼接处
    //ImShow("dst", dst);
    ans = dst;
    Scroll_handler_global::instance()->cal_middle_width(process_width);
    Scroll_handler_global::instance()->cal_delta_width(delta_width);

    //cv::imwrite("F:/dinfo/" + std::to_string(QDateTime::currentMSecsSinceEpoch()) + ".png", ans);
    return 0;
}

