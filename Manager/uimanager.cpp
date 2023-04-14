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
    if(_isDynamic) {
        ans = combineSettings(ans);
    }
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

    if(object.contains("dynamic")) {
        _isDynamic = object.value("dynamic").toBool(false);
    }
}

void UIManager::serialized(QJsonObject *json) {

}

QString UIManager::combineSettings(QString str) {
    QRegExp ex;
    ex.setPattern("var\\(\\s*\\-\\-(\\w*)\\)");
    int pos = 0;
    while(pos >= 0) {
        pos = ex.indexIn(str, pos);
        if(pos != -1) {
            QString key = ex.capturedTexts().at(1);
            if(intDatas.find(key) != intDatas.end()) {
                str.replace(pos, ex.matchedLength(), intDatas[key]);
                pos += intDatas[key].size();

            } else if(colorDatas.find(key) != colorDatas.end()) {
                str.replace(pos, ex.matchedLength(), colorDatas[key]);
                pos += colorDatas[key].size();
            } else {
                qWarning() << "未找到匹配";
            }
        }
    }
    return str;
}

QFileInfoList copyDir(QString src, QString dst) {
    QDir source(src);
    QDir dest(dst);
    if(!dest.exists()) {
        if(!dest.mkdir(dest.absolutePath())) {
            qDebug() << "mkdir fail";
            return QFileInfoList();
        }
    }

    QFileInfoList fileInfoList = source.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList) {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if(fileInfo.isDir()) {
            /**< 当为目录时，递归的进行copy */
            copyDir(fileInfo.filePath(), dest.filePath(fileInfo.fileName()));
        } else {         /**< 当允许覆盖操作时，将旧文件进行删除操作 */
            if(dest.exists(fileInfo.fileName())) {
                dest.remove(fileInfo.fileName());
            }

            /// 进行文件copy
            if(!QFile::copy(fileInfo.filePath(),
                            dest.filePath(fileInfo.fileName()))) {
                return QFileInfoList();
            }
        }
    }
    return fileInfoList;
}

void UIManager::buildRelease(QString outPath) {
    QFileInfoList fileList = copyDir(UIPath + Config::getConfig<QString>(Config::ui_theme_name) + "/", outPath);
    for(QFileInfo file : fileList) {
        if(file.fileName().endsWith(".qss")) {
            QString result = combineSettings(getQSS(file.filePath()));
            QFile outFile(outPath + "/" + file.fileName());
            if(!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                qCritical() << "file open error";
            } else {
                QTextStream out(&outFile);
                out.setCodec("utf-8");
                out << result;
                outFile.close();
            }
        }
    }
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

int UIManager::getMosaicRange() {
    return globalParser.getMosaicRange();
}

int UIManager::getMosaicUnitSize() {
    return globalParser.getMosaicUnitSize();
}
