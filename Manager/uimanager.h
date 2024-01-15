#ifndef UIMANAGER_H
#define UIMANAGER_H
#include <QString>
#include "../Helper/Serialize.h"
#include "Data/uiglobalparser.h"

class UIManager : Serializable {
public:
    UIManager();
    ~UIManager();
    static UIManager* instance() {
        if(_instance == nullptr) {
            _instance = new UIManager();
        }
        return _instance;
    }

    QString load(QString name);
    void save(QString path);
    /**
     * @brief 载入config.json的设置
     * @details 根据config.json载入int值, color值和绘制形状的样式
     */
    void loadConfig();

    void serialized(QJsonObject *json) override;
    void deserialized(QJsonObject *json) override;
    PaintData getPencilData();
    PaintData getHighlighterData();
    PaintData getRectData();
    FontData getFontData();
    PaintData getArrowData();
    PaintData getBorderData();
    QColor getCaptureBackground();
    int getMosaicUnitSize();
    int getMosaicRange();
    void buildRelease(QString outPath);
private:
    QString combineSettings(QString str);

private:
    static UIManager* _instance;
    const QString UIPath = "Data/UI/";
    QHash<QString, QString> intDatas;
    QHash<QString, QString> colorDatas;
    UIGlobalParser globalParser;
    bool _isDynamic;
};

#endif // UIMANAGER_H
