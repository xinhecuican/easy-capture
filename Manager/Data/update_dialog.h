#ifndef UPDATE_DIALOG_H
#define UPDATE_DIALOG_H
#include<QDialog>
#include "update_data.h"

class Update_dialog : public QDialog
{
public:
    Update_dialog();
    Update_dialog(Update_data data, QWidget* widget);
private:
    Update_data data;
};

#endif // UPDATE_DIALOG_H
