#include "flow_edit_panel.h"
#include<QToolButton>
#include<QColorDialog>
#include "Helper/mstring.h"
#include<QVector>
#include<QStandardItemModel>
#include<QCompleter>
#include<QAbstractItemView>
#include<QDebug>

Flow_edit_panel* Flow_edit_panel::_instance = NULL;

Flow_edit_panel::Flow_edit_panel()
{
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setWindowTitle("字体样式选择");
    color = QColor(0, 0, 0);
    QToolButton* brush_button = new QToolButton(this);
    brush_button->setIcon(QIcon(":/image/brush.png"));
    brush_button->setToolTip("格式刷");
    connect(brush_button, &QToolButton::clicked, this, [=](){
        emit text_brush();
    });
    addWidget(brush_button);
    color_selector_button = new QToolButton(this);
    connect(color_selector_button, &QToolButton::clicked, this, [=](){
        QColorDialog dialog;
        QColor color = dialog.getColor(color, this, MString::search("{6Of41PN3eL}选择字体颜色"),
                                       QColorDialog::ShowAlphaChannel);
        if(this->color != color)
        {
            int r = 0, g = 0, b = 0;
            color.getRgb(&r,&g,&b);
            this->color.setRgb(r, g, b);
            color_selector_button->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(r).arg(g).arg(b));
            emit font_change();
        }
    });
    int r = 0, g = 0, b = 0;
    color.getRgb(&r,&g,&b);
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
        if(!name.isEmpty()){
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
    m_ComboBox->setStyleSheet("QComboBox{combobox-popup: 0;}");	//linux系统上，防止下拉框过长
    m_ComboBox->setMaxVisibleItems(10);	//下拉最大高度
    m_ComboBox->setCurrentText("宋体");
    font.setFamily("宋体");
    connect(m_ComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged)
            , this, [=](const QString& text){
        font.setFamily(text);
        emit font_change();
    });
    addWidget(m_ComboBox);
    addSeparator();

    width_button = new QComboBox(this);
    QList<QString> width_text = {"2", "3", "4", "5", "6", "8", "10", "12", "14", "16", "18", "20"};
    width_button->addItems(width_text);
    width_button->setEditable(true);
    font.setPointSize(12);
    width_button->setCurrentText(QString::number(font.pointSize()));
    connect(width_button, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged)
            , this, [=](const QString& text){
        bool success = false;
        int num = text.toInt(&success);
        if(success && num > 0)
        {
            font.setPointSize(num);
            emit font_change();
        }
    });
    addWidget(width_button);
    addSeparator();

    bold_button = new QToolButton(this);
    bold_button->setIcon(QIcon(":/image/bold.png"));
    bold_button->setCheckable(true);
    connect(bold_button, &QToolButton::clicked, this, [=](){
        if(font.bold())
        {
            font.setBold(false);
            bold_button->setChecked(false);
        }
        else
        {
            font.setBold(true);
            bold_button->setChecked(true);
        }
        emit font_change();
    });
    addWidget(bold_button);
    italic_button = new QToolButton(this);
    italic_button->setIcon(QIcon(":/image/italic.png"));
    italic_button->setCheckable(true);
    connect(italic_button, &QToolButton::clicked, this, [=](){
        if(font.italic())
        {
            font.setItalic(false);
            italic_button->setChecked(false);
        }
        else
        {
            font.setItalic(true);
            italic_button->setChecked(true);
        }
        emit font_change();
    });
    addWidget(italic_button);
    underline_button = new QToolButton(this);
    underline_button->setIcon(QIcon(":/image/underline.png"));
    underline_button->setCheckable(true);
    connect(underline_button, &QToolButton::clicked, this, [=](){
        if(font.underline())
        {
            font.setUnderline(false);
            underline_button->setChecked(false);
        }
        else
        {
            font.setUnderline(true);
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

Flow_edit_panel::~Flow_edit_panel()
{
    _instance = NULL;
}

Flow_edit_panel* Flow_edit_panel::instance()
{
    if(_instance == NULL)
    {
        _instance = new Flow_edit_panel();
    }
    return _instance;
}

void Flow_edit_panel::set_format(QFont font, QColor color)
{
    this->color = color;
    this->font = font;
    int r = 0, g = 0, b = 0;
    color.getRgb(&r,&g,&b);
    color_selector_button->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(r).arg(g).arg(b));
    m_ComboBox->setCurrentText(this->font.family());
    width_button->setCurrentText(QString::number(font.pointSize()));
    bold_button->setChecked(font.bold());
    italic_button->setChecked(font.italic());
    underline_button->setChecked(font.underline());
}

QList<QString> Flow_edit_panel::get_font_text()
{
    QList<QString> font_text =  QList<QString>({"Agency FB", "Algerian", "Arial", "Arial Black", "Arial Narrow",
                                                    "Arial Rounded MT Bold",
            "Arial Unicode MS", "Bahnschrift", "Bahnschrift Condensed", "Bahnschrift Light",
            "Bauhaus 93", "Bell MT", "Berlin Sans FB", "Berlin Sans FB Demi", "Bernard MT Condensed", "Blackadder ITC",
            "Bodoni MT", "Bodoni MT Black", "Bodoni MT Condensed", "Book Antiqua",
            "Bookman Old Style", "Bookshelf Symbol 7", "Bradley Hand ITC", "Britannic Bold", "Broadway",
            "Brush Script MT", "Calibri", "Calibri Light", "Californian FB", "Calisto MT", "Cambria", "Cambria Math",
            "Candara", "Candara Light", "Castellar", "Centaur", "Century", "Century Gothic", "Century Schoolbook",
            "Chiller", "Colonna MT", "Comic Sans MS", "Consolas", "Constantia", "Cooper Black",
            "Corbel", "Corbel Light", "Courier", "Courier New", "Curlz MT", "DejaVu Sans Mono",
            "Dubai", "Dubai Light", "Dubai Medium", "Ebrima", "Elephant", "Engravers MT",
            "Eras Bold ITC", "Eras Demi ITC", "Eras Light ITC", "Eras Medium ITC", "Felix Titling", "Fixedsys", "Forte",
            "Freestyle Script", "Gabriola", "Gadugi", "Garamond", "Georgia", "Gigi", "Gill Sans MT",
            "Goudy Old Style","Goudy Stout", "Haettenschweiler", "Harlow Solid Italic","Harrington",
                                                "HGB1_CNKI", "HGB1X_CNKI",
            "HGB2_CNKI", "HGB2X_CNKI", "HGB3_CNKI", "HGB3X_CNKI", "HGB4_CNKI", "HGB4X_CNKI", "HGB5_CNKI",
            "HGB5X_CNKI", "HGB6_CNKI", "HGB6X_CNKI", "HGB7_CNKI", "HGB7X_CNKI", "HGB8_CNKI", "HGB8X_CNKI", "HGBD_CNKI",
            "HGBKB_CNKI", "HGBKBX_CNKI", "HGBKH_CNKI", "HGBKHX_CNKI", "HGBX_CNKI", "HGBZ_CNKI", "HGDY_CNKI",
            "HGF1_CNKI", "HGF1X_CNKI", "HGF2_CNKI", "HGF2X_CNKI", "HGF3_CNKI", "HGF4_CNKI", "HGF4X_CNKI", "HGF5_CNKI",
            "HGF5X_CNKI", "HGF6_CNKI", "HGF6X_CNKI", "HGF7_CNKI", "HGF7X_CNKI", "HGF8_CNKI", "HGF9_CNKI", "HGF9X_CNKI",
            "HGFX_CNKI", "HGFZ_CNKI", "HGH1_CNKI", "HGH1X_CNKI", "HGH2_CNKI", "HGH2X_CNKI", "HGH3_CNKI", "HGH3X_CNKI",
            "HGH4_CNKI", "HGH4X_CNKI", "HGH5_CNKI", "HGH5X_CNKI", "HGH6_CNKI", "HGH6X_CNKI", "HGH7_CNKI", "HGH7X_CNKI",
            "HGHD_CNKI", "HGHT1_CNKI", "HGHT2_CNKI", "HGHUATI_CNKI", "HGHX_CNKI", "HGHZ_CNKI", "HGKY_CNKI", "HGNBS_CNKI",
            "HGOCR_CNKI", "HGSXT_CNKI", "HGTT_CNKI", "HGX1_CNKI", "HGX1X_CNKI", "HGXF1_CNKI", "HGXFX_CNKI", "HGXFZ_CNKI",
            "HGXT_CNKI", "HGXY_CNKI", "HGYB_CNKI", "HGYT1_CNKI", "HGYT2_CNKI", "High Tower Text", "HoloLens MDL2 Assets",
            "HP Simplified", "HP Simplified Light", "icomoon", "Impact", "Imprint MT Shadow", "Informal Roman",
                                                    "Ink Free",
            "Javanese Text", "Jokerman", "Juice ITC", "Kristen ITC", "Kunstler Script", "Leelawadee UI",
            "Leelawadee UI Semilight", "Lucida Bright", "Lucida Calligraphy", "Lucida Console", "Lucida Fax",
            "Lucida Handwriting", "Lucida Sans", "Lucida Sans Typewriter", "Lucida Sans Unicode", "Magneto",
            "Maiandra GD", "Malgun Gothic", "Malgun Gothic Semilight", "Marlett", "Matura MT Script Capitals",
            "Microsoft Himalaya", "Microsoft JhengHei", "Microsoft JhengHei Light", "Microsoft JhengHei UI",
            "Microsoft New Tai Lue", "Microsoft PhagsPa", "Microsoft Sans Serif",
            "Microsoft Tai Le", "Microsoft YaHei UI", "Microsoft YaHei UI Light", "Microsoft Yi Baiti", "MingLiU-ExtB",
            "MingLiU_HKSCS-ExtB", "Mistral", "Modern", "Modern No. 20", "Mongolian Baiti", "Monotype Corsiva",
            "MS Gothic", "MS Outlook", "MS PGothic", "MS Reference Sans Serif", "MS Reference Specialty",
            "MS Sans Serif", "MS Serif", "MS UI Gothic", "MT Extra", "MV Boli", "Myanmar Text", "Niagara Engraved",
            "Niagara Solid", "Nirmala UI", "Nirmala UI Semilight", "NumberOnly", "OCR A Extended", "Old English Text MT",
            "Onyx", "Open Sans", "Open Sans Semibold", "Palace Script MT", "Palatino Linotype", "Papyrus", "Parchment",
            "Perpetua", "Perpetua Titling MT", "Playbill", "PMingLiU-ExtB", "Poor Richard", "Pristina", "Rage Italic",
            "Ravie", "Rockwell", "Rockwell Condensed", "Rockwell Extra Bold", "Roman", "Script", "Script MT Bold",
            "Segoe MDL2 Assets", "Segoe Print", "Segoe Script", "Segoe UI", "Segoe UI Black", "Segoe UI Emoji",
            "Segoe UI Historic", "Segoe UI Light", "Segoe UI Semibold", "Segoe UI Semilight", "Segoe UI Symbol",
            "Showcard Gothic", "SimSun-ExtB", "Sitka Banner", "Sitka Display", "Sitka Heading", "Sitka Small",
            "Sitka Subheading", "Sitka Text", "Small Fonts", "Snap ITC", "Stencil", "Sylfaen", "Symbol", "System",
            "Tahoma", "Tempus Sans ITC", "Terminal", "Times New Roman", "Trebuchet MS", "Tw Cen MT",
            "Tw Cen MT Condensed", "Verdana", "Viner Hand ITC", "Vivaldi",
            "Vladimir Script", "Webdings", "Wide Latin", "Wingdings", "Wingdings 2", "Wingdings 3", "Yu Gothic",
            "Yu Gothic Light", "Yu Gothic Medium", "Yu Gothic UI", "Yu Gothic UI Light", "Yu Gothic UI Semibold",
            "Yu Gothic UI Semilight", "仿宋", "华光中圆_CNKI", "华光中楷_CNKI", "华光中等线_CNKI", "华光中长宋_CNKI",
            "华光中雅_CNKI", "华光书宋_CNKI", "华光书宋一_CNKI", "华光书宋二_CNKI", "华光仿宋_CNKI", "华光仿宋一_CNKI",
            "华光仿宋二_CNKI", "华光准圆_CNKI", "华光大标宋_CNKI", "华光大黑_CNKI", "华光大黑二_CNKI", "华光姚体_CNKI",
            "华光小标宋_CNKI", "华光平黑_CNKI", "华光幼线_CNKI", "华光广告_CNKI", "华光彩云_CNKI", "华光报宋_CNKI",
            "华光报宋一_CNKI", "华光报宋二_CNKI", "华光敦韵宋_CNKI", "华光文韵宋_CNKI", "华光方珊瑚_CNKI",
            "华光标题宋_CNKI", "华光标题黑_CNKI", "华光楷体_CNKI", "华光楷体一_CNKI", "华光楷体二_CNKI", "华光淡古印_CNKI",
            "华光琥珀_CNKI", "华光秀丽_CNKI", "华光粗圆_CNKI", "华光粗黑_CNKI", "华光细圆_CNKI", "华光细黑_CNKI",
            "华光细黑一_CNKI", "华光综艺_CNKI", "华光美黑_CNKI", "华光胖头鱼_CNKI", "华光行书_CNKI", "华光行楷_CNKI",
            "华光行草_CNKI", "华光超粗黑_CNKI", "华光通心圆_CNKI", "华光隶书_CNKI", "华光隶变_CNKI", "华光魏体_CNKI",
            "华光黑体_CNKI", "华光黑变_CNKI", "华文中宋", "华文仿宋", "华文宋体", "华文彩云", "华文新魏", "华文楷体",
            "华文琥珀", "华文细黑", "华文行楷", "华文隶书", "宋体", "幼圆", "微软雅黑", "微软雅黑 Light", "思源黑体",
            "新宋体", "方正姚体", "方正粗黑宋简体", "方正舒体", "楷体", "等线", "等线 Light", "隶书", "黑体"});
    return font_text;
}

QFont Flow_edit_panel::get_font()
{
    return font;
}

QColor Flow_edit_panel::get_color()
{
    return color;
}
