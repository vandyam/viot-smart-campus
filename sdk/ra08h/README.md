# animal-tracker

# 0.Introduction

Ai-Thinker is an expert in IoT wireless design, focusing on solutions that are simple, flexible, easy to manufacture and deploy. Ai-Thinker develops and designs wireless system-level module products with integrated SoC, stable performance, and low power consumption in the IoT industry, and various modules with Wi-Fi, LoRaWAN, Bluetooth, and UWB functions. The modules have excellent RF performance.

Ra-08(H) module is a LoRaWAN module jointly developed by Ai-Thinker Technology and Shanghai ASR Microelectronics Co., Ltd.(ASR) in depth. This repository is a guide for getting started with the secondary development of the LoRaWAN module SoC. The corresponding chip model is ASR6601CB, Flash 128 KB, SRAM 16 KB, 32-bit 48 MHz ARM Cortex-M4 kernel.

The Ra-08(H) module has a built-in AT firmware program default, and it can be used directly to connect to the LoRaWAN gateway. If you need to connect to Ali LinkWAN, you need to program this repository code.


## 1.Purpose

Based on the linux environment, this article introduces the specific process of secondary development of example code of An-Thinker’s Ra-08(H) module for your reference.


## 2.Hardware preparation

1.  Linux environment
The necessary environment for compiling & programming & running operations, this article takes (Ubuntu18.04) as an example.
'(Windows users can install a virtual machine and then install linux in the virtual machine.)'

2.  Equipment
Get samples from Ai-Thinker’s Alibaba store to get 2 pcs module and the antenna as well.

3.  USB cable
Connect PC and Ra-08 development board to burn/download programs, view logs, etc.




## 3. Build the compiler environment
```bash
sudo apt-get install gcc-arm-none-eabi 
```
```bash
sudo apt-get install git
```
```bash
sudo apt-get install vim
```
```bash
sudo apt-get install python
```
```bash
sudo apt-get install python-pip
```
```bash
pip install pyserial configparser
```
```bash
pip install configparser
```


## 4.SDK preparation
```bash
git clone git@github.com:vandyam/animal-tracker.git
```
## 6. Compiling & Programming & Running

1. Configure environment variables\
```bash
    source build/envsetup.sh
```
2. Example of compiling Ra-08-kit-LoraTest\
```bash
    cd Ra-08-kit-LoraTest
```
```bash
    make
```

## 7. Flash & download firmware & view log

Connect the USB cable to the device and PC, make sure the programming port is correct.
and put the board in download mode as shown below

![Screenshot from 2023-08-05 13-03-51](https://github.com/vandyam/animal-tracker/assets/110624697/c4177cda-b6ad-4116-a255-d6997bd30879)


First modify the default port number and baud rate to connect to the hardware, and modify it in the file\
`\build\make\common.mk`

```
# flash settings
TREMO_LOADER := $(SCRIPTS_PATH)/tremo_loader.py
SERIAL_PORT        ?= /dev/ttyUSB0
SERIAL_BAUDRATE    ?= 921600
```
Clear the project compilation file
```bash
make clean
```

Burning program
```bash
make flash
```
Press the RST button on two Ra-08-Kit development boards, and you can see the logs on serial monitor

Press the IO2 button to send data.
