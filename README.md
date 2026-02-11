# GIT Workshop Firmware Setup Guide

## 1. Pre-Build Modifications

### 1.1 Install Required Tools

Run the following commands to install necessary tools:

```bash
sudo apt-get install make
sudo apt-get install vim # Code Editor
sudo apt-get install python
sudo apt-get install python-pip
sudo apt-get install git
sudo apt-get install minicom
```


### 1.2 Download and Install the Toolchain

wget https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi.tar.xz

**GNU Arm Embedded Toolchain (GCC-ARM):** A popular choice for ARM-based microcontrollers.

**Export the Toolchain Path:** After installation, add the toolchain's directory to your system's PATH environment variable. This ensures that the build commands can locate the necessary tools.

For Unix-based systems (Linux, etc):

```bash
export PATH="/home/sp/toolchain/gnu_arm/10.3_2021.10/bin:$PATH"
```


### 1.3 Download the viot-smart-campus Repository

Before proceeding, ensure Git is installed. If not, download and install it from the official Git website.

```bash
sudo apt install git
```

To download the code for the viot-smart-campus project:

```bash
git@github.com:vandyam/viot-smart-campus.git
```


## 2. Build Environment

This project uses a combination of Make and Bash scripting.

**Important Note:**

Before flashing (burning the code), ensure the USB cable is connected and follow the steps to put the device into download mode:

1. Press and hold the **IO2 Button**.
2. Press the **RST (Reset) Button** while holding IO2.
3. Release the **RST Button** first, then IO2.

## 3. Build Process Steps and Flashing Procedure

### 3.1 Build Process

1. Navigate to the project's utils directory:

```bash
cd /path/to/your/viot-smart-campus/vandyam
```

2. Execute the build script:

```bash
make clean
make
```


3. Flash the device info blob:

```bash
python3 ../sdk/ra08h//build/scripts/tremo_loader.py -p /dev/ttyUSB0 -b 921600 flash 0x08000000 out/vandyam.bin
```
