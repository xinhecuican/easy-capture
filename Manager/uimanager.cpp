#include "uimanager.h"
#include <QFile>
#include <QDebug>
#include "Helper/common.h"
#include "config.h"
#include "Helper/mstring.h"
#include <QJsonArray>
#include "Manager/Data/uiparser.h"
#include "Manager/Data/uiglobalparser.h"

UIManager* UIManager::_instance = nullptr;

UIManager::UIManager() {
    loadConfig();
}

UIManager::~UIManager() {
    _instance = nullptr;
}

QString UIManager::load(QString name) {
    QString ans = getQSS(UIPath + Config::getConfig<QString>(Config::ui_theme_name) + "/" + name + ".qss");
    return ans;
}

void UIManager::save(QString path) {

}

void UIManager::loadConfig() {
    QString path = UIPath + Config::getConfig<QString>(Config::ui_theme_name) + "/config.json";
    Serialize::deserialize(path, this);
}

void UIManager::deserialized(QJsonObject *json) {
    QJsonObject object= *json;
    if(object.contains("int") && object["int"].isArray()) {
        for(QJsonValue value : object.value("int").toArray()) {
            UIParser parser;
            Serialize::deserialize(UIPath + Config::getConfig<QString>(Config::ui_theme_name) + "/" + value.toString(), &parser);
            intDatas.unite(parser.getData());
        }
    }
    if(object.contains("color") && object["color"].isArray()) {
        for(QJsonValue value : object["color"].toArray()) {
            UIParser parser;
            Serialize::deserialize(UIPath + Config::getConfig<QString>(Config::ui_theme_name) + "/" + value.toString(), &parser);
            colorDatas.unite(parser.getData());
        }
    }
    if(object.contains("global")) {
        Serialize::deserialize(UIPath + Config::getConfig<QString>(Config::ui_theme_name) + "/" + object["global"].toString(), &globalParser);
    }
}

void UIManager::serialized(QJsonObject *json) {

}

PaintData UIManager::getPencilData() {
    return globalParser.getPencilData();
}

PaintData UIManager::getHighlighterData() {
    return globalParser.getHighlighterData();
}

PaintData UIManager::getRectData() {
    return globalParser.getRectData();
}

FontData UIManager::getFontData() {
    return globalParser.getFontData();
}

PaintData UIManager::getArrowData() {
    return globalParser.getArrowData();
}

PaintData UIManager::getBorderData() {
    return globalParser.getBorderData();
}

QColor UIManager::getCaptureBackground() {
    return globalParser.getCaptureBackground();
}
