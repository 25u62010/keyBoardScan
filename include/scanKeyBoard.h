#ifndef _SCANKEYBOARD_
#define _SCANKEYBOARD_

#include "iostream"
#include "ros/ros.h"
#include <termio.h>
#include <stdio.h> 
#include <thread>
#include <mutex>
#include <unordered_map>
#include <linux/input.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 

#define DEV_PATH "/dev/input/event1"   //difference is possible

#define keyCodeW 17
#define keyCodeA 30
#define keyCodeS 31
#define keyCodeD 32
#define keyCodeQ 16
#define keyCodeE 18
#define keyCodeEsc 1
#define keyStatePressed true
#define keyStateReleased false
using namespace std;

class scanKeyBoard{
private:
    int keys_fd;
    unordered_map<unsigned char,bool> mKeyBoardState;
    mutex mLock;
    bool mbFinishFlag=false;
    inline void checkStateAndPrint(unsigned char keyBoardCode);
    void updateScreen();
public:
    scanKeyBoard();
    bool readKeyState(unsigned char keyCode);
    bool isFinished();
    int run();
};
#endif
