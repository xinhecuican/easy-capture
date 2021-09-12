#ifndef CLOSE_DIALOG_H
#define CLOSE_DIALOG_H
#include<QDialog>
#include "Paint/Widgets/Paint_area.h"

class Close_dialog : public QDialog
{
    Q_OBJECT
public:
    Close_dialog();
    Close_dialog(Paint_area* area, QWidget* parent);
    void closeEvent(QCloseEvent *) override;
signals:
    void ignore_close();
    void hide_paint();
private:
    Paint_area* area;
    bool is_close;
};

#endif // CLOSE_DIALOG_H
