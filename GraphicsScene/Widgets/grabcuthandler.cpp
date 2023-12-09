#include "grabcuthandler.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <QFile>
#include <QTextStream>
#include "../../Helper/imagehelper.h"
#include "../../Helper/debug.h"
#include <QDateTime>

GrabcutHandler::GrabcutHandler(QObject* parent)
    : QObject(parent),
      _finish(false),
      working(false),
      waiting(false)
{
    qRegisterMetaType<QList<QPainterPath>>("QList<QPainterPath>");
    worker = new GrabcutWorker();
    worker->moveToThread(&thread);
    connect(this, &GrabcutHandler::startWork, worker, &GrabcutWorker::work);
    connect(worker, &GrabcutWorker::finish, this, [=](QImage mask){
        emit finish(mask);
        if(waiting){
            emit startWork(waitImage, waitfgd, waitbgd);
            waiting = false;
            return;
        }
        _finish = true;
        working = false;
    });
    thread.start();
}

GrabcutHandler::~GrabcutHandler(){
    thread.quit();
    thread.wait();
    worker->deleteLater();
}

void GrabcutHandler::start(QImage image, QList<QPainterPath> fgd, QList<QPainterPath> bgd){
    if(working){
        waitImage = image;
        waitfgd = fgd;
        waitbgd = bgd;
        waiting = true;
        return;
    }
    emit startWork(image, fgd, bgd);
    _finish = false;
    working = true;
}

bool GrabcutHandler::isFinish(){
    return _finish;
}

GrabcutWorker::GrabcutWorker(QObject* parent) : QObject(parent){

}

void GrabcutWorker::work(QImage image, QList<QPainterPath> fgd, QList<QPainterPath> bgd){
    cv::Mat inputMat = ImageHelper::QImage2Mat(image);
    cv::cvtColor(inputMat, inputMat, cv::COLOR_RGBA2RGB);

    cv::Mat maskMat(image.height(), image.width(), CV_8UC1);
    maskMat.setTo(cv::Scalar::all(cv::GC_PR_BGD));

    cv::Mat borderMat;
    cv::Canny(inputMat, borderMat, 50, 150);
    maskMat.setTo(cv::GC_PR_FGD, borderMat);
    for(int i=0; i<fgd.size(); i++){
        for(int k=1; k<fgd[i].elementCount(); k++){
            cv::Point p1;
            p1.x = fgd[i].elementAt(k-1).x;
            p1.y = fgd[i].elementAt(k-1).y;
            cv::Point p2;
            p2.x = fgd[i].elementAt(k).x;
            p2.y = fgd[i].elementAt(k).y;
            cv::line(maskMat, p1, p2, cv::GC_FGD, 5);
        }
    }
    for(int i=0; i<bgd.size(); i++){
        for(int k=1; k<bgd[i].elementCount(); k++){
            cv::Point p1;
            p1.x = bgd[i].elementAt(k-1).x;
            p1.y = bgd[i].elementAt(k-1).y;
            cv::Point p2;
            p2.x = bgd[i].elementAt(k).x;
            p2.y = bgd[i].elementAt(k).y;
            cv::line(maskMat, p1, p2, cv::GC_BGD, 5);
        }
    }

    cv::Mat bgdModel, fgdModel;
    Debug::beginTimer("grabcut");
    cv::grabCut(inputMat, maskMat, cv::Rect(), bgdModel, fgdModel, 1, cv::GC_INIT_WITH_MASK);
    Debug::endTimer();
    std::string currentTime = std::to_string(QDateTime::currentMSecsSinceEpoch());
    cv::Mat result, binMask;
    maskMat = maskMat & 1;
    maskMat.setTo(255, maskMat == 1);
    emit finish(ImageHelper::Mat2QImage(maskMat));
}
