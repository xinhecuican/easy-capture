#include "flow_edit_panel.h"
#include<QToolButton>
#include<QColorDialog>
#include "Helper/mstring.h"
#include<QVector>
#include<QStandardItemModel>
#include<QCompleter>
#include<QAbstractItemView>
#include<QDebug>
#include "Manager/uimanager.h"
#include "Helper/imagehelper.h"

Flow_edit_panel* Flow_edit_panel::_instance = NULL;

Flow_edit_panel::Flow_edit_panel() {
    fontData = UIManager::instance()->getFontData();
    setObjectName("font_edit_panel");
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setWindowFlag(Qt::WindowSystemMenuHint, false);
    QToolButton* brush_button = new QToolButton(this);
    brush_button->setIcon(ImageHelper::getIcon("brush"));
    brush_button->setToolTip("格式刷");
    connect(brush_button, &QToolButton::clicked, this, [=]() {
        emit text_brush();
    });
    addWidget(brush_button);
    color_selector_button = new QToolButton(this);
    connect(color_selector_button, &QToolButton::clicked, this, [=]() {
        QColorDialog dialog;
        QColor color = dialog.getColor(color, this, MString::search("{6Of41PN3eL}选择字体颜色"),
                                       QColorDialog::ShowAlphaChannel);
        if(this->fontData.color != color) {
            int r = 0, g = 0, b = 0;
            color.getRgb(&r,&g,&b);
            this->fontData.color.setRgb(r, g, b);
            color_selector_button->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(r).arg(g).arg(b));
            emit font_change();
        }
    });
    int r = 0, g = 0, b = 0;
    fontData.color.getRgb(&r,&g,&b);
    color_selector_button->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(r).arg(g).arg(b));
    addWidget(color_selector_button);
    addSeparator();

    m_ComboBox=new QComboBox(this);
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
    m_ComboBox->setModel(model);	//QComboBox设置数据源
    m_ComboBox->setEditable(true);
    QCompleter *completer=new QCompleter(m_ComboBox);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setModel(model);
    completer->setCompletionColumn(m_ComboBox->modelColumn());
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    completer->setMaxVisibleItems(10);	//下拉最大高度
    m_ComboBox->setCompleter(completer);	//设置自动补全
//    m_ComboBox->setStyleSheet("QComboBox{combobox-popup: 0;}");	//linux系统上，防止下拉框过长,消耗时间较多
    m_ComboBox->setMaxVisibleItems(10);	//下拉最大高度
    m_ComboBox->setCurrentText("宋体");
    connect(m_ComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged)
    , this, [=](const QString& text) {
        fontData.font.setFamily(text);
        clearFocus();
        emit font_change();
    });
    addWidget(m_ComboBox);
    addSeparator();

    width_button = new QComboBox(this);
    QList<QString> width_text = {"4", "8", "15", "20", "22", "26", "30", "40", "50"};
    width_button->addItems(width_text);
    width_button->setEditable(true);
    width_button->setCurrentText(QString::number(fontData.font.pointSize()));
    connect(width_button, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged)
    , this, [=](const QString& text) {
        bool success = false;
        int num = text.toInt(&success);
        if(success && num > 0) {
            clearFocus();
            fontData.font.setPointSize(num);
            emit font_change();
        }
    });
    addWidget(width_button);
    addSeparator();

    bold_button = new QToolButton(this);
    bold_button->setIcon(ImageHelper::getIcon("bold", 48, 48));
    bold_button->setCheckable(true);
    connect(bold_button, &QToolButton::clicked, this, [=]() {
        if(fontData.font.bold()) {
            fontData.font.setBold(false);
            bold_button->setChecked(false);
        } else {
            fontData.font.setBold(true);
            bold_button->setChecked(true);
        }
        emit font_change();
    });
    addWidget(bold_button);
    italic_button = new QToolButton(this);
    italic_button->setIcon(ImageHelper::getIcon("italic", 48, 48));
    italic_button->setCheckable(true);
    connect(italic_button, &QToolButton::clicked, this, [=]() {
        if(fontData.font.italic()) {
            fontData.font.setItalic(false);
            italic_button->setChecked(false);
        } else {
            fontData.font.setItalic(true);
            italic_button->setChecked(true);
        }
        emit font_change();
    });
    addWidget(italic_button);
    underline_button = new QToolButton(this);
    underline_button->setIcon(ImageHelper::getIcon("underline", 48, 48));
    underline_button->setCheckable(true);
    connect(underline_button, &QToolButton::clicked, this, [=]() {
        if(fontData.font.underline()) {
            fontData.font.setUnderline(false);
            underline_button->setChecked(false);
        } else {
            fontData.font.setUnderline(true);
            italic_button->setChecked(true);
        }
        emit font_change();
    });
    color_selector_button->setFixedSize(m_ComboBox->height(), m_ComboBox->height());
    bold_button->setFixedSize(m_ComboBox->height(), m_ComboBox->height());
    italic_button->setFixedSize(m_ComboBox->height(), m_ComboBox->height());
    underline_button->setFixedSize(m_ComboBox->height(), m_ComboBox->height());
    addWidget(underline_button);
    emit font_change();//初始化
}

Flow_edit_panel::~Flow_edit_panel() {
    _instance = NULL;
}

Flow_edit_panel* Flow_edit_panel::instance() {
    if(_instance == NULL) {
        _instance = new Flow_edit_panel();
    }
    return _instance;
}

void Flow_edit_panel::reset() {
    setParent(NULL);
    fontData = UIManager::instance()->getFontData();
    set_format(fontData.font, fontData.color);
}

void Flow_edit_panel::set_format(QFont font, QColor color) {
    fontData.font = font;
    fontData.color = color;
    int r = 0, g = 0, b = 0;
    color.getRgb(&r,&g,&b);
    color_selector_button->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(r).arg(g).arg(b));
    m_ComboBox->setCurrentText(this->fontData.font.family());
    width_button->setCurrentText(QString::number(font.pointSize()));
    bold_button->setChecked(font.bold());
    italic_button->setChecked(font.italic());
    underline_button->setChecked(font.underline());
}

QList<QString> Flow_edit_panel::get_font_text() {
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

QFont Flow_edit_panel::get_font() {
    return fontData.font;
}

QColor Flow_edit_panel::get_color() {
    return fontData.color;
}
