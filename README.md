# MBlue
MBlue is a light-weight embedded framework. It is designed to help embedde developers to write portable bussiness code and reduce complexity of their projects. 

# Features:
* Portable lowlevel operations(including threads, memory, semaphore and etc).
* SUB/PUB bus.
* IPC.

# Quick Start
The quickest way to get started with mblue is to clone the project and run the demo in a Linux:
Install development tools:
```bash
$ sudo apt install build-essential cmake
```
Compile MBlue with cmake(make sure you are in the root directory of MBlue)
```bash
$ cmake .
$ make
$ ./mblue
```
You will see that a simple demo is running with a 1s timers

