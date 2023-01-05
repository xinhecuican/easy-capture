#include "cliplayer.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include "Helper/math.h"
#include "Helper/plist.h"
#include <QGuiApplication>
#include <QScreen>
#include "Manager/window_manager.h"
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
    begin_clip(false),
    is_save(false),
    is_enable(true),
    scene(scene)
{
    screen_rect = QGuiApplication::primaryScreen()->geometry();
    mask_layer = new MaskLayer(this);
    connect(mask_layer, &MaskLayer::regionChanged, this, [=](){
        calBarPos();
    });
    this->widget_parent = widget_parent;
    rect_setting = Style_manager::default_pencil;
    pencil_setting = Style_manager::default_pencil;
    highlighter_setting = Style_manager::default_highlighter;
    widgetParent = NULL;
    toolbar = NULL;
    attribute_toolbar = NULL;
    // toolbar初始化耗时较长，使用回调避免首次启动阻塞
    QTimer::singleShot(0, this, [=](){
        setToolBar();
        setAttributeToolbar();
        if(widgetParent != NULL)
        {
            toolbar->setParent(widgetParent);
            attribute_toolbar->setParent(widgetParent);
        }
    });
//    setToolBar();
//    setAttributeToolbar();
}

ClipLayer::~ClipLayer()
{
    delete toolbar;
    delete attribute_toolbar;
}

QPainterPath ClipLayer::shape() const
{
    QPainterPath path;
    path.addRect(screen_rect);
    return path;
}

QRectF ClipLayer::boundingRect() const
{
    return screen_rect;
}

void ClipLayer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    button = event->button();
    if(button == Qt::RightButton)
    {
        if(mask_layer->getRegionCount() == 0)
        {
            Window_manager::change_window("tray");
        }
        else
        {
            emit needReset();
        }
    }
    else if(button == Qt::MidButton)
    {
        switch (Config::getConfig<int>(Config::middle_button_type))
        {
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
    }
    else
    {
        toolbar->hide();
        attribute_toolbar->hide();
    }
}

void ClipLayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
}

void ClipLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
//    if(!is_enable)
    {
        calBarPos();
    }
}

void ClipLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!is_save)
    {
        painter->drawPixmap(QPointF(0, 0), pix);
    }
    else
    {
        painter->drawPixmap(QPointF(0, 0), savePix);
    }
}

void ClipLayer::prepareSave()
{
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

void ClipLayer::endSave()
{
    is_save = false;
    mask_layer->endSave();
}

void ClipLayer::capture(QPixmap pix)
{
    QPainterPath path;
    QPolygonF polygon;
    QPixmap ans;
    QRect rect;
    for(int i=0; i<regions.size(); i++)
    {
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
    Window_manager::change_window("Paint_window");
    Window_manager::get_window("Paint_window")->set_pic(ans, rect);
}

void ClipLayer::setPic(QPixmap pix)
{
    this->pix = pix;
    update();
}

QRectF ClipLayer::getClipRect()
{
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

void ClipLayer::reset()
{
    is_save = false;
    mask_layer->reset();
    if(toolbar != NULL)
        toolbar->hide();
    if(attribute_toolbar != NULL)
        attribute_toolbar->hide();
}

void ClipLayer::sendRequestImage()
{
    emit requestImage();
}

void ClipLayer::setToolBar()
{
    toolbar = new QToolBar();
    toolbar->setWindowFlags(toolbar->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    toolbar->setWindowFlag(Qt::WindowSystemMenuHint, false);
    toolbar->setStyleSheet(getQSS(":/qss/toolbar.qss"));
    toolbar->hide();

    ocrButton = new QToolButton(toolbar);
    ocrButton->setIcon(QIcon(":/image/ocr.png"));
    ocrButton->setToolTip(MString::search("{SvJhCjRGF0}提取文字"));
    connect(ocrButton, &QToolButton::clicked, this, [=](){
        emit requestOcr();
    });

    save_button = new QToolButton(toolbar);
    save_button->setIcon(QIcon(":/image/save.png"));
    save_button->setToolTip(MString::search("{pJqTHhEQdb}保存"));
    connect(save_button, &QToolButton::clicked, this, [=](){
        emit needSave();
    });

    clip_button = new QToolButton(toolbar);
    clip_button->setIcon(QIcon(":/image/clipboard.png"));
    clip_button->setToolTip(MString::search("{ntbJbEqxwF}复制到剪切板"));
    connect(clip_button, &QToolButton::clicked, this, [=](){
        emit needClip();
    });

    erase_button = new QToolButton(toolbar);
    erase_button->setIcon(QIcon(":/image/eraser.png"));
    erase_button->setToolTip(MString::search("{7cwKObEhcx}擦除"));
    erase_button->setCheckable(true);

    ok_button = new QToolButton(toolbar);
    ok_button->setIcon(QIcon(":/image/ok.svg"));
    connect(ok_button, &QToolButton::clicked, this, [=](){
        emit requestImage();
    });

    cancel_button = new QToolButton(toolbar);
    cancel_button->setIcon(QIcon(":/image/cancel.svg"));
    connect(cancel_button, &QToolButton::clicked, this, [=](){
        Window_manager::change_window("tray");
    });

    undo_button = new QToolButton(toolbar);
    undo_button->setIcon(QIcon(":/image/undo.png"));
    undo_button->setToolTip(MString::search("{h5KymvIMTN}撤销"));
    connect(undo_button, &QPushButton::clicked, this, [=](){
        Recorder::instance()->back();
    });

    redo_button = new QToolButton(toolbar);
    redo_button->setToolTip(MString::search("{a7CaC7NOL5}恢复"));
    redo_button->setIcon(QIcon(":/image/redo.png"));
    connect(redo_button, &QPushButton::clicked, this, [=](){
        Recorder::instance()->forward();
    });
    connect(Recorder::instance(), &Recorder::recordChange, this, [=](){
        if(!Recorder::instance()->undoAvaliable())
        {
            undo_button->setEnabled(false);
        }
        else
        {
            undo_button->setEnabled(true);
        }
        if(!Recorder::instance()->redoAvaliable())
        {
            redo_button->setEnabled(false);
        }
        else
        {
            redo_button->setEnabled(true);
        }
    });

    rect_button = new QToolButton(toolbar);
    rect_button->setToolTip("矩形");
    rect_button->setIcon(QIcon(":/image/rect.png"));
    rect_button->setCheckable(true);

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
    button_group->addButton(rect_button, 0);
    button_group->addButton(mosaic_button, 1);
    button_group->addButton(cursor_button, 2);
    button_group->addButton(pencil_button, 3);
    button_group->addButton(highlighter_button, 4);
    button_group->addButton(text_button, 5);
    button_group->addButton(shape_button, 6);
    button_group->addButton(erase_button, 7);
    connect(button_group, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked)
            , this, [=](int id){
        onToolbarButtonClick(id);
    });

    toolbar->addWidget(rect_button);
    toolbar->addWidget(mosaic_button);
    toolbar->addWidget(cursor_button);
    toolbar->addWidget(pencil_button);
    toolbar->addWidget(highlighter_button);
    toolbar->addWidget(text_button);
    toolbar->addWidget(shape_button);
    toolbar->addWidget(erase_button);
    toolbar->addSeparator();
    toolbar->addWidget(undo_button);
    toolbar->addWidget(redo_button);
    toolbar->addWidget(ocrButton);
    toolbar->addWidget(clip_button);
    toolbar->addWidget(save_button);
    toolbar->addWidget(ok_button);
    toolbar->addWidget(cancel_button);
}

void ClipLayer::calBarPos()
{
    if(!toolbar->isVisible())
        toolbar->show();
    if(!attribute_toolbar->isVisible())
        attribute_toolbar->show();

    if(button_group->checkedButton() == NULL)
    {
        qDebug() << "ClipLayer::calBarPos checked button NULL";
        return;
    }
    int bar_height = toolbar->height();
    int bar_width = toolbar->width();
    QRectF mask_bound = mask_layer->getClipRect();
    int left = mask_bound.right() - bar_width;
    if(mask_bound.bottom() + 2 * bar_height < screen_rect.bottom())
    {
        toolbar->move(left, mask_bound.bottom());
        attribute_toolbar->move(left + button_group->checkedButton()->pos().x(), mask_bound.bottom() + bar_height + 2);
    }
    else if(mask_bound.top() - 2 * bar_height >= 0)
    {
        toolbar->move(left, mask_bound.top() - 2 * bar_height);
        attribute_toolbar->move(left + button_group->checkedButton()->pos().x(), mask_bound.top() - bar_height + bar_height + 2);
    }
    else
    {
        toolbar->move(left, mask_bound.bottom() - bar_height * 2);
        attribute_toolbar->move(left + button_group->checkedButton()->pos().x(), mask_bound.bottom() - bar_height + 2);
    }

}

void ClipLayer::setEnable(bool enable)
{
    is_enable = enable;
    if(!is_enable)
    {
        calBarPos();
    }
    mask_layer->setEnable(enable);
}

void ClipLayer::updateAttributeToolbar(int id)
{
    switch(before_id)
    {
    case 0: // rect
        rect_setting.color = Style_manager::instance()->get_now().color;
        rect_setting.width = Style_manager::instance()->get_now().width;
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
    switch(id)
    {
    case 0: // rect
        color_widget->setCurrentStyle(rect_setting.color);
        Style_manager::instance()->change_color(rect_setting.color);
        Style_manager::instance()->change_width(rect_setting.width);
        width_button->setValue(rect_setting.width);
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
        width_button->setValue(highlighter_button->width());
        attribute_toolbar->add(width_button);
        attribute_toolbar->add(color_widget);
        break;
    case 5:
        attribute_toolbar->add(Flow_edit_panel::instance());
        break;
    case 6:
        attribute_toolbar->add(arrow_button);
        break;
    }
    calBarPos();
}

void ClipLayer::setAttributeToolbar()
{
    attribute_toolbar = new AttributeToolbar();
    attribute_toolbar->setWindowFlags(toolbar->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    attribute_toolbar->setWindowFlag(Qt::WindowSystemMenuHint, false);
    attribute_toolbar->setStyleSheet(getQSS(":/qss/toolbar.qss"));


    color_widget = new ColorWidget(attribute_toolbar);
    width_button = new QSpinBox(attribute_toolbar);
    width_button->setRange(1, 50);
    width_button->setValue(3);
    width_button->setAccelerated(true);
    width_button->setWrapping(true);
    width_button->setKeyboardTracking(true);
    connect(width_button, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value){
        Style_manager::instance()->change_width(value);
    });


    mosaic_sample = new MosicSample(attribute_toolbar);
    mosaic_size = new QSpinBox(attribute_toolbar);
    mosaic_size->setRange(1, 15);
    mosaic_size->setValue(5);
    mosaic_size->setAccelerated(true);
    mosaic_size->setWrapping(true);
    mosaic_size->setKeyboardTracking(true);
    connect(mosaic_size, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value){
        mosaic_sample->setUnitSize(value);
        emit mosaicChange(false, value);
    });
    mosaic_range = new QSlider(attribute_toolbar);
    mosaic_range->setValue(15);
    mosaic_range->setMinimum(1);
    mosaic_range->setMaximum(50);
    mosaic_range->setOrientation(Qt::Horizontal);
    mosaic_range->setToolTip("15");
    connect(mosaic_range, &QSlider::valueChanged, this, [=](){
        emit mosaicChange(true, mosaic_range->value());
        mosaic_size->setMaximum(mosaic_range->value());
        mosaic_range->setToolTip(QString::number(mosaic_range->value()));
    });
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
            , this, [=](int id){
        if(id == 0)
        {
            for(int i=Config::capture_mode_begin; i<=Config::capture_mode_end; i++)
            {
                Config::setConfig(i, false);
            }
            Config::setConfig(Config::rect_capture, true);
        }
        else if(id == 1)
        {
            for(int i=Config::capture_mode_begin; i<=Config::capture_mode_end; i++)
            {
                Config::setConfig(i, false);
            }
            Config::setConfig(Config::free_capture, true);
        }
    });
    Flow_edit_panel::instance()->setParent(attribute_toolbar);
    arrow_button = new QToolButton(attribute_toolbar);
    arrow_button->setIcon(QIcon(":/image/paint_arrow.png"));
    arrow_button->setToolTip("{D7HSBXWTLj}箭头");
    connect(arrow_button, &QToolButton::clicked, this, [=](){
        emit paintShape(PAINT_ARROW);
    });

    color_widget->hide();
    width_button->hide();
    mosaic_sample->hide();
    mosaic_size->hide();
    mosaic_range->hide();
    rect_capture->hide();
    free_capture->hide();
    arrow_button->hide();
    Flow_edit_panel::instance()->hide();

    updateAttributeToolbar(2);
}

void ClipLayer::setWidgetParent(QWidget *parent)
{
    widgetParent = parent;
    if(attribute_toolbar != NULL)
    {
        toolbar->setParent(parent);
        attribute_toolbar->setParent(parent);
    }
}

void ClipLayer::buttonEnter(int id)
{
    button_group->button(id)->setChecked(true);
    onToolbarButtonClick(id);
}

void ClipLayer::onToolbarButtonClick(int id)
{
    updateAttributeToolbar(id);
    switch(id)
    {
    case 0: // rect
        emit stateChange(SHAPE);
        emit paintShape(RECTANGLE);
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
    case 6: // shape
        emit stateChange(SHAPE);
        break;
    case 7:
        emit stateChange(ERASE);
        break;
    }
}
