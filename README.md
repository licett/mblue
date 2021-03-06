# MBlue
MBlue is a light-weight embedded framework. 
It is designed to help embedded developers to write portable bussiness code and reduce complexity of their projecs. 

# Features
* Portable lowlevel operations(including threads, memory, semaphore and etc).
* SUB/PUB bus.
* Synchronized and asynchronous Inter-task call.

# Current Status
MBlue is now fairly stable and is running on several products on sales such as Motor trackers, AI mouse and Earphones.

# Quick Start
The quickest way to get started with Mblue is to clone the project and run the sample on a Linux.

Install development tools:
```bash
$ sudo apt install build-essential gcc-multilib
```
You need to install cmake by downloading the lasteast release from [CMAKE](https://cmake.org/download/).

Then compile MBlue with cmake(make sure you are in the root directory of MBlue):
```bash
$ mkdir build && cd build && cmake -DCMAKE_TOOLCHAIN_FILE=../linux_m32.cmake ..
$ make
$ ./mblue
```
You will see that the sample is running with a timer fired once per second.
Please refer to [FAQ](https://github.com/licett/mblue/wiki#faq) about what this sample tries to demonstrate.

# Learn More
More infomation can be found at [wiki](https://github.com/licett/mblue/wiki).

# License
MBlue is under the MIT license. See the [LICENSE](https://github.com/licett/mblue/blob/master/LICENSE) file for details.

# The End
If you are interested in MBlue, don't forget to STAR it.

MBlue's mission is MAKING EMBEDDED SIMPLE!

Thank you for reading.
