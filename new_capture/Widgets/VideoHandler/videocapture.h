#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H
#include "ecvideocapture.h"
#include <QObject>

class VideoCaptureHandler : public QObject
{
public:
    VideoCaptureHandler(QObject* parent);
    void initHandler(RecordInfo info);
    void setCaptureInfo(RecordInfo info);
    void startCapture();
    void stopCapture();
    void pauseOrResume();
    bool getIsPause();
    bool isValid();
    QRect getBound();
private:
    void resumeCapture();
    void pauseCapture();
    std::unique_ptr<ECVideoCapture> handler;
    bool isInit;
    bool isPause;
    QRect bound;
};

#endif // VIDEOCAPTURE_H
