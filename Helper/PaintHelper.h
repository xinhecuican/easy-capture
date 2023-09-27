#ifndef PAINTHELPER_H
#define PAINTHELPER_H
#include <QPainter>
#include <qmath.h>
#include <QDebug>

class PaintHelper{
public:
    /**
     * @brief paintShadow 绘制阴影
     * @param painter
     * @param rect 绘制阴影的窗口范围，将在这个范围外绘制
     * @param shadowWidth
     * @param color 阴影的颜色，其中a通道表示最深的阴影颜色
     * @param roundRaidus 圆角矩形半径
     */
    static void paintShadow(QPainter* painter, QRectF rect, int shadowWidth, QColor color, int roundRaidus=5){
        int alpha = color.alpha();
        for (int i = 0; i < shadowWidth; i++)
        {
            int currentAlpha = alpha * pow((double)(shadowWidth - i) / (double)(shadowWidth), 3);
            color.setAlpha(currentAlpha);
            painter->setPen(color);
            painter->drawRoundedRect(rect.left() - i, rect.top() - i, rect.width() + i * 2, rect.height() + i * 2, roundRaidus, roundRaidus);
        }
    }
};

#endif // PAINTHELPER_H
