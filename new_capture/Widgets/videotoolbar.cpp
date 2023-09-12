#include "videotoolbar.h"
#include <QRadioButton>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QToolButton>
#include <QFileDialog>
#include "Paint/Widgets/history.h"
#include <QLineEdit>
#include "windows.h"
#include <memory>
#include <QDebug>
#include "Helper/mstring.h"
#include "Helper/imagehelper.h"
#pragma comment(lib, "Winmm.lib")

VideoToolbar::VideoToolbar(QWidget* parent) : QWidget(parent)
{
    layout = new QHBoxLayout();

    QRadioButton* enableAudioButton = new QRadioButton(this);
    enableAudioButton->setText(MString::search("{rh0LYgOmDD}录制声音"));
    enableAudioButton->setChecked(true);
    enableAudioButton->setCheckable(true);
    info.enableAudio = true;
    info.channel = 2;
    connect(enableAudioButton, &QRadioButton::clicked, this, [=](bool check){
        info.enableAudio = check;
    });

    QComboBox* deviceSelector = new QComboBox(this);
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

    QLabel* fpsLabel = new QLabel("fps", this);
    QSpinBox* fpsSelector = new QSpinBox(this);
    fpsSelector->setRange(1, 60);
    fpsSelector->setValue(20);
    info.fps = 20;
    fpsSelector->setAccelerated(true);
    fpsSelector->setWrapping(true);  // 开启循环
    fpsSelector->setKeyboardTracking(true);
    connect(fpsSelector, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int index){
        info.fps = index;
    });

    QToolButton* pathChooseButton = new QToolButton(this);
    pathChooseButton->setIcon(ImageHelper::getIcon("folder"));
    pathChooseButton->setToolTip(History::instance()->getVideoSavePath());
    connect(pathChooseButton, &QToolButton::clicked, this, [=](){
        QString result = QFileDialog::getExistingDirectory(this, MString::search("{nmqfWFejoz}选择保存目录"), History::instance()->getVideoSavePath());
        if(result != ""){
            History::instance()->setVideoSavePath(result);
            pathChooseButton->setToolTip(History::instance()->getVideoSavePath());
        }
    });

    QLineEdit* fileNameEdit = new QLineEdit(this);
    fileNameEdit->setPlaceholderText(MString::search("{cR3jOHb9Qw}新建"));
    name = MString::search("{cR3jOHb9Qw}新建");
    fileNameEdit->setMaxLength(20);
    connect(fileNameEdit, &QLineEdit::textChanged, this, [=](QString text){
        name = text;
        if(text == ""){
            name = MString::search("{cR3jOHb9Qw}新建");
        }
    });

    QComboBox* suffixSelector = new QComboBox(this);
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

    QToolButton* helpButton = new QToolButton(this);
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
    setLayout(layout);
}

RecordInfo VideoToolbar::getRecordInfo(){
    info.recordPath = QDir::cleanPath(History::instance()->getVideoSavePath() + "/" + name + "." + suffix);
    if(suffix == "gif"){
        info.enableAudio = false;
    }
    return info;
}
