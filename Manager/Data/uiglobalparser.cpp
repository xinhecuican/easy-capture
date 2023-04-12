#include "uiglobalparser.h"
#include <QDebug>

UIGlobalParser::UIGlobalParser() {
    pencilData = PaintData(QColor(150, 50, 50), 3);
    highlighterData = PaintData(QColor(255, 255, 0, 100), 12, QColor(255, 255, 255), Qt::RoundCap, Qt::RoundJoin);
}

void UIGlobalParser::serialized(QJsonObject *json) {

}

void UIGlobalParser::deserialized(QJsonObject *json) {
    if(json->contains("default_pencil")) {
        QJsonObject pencilObject = json->value("default_pencil").toObject();
        bool success;
        QVariant var = parse(COLOR, "color", &pencilObject, success);
        if(success) {
            pencilData.color = var.value<QColor>();
        }
        var = parse(WIDTH, "width", &pencilObject, success);
        if(success) {
            pencilData.width = var.value<int>();
        }
        var = parse(COLOR, "background_color", &pencilObject, success);
        if(success) {
            pencilData.back_color = var.value<QColor>();
        }
        var = parse(CAP_STYLE, "cap_style", &pencilObject, success);
        if(success) {
            pencilData.cap_style = var.value<Qt::PenCapStyle>();
        }
        var = parse(JOIN_STYLE, "join_style", &pencilObject, success);
        if(success) {
            pencilData.join_style = var.value<Qt::PenJoinStyle>();
        }
    }

    if(json->contains("default_highlighter")) {
        QJsonObject highlighterObject = json->value("default_highlighter").toObject();
        bool success;
        QVariant var = parse(COLOR, "color", &highlighterObject, success);
        if(success) {
            highlighterData.color = var.value<QColor>();
        }
        var = parse(WIDTH, "width", &highlighterObject, success);
        if(success) {
            highlighterData.width = var.value<int>();
        }
        var = parse(COLOR, "background_color", &highlighterObject, success);
        if(success) {
            highlighterData.back_color = var.value<QColor>();
        }
        var = parse(CAP_STYLE, "cap_style", &highlighterObject, success);
        if(success) {
            highlighterData.cap_style = var.value<Qt::PenCapStyle>();
        }
        var = parse(JOIN_STYLE, "join_style", &highlighterObject, success);
        if(success) {
            highlighterData.join_style = var.value<Qt::PenJoinStyle>();
        }
    }

    if(json->contains("default_rect")) {
        QJsonObject rectObject = json->value("default_rect").toObject();
        bool success;
        QVariant var = parse(COLOR, "color", &rectObject, success);
        if(success) {
            rectData.color = var.value<QColor>();
        }
        var = parse(WIDTH, "width", &rectObject, success);
        if(success) {
            rectData.width = var.toInt();
        }
    }

    if(json->contains("default_text")) {
        QJsonObject textObject = json->value("default_text").toObject();
        bool success;
        QVariant var = parse(COLOR, "color", &textObject, success);
        if(success) {
            fontData.color = var.value<QColor>();
        }
        var = parse(FONT, "font", &textObject, success);
        if(success) {
            fontData.font.setFamily(var.toString());
        }
        var = parse(WIDTH, "font_size", &textObject, success);
        if(success) {
            fontData.font.setPointSize(var.toInt());
        }
    }

    if(json->contains("default_arrow")) {
        QJsonObject arrowObject = json->value("default_arrow").toObject();
        bool success;
        QVariant var = parse(COLOR, "color", &arrowObject, success);
        if(success) {
            arrowData.color = var.value<QColor>();
        }
        var = parse(WIDTH, "width", &arrowObject, success);
        if(success) {
            arrowData.width = var.toInt();
        }
    }

    if(json->contains("default_border")) {
        QJsonObject borderObject = json->value("default_border").toObject();
        bool success;
        QVariant var = parse(COLOR, "color", &borderObject, success);
        if(success) {
            borderData.color = var.value<QColor>();
        }
        var = parse(WIDTH, "width", &borderObject, success);
        if(success) {
            borderData.width = var.toInt();
        }
    }

    if(json->contains("capture_background")) {
        QJsonObject backgroundObject = json->value("capture_background").toObject();
        bool success;
        QVariant var = parse(COLOR, "color", &backgroundObject, success);
        if(success) {
            captureBackgroundColor = var.value<QColor>();
        }
    }
}

QVariant UIGlobalParser::parse(ParseStyle style, QString name, QJsonObject *object, bool& success) {
    success = false;
    if(!object->contains(name))
        return QVariant();

    QJsonValue value = object->value(name);
    switch(style) {
    case COLOR: {
        success = true;
        int r, g, b, a;
        parseColor(value.toString(), r, g, b, a);
        return QVariant(QColor(r, g, b, a));
    }
    case WIDTH: {
        success = value.isDouble();
        if(!success)
            return QVariant();
        int ans = value.toInt();
        return QVariant(ans);
    }
    case CAP_STYLE: {
        success = value.isDouble();
        Qt::PenCapStyle style = (Qt::PenCapStyle)value.toInt();
        return QVariant(style);
    }
    case JOIN_STYLE: {
        success = value.isDouble();
        Qt::PenJoinStyle style = (Qt::PenJoinStyle)value.toInt();
        return QVariant(style);
    }
    case FONT: {
        success = value.isString();
        return QVariant(value.toString());
    }
    }
}

void UIGlobalParser::parseColor(QString colorString, int &r, int &g, int &b, int& a) {
    QRegExp ex;
    ex.setPattern("(rgb)a?\\(\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+),?\\s*(\\d*).*|(#)(.{2})(.{2})(.{2})(.{2})*");
    int pos = ex.indexIn(colorString);
    if(pos > -1) {
        bool ok;
        if(ex.capturedTexts().at(6) != "") {
            r = ex.capturedTexts().at(7).toInt(&ok, 16);
            if(!ok)
                r = 150;
            g = ex.capturedTexts().at(8).toInt(&ok, 16);
            if(!ok)
                g = 50;
            b = ex.capturedTexts().at(9).toInt(&ok, 16);
            if(!ok)
                b = 50;
            a = ex.capturedTexts().at(10).toInt(&ok, 16);
            if(!ok)
                a = 255;
        } else {
            r = ex.capturedTexts().at(2).toInt(&ok);
            if(!ok)
                r = 150;
            g = ex.capturedTexts().at(3).toInt(&ok);
            if(!ok)
                g = 50;
            b = ex.capturedTexts().at(4).toInt(&ok);
            if(!ok)
                b = 50;
            a = ex.capturedTexts().at(5).toInt(&ok);
            if(!ok)
                a = 255;
        }
    } else {
        qWarning() << "color parse error";
    }
}

PaintData UIGlobalParser::getPencilData() {
    return pencilData;
}

PaintData UIGlobalParser::getHighlighterData() {
    return highlighterData;
}

PaintData UIGlobalParser::getRectData() {
    return rectData;
}

FontData UIGlobalParser::getFontData() {
    return fontData;
}

PaintData UIGlobalParser::getArrowData() {
    return arrowData;
}

PaintData UIGlobalParser::getBorderData() {
    return borderData;
}

QColor UIGlobalParser::getCaptureBackground() {
    return captureBackgroundColor;
}
