```
wget -O opencv.zip https://github.com/opencv/opencv/archive/4.5.1.zip
wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.5.1.zip
unzip opencv.zip
unzip opencv_contrib.zip
mkdir -p build && cd build
cmake -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib-4.5.1/modules ../opencv-4.5.1
cmake --build .
sudo make install
```
如果无法下载boostdesc,可以从[这里找到解决方法](https://github.com/Linfeng-Lee/OpenCV_boostdesc_vgg_file)

[出现fatal error: features2d/test/test_detectors_regression.impl.hpp: 没有那个文件或目录](https://blog.csdn.net/xiewenrui1996/article/details/108683866)
