#include "videocapture.h"

VideoCaptureHandler::VideoCaptureHandler(QObject* parent) : QObject(parent)
{
    isInit = false;
}

void VideoCaptureHandler::initHandler(RecordInfo info){
    if(!isInit){
        bound = info.bound;
        handler = createRecorder(info);
        isInit = true;
    }
}

void VideoCaptureHandler::setCaptureInfo(RecordInfo info){
    if(!isInit){
        bound = info.bound;
        handler = createRecorder(info);
        isInit = true;
        return;
    }
    bound = info.bound;
    handler->setRecordInfo(info);
}

void VideoCaptureHandler::startCapture(){
    if(!isInit){
        return;
    }
    handler->startRecord();
    isPause = false;
}

void VideoCaptureHandler::stopCapture(){
    if(!isInit){
        return;
    }
    handler->stopRecord();
    isPause = true;
}

void VideoCaptureHandler::resumeCapture(){
    if(!isInit){
        return;
    }
    handler->resumeRecord();
    isPause = false;
}

void VideoCaptureHandler::pauseCapture(){
    if(!isInit){
        return;
    }
    handler->pauseRecord();
}

void VideoCaptureHandler::pauseOrResume(){
    if(isPause){
        resumeCapture();
    }
    else{
        pauseCapture();
    }
}

bool VideoCaptureHandler::getIsPause(){
    return isPause;
}

QRect VideoCaptureHandler::getBound(){
    return bound;
}

bool VideoCaptureHandler::isValid(){
    return bound.width() > 0 && bound.height() > 0;
}
