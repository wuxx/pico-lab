Pico-PMOD-ExtBoard
-----------
[中文](./README_cn.md) [English](./README.md)

* [Introduction](#pico-PMOD-extboard-introduction) 
* [Feature](#feature)
* [How to Use](#how-to-use)
	* [Serial](#serial)
	* [SWD](#swd)
* [How to Compile](#how-to-compile)
* [Product Link](#product-link)
* [Reference](#reference)


# Pico PMOD ExtBoard Introduction
Pico PMOD ExtBoard is a user-friendly expansion board for Pico launched by MuseLab. The ExtBoard leads all GPIOs of Pico with PMOD standard, and the pico is connected with SWD download interface and serial port provided by onboard debugger DAPLink, which can be easily used for program & test of Pico.

<div align=center>
<img src="https://github.com/wuxx/pico-lab/blob/main/doc/4.jpg" width = "400" alt="" align=center />
<img src="https://github.com/wuxx/pico-lab/blob/main/doc/1.jpg" width = "400" alt="" align=center />
</div>

# Feature
- Standard PMOD interface
- USB CDC Serial port
- SWD
- TYPE-C for power & debug

# How to Use
### Serial
DAPLink | Pico
---|---
GND | GND
PA9/UART1_TX  | GP17/UART0_RX
PA10/UART1_RX | GP16/UART0_TX

under Linux, you can use the minicom or picocom to open the serial port, for example：
```
$sudo apt install minicom
$minicom -b 115200 -o -D /dev/ttyACM0

$sudo apt install picocom
$picocom -b 115200 /dev/ttyACM0

```


### SWD
Pico's onchiprom program implements a USB disk with drag-and-drop burning function, However, in some development scenarios, if you need to frequently modify the code and test, you need to repeatedly power down Pico, hold down the button and power up again, and wait for the USB enumeration to complete before you can drag and drop, which is a slightly tedious process. In fact, the Pico can be programmed & debugged through the SWD interface with the openocd, no need to re-power the Pico, just enter a command to complete, the detail is described as follows

1. install openocd
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
  
2. This repository has wrapped the command into scripts, after importing the environments variables, you can call the script in any path, note that the format of the burn suffix is `hex` or `bin`, not the drag and drop `uf2` file
```
$cd pico-lab/tools
$. ./env.sh
$dfw xxx.hex/xxx.bin
```


# How to Compile
```
$cd tools && source env.sh && cd -
$cd software/infones
$mkdir -p build && cd build
$cmake ..
$make -j
$dfw infones/pico_infones.bin
$dreset
```

# Product Link
[Pico-PMOD-ExtBoard](https://www.aliexpress.com/item/1005002511135057.html?spm=2114.12010615.8148356.1.617667e7KNATJJ)

# Reference
### pico-sdk
https://github.com/raspberrypi/pico-sdk
### pico-examples
https://github.com/raspberrypi/pico-examples
### RPI-HAT-Pico-BreakoutBoard
https://github.com/wuxx/RPI-HAT-Pico-BreakoutBoard
### DAPLink
https://github.com/ARMmbed/DAPLink
