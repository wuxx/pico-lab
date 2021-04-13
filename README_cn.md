Pico-PMOD-ExtBoard
-----------
[中文](./README.md) [English](./README_en.md)

* [介绍](#pico-PMOD扩展板介绍) 
* [特性](#特性)
* [使用说明](#使用说明)
	* [串口](#串口)
	* [SWD下载](#swd下载)
* [工程编译说明](#工程编译说明)
* [产品链接](#产品链接)
* [参考](#参考)


# Pico PMOD扩展板介绍 
Pico PMOD扩展板是MuseLab推出的方便用户使用Pico的扩展板，扩展板以PMOD标准引出Pico所有GPIO，同时和板载调试器DAPLink提供的SWD下载接口、串口对接，可以方便的对Pico进行烧录测试  
<div align=center>
<img src="https://github.com/wuxx/pico-lab/blob/main/doc/4.jpg" width = "400" alt="" align=center />
<img src="https://github.com/wuxx/pico-lab/blob/main/doc/1.jpg" width = "400" alt="" align=center />
</div>


# 特性   
- 扩展板两侧通过标准PMOD接口引出所有GPIO  
- 提供一路串口  
- SWD下载调试接口  
- TYPE-C接口供电调试  

# 使用说明
### 串口
板载的DAPLink的串口和Pico接线说明如下
DAPLink | Pico
---|---
GND | GND
PA9/UART1_TX  | GP17/UART0_RX
PA10/UART1_RX | GP16/UART0_TX

Linux环境下可使用minicom或者picocom串口工具打开串口进行调试，举例如下：
```
$sudo apt install minicom
$minicom -b 115200 -o -D /dev/ttyACM0

$sudo apt install picocom
$picocom -b 115200 /dev/ttyACM0

```


### SWD下载
Pico的onchiprom程序实现了一个U盘拖拽烧录的功能，可以通过拖拽uf2文件到虚拟U盘中实现烧录，然而在某些开发场景下，若需要频繁修改代码烧录测试，则需要反复将Pico下电，按住按键再重新上电，等待USB枚举完成，才能进行拖拽烧录，过程略微有些繁琐。 实际上可以通过Pico的SWD接口实现烧录调试，配合openocd开源调试软件，无需重新对Pico上下电，只需输入一条命令即可完成烧录，具体过程说明如下
1. 安装openocd
```
$ cd ~/pico
$ sudo apt install automake autoconf build-essential texinfo libtool libftdi-dev libusb-1.0-0-
dev
$ git clone https://github.com/raspberrypi/openocd.git --recursive --branch rp2040 --depth=1
$ cd openocd
$ ./bootstrap
$ ./configure --enable-cmsis-dap
$ make -j4
$ sudo make install
```
  
2. 本仓库已经封装好相应的脚本，导入环境变量之后即可在任意路径下调用烧录目标文件，注意烧录的格式后缀为`hex`或者`bin`，而非拖拽烧录的`uf2`文件。  
```
$cd pico-lab/tools
$source env.sh
$dfw xxx.hex/xxx.bin
```


# 工程编译说明
```
$cd tools && source env.sh && cd -
$cd software/infones
$mkdir -p build && cd build
$cmake ..
$make -j
$dfw infones/pico_infones.bin
$dreset
```

# 产品链接
[Pico-PMOD-ExtBoard](https://item.taobao.com/item.htm?spm=a1z10.3-c-s.w4002-21349689064.12.28c0773de8TPq0&id=642011801058)

# 参考
### pico-sdk
https://github.com/raspberrypi/pico-sdk
### pico-examples
https://github.com/raspberrypi/pico-examples
### RPI-HAT-Pico-BreakoutBoard
https://github.com/wuxx/RPI-HAT-Pico-BreakoutBoard
### DAPLink
https://github.com/ARMmbed/DAPLink
