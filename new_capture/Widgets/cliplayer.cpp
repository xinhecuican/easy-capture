#include "cliplayer.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include "Helper/math.h"
#include "Helper/plist.h"
#include <QGuiApplication>
#include <QScreen>
#include "Manager/WindowManager.h"
#include <QPixmap>
#include <QBitmap>
#include <QFile>
#include <QFileDialog>
#include "Paint/Widgets/history.h"
#include "Paint/Widgets/recorder.h"
#include <QButtonGroup>
#include "Paint/Widgets/style_manager.h"
#include "Helper/common.h"
#include <QGraphicsWidget>
#include <QGraphicsScene>
#include "Manager/config.h"
#include "Paint/Widgets/Panels/flow_edit_panel.h"
#include <QTimer>
#include <QProcess>
#include "Scroll_handler/Scroll_handler_global.h"

ClipLayer::ClipLayer(QWidget* widget_parent, QGraphicsScene* scene, QGraphicsItem* parent) : QGraphicsObject(parent),
    is_drag(false),
    pixSet(false),
    begin_clip(false),
    is_save(false),
    is_enable(true),
    scene(scene),
    currentshape(SHAPE_RECT) {
    screen_rect = QGuiApplication::primaryScreen()->geometry();
    mask_layer = new MaskLayer(this);
    connect(mask_layer, &MaskLayer::regionChanged, this, [=]() {
        calBarPos();
    });
    this->widget_parent = widget_parent;
    rect_setting = UIManager::instance()->getRectData();
    arrowSetting = UIManager::instance()->getArrowData();
    pencil_setting = UIManager::instance()->getPencilData();
    highlighter_setting = UIManager::instance()->getHighlighterData();
    widgetParent = NULL;
    toolbar = NULL;
    attribute_toolbar = NULL;
    setToolBar();
    setAttributeToolbar();
    if(widgetParent != NULL) {
        toolbar->setParent(widgetParent);
        attribute_toolbar->setParent(widgetParent);
    }
}

ClipLayer::~ClipLayer() {
    delete toolbar;
    delete attribute_toolbar;
}

QPainterPath ClipLayer::shape() const {
    QPainterPath path;
    path.addRect(screen_rect);
    return path;
}

QRectF ClipLayer::boundingRect() const {
    return screen_rect;
}

void ClipLayer::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    button = event->button();
    if(button == Qt::RightButton) {
        if(mask_layer->getRegionCount() == 0) {
            WindowManager::changeWindow("tray");
        } else {
            emit needReset();
        }
    } else if(button == Qt::MidButton) {
        switch (Config::getConfig<int>(Config::middle_button_type)) {
        case PaintWindow:
            emit requestImage();
            break;
        case Save2Clip:
            emit needClip();
            break;
        case Save2File:
            emit needSave();
            break;
        }
    } else {
        toolbar->hide();
        attribute_toolbar->hide();
    }
}

void ClipLayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
}

void ClipLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
//    if(!is_enable)
    {
        calBarPos();
    }
}

void ClipLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if(pixSet) {
        if(!is_save) {
            painter->drawPixmap(QPointF(0, 0), pix);
        } else {
            painter->drawPixmap(QPointF(0, 0), savePix);
        }
    }
}

void ClipLayer::prepareSave() {
    mask_layer->prepareSave();
    is_save = true;
    QPainterPath path = mask_layer->getPath();
    QPixmap ans;
    QPixmap mask = QPixmap(pix.width(), pix.height());
    mask.fill(Qt::transparent);
    QPainter painter(&mask);
    painter.fillPath(path, QColor(1, 1, 1));
    savePix = pix;
    savePix.setMask(mask.createMaskFromColor(QColor(1, 1, 1), Qt::MaskOutColor));
}

void ClipLayer::endSave() {
    is_save = false;
    mask_layer->endSave();
}

void ClipLayer::capture(QPixmap pix) {
    QPainterPath path;
    QPolygonF polygon;
    QPixmap ans;
    QRect rect;
    for(int i=0; i<regions.size(); i++) {
        QPolygonF temp_polygon = regions[i]->getPolygon();
        path.addPolygon(temp_polygon);
        polygon = polygon.united(temp_polygon);
    }
    QPixmap mask = QPixmap(pix.width(), pix.height());
    mask.fill(Qt::transparent);
    QPainter painter(&mask);
    painter.fillPath(path, QColor(1, 1, 1));
    pix.setMask(mask.createMaskFromColor(QColor(1, 1, 1), Qt::MaskOutColor));
    rect = path.boundingRect().toRect();
    ans = pix.copy(rect);
    rect.moveTo(0, 0);
    WindowManager::changeWindow("PaintWindow");
    WindowManager::getWindow("PaintWindow")->setPic(ans, rect);
}

void ClipLayer::setPic(QPixmap pix) {
    pixSet = true;
    this->pix = pix;
    update();
}

QRectF ClipLayer::getClipRect() {
    QRectF ans_rect = mask_layer->getClipRect();
    if(ans_rect.left() < 0)
        ans_rect.setLeft(0);
    if(ans_rect.top() < 0)
        ans_rect.setTop(0);
    if(ans_rect.bottom() > screen_rect.bottom())
        ans_rect.setBottom(screen_rect.bottom());
    if(ans_rect.right() > screen_rect.right())
        ans_rect.setRight(screen_rect.right());
    return ans_rect;
}

void ClipLayer::reset() {
    is_save = false;
    buttonEnter(2);
    mask_layer->reset();
    if(toolbar != NULL)
        toolbar->hide();
    if(attribute_toolbar != NULL)
        attribute_toolbar->hide();
    update();
}

void ClipLayer::sendRequestImage() {
    emit requestImage();
}

void ClipLayer::setToolBar() {
    toolbar = new QToolBar();
    toolbar->setWindowFlags(toolbar->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    toolbar->setWindowFlag(Qt::WindowSystemMenuHint, false);
    toolbar->hide();

    ocrButton = new QToolButton(toolbar);
    ocrButton->setIcon(QIcon(":/image/ocr.png"));
    ocrButton->setToolTip(MString::search("{SvJhCjRGF0}提取文字"));
    connect(ocrButton, &QToolButton::clicked, this, [=]() {
        emit requestOcr();
    });

    save_button = new QToolButton(toolbar);
    save_button->setIcon(QIcon(":/image/save.png"));
    save_button->setToolTip(MString::search("{pJqTHhEQdb}保存"));
    connect(save_button, &QToolButton::clicked, this, [=]() {
        emit needSave();
    });

    clip_button = new QToolButton(toolbar);
    clip_button->setIcon(QIcon(":/image/clipboard.png"));
    clip_button->setToolTip(MString::search("{ntbJbEqxwF}复制到剪切板"));
    connect(clip_button, &QToolButton::clicked, this, [=]() {
        emit needClip();
    });

    erase_button = new QToolButton(toolbar);
    erase_button->setIcon(QIcon(":/image/eraser.png"));
    erase_button->setToolTip(MString::search("{7cwKObEhcx}擦除"));
    erase_button->setCheckable(true);

    videoButton = new QToolButton(toolbar);
    videoButton->setIcon(QIcon(":/image/videocam.png"));
    videoButton->setToolTip(MString::search("{UowiwjDUIy}视频"));
    videoButton->setCheckable(true);

    ok_button = new QToolButton(toolbar);
    ok_button->setIcon(QIcon(":/image/ok.svg"));
    connect(ok_button, &QToolButton::clicked, this, [=]() {
        emit requestImage();
    });

    cancel_button = new QToolButton(toolbar);
    cancel_button->setIcon(QIcon(":/image/cancel.svg"));
    connect(cancel_button, &QToolButton::clicked, this, [=]() {
        WindowManager::changeWindow("tray");
    });

    undo_button = new QToolButton(toolbar);
    undo_button->setIcon(QIcon(":/image/undo.png"));
    undo_button->setToolTip(MString::search("{h5KymvIMTN}撤销"));
    connect(undo_button, &QPushButton::clicked, this, [=]() {
        Recorder::instance()->back();
    });

    redo_button = new QToolButton(toolbar);
    redo_button->setToolTip(MString::search("{a7CaC7NOL5}恢复"));
    redo_button->setIcon(QIcon(":/image/redo.png"));
    connect(redo_button, &QPushButton::clicked, this, [=]() {
        Recorder::instance()->forward();
    });
    connect(Recorder::instance(), &Recorder::recordChange, this, [=]() {
        if(!Recorder::instance()->undoAvaliable()) {
            undo_button->setEnabled(false);
        } else {
            undo_button->setEnabled(true);
        }
        if(!Recorder::instance()->redoAvaliable()) {
            redo_button->setEnabled(false);
        } else {
            redo_button->setEnabled(true);
        }
    });

    mosaic_button = new QToolButton(toolbar);
    mosaic_button->setIcon(QIcon(":/image/mosaic.png"));
    mosaic_button->setToolTip("马赛克");
    mosaic_button->setCheckable(true);

    cursor_button = new QToolButton(toolbar);
    cursor_button->setIcon(QIcon(":/image/cursor.png"));
    cursor_button->setToolTip(MString::search("{l4yTU9QXUd}指针"));
    cursor_button->setCursor(QCursor(QPixmap(":/image/cursor.png"), 0, 0));
    cursor_button->setCheckable(true);
    cursor_button->setChecked(true);

    pencil_button = new QToolButton(toolbar);
    pencil_button->setIcon(QIcon(":/image/pencil.png"));
    pencil_button->setCheckable(true);

    highlighter_button = new QToolButton(toolbar);
    highlighter_button->setToolTip(MString::search("{j54u1kWtCx}荧光笔"));
    highlighter_button->setIcon(QIcon(":/image/highlighter.png"));
    highlighter_button->setCheckable(true);

    text_button = new QToolButton(toolbar);
    text_button->setToolTip("文本框");
    text_button->setIcon(QIcon(":/image/text.png"));
    text_button->setCheckable(true);

    shape_button = new QToolButton(toolbar);
    shape_button->setToolTip(MString::search("{25JzGpOvFt}形状"));
    shape_button->setIcon(QIcon(":/image/shape.png"));
    shape_button->setCheckable(true);

    button_group = new QButtonGroup(toolbar);
    button_group->setExclusive(true);
    button_group->addButton(shape_button, 0);
    button_group->addButton(mosaic_button, 1);
    button_group->addButton(cursor_button, 2);
    button_group->addButton(pencil_button, 3);
    button_group->addButton(highlighter_button, 4);
    button_group->addButton(text_button, 5);
    button_group->addButton(erase_button, 6);
    button_group->addButton(videoButton, 7);
    connect(button_group, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked)
    , this, [=](int id) {
        onToolbarButtonClick(id);
    });

    toolbar->addWidget(shape_button);
    toolbar->addWidget(mosaic_button);
    toolbar->addWidget(cursor_button);
    toolbar->addWidget(pencil_button);
    toolbar->addWidget(highlighter_button);
    toolbar->addWidget(text_button);
    toolbar->addWidget(erase_button);
    toolbar->addWidget(videoButton);
    toolbar->addSeparator();
    toolbar->addWidget(undo_button);
    toolbar->addWidget(redo_button);
    toolbar->addWidget(ocrButton);
    toolbar->addWidget(clip_button);
    toolbar->addWidget(save_button);
    toolbar->addWidget(ok_button);
    toolbar->addWidget(cancel_button);
}

void ClipLayer::calBarPos() {
    if(!toolbar->isVisible())
        toolbar->show();
    if(!attribute_toolbar->isVisible())
        attribute_toolbar->show();

    if(button_group->checkedButton() == NULL) {
        qDebug() << "ClipLayer::calBarPos checked button NULL";
        return;
    }
    int bar_height = toolbar->height();
    int bar_width = toolbar->width();
    QRectF mask_bound = mask_layer->getClipRect();
    int left = mask_bound.right() - bar_width;
    int top = mask_bound.bottom();

    int attributeBarWidth = attribute_toolbar->width();
    int attributeBarHeight = attribute_toolbar->height();
    int maxWidth = button_group->checkedButton()->pos().x() + attributeBarWidth > bar_width ? button_group->checkedButton()->pos().x() + attributeBarWidth : bar_width;
    if(left + maxWidth > screen_rect.right()) {
        left = screen_rect.right() - maxWidth;
    }
    if(left < 0) {
        left = 0;
    }
    if(top + bar_height + attributeBarHeight > screen_rect.bottom()) {
        top = screen_rect.bottom() - bar_height - attributeBarHeight;
    }
    if(top < 0) {
        top = 0;
    }
    toolbar->move(left, top);
    attribute_toolbar->move(left + button_group->checkedButton()->pos().x(), top + bar_height);
}

void ClipLayer::setEnable(bool enable) {
    is_enable = enable;
    if(!is_enable) {
        calBarPos();
    }
    mask_layer->setEnable(enable);
}

void ClipLayer::updateAttributeToolbar(int id) {
    switch(before_id) {
    case 0: // shape
        switch(currentshape) {
        case SHAPE_RECT:
            rect_setting.color = Style_manager::instance()->get_now().color;
            rect_setting.width = Style_manager::instance()->get_now().width;
            break;
        case SHAPE_ARROW:
            arrowSetting.color = Style_manager::instance()->get_now().color;
            arrowSetting.width = Style_manager::instance()->get_now().width;
            break;
        }
        break;
    case 1: // mosaic
        break;
    case 2: // cursor
        break;
    case 3: // pencil
        pencil_setting.color = Style_manager::instance()->get_now().color;
        pencil_setting.width = Style_manager::instance()->get_now().width;
        break;
    case 4: // highlighter
        highlighter_setting.color = Style_manager::instance()->get_now().color;
        highlighter_setting.width = Style_manager::instance()->get_now().width;
        break;
    }
    attribute_toolbar->removeAll();
    initAttributeToolbarWidget(id);
    switch(id) {
    case 0: // shape
        switch(currentshape) {
        case SHAPE_RECT:
            color_widget->setCurrentStyle(rect_setting.color);
            Style_manager::instance()->change_color(rect_setting.color);
            Style_manager::instance()->change_width(rect_setting.width);
            width_button->setValue(rect_setting.width);
            break;
        case SHAPE_ARROW:
            color_widget->setCurrentStyle(arrowSetting.color);
            Style_manager::instance()->change_color(arrowSetting.color);
            Style_manager::instance()->change_width(arrowSetting.width);
            width_button->setValue(arrowSetting.width);
            break;
        }
        attribute_toolbar->add(rect_button);
        attribute_toolbar->add(arrow_button);
        attribute_toolbar->add(width_button);
        attribute_toolbar->add(color_widget);
        break;
    case 1: // mosaic
        attribute_toolbar->add(mosaic_sample);
        attribute_toolbar->add(mosaic_size);
        attribute_toolbar->add(mosaic_range);
        break;
    case 2: // cursor
        attribute_toolbar->add(rect_capture);
        attribute_toolbar->add(free_capture);
        break;
    case 3:
        color_widget->setCurrentStyle(pencil_setting.color);
        Style_manager::instance()->change_color(pencil_setting.color);
        Style_manager::instance()->change_width(pencil_setting.width);
        width_button->setValue(pencil_setting.width);
        attribute_toolbar->add(width_button);
        attribute_toolbar->add(color_widget);
        break;
    case 4:
        color_widget->setCurrentStyle(highlighter_setting.color);
        Style_manager::instance()->change_color(highlighter_setting.color);
        Style_manager::instance()->change_width(highlighter_setting.width);
        width_button->setValue(highlighter_setting.width);
        attribute_toolbar->add(width_button);
        attribute_toolbar->add(color_widget);
        break;
    case 5:
        attribute_toolbar->add(Flow_edit_panel::instance());
        break;
    case 7:
        attribute_toolbar->add(videoToolbar);
        break;
    }
    calBarPos();
}

void ClipLayer::initAttributeToolbarWidget(int id) {
    switch(id) {
    case 0:
        if(shapeGroup == NULL) {
            shapeGroup = new QButtonGroup(attribute_toolbar);
            shapeGroup->setExclusive(true);
            connect(shapeGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked)
            , this, [=](int id) {
                switch(id) {
                case 0:
                    currentshape = SHAPE_RECT;
                    break;
                case 1:
                    currentshape = SHAPE_ARROW;
                    break;
                }
                onToolbarButtonClick(0);
            });
        }
        if(rect_button == NULL) {
            rect_button = new QToolButton(attribute_toolbar);
            rect_button->setObjectName("rectButton");
            rect_button->setToolTip("矩形");
            rect_button->setIcon(QIcon(":/image/rect.png"));
            rect_button->setCheckable(true);
            rect_button->setChecked(true);
            rect_button->hide();
            shapeGroup->addButton(rect_button, 0);
        }
        if(arrow_button == NULL) {
            arrow_button = new QToolButton(attribute_toolbar);
            arrow_button->setObjectName("arrowButton");
            arrow_button->setIcon(QIcon(":/image/paint_arrow.png"));
            arrow_button->setToolTip("{D7HSBXWTLj}箭头");
            arrow_button->setCheckable(true);
            arrow_button->hide();
            shapeGroup->addButton(arrow_button, 1);
        }
        if(color_widget == NULL) {
            color_widget = new ColorWidget(attribute_toolbar);
            color_widget->hide();
        }
        if(width_button == NULL) {
            width_button = new QSpinBox(attribute_toolbar);
            width_button->setRange(1, 50);
            width_button->setValue(3);
            width_button->setAccelerated(true);
            width_button->setWrapping(true);
            width_button->setKeyboardTracking(true);
            connect(width_button, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
                Style_manager::instance()->change_width(value);
            });
            width_button->hide();
        }
        break;
    case 3:
    case 4:
        if(color_widget == NULL) {
            color_widget = new ColorWidget(attribute_toolbar);
            color_widget->hide();
        }
        if(width_button == NULL) {
            width_button = new QSpinBox(attribute_toolbar);
            width_button->setRange(1, 50);
            width_button->setValue(3);
            width_button->setAccelerated(true);
            width_button->setWrapping(true);
            width_button->setKeyboardTracking(true);
            connect(width_button, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
                Style_manager::instance()->change_width(value);
            });
            width_button->hide();
        }
        break;
    case 1:
        if(mosaic_sample == NULL) {
            mosaic_sample = new MosicSample(attribute_toolbar);
            mosaic_size = new QSpinBox(attribute_toolbar);
            mosaic_size->setRange(4, 20);
            mosaic_size->setValue(4);
            mosaic_size->setSingleStep(4);
            mosaic_size->setAccelerated(true);
            mosaic_size->setWrapping(true);
            mosaic_size->setKeyboardTracking(true);
            connect(mosaic_size, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
                mosaic_sample->setUnitSize(value);
                emit mosaicChange(false, value);
            });
            mosaic_range = new QSlider(attribute_toolbar);
            mosaic_range->setValue(12);
            mosaic_range->setMinimum(4);
            mosaic_range->setMaximum(48);
            mosaic_range->setSingleStep(4);
            mosaic_range->setOrientation(Qt::Horizontal);
            mosaic_range->setToolTip("12");
            connect(mosaic_range, &QSlider::valueChanged, this, [=]() {
                emit mosaicChange(true, mosaic_range->value());
                mosaic_size->setMaximum(mosaic_range->value());
                mosaic_range->setToolTip(QString::number(mosaic_range->value()));
            });
            mosaic_sample->hide();
            mosaic_size->hide();
            mosaic_range->hide();
        }
        break;
    case 2:
        if(rect_capture == NULL) {
            QButtonGroup* mode_group = new QButtonGroup(attribute_toolbar);
            mode_group->setExclusive(true);

            rect_capture = new QToolButton(attribute_toolbar);
            rect_capture->setIcon(QIcon(QPixmap(":/image/rect.png").scaled(42, 42)));
            rect_capture->setToolTip(MString::search("{OBwjJUhTkh}矩形窗口"));
            rect_capture->setChecked(true);
            rect_capture->setCheckable(true);
            rect_capture->setIconSize(QSize(36, 36));
            free_capture = new QToolButton(attribute_toolbar);
            free_capture->setIcon(QIcon(QPixmap(":/image/painterpath.png").scaled(42,42)));
            free_capture->setIconSize(QSize(36, 36));
            free_capture->setCheckable(true);
            mode_group->addButton(rect_capture, 0);
            mode_group->addButton(free_capture, 1);
            connect(mode_group, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked)
            , this, [=](int id) {
                if(id == 0) {
                    Config::setConfig(Config::capture_mode, Config::RECT_CAPTURE);
                } else if(id == 1) {
                    Config::setConfig(Config::capture_mode, Config::FREE_CAPTURE);
                }
            });
            rect_capture->hide();
            free_capture->hide();
        }
        break;
    case 5:
        if(Flow_edit_panel::instance()->parent() != attribute_toolbar) {
            Flow_edit_panel::instance()->setParent(attribute_toolbar);
            Flow_edit_panel::instance()->hide();
        }
        break;
    case 7: // video
        if(videoToolbar == NULL) {
            videoToolbar = new VideoToolbar(attribute_toolbar);
            videoToolbar->hide();
        }
        break;
    }
}

void ClipLayer::setAttributeToolbar() {
    attribute_toolbar = new AttributeToolbar();
    attribute_toolbar->setWindowFlags(toolbar->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    attribute_toolbar->setWindowFlag(Qt::WindowSystemMenuHint, false);

    color_widget = NULL;
    width_button = NULL;
    mosaic_sample = NULL;
    mosaic_size = NULL;
    mosaic_range = NULL;
    rect_capture = NULL;
    free_capture = NULL;
    rect_button = NULL;
    shapeGroup = NULL;
    arrow_button = NULL;
    videoToolbar = NULL;

    updateAttributeToolbar(2);
}

void ClipLayer::setWidgetParent(QWidget *parent) {
    widgetParent = parent;
    if(attribute_toolbar != NULL) {
        toolbar->setParent(parent);
        attribute_toolbar->setParent(parent);
    }
}

void ClipLayer::buttonEnter(int id) {
    button_group->button(id)->setChecked(true);
    onToolbarButtonClick(id);
}

void ClipLayer::onToolbarButtonClick(int id) {
    updateAttributeToolbar(id);
    switch(id) {
    case 0: // shape
        switch(currentshape) {
        case SHAPE_RECT:
            emit stateChange(SHAPE);
            emit paintShape(RECTANGLE);
            break;
        case SHAPE_ARROW:
            emit stateChange(SHAPE);
            emit paintShape(PAINT_ARROW);
            break;
        }
        break;
    case 1: // mosaic
        emit stateChange(SHAPE);
        emit paintShape(BLUR);
        break;
    case 2: // cursor
        emit stateChange(ARROW);
        break;
    case 3: // pencil
        emit stateChange(PAINT);
        break;
    case 4: // highlighter
        emit stateChange(PAINT);
        break;
    case 5: // text
        emit stateChange(SHAPE);
        emit paintShape(TEXT);
        break;
    case 6: // erase
        emit stateChange(ERASE);
        break;
    }
}

RecordInfo ClipLayer::getRecordInfo() {
    RecordInfo ans;
    if(videoToolbar == NULL) {
        ans.fps = 20;
        ans.channel = 2;
        ans.enableAudio = true;
        ans.audioDeviceIndex = 0;
        ans.recordPath = QDir::cleanPath(History::instance()->getVideoSavePath() + "/" + "新建.mp4");
    } else {
        ans = videoToolbar->getRecordInfo();
    }
    ans.bound = getClipRect().toRect();
    if((ans.bound.width() & 1) != 0) {
        ans.bound.setWidth(ans.bound.width() - 1);
    }
    if((ans.bound.height() & 1) != 0) {
        ans.bound.setHeight(ans.bound.height() -1);
    }
    ans.outWidth = ans.bound.width();
    ans.outHeight = ans.bound.height();
    return ans;
}
