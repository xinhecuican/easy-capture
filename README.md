# 简介

简截是一款截图软件。它的主要功能是图片透明化。

| 透明化前|透明化后 |
|-|-|
| <img src="https://cdn.jsdelivr.net/gh/xinhecuican/xinhecuican.github.io/images/%E7%AE%80%E6%88%AA1.png"> | <img src="https://cdn.jsdelivr.net/gh/xinhecuican/xinhecuican.github.io/images/%E7%AE%80%E6%88%AA2.png"> |

此外他还支持多种截图模式并且可以图片上编辑



# 环境



* windows环境
* qt 5.14.2 64位
* opencv 4.5.1 with contrib

# 下载及代码

[发布页](https://xinhecuican.github.io/post/f0fbe9f2.html#%E7%AE%80%E4%BB%8B)

[github地址页](https://github.com/xinhecuican/easy-capture)

[源码结构及一些编写心得](https://xinhecuican.github.io/post/35b71592.html)

欢迎拿走源码自己修改。也可以在评论区提意见（我设置了邮箱提醒）。

# 功能介绍


* 支持开机自启，自动更新，历史记录保存，自动复制到剪切板等功能

* 分离区域截图

  ​     ![](https://cdn.jsdelivr.net/gh/xinhecuican/xinhecuican.github.io/images/%E7%AE%80%E6%88%AA3.gif)

* 可合并区域截图

  ​    ![](https://cdn.jsdelivr.net/gh/xinhecuican/xinhecuican.github.io/images/%E7%AE%80%E6%88%AA4.gif)

     多个区域截图主要是为了截取一些相关但不相连的部分。

* 自由截图

  ![](https://cdn.jsdelivr.net/gh/xinhecuican/xinhecuican.github.io/images/%E7%AE%80%E6%88%AA5.gif)

* 滚动截图

	使用opencv库的图像拼接实现，并且加上了多线程，位置在Capture_window/.../Scroll_handle。

	
	
	此外滚动截图可以调整滚动速度。滚动速度较快时可以利用所有cpu。
	
	![](https://cdn.jsdelivr.net/gh/xinhecuican/xinhecuican.github.io/images/%E7%AE%80%E6%88%AA8.gif)
	
* 绘制
	当前仅支持笔，荧光笔和文本绘制，并且支持撤销、恢复和擦除。

	![](https://cdn.jsdelivr.net/gh/xinhecuican/xinhecuican.github.io/images/%E7%AE%80%E6%88%AA6.gif)

* 透明

	目前仅可以做到对某一个色素透明，对于渐变和拍屏无效。并且由于文字都是由不同种类像素组成因此无法做到选取文字（正好下学期有图像处理，看看通过反锯齿可以直接把文字的范围弄出来）。

# 快捷键

鼠标：

| 区域 | 动作 | 作用 |
|-|-|-|
| 截图 | 左键 | 在区域上时为移动区域，不在为创建新区域 |
| 截图 | 中键 | 确认 |


| 区域 | 快捷键 | 作用 |
|-|-|-|
| 全局 | Ctrl+F1 | 如果当前为隐藏，则唤醒并跳转到截图窗口。否则无效 |
| 主窗口 | Ctrl+N | 打开截图窗口 |
| 截图窗口 | 1 | 只允许单个矩形区域 |
| 截图窗口 | 2 | 允许多个分离的矩形区域 |
| 截图窗口 | 3 | 允许多个可合并的矩形区域 |
| 截图窗口 | Esc | 退出 |
| 截图窗口 | Enter | 确认 |
| 绘图窗口 | Ctrl+S | 保存 |
| 绘图窗口 | Ctrl+Z | 撤销 |
| 绘图窗口 | Ctrl+X | 恢复 |
| 绘图窗口 | Ctrl+N | 跳转到截图窗口 |

# 更新计划

1. 完成滚动截屏(完成)
2. 修bug，重构部分代码(完成)
3. 完善绘图区域，添加文本框，箭头和水印图片，支持旋转
4. 完成UI_manager，可以利用qss(和css语法类似)修改样式并且提供主题
5. 完善透明度选择，除了删除某些像素外可以选择保留哪些像素，并支持渐变颜色删除。

# 翻译

本软件提供了便利的翻译方式，翻译文件位于Data/Languages下。翻译方式为将该目录下chinese.xml复制并且重命名为对应语言，例如english.xml。然后将文件里面的中文换成对应语言。以Data/Languages/MainWindow/chinese.xml为例

```
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
