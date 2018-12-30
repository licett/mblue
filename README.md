# MBlue
MBlue is a light-weight embedded framework. It is designed to help embedde developers to write portable bussiness code and reduce complexity of their projects. 

# Motivation
It is relatively common to utilize a embedded OS(such as FreeRTOs, μC/OS) in a IOT or other smart-hardware projects. 
And it is also a good way to make sure that core operations of our system are splitted into different tasks with different priorities becase we know that if our system is well organized it is less likely for us to meet strange problems in final part of our development cycle such as system hang, multi-thread issue.
Unfortunately, when complexity of our projects grows, we usually found it painful to handle plenty of asynchronous messages(timers, semaphores, mailbox and etc.), which leads to cumbersome and unmaintainable code. 
So I think if I can build a framework to manage these boring relation between different tasks so that we can write clean and portable bussiness code.
Then MBlue was born.

# Features
* Portable lowlevel operations(including threads, memory, semaphore and etc).
* SUB/PUB bus.
* IPC.

# Quick Start
The quickest way to get started with Mblue is to clone the project and run the demo in a Linux.

Install development tools:
```bash
$ sudo apt install build-essential cmake
```
Compile MBlue with cmake(make sure you are in the root directory of MBlue):
```bash
$ cmake .
$ make
$ ./mblue
```
You will see that a simple demo is running with a 1s timers.

