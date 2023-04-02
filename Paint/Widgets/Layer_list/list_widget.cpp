#include "list_widget.h"
#include<QPushButton>
#include<QDebug>


List_widget::List_widget()
{

}

List_widget::List_widget(QWidget* parent) : QListWidget(parent)
{
    setViewMode(QListView::ListMode);
    setFlow(QListView::TopToBottom);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    QPushButton* add_widget_button = new QPushButton(this);
    add_widget_button->setIcon(QIcon(":/image/add.svg"));
    connect(add_widget_button, &QPushButton::clicked, this, [=](){
        emit button_click(-1, List_item::APPEND);
    });
    QListWidgetItem* item = new QListWidgetItem(this);
    add_widget_button->setFixedSize(24, 24);
    item->setSizeHint(QSize(100, 48));
    this->addItem(item);
    this->setItemWidget(item, add_widget_button);
}

void List_widget::add_widget(QString name)
{
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(QSize(100, 48));
    insertItem(count()-1, item);
    List_item* list_widget = new List_item(this, name, name == "第0层" ? false : true);
    connect(list_widget, &List_item::text_change, this, [=](QString text){
        emit text_change(this->indexFromItem(item).row(), text);
    });
    connect(list_widget, &List_item::button_click, this, [=](List_item::button_type index){
        int row = this->indexFromItem(item).row();
        if((index == List_item::UP && row <= 0) || (index == List_item::DOWN && row >= this->count()-2)
                || (index == List_item::CLOSE && (row < 0 || row > this->count() - 2)))//最后一个是添加按钮
        {
            return;
        }
        emit button_click(row, index);
        if(index == List_item::UP)
        {
            swap_item(row-1, row);
            scrollToItem(this->item(row-1));
            this->setItemSelected(this->item(row-1), true);
        }
        else if(index == List_item::DOWN)
        {
            swap_item(row, row+1);
            scrollToItem(this->item(row+1));
            this->setItemSelected(this->item(row+1), true);
        }
        else if(index == List_item::CLOSE)
        {
            remove_item(row);
        }
    });
    this->setItemWidget(item, list_widget);
}

void List_widget::remove_item(int row)
{
    QListWidgetItem *item = this->item(row);
    QWidget *widget = itemWidget(item);
    if (nullptr != widget)
    {
        removeItemWidget(item);
        widget->deleteLater();
    }

    item = takeItem(row);
    delete item;
    item = nullptr;
}

void List_widget::swap_item(int first_row, int second_row)
{
    //保证first row永远在second_row之前，方便后面的处理
    if (first_row > second_row)
    {
        int temp = first_row;
        first_row = second_row;
        second_row = temp;
    }
    else if (first_row == second_row)//相同的两行不允许进行后续交换操作
    {
        return;
    }
    if (second_row > 0)
    {
        QListWidgetItem* item1 = item(first_row);
        QListWidgetItem* item2 = item(second_row);
        List_item* widget1 = static_cast<List_item*>(itemWidget(item1));
        List_item* widget2 = static_cast<List_item*>(itemWidget(item2));
        QString name1 = widget1->getName();
        widget1->setName(widget2->getName());
        widget2->setName(name1);
    }
}
/*
void List_widget::wheelEvent(QWheelEvent* event)
{
    int degress = event->angleDelta().ry() * 0.5;
    if(event->orientation() == Qt::Vertical)
    verticalScrollBar()->setValue(verticalScrollBar()->value() - degress);
    event->accept();
}
*/
