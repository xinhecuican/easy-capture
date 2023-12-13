#include "videolayercontainer.h"
#include "../paintarea.h"
#include <QRadioButton>
#include "../../Helper/mstring.h"
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include "../../Helper/imagehelper.h"
#include <QLineEdit>
#include <QFileDialog>
#include "../../Manager/config.h"

VideoLayerContainer::VideoLayerContainer(PaintArea* area) : LayerContainer(area)
{

}

QWidget* VideoLayerContainer::onValid(QWidget *widgetParent){
    if(widget == NULL){
        widget = new QWidget(widgetParent);
        QHBoxLayout* layout = new QHBoxLayout();

        QRadioButton* enableAudioButton = new QRadioButton(widgetParent);
        enableAudioButton->setText(MString::search("{rh0LYgOmDD}录制声音"));
        enableAudioButton->setChecked(true);
        enableAudioButton->setCheckable(true);
        info.enableAudio = true;
        info.channel = 2;
        connect(enableAudioButton, &QRadioButton::clicked, this, [=](bool check){
            info.enableAudio = check;
        });

        QComboBox* deviceSelector = new QComboBox(widgetParent);
        int nDeviceNum = waveInGetNumDevs();
        for (int i = 0; i < nDeviceNum; ++i)
        {
            WAVEINCAPS wic;
            waveInGetDevCaps(i, &wic, sizeof(wic));

            //转成utf-8
            int nSize = WideCharToMultiByte(CP_UTF8, 0, wic.szPname, wcslen(wic.szPname), NULL, 0, NULL, NULL);
            std::shared_ptr<char> spDeviceName(new char[nSize + 1]);
            memset(spDeviceName.get(), 0, nSize + 1);
            WideCharToMultiByte(CP_UTF8, 0, wic.szPname, wcslen(wic.szPname), spDeviceName.get(), nSize, NULL, NULL);
            deviceSelector->addItem(spDeviceName.get());
        }
        info.audioDeviceIndex = 0;
        deviceSelector->setMaximumWidth(150);
        connect(deviceSelector, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index){
            info.audioDeviceIndex = index;
        });

        QLabel* fpsLabel = new QLabel("fps", widgetParent);
        QSpinBox* fpsSelector = new QSpinBox(widgetParent);
        fpsSelector->setRange(1, 60);
        fpsSelector->setValue(20);
        info.fps = 20;
        fpsSelector->setAccelerated(true);
        fpsSelector->setWrapping(true);  // 开启循环
        fpsSelector->setKeyboardTracking(true);
        connect(fpsSelector, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int index){
            info.fps = index;
        });

        QToolButton* pathChooseButton = new QToolButton(widgetParent);
        pathChooseButton->setIcon(ImageHelper::getIcon("folder"));
        pathChooseButton->setToolTip(Config::getConfig<QString>(Config::video_save_path));
        connect(pathChooseButton, &QToolButton::clicked, this, [=](){
            QString result = QFileDialog::getExistingDirectory(widgetParent, MString::search("{nmqfWFejoz}选择保存目录"), Config::getConfig<QString>(Config::video_save_path));
            if(result != ""){
                Config::setConfig(Config::video_save_path, result);
                Config::updateConfig(Config::video_save_path);
                pathChooseButton->setToolTip(result);
            }
        });

        QLineEdit* fileNameEdit = new QLineEdit(widgetParent);
        fileNameEdit->setPlaceholderText(MString::search("{cR3jOHb9Qw}新建"));
        name = MString::search("{cR3jOHb9Qw}新建");
        fileNameEdit->setMaxLength(20);
        connect(fileNameEdit, &QLineEdit::textChanged, this, [=](QString text){
            name = text;
            if(text == ""){
                name = MString::search("{cR3jOHb9Qw}新建");
            }
        });

        QComboBox* suffixSelector = new QComboBox(widgetParent);
        suffixSelector->addItem("mp4");
        suffixSelector->addItem("gif");
        suffix = "mp4";
        connect(suffixSelector, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index){
            if(index == 0){
                suffix = "mp4";
            }
            else if(index == 1){
                suffix = "gif";
            }
        });

        QToolButton* helpButton = new QToolButton(widgetParent);
        helpButton->setIcon(ImageHelper::getIcon("help"));
        helpButton->setToolTip(MString::search("{SHLK0bd4wt}开始录屏: F5\n暂停/恢复录屏: F6\n结束录屏: F7"));

        layout->addWidget(enableAudioButton);
        layout->addWidget(deviceSelector);
        layout->addWidget(fpsLabel);
        layout->addWidget(fpsSelector);
        layout->addWidget(pathChooseButton);
        layout->addWidget(fileNameEdit);
        layout->addWidget(suffixSelector);
        layout->addWidget(helpButton);
        widget->setLayout(layout);
    }
    return widget;
}
