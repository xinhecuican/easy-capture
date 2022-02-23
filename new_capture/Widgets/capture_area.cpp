#include "capture_area.h"
#include<QDebug>
#include<QSizePolicy>
#include<math.h>
#include<QPainter>
#include<QDesktopWidget>
#include<QApplication>
#include<QScreen>
#include "Manager/config.h"
#include "Manager/window_manager.h"
#include<QBitmap>
#include<QClipboard>
#include<QFileDialog>
#include "Paint/Widgets/history.h"
#include<QTimer>

Capture_area::Capture_area(QWidget* parent):QWidget(parent)
{
    this->parent = parent;
    setFixedSize(0, 0);
    begin_point = QPoint(0, 0);
    end_point = QPoint(0, 0);
    regions = PList<Capture_region*>();
    key_press = false;
    begin_draw = false;
    button_box = new QDialogButtonBox(parent);
    button_box->resize(130, 30);

    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setMouseTracking(true);
}

void Capture_area::reset()
{
    begin_point.setX(0);
    begin_point.setY(0);
    end_point.setX(0);
    end_point.setY(0);
    regions.clear_all();
    begin_draw = false;
    key_press = false;
    _is_press_region = false;
    if(button_box->children().size() != 1)
    {
        delete ok;
        delete cancel;
        delete clip;
        delete save;
        button_box->clear();
    }

}


Capture_area::~Capture_area()
{

}

void Capture_area::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    if(begin_draw)
    {
        end_point = event->globalPos();
    }
    else if(key_press || (Config::get_config(Config::capture_one_window) && _is_press_region))//按下ctrl键全部移动
    {
        end_point = event->globalPos();
        int w = end_point.x() - begin_point.x();
        int h = end_point.y() - begin_point.y();
        begin_point = end_point;
        for(int i=0; i<regions.count(); i++)
        {
            regions[i]->move(w, h);
        }
    }
    else
    {
        end_point = event->globalPos();
        int w = end_point.x() - begin_point.x();
        int h = end_point.y() - begin_point.y();
        begin_point = end_point;
        if(Config::get_config(Config::capture_multi_window_combine))
        {
            Capture_region* temp_region = NULL;
            for(int i=0; i<regions.count(); i++)
            {
                if(regions[i]->contains(QPoint(event->globalX(), event->globalY())))
                {
                    regions[i]->move(w, h);
                    temp_region = regions[i];
                    regions.removeAt(i);
                    break;
                }
            }
            combine_region(temp_region);
        }
        else if(Config::get_config(Config::capture_multi_window_separate) && now_index != -1)
        {
            regions[now_index]->move(w, h);
        }
    }
}

void Capture_area::mousePressEvent(QMouseEvent *event)
{
    event->accept();

    if(!region_contain(event->globalPos()))
    {
        _is_press_region = false;
        if(Config::get_config(Config::capture_one_window) && regions.count() >= 1)
        {
            begin_draw = false;
        }
        else
        {
            begin_draw = true;
        }
        begin_point = event->globalPos();
        end_point = begin_point;
    }
    else
    {
        button_box->hide();
        _is_press_region = true;
        begin_point = event->globalPos();
        end_point = begin_point;
        now_index = -1;
        if(Config::get_config(Config::capture_multi_window_separate))
        {
            for(int i=0; i<regions.count(); i++)
            {
                if(regions[i]->contains(event->globalPos()))
                {
                    now_index = i;
                }
            }
        }

    }
}


void Capture_area::mouseReleaseEvent(QMouseEvent *event)
{
    if(begin_draw)
    {
        begin_draw = false;
        QRect rect = QRect(get_x(), get_y(), get_w(), get_h());
        if(rect.width() > 10 && rect.height() > 10)
            combine_region(rect);
    }
    if(button_box->children().size() == 1)//默认有一个Layout组件
    {
        ok = new QPushButton(parent);
        ok->setIcon(QIcon(":/image/ok.svg"));
        QFile file(":/qss/capture_ok_button.qss");
        file.open(QFile::ReadOnly);
        QString str = file.readAll();
        file.close();
        ok->setStyleSheet(str);
        ok->connect(ok, &QPushButton::clicked, this, &Capture_area::on_click_ok);
        cancel = new QPushButton(parent);
        cancel->setIcon(QIcon(":/image/cancel.svg"));
        cancel->setStyleSheet(str);
        cancel->connect(cancel, &QPushButton::clicked, this, [=](){Window_manager::pop_window();});
        clip = new QPushButton(parent);
        clip->setIcon(QIcon(":/image/clipboard.png"));
        clip->setStyleSheet(str);
        clip->connect(clip, &QPushButton::clicked, this, &Capture_area::save2clip);
        save = new QPushButton(parent);
        save->setIcon(QIcon(":/image/save.png"));
        save->setStyleSheet(str);
        save->connect(save, &QPushButton::clicked, this, &Capture_area::save2file);
        button_box->addButton(ok, QDialogButtonBox::ActionRole);
        button_box->addButton(clip, QDialogButtonBox::AcceptRole);
        button_box->addButton(save, QDialogButtonBox::YesRole);
        button_box->addButton(cancel, QDialogButtonBox::RejectRole);
    }
    cal_button_pos();
    button_box->show();
}

void Capture_area::cal_button_pos()
{
    QRect bound = bounded_rect();
    QDesktopWidget* desktop = QApplication::desktop();
    QRect total = desktop->screenGeometry();
    if(bound.right()+130 < total.right() && bound.bottom() + 30 < total.bottom())
    {
        button_box->move(bound.bottomRight());
    }
    else if(bound.left() - 130 > total.left() &&  bound.top() - 30 > total.top())
    {
        button_box->move(bound.topLeft() - QPoint(130, 30));
    }
    else
    {
        button_box->move(bound.bottomRight() - QPoint(130, 30));
    }
}

void Capture_area::paintEvent(QPaintEvent *event)
{

}

void Capture_area::add_region(Capture_region* region)
{
    regions.append(region);
}

void Capture_area::on_region_remove(Capture_region* region)
{
    region->clear();
}


void Capture_area::combine_region(QRect rect)
{
    PList<Capture_region*> temp;
    PList<Capture_region*>::iterator iter = regions.begin();
    while(iter != regions.end())
    {

        if((*iter)->intersected(rect))
        {
            temp.append(*iter);
            iter = regions.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    Capture_region* temp_region = new Capture_region(this, parent, rect);
    if(!temp.isEmpty())
    {
        foreach(Capture_region* region, temp)
        {
            temp_region->unit(*region);
        }
    }
    add_region(temp_region);
    temp.clear_all();
    for(int i=0; i<regions.size(); i++)
    {
        regions[i]->on_pos_change(i);
    }
}

void Capture_area::combine_region(Capture_region* region)
{
    if(region == NULL)
    {
        return;
    }

    PList<Capture_region*> temp;
    PList<Capture_region*>::iterator iter = regions.begin();
    while(iter != regions.end())
    {
        if((*iter)->intersected(*region))
        {
            temp.append(*iter);
            iter = regions.erase(iter);
        }
        else
        {
            ++iter;
        }
    }


    foreach(Capture_region* cregion, temp)
    {
        region->unit(*cregion);
    }
    add_region(region);
    temp.clear_all();

}

QRect Capture_area::bounded_rect()
{
    int tx = 0x3f3f3f3f;
    int ty = -10000;
    int bx = -10000;
    int by = 0x3f3f3f3f;
    for(int i=0; i<regions.size(); i++)
    {
        QRect temp = regions[i]->get_polygon().boundingRect();
        tx = temp.x() < tx ? temp.x() : tx;
        ty = temp.y() > ty ? temp.y() : ty;
        bx = temp.right() > bx ? temp.right() : bx;
        by = temp.bottom() < by ? temp.bottom() : by;
    }
    return QRect(tx, ty, bx-tx, by - ty);
}

int Capture_area::get_x()
{
    return begin_point.x() > end_point.x() ? end_point.x() : begin_point.x();
}

int Capture_area::get_y()
{
    return begin_point.y() > end_point.y() ? end_point.y() : begin_point.y();
}

int Capture_area::get_w()
{
    return abs(end_point.x() - begin_point.x());
}

int Capture_area::get_h()
{
    return abs(end_point.y() - begin_point.y());
}

void Capture_area::set_captured_rect(QRect rect)
{
    begin_point = rect.topLeft();
    end_point = rect.bottomRight();
}

int Capture_area::get_region_count()
{
    return regions.count();
}

bool Capture_area::is_begin_draw()
{
    return begin_draw;
}

bool Capture_area::is_press_region()
{
    return _is_press_region;
}

QList<Capture_region*>& Capture_area::get_region()
{
    return regions;
}

bool Capture_area::region_contain(QPoint p)
{
    foreach(Capture_region* region, regions)
    {
        if(region->contains(p))
        {
            return true;
        }
    }
    return false;
}

void Capture_area::control_point_position_change(int index, QList<int> position, int dx, int dy)
{
    regions[index]->point_move(position, dx, dy);
    cal_button_pos();
}

void Capture_area::on_click_ok()
{
    QPixmap ans;
    QRect rect;
    getPic(ans, rect);
    Window_manager::change_window("Paint_window");
    Window_manager::get_window("Paint_window")->set_pic(ans, rect);
}

void Capture_area::getPic(QPixmap &ans, QRect &rect)
{
    parent->hide();
    QScreen* screen = QGuiApplication::primaryScreen();
    QPixmap p = screen->grabWindow(0);
    QPainterPath path;
    QPolygon polygon;
    for(int i=0; i<regions.size(); i++)
    {
        QPolygon temp_polygon = regions[i]->get_polygon();
        path.addPolygon(temp_polygon);
        polygon = polygon.united(temp_polygon);
    }
    QPixmap mask = QPixmap(p.width(), p.height());
    mask.fill(Qt::transparent);
    QPainter painter(&mask);
    painter.fillPath(path, QColor(1, 1, 1));
    p.setMask(mask.createMaskFromColor(QColor(1, 1, 1), Qt::MaskOutColor));
    rect = path.boundingRect().toRect();
    ans = p.copy(rect);
    rect.moveTo(0, 0);
}

void Capture_area::is_key_press(bool enter)
{
    this->key_press = enter;
}

void Capture_area::save2file()
{
    QString file_name = QFileDialog::getSaveFileName(this,
                                                     "保存",
                                                     History::instance()->get_last_directory(),
                                                     "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
    QTimer::singleShot(50, [=](){
        QPixmap pix;
        QRect rect;
        getPic(pix, rect);
        pix.save(file_name);
        History::instance()->log(History_data::Persist, file_name);
        Window_manager::change_window("MainWindow");
        Window_manager::hide_now();
    });
}

void Capture_area::save2clip()
{
    QClipboard *clip=QApplication::clipboard();
    QPixmap pix;
    QRect rect;
    getPic(pix, rect);
    clip->setPixmap(pix);
    Window_manager::change_window("MainWindow");
    Window_manager::hide_now();
}
