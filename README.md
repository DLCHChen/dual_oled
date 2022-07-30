# dual_oled
It is a dual oled code for stm32f103c8 with mic and fft feature.

此活动，早些年就开始了。中间停停做做。直到现在才比较满意。

2022-07-30

#### 用到了以下的材料（如图所示）：
* stm32f103c8 开发板

* oled和lcd屏

* mic模块

* 按键

* 等

![图片](dfiles/components.jpg)

#### 以上材料的连接，请查看dfiles目录里的stm32f103c8-io-ref.pdf

杂乱的连接示意图

![图片](dfiles/connection.jpg)

#### 基本实现方式：
通过ADC采样输入的声音信号，然后经过FFT转换，再转dB，然后在oled和LCD屏上显示。
按键可以切换不同的显示方式。

#### 使用MDK5编译，需要安装Keil.STM32F1xx_DFP.2.3.0.pack芯片支持包

#### 演示图片

![图片](dfiles/showpic.png)




