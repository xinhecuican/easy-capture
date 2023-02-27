#ifndef VIDEOTOOLBAR_H
#define VIDEOTOOLBAR_H
#include <QWidget>
#include <QHBoxLayout>
#include "ecvideocapture.h"

class VideoToolbar : public QWidget
{
public:
    VideoToolbar(QWidget* parent=nullptr);
    RecordInfo getRecordInfo();
private:
    QHBoxLayout* layout;
    RecordInfo info;
    QString name;
    QString suffix;
};

#endif // VIDEOTOOLBAR_H
