#ifndef ILAYERCONTROL_H
#define ILAYERCONTROL_H
#include <QImage>

class LayerBase;
/**
 * @brief 供@ref LayerBase 访问和操作图像及提供一些全局操作
 */
class ILayerControl
{
public:
    enum SaveType{ClipBoard, Persist, Temp};
    ILayerControl();
    virtual ~ILayerControl(){}
    virtual bool save(SaveType type, const QString& path="")=0;
    virtual QImage getOriginImage() const;
    virtual QImage getImage() const;
    bool isImageValid();
    virtual bool hasFocus(LayerBase* layer)=0;
    virtual void requestFocus(LayerBase* layer)=0;
    virtual void removeThis(LayerBase* layer)=0;
    virtual void modifyImage(const QPoint& pos, const QColor& color);
    virtual void maskImage(const QImage& mask);
    virtual void remaskImage(const QImage& mask);
    virtual void setEraseEnable(bool enable)=0;
    virtual void setEnable(bool enable, int index=0)=0;
    virtual QRectF getClipRect()=0;
protected:
    bool imageValid;
    QImage image;
    QImage modifiedImage;
    bool modified;
};

#endif // ILAYERCONTROL_H
