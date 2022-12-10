#ifndef MOSICSAMPLE_H
#define MOSICSAMPLE_H
#include <QWidget>
#include <QImage>

class MosicSample : public QWidget
{
public:
    MosicSample(QWidget* parent=nullptr);
    void setRange(int range);
    void setUnitSize(int unit_size);
    void paintEvent(QPaintEvent *event) override;
private:
    void reset();
    QImage image;
    QImage mask;
    int unit_size;
};

#endif // MOSICSAMPLE_H
