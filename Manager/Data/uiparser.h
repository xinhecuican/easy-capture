#ifndef UIPARSER_H
#define UIPARSER_H
#include "Base/Serializable.h"
#include <QHash>

class UIParser : public Serializable {
public:
    void serialized(QJsonObject *json) override;
    void deserialized(QJsonObject *json) override;
    QHash<QString, QString> getData();
private:
    QHash<QString, QString> data;
};

#endif // UIINTPARSER_H
