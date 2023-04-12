#include "uiparser.h"

void UIParser::serialized(QJsonObject *json) {

}

void UIParser::deserialized(QJsonObject *json) {
    QJsonObject object = *json;
    for(auto iter=object.begin(); iter!=object.end(); iter++) {
        data.insert(iter.key(), iter.value().toString());
    }
}

QHash<QString, QString> UIParser::getData() {
    return data;
}
