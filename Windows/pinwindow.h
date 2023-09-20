#ifndef PINWINDOW_H
#define PINWINDOW_H
#include "Base/WindowBase.h"
#include "Widgets/pinwidget.h"

class PinWindow : public WindowBase
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit PinWindow(QWidget *parent = nullptr);
    void onWindowCancel() override;
    void onWindowSelect() override;
    /**
     * @brief receiveData
     * @param data1 image
     * @param data2 bound
     */
    void receiveData(QVariant data1 =QVariant(), QVariant data2 =QVariant()) override;
    bool controlWindowClose() override;
private:
    QList<PinWidget*> pinWidgets;
};

#endif // PINWINDOW_H
