# 简介

简截是一款截图软件。它拥有基本的截图及处理功能，并且还有滚动截屏，ocr，图片透明化，录屏等功能

[文档](https://doc.easycapture.xinhecuican.tech)

# 环境



* windows环境
* qt 5.14.2 64位
* opencv 4.5.1 with contrib
* ffmpeg 5.1
* quazip0.7.3
* zlib
* qbreakpad

[库文件下载地址, 密码bsmo](https://xinhecuican.lanzouj.com/idDKk1b9lcej)

# 下载及代码

[蓝奏云 密码: 8nq4](https://xinhecuican.lanzouy.com/b041tvkfg)

[本地下载](http://121.37.81.150:8200/easycapture/downloads/easy_capture0.5.0.exe)

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
  
* 贴图

  ![](https://xinhecuican.tech/images/简截10.png)

# 快捷键

鼠标：

| 区域 | 动作 | 作用 |
|-|-|-|
| 截图 | 左键 | 在区域上时为移动区域，不在为创建新区域 |
| 截图 | 中键 | 保存置剪切板，可以在设置中更改 |
| 滚动截图 | 滚轮 | 调节滚动截屏的速度 |
| 贴图 | 滚轮 | 调节贴图的大小 |


| 区域 | 快捷键 | 作用 |
|-|-|-|
| 全局 | Ctrl+F1 | 如果当前为隐藏，则唤醒并跳转到截图窗口。否则无效 |
| 全局 | Ctrl+F2 | 截全屏并自动保存，可在设置中配置保存位置 |
| 全局 | F5 | 开始录屏 |
| 全局 | F6 | 暂停/恢复录屏 |
| 全局 | F7 | 终止录屏 |

还有许多快捷键可以在**设置-快捷键**中查看

如果你觉得本软件对你有帮助，可以请作者喝一杯咖啡 ^_^

<img src="https://image.xinhecuican.tech/img/%E8%B5%9E%E8%B5%8F%E7%A0%81.png" alt="赞赏码" style="zoom:50%;" />
