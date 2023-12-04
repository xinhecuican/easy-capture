#include "textlayercontainer.h"
#include "../Layer/textlayer.h"
#include "../paintarea.h"
#include "../../Manager/uimanager.h"
#include <QToolButton>
#include "../../Helper/imagehelper.h"
#include "../../Helper/mstring.h"

TextLayerContainer::TextLayerContainer(PaintArea* area) :
    LayerContainer(area),
    textId(0),
    data(UIManager::instance()->getFontData())
{
}

void TextLayerContainer::layerMousePressEvent(QGraphicsSceneMouseEvent *event){
    TextLayer* textLayer = new TextLayer(event->scenePos(), "text" + QString::number(textId), area, NULL);
    textId++;
    textLayer->setEnable(true);
    connect(textLayer, &TextLayer::formatChange, this, [=](const QFont& font, const QBrush& brush){
        int r = 0, g = 0, b = 0;
        brush.color().getRgb(&r,&g,&b);
        colorSelectButton->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(r).arg(g).arg(b));

        fontBox->setCurrentText(font.family());
        widthBox->setCurrentText(QString::number(font.pointSize()));
        boldButton->setChecked(font.bold());
        italicButton->setChecked(font.italic());
        underlineButton->setChecked(font.underline());
        data.font = font;
        data.color = brush.color();
    });
    area->addLayer(textLayer);
}

QWidget* TextLayerContainer::onValid(QWidget *widgetParent){
    if(widget == NULL){
        widget = new QWidget(widgetParent);
        QHBoxLayout* layout = new QHBoxLayout();

        QToolButton* brushButton = new QToolButton(widgetParent);
        brushButton->setIcon(ImageHelper::getIcon("brush"));
        brushButton->setToolTip(MString::search("{hULcO1ELot}格式刷"));
        connect(brushButton, &QToolButton::clicked, this, [=](){
            LayerBase* focusLayer = area->getFocusLayer();
            if(focusLayer!= NULL && focusLayer->getName().startsWith("text")){
                focusLayer->setParameter("brush", 1);
            }
        });

        colorSelectButton = new QToolButton(widgetParent);
        int r = 0, g = 0, b = 0;
        data.color.getRgb(&r,&g,&b);
        colorSelectButton->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(r).arg(g).arg(b));
        connect(colorSelectButton, &QToolButton::clicked, this, [=](){
            QColorDialog dialog;
            QColor color = dialog.getColor(color, widgetParent, MString::search("{6Of41PN3eL}选择字体颜色"),
            QColorDialog::ShowAlphaChannel);
            int r = 0, g = 0, b = 0;
            color.getRgb(&r,&g,&b);
            data.color.setRgb(r, g, b);
            colorSelectButton->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(r).arg(g).arg(b));
            LayerBase* focusLayer = area->getFocusLayer();
            if(focusLayer!= NULL && focusLayer->getName().startsWith("text")){
                focusLayer->setParameter("color", data.color);
            }
        });

        fontBox = new QComboBox(widgetParent);
        QStandardItemModel *model=new QStandardItemModel(this);    //源数据
        QList<QStandardItem *> row;
        QStandardItem *blankitem=new QStandardItem("");
        model->appendRow(blankitem);	//增加一行空白
        QList<QString> list = get_font_text();
        foreach (QString name, list) {
            if(!name.isEmpty()) {
                row.clear();
                QStandardItem *item=new QStandardItem(name);
                model->appendRow(item);	//添加一行，2列数据
            }
        }
        fontBox->setModel(model);	//QComboBox设置数据源
        fontBox->setEditable(true);
        QCompleter *completer=new QCompleter(fontBox);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setModel(model);
        completer->setCompletionColumn(fontBox->modelColumn());
        completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
        completer->setMaxVisibleItems(10);	//下拉最大高度
        fontBox->setCompleter(completer);	//设置自动补全
        //    m_ComboBox->setStyleSheet("QComboBox{combobox-popup: 0;}");	//linux系统上，防止下拉框过长,消耗时间较多
        fontBox->setMaxVisibleItems(10);	//下拉最大高度
        fontBox->setCurrentText("宋体");
        connect(fontBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged)
                                , this, [=](const QString& text) {
                    data.font.setFamily(text);
                    fontBox->clearFocus();
                    LayerBase* focusLayer = area->getFocusLayer();
                    if(focusLayer!= NULL && focusLayer->getName().startsWith("text")){
                        focusLayer->setParameter("font", data.font);
                    }
        });

        widthBox = new QComboBox(widgetParent);
        QList<QString> width_text = {"4", "8", "15", "20", "22", "26", "30", "40", "50"};
        widthBox->addItems(width_text);
        widthBox->setEditable(true);
        widthBox->setCurrentText(QString::number(data.font.pointSize()));
        connect(widthBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged)
                                  , this, [=](const QString& text) {
                    bool success = false;
                    int num = text.toInt(&success);
                    if(success && num > 0) {
                        widthBox->clearFocus();
                        data.font.setPointSize(num);
                        LayerBase* focusLayer = area->getFocusLayer();
                        if(focusLayer!= NULL && focusLayer->getName().startsWith("text")){
                            focusLayer->setParameter("font", data.font);
                        }
                    }
        });
        boldButton = new QToolButton(widgetParent);
        boldButton->setIcon(ImageHelper::getIcon("bold", 48, 48));
        boldButton->setCheckable(true);
        connect(boldButton, &QToolButton::clicked, this, [=]() {
            if(data.font.bold()) {
                data.font.setBold(false);
                boldButton->setChecked(false);
            } else {
                data.font.setBold(true);
                boldButton->setChecked(true);
            }
            LayerBase* focusLayer = area->getFocusLayer();
            if(focusLayer!= NULL && focusLayer->getName().startsWith("text")){
                focusLayer->setParameter("font", data.font);
            }
        });
        italicButton = new QToolButton(widgetParent);
        italicButton->setIcon(ImageHelper::getIcon("italic", 48, 48));
        italicButton->setCheckable(true);
        connect(italicButton, &QToolButton::clicked, this, [=]() {
            if(data.font.italic()) {
                data.font.setItalic(false);
                italicButton->setChecked(false);
            } else {
                data.font.setItalic(true);
                italicButton->setChecked(true);
            }
            LayerBase* focusLayer = area->getFocusLayer();
            if(focusLayer!= NULL && focusLayer->getName().startsWith("text")){
                focusLayer->setParameter("font", data.font);
            }
        });
        underlineButton = new QToolButton(widgetParent);
        underlineButton->setIcon(ImageHelper::getIcon("underline", 48, 48));
        underlineButton->setCheckable(true);
        connect(underlineButton, &QToolButton::clicked, this, [=]() {
            if(data.font.underline()) {
                data.font.setUnderline(false);
                underlineButton->setChecked(false);
            } else {
                data.font.setUnderline(true);
                underlineButton->setChecked(true);
            }
            LayerBase* focusLayer = area->getFocusLayer();
            if(focusLayer!= NULL && focusLayer->getName().startsWith("text")){
                focusLayer->setParameter("font", data.font);
            }
        });
        colorSelectButton->setFixedSize(fontBox->height(), fontBox->height());
        boldButton->setFixedSize(fontBox->height(), fontBox->height());
        italicButton->setFixedSize(fontBox->height(), fontBox->height());
        underlineButton->setFixedSize(fontBox->height(), fontBox->height());

        layout->addWidget(brushButton);
        layout->addWidget(colorSelectButton);
        layout->addWidget(fontBox);
        layout->addWidget(widthBox);
        layout->addWidget(boldButton);
        layout->addWidget(italicButton);
        layout->addWidget(underlineButton);
        widget->setLayout(layout);
    }
    return widget;
}

QList<QString> TextLayerContainer::get_font_text() {
    QList<QString> font_text =  QList<QString>({"Agency FB", "Algerian", "Arial", "Arial Black", "Arial Narrow",
                                               "Bauhaus 93", "Bell MT",
                                               "Bodoni MT", "Bodoni MT Black", "Bodoni MT Condensed", "Book Antiqua", "Broadway",
                                               "Calibri", "Calibri Light", "Californian FB", "Calisto MT",
                                               "Candara", "Candara Light", "Castellar", "Centaur", "Century", "Century Gothic",
                                               "Chiller", "Colonna MT", "Comic Sans MS", "Consolas", "Constantia", "Cooper Black",
                                               "Corbel", "Corbel Light", "Courier", "Courier New", "Curlz MT",
                                               "Dubai", "Dubai Light", "Dubai Medium", "Ebrima", "Elephant", "Engravers MT",
                                               "Ink Free",
                                               "Microsoft Himalaya", "Microsoft JhengHei",
                                               "Microsoft New Tai Lue", "Microsoft PhagsPa", "Microsoft Sans Serif",
                                               "Microsoft Tai Le", "Microsoft YaHei UI", "MingLiU-ExtB",
                                               "Mistral", "Modern", "Modern No. 20", "Mongolian Baiti", "Monotype Corsiva",
                                               "MS Gothic", "MS Outlook", "MS PGothic",
                                               "MS Sans Serif", "MS Serif", "MS UI Gothic", "MT Extra", "MV Boli", "Myanmar Text", "Niagara Engraved",
                                               "Niagara Solid", "Nirmala UI", "Nirmala UI Semilight", "NumberOnly",
                                               "Onyx", "Open Sans", "Papyrus", "Parchment",
                                               "Perpetua",
                                               "Ravie", "Rockwell",
                                               "Small Fonts", "Snap ITC", "Stencil", "Sylfaen", "Symbol", "System",
                                               "Tahoma", "Terminal", "Times New Roman", "Verdana",
                                               "仿宋", "华文中宋", "华文仿宋", "华文宋体", "华文彩云", "华文新魏", "华文楷体",
                                               "华文琥珀", "华文细黑", "华文行楷", "华文隶书", "宋体", "幼圆", "微软雅黑", "微软雅黑 Light", "思源黑体",
                                               "新宋体", "方正姚体", "方正粗黑宋简体", "方正舒体", "楷体", "等线", "等线 Light", "隶书", "黑体"});
    return font_text;
}
