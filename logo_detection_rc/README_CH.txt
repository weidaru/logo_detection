1.安装
需要事先安装ImageMagika并把它加到系统path中，在./vendor或是根目录下可以找到。

2.文件格式
1）所有的图片都用JPG格式。
2）所有的视频都用了AVI格式，并用JPEG encoding.

3.配置
所有的配置信息都在./config/config.ini中，大多数信息可以直接从名字上看出意思。接下来解释几个比较难理解的选项

enableDebug:0时表示普通模式，1时表示debug模式。在debug模式中，logo被logoletSize*logoletSize的小块标识出而不是一个整合后的区域。

logoletSize:最小区域大小。初始值为12，改变后程序还没有经过测试。

mode：0表示检测模式，也就是普通模式，1时程序会生成神经网络需要的输入。

在Detector下的参数尽量不要进行改变。当然，如果你知道他们是什么意思的话就随意了。

其他的东西大家都懂的。