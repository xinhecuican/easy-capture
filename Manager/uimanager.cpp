#include "uimanager.h"
#include <QFile>
#include <QDebug>
#include "Helper/common.h"
#include "config.h"
#include "Helper/mstring.h"

UIManager* UIManager::_instance = nullptr;

UIManager::UIManager() {

}

UIManager::~UIManager() {
    _instance = nullptr;
}

QString UIManager::load(QString name) {
    QString ans = getQSS("Data/UI/" + Config::getConfig<QString>(Config::ui_theme_name) + "/" + name + ".qss");
    return ans;
}

void UIManager::save(QString path) {

}
