# fw-AC63_GP_MCU
About Firmware for Generic MCU SDK（AC63 series）, Support AC638N

[中文](./README.md) | EN

firmware for Generic MCU SDK（AC63 series）

This repository contains the Jieli source code, aims at helping the developers for the toy & generic MCU applications.
It must be combined with lib.a and the repositories that use the same
naming convention to build the provided samples and to use the additional
subsystems and libraries.

Getting Started
------------

Welcome to JL open source! See the `Introduction to SDK` for a high-level overview,
and the documentation's `Getting Started Guide` to start developing.

Toolchain
------------

How to get the `JL Toolchain` and setup the build enviroment,see below

* Complie Tool ：install the JL complie tool to setup the build enviroment, [download link](https://pan.baidu.com/s/1f5pK7ZaBNnvbflD-7R22zA) code: `ukgx`

* USB updater : program flash tool to download the `hex` file to the target board, please accquire the tool form the [link](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w4004-22883854875.5.504d246bXKwyeH&id=620295020803) and check the related configuration and [document](.doc/stuff/ISD_CONFIG.INI配置文件说明.pdf)


Documentation
------------

* Chipset brief : [SoC datasheet](./doc)

* SDK Version: [SDK History](doc/AD150_SDK_发布版本信息.pdf)

* SDK introduction : [SDK quick start guide](./doc/AD150_SDK手册.pdf)

* SDK architure : [SDK module architure ](./doc/architure)

Build
-------------
Select a project to build. The following folders contains buildable projects:

* APP_TOY: [Toy](./apps/spp_and_le), usage: 

* APP_MCU: [MCU](./apps/hid), usage: 


Comming Soon：

* APP_Bluetooth ：`IoT (ipv6 / 6lowpan)`

SDK support Codeblock & Make to build to project,make sure you already setup the enviroment

* Codeblock build : enter the project directory and find the `.cbp`,double click and build.

* Makefile build : `apps/app_cfg` select the target you want to build,double click the `make_prompt` and excute `make`

  `before build the project make sure the USB updater is connect and enter the update mode correctly`


Hardware
-------------

* EV Board ：(https://shop321455197.taobao.com/?spm=a230r.7195193.1997079397.2.2a6d391d3n5udo)

* Production Tool : massive prodution and program the SoC, please accquire the tool from the [link](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w4004-22883854875.8.504d246bXKwyeH&id=620941819219) and check the releated [doc](./doc/stuff/烧写器使用说明文档.pdf)

* Wireless Tester : Over the air update/RF Calibration/Fast production test, please accuire the tool from the [link](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w4004-22883854875.10.504d246bXKwyeH&id=620942507511),check [doc](./doc/stuff/AC690x_1T2测试盒使用说明.pdf) for more infomation.


Disclaimer
------------

AC63N_SDK supports development with AC63 series devices.
AC63 Series devices (which are pre-production) are supported for development in Release for production and deployment in end products.
