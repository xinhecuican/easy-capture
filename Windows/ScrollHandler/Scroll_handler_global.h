#ifndef SCROLL_HANDLER_GLOBAL_H
#define SCROLL_HANDLER_GLOBAL_H
#include<iostream>
#include<QProcess>
#include<QPair>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/xfeatures2d.hpp"

class Scroll_handler_global {
public:
    Scroll_handler_global() {
        middle_width = 0;
        delta_width = 0;
        QProcess queryCPU;
        QPair<int, int> pairResult;

        const QString queryCPUNumberOfCores = "wmic cpu get NumberOfCores";
        queryCPU.start(queryCPUNumberOfCores);
        queryCPU.waitForFinished();
        QString result = QString::fromLocal8Bit(queryCPU.readAllStandardOutput());
        QStringList list = queryCPUNumberOfCores.split(" ");
        result = result.remove(list.last(), Qt::CaseInsensitive);
        result = result.replace("\r", "");
        result = result.replace("\n", "");
        result = result.simplified();
        pairResult.first = result.toInt();
        num_core = pairResult.first;
    }

    ~Scroll_handler_global();

    static Scroll_handler_global* instance() {
        Scroll_handler_global* p = _instance;
        if (p == nullptr) {
            std::lock_guard<std::mutex> lock{ mutex };
            if ((p = _instance) == nullptr) {
                _instance = p = new Scroll_handler_global();
            }
        }
        return p;
    }
    void cal_middle_width(int new_width) {
        if(middle_width == 0) {
            middle_width = new_width;
            return;
        }
        middle_width = (float)middle_width * 0.9 + (float)new_width * 0.1;
    }
    void cal_delta_width(int new_width) {
        if(delta_width == 0) {
            delta_width = new_width;
            return;
        }
        delta_width = (float)delta_width * 0.9 + (float)new_width * 0.1;
    }
    int middle_width;
    int delta_width;
    int num_core;
private:
    static std::atomic<Scroll_handler_global*> _instance;
    static std::mutex mutex;
};

#endif // SCROLL_HANDLER_GLOBAL_H
