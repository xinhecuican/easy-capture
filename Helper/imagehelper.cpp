#include "imagehelper.h"
#include<QDebug>
#include <QGuiApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QIcon>
#include "common.h"
#include <QSvgRenderer>
#include <QPainter>

QImage ImageHelper::Mat2QImage(cv::Mat const& mat) {
    if(mat.type() == CV_8UC1) {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++) {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++) {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3) {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    } else if(mat.type() == CV_8UC4) {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    } else {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

cv::Mat ImageHelper::QImage2Mat(QImage const& image) {
    cv::Mat mat;
    switch(image.format()) {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
    case QImage::Format_Grayscale8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

cv::Mat ImageHelper::HBitmapToMat(HBITMAP& _hBmp) {
    //BITMAP操作
    BITMAP bmp;
    GetObject(_hBmp,sizeof(BITMAP),&bmp);
    int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel/8 ;
    int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
    //mat操作
    cv::Mat v_mat;
    v_mat.create(cvSize(bmp.bmWidth,bmp.bmHeight), CV_MAKETYPE(CV_8U,nChannels));
    GetBitmapBits(_hBmp,bmp.bmHeight*bmp.bmWidth*nChannels,v_mat.data);
    return v_mat;
}

bool ImageHelper::is_equal(const cv::Mat &data1, const cv::Mat &data2) {
    bool success = true;
    // check if is multi dimensional
    if(data1.dims > 2 || data2.dims > 2) {
        if( data1.dims != data2.dims || data1.type() != data2.type() ) {
            return false;
        }
        for(int dim = 0; dim < data1.dims; dim++) {
            if(data1.size[dim] != data2.size[dim]) {
                return false;
            }
        }
    } else {
        if(data1.size() != data2.size() || data1.channels() != data2.channels() || data1.type() != data2.type()) {
            return false;
        }
    }
    int nrOfElements = data1.total()*data1.elemSize1();
    //bytewise comparison of data
    int cnt = 0;
    for(cnt = 0; cnt < nrOfElements && success; cnt++) {
        if(data1.data[cnt] != data2.data[cnt]) {
            success = false;
        }
    }
    return success;
}

QPixmap ImageHelper::grabScreen() {
#if defined(Q_OS_WIN) && defined (_WIN32_WINNT_WIN10)
    bool success = false;
    QPixmap pix = DxgiManager::instance()->grabScreen(&success);
    if(!success){
        QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
        return screen->grabWindow(0);
    }
    return pix;
#else
    QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
    return screen->grabWindow(0);
#endif
}

QPixmap ImageHelper::grabScreen(int index) {
#if defined (Q_OS_WIN) && defined (_WIN32_WINNT_WIN10)
    bool success = false;
    QPixmap pix =  DxgiManager::instance()->grabScreen(index, &success);
    if(!success){
        QList<QScreen*> screens = QGuiApplication::screens();
        return screens.at(index)->grabWindow(0);
    }
    return pix;
#else
    QList<QScreen*> screens = QGuiApplication::screens();
    return screens.at(index)->grabWindow(0);
#endif
}

QScreen* ImageHelper::getCurrentScreen() {
    return QGuiApplication::screenAt(QCursor::pos());
}

QPixmap ImageHelper::grabScreen(int x, int y, int width, int height) {
#if defined (Q_OS_WIN) && defined (_WIN32_WINNT_WIN10)
    bool success = false;
    QPixmap pix = DxgiManager::instance()->grabScreen(&success);
    if(!success){
        QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
        return screen->grabWindow(0, x, y, width, height);
    }
    return pix.copy(x, y, width, height);
#else
    QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
    return screen->grabWindow(0, x, y, width, height);
#endif
}

QPixmap ImageHelper::grabScreen(int index, int x, int y, int width, int height) {
#if defined (Q_OS_WIN) && defined (_WIN32_WINNT_WIN10)
    bool success = false;
    QPixmap pix = DxgiManager::instance()->grabScreen(index, &success);
    if(!success){
        QScreen* screen = QGuiApplication::screens().at(index);
        return screen->grabWindow(0, x, y, width, height);
    }
    return pix.copy(x, y, width, height);
#else
    QScreen* screen = QGuiApplication::screens().at(index);
    return screen->grabWindow(0, x, y, width, height);
#endif
}

QPixmap ImageHelper::grabScreen(int index, WId wid) {
    QList<QScreen*> screens = QGuiApplication::screens();
    if(index >= screens.size()) {
        qWarning() << "index过大";
        return QPixmap();
    }
    return screens.at(index)->grabWindow(wid);
}

int ImageHelper::getCurrentIndex() {
    QList<QScreen*> screens = QGuiApplication::screens();
    QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
    for(int i=0; i<screens.size(); i++) {
        if(screens.at(i) == screen) {
            return i;
        }
    }
    return 0;
}

QRect ImageHelper::getCurrentGeometry(){
    return getCurrentScreen()->geometry();
}

QIcon ImageHelper::getIcon(QString name, int pt_w, int pt_h){
    QSvgRenderer render;
    render.load(":/image/" + name + ".svg");
    QPixmap pix(pt2px(pt_w), pt2px(pt_h));
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    render.render(&painter);
    QIcon icon(pix);
    return icon;
}

QPixmap ImageHelper::getPixmap(QString name, int pt_w, int pt_h){
    QSvgRenderer render;
    render.load(":/image/" + name + ".svg");

    QPixmap pix(pt2px(pt_w), pt2px(pt_h));
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    render.render(&painter);
    return pix;
}
