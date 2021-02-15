#include "scanKeyBoard.h"

scanKeyBoard::scanKeyBoard(){
    char ret[2];    
    keys_fd=open(DEV_PATH, O_RDONLY);
    if(keys_fd <= 0){
	    printf("open /dev/input/event0 device error!\n");
	    return;
    }
    mKeyBoardState={
        {keyCodeW,false},
        {keyCodeA,false},
        {keyCodeS,false},
        {keyCodeD,false},
        {keyCodeQ,false},
        {keyCodeE,false}
        
    };
}
bool scanKeyBoard::readKeyState(unsigned char keyCode){
    unique_lock<mutex> lock(mLock);
    return mKeyBoardState[keyCode];
}
bool scanKeyBoard::isFinished(){
    unique_lock<mutex> readLock(mLock);
    return mbFinishFlag;
}
int scanKeyBoard::run(){
    struct input_event t;
    updateScreen();
    while(ros::ok()){     
	    if(read(keys_fd, &t, sizeof(t)) == sizeof(t)){
	        updateScreen();
	        if(t.type==EV_KEY&&(t.value==0||t.value==1)){
	            bool keyState=t.value?true:false;
	            bool escFlag=false;
	            {
	            unique_lock<mutex> readLock(mLock);
	            switch(t.code){
	                case keyCodeW:
	                case keyCodeA:
	                case keyCodeS:
	                case keyCodeD:
	                case keyCodeQ:
	                case keyCodeE:
	                    mKeyBoardState[t.code]=keyState;
	                break;
	                case keyCodeEsc:
	                    escFlag=true;
	                break; 
	            }		            
	            }//mlock
	            if(escFlag){
	                system ("clear");
	                printf("safe quit!\n");
	                break;
	            }		            
	        }	    
	    }
	    
    }
    close(keys_fd);
    {
    unique_lock<mutex> readLock(mLock);
    mbFinishFlag=true;
    }
    return 0;
}
void scanKeyBoard::updateScreen(){
    system ("clear");
    checkStateAndPrint(keyCodeQ);printf("Q  ");checkStateAndPrint(keyCodeW);printf("  W  ");checkStateAndPrint(keyCodeE);printf("  E  \n\n");
    checkStateAndPrint(keyCodeA);printf("  A  ");checkStateAndPrint(keyCodeS);printf("  S  ");checkStateAndPrint(keyCodeD);printf("  D  \n\n");
    printf("\033[0m");
    printf("\033[31mEnter ESC quit!\033[0m\n");
} 
void scanKeyBoard::checkStateAndPrint(unsigned char keyBoardCode){
    unique_lock<mutex> lock(mLock);
    if(mKeyBoardState[keyBoardCode]==true){
        printf("\033[1;44m");
    }
    else{
        printf("\033[0m");
    }
}
