# 简介

简截是一款截图软件。它拥有基本的截图及处理功能，并且还有滚动截屏，ocr，图片透明化，录屏等功能

# 环境



* windows环境
* qt 5.14.2 64位
* opencv 4.5.1 with contrib
* ffmpeg 5.1

# 下载及代码

[蓝奏云 密码: 9qkn](https://xinhecuican.lanzouy.com/ibWMi0oq1e0b)

[本地下载](121.37.81.150:8200/easycapture/downloads/easy_capture0.5.0.exe)

# 功能介绍


* 支持开机自启，自动更新，历史记录保存，自动复制到剪切板等功能

* 多区域截图

  ​     ![](https://xinhecuican.tech/images/%E7%AE%80%E6%88%AA3.gif)

* 滚动截图

	使用opencv库的图像拼接实现，并且加上了多线程，位置在Capture_window/.../Scroll_handle。

	
	
	此外滚动截图可以调整滚动速度。滚动速度较快时可以利用所有cpu。
	
	![](https://xinhecuican.tech/images/%E7%AE%80%E6%88%AA8.gif)
	
* 绘制
	当前仅支持笔，荧光笔和文本绘制，并且支持撤销、恢复和擦除。

	![](https://xinhecuican.tech/images/%E7%AE%80%E6%88%AA6.gif)

* 透明

	目前仅可以做到对某一个色素透明，对于渐变和拍屏无效。并且由于文字都是由不同种类像素组成因此无法做到选取文字（正好下学期有图像处理，看看通过反锯齿可以直接把文字的范围弄出来）。
	
* ocr

  使用[RapidOCR](https://github.com/RapidAI/RapidOCR)引擎进行识别![](https://xinhecuican.tech/images/简截9.png)

* 录屏

  录屏基于[NanaRecorder](https://github.com/onlyet/NanaRecorder)改进而来

# 快捷键

鼠标：

| 区域 | 动作 | 作用 |
|-|-|-|
| 截图 | 左键 | 在区域上时为移动区域，不在为创建新区域 |
| 截图 | 中键 | 确认 |


| 区域 | 快捷键 | 作用 |
|-|-|-|
| 全局 | Ctrl+F1 | 如果当前为隐藏，则唤醒并跳转到截图窗口。否则无效 |
| 全局 | Ctrl+F2 | 截全屏并自动保存，可在设置中配置保存位置 |
| 全局 | F5 | 开始录屏 |
| 全局 | F6 | 暂停/恢复录屏 |
| 全局 | F7 | 终止录屏 |

还有许多快捷键可以在**设置-快捷键**中查看

# 更新计划

- [x] 完成滚动截屏
- [x] 修bug，重构部分代码
- [x] 完善绘图区域，添加文本框，箭头和水印图片，支持旋转
- [ ] 完成UI_manager，可以利用qss(和css语法类似)修改样式并且提供主题
- [ ]  完善透明度选择，除了删除某些像素外可以选择保留哪些像素，并支持渐变颜色删除。

# 翻译

本软件提供了便利的翻译方式，翻译文件位于Data/Languages下。翻译方式为将该目录下chinese.xml复制并且重命名为对应语言，例如english.xml。然后将文件里面的中文换成对应语言。以Data/Languages/MainWindow/chinese.xml为例

```xml
<?xml version="1.0" encoding="utf-8"?>
<strings language="chinese">//将language标签替换成对应语言，目前只有english,如果想翻译其他语言请先和作者联系
    <string id="cR3jOHb9Qw" text="新建"/>//中文翻译为对应语言
    <string id="7yUWnx82jI" text="模式"/>
    <string id="OBwjJUhTkh" text="矩形窗口"/>
    <string id="fnGapBU4vo" text="自由截图"/>
    <string id="ETY295cnab" text="滚动截图"/>
    <string id="rzdUgOw26Y" text="活动窗口截屏"/>
    <string id="FHFzLMcLYa" text="全屏"/>
</strings>
```
如果先练练英文或其他语言的可以尝试一下
