#ifndef UIGLOBALPARSER_H
#define UIGLOBALPARSER_H
#include "../../Base/Serializable.h"
#include "../../GraphicsScene/Widgets/PaintData.h"
#include <QVariantList>
#include "../../GraphicsScene/Widgets/FontData.h"

/**
 * @brief 主题global.json的解析类
 * @details
 *
 * 主要用来设置一些无法使用qss进行设置的样式，如绘图时初始笔的样式，文字的字体，形状的样式等
 *
 * 每种样式都是由若干属性组成，现在可以解析 @ref ParseStyle 说定义的属性
 *
 * 目前可以自定义的样式有：
 *
 * default_pencil: 画笔的样式,包含的属性有: color, width, default_color, cap_style, join_style
 *
 * default_highlighter: 记号笔的样式，包含的属性和画笔属性相同
 *
 * default_rect: 默认的矩形样式,属性有: color, width
 *
 * default_text: 默认文本框文字样式，属性有: color, font, font_size
 *
 * default_arrow: 默认箭头样式，属性有: color, width
 *
 * default_border: 默认的截图边框样式, 属性: color, width
 *
 * capture_background: 截图阴影，属性: color
 *
 * default_mosaic: 默认的马赛克设置，属性: unit_size, range。
 *
 * 其中unit_size是指将计算多大的正方形的平均值，越大模糊效果越好，range是模糊的范围, range>unit_size
 */
class UIGlobalParser : public Serializable {
public:
    UIGlobalParser();
    void serialized(QJsonObject *json) override;
    void deserialized(QJsonObject *json) override;
    void parseColor(QString colorString, int& r, int& g, int& b, int& a);
    PaintData getPencilData();
    PaintData getHighlighterData();
    PaintData getRectData();
    FontData getFontData();
    PaintData getArrowData();
    PaintData getBorderData();
    QColor getCaptureBackground();
    int getMosaicUnitSize();
    int getMosaicRange();
    /**
     * @brief 可以解析的样式种类
     */
    enum ParseStyle {
        COLOR, /// 颜色，样式必须为rgb(x, x, x)或者是#ffffff
        WIDTH, /// 粗细, int
        CAP_STYLE, /// 线段端点样式, Qt::PenCapStyle
        JOIN_STYLE, /// 线段连接样式, Qt::PenJoinStyle
        FONT /// 字体, QString
    };
private:

    QVariant parse(ParseStyle style, QString name, QJsonObject* object, bool& success);
private:
    PaintData pencilData;
    PaintData highlighterData;
    PaintData rectData;
    PaintData arrowData;
    FontData fontData;
    PaintData borderData;
    QColor captureBackgroundColor;
    int mosaicUnitSize;
    int mosaicRange;
};

#endif // UIGLOBALPARSER_H
