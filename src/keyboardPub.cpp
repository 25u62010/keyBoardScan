#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "iostream"
#include <opencv2/core/core.hpp>
#include "scanKeyBoard.h"
using namespace std;

struct zVector3f{
    float x;
    float y;
    float z;
    zVector3f(float _x,float _y,float _z):x(_x),y(_y),z(_z){}
};
int main(int argc,char** argv){
    if(argc<2){
        cout<<"please input path of config file"<<endl;
        return -1;
    }
    const string configFile=argv[1];
    cv::FileStorage fSettings(configFile.c_str(), cv::FileStorage::READ);
    if(!fSettings.isOpened()){
        cout<<"cannot open config file"<<configFile<<endl;
    }
    
    ros::init(argc,argv,"keyboardPub_node");  
    ros::NodeHandle handle;
    ros::Publisher twistCmd=handle.advertise<geometry_msgs::Twist>("/robot0/cmd_vel", 10);
    geometry_msgs::Twist twistVal;
    ros::Rate loopRate(50);
    scanKeyBoard keyBoard;
    thread keyBoardScanThread(&scanKeyBoard::run,&keyBoard);
    
    zVector3f acc(0.1,0.1,0);
    acc.x=fSettings["acc.x"];
    acc.y=fSettings["acc.x"];
    acc.z=fSettings["acc.x"];
    zVector3f maxSpeed(5.0,5.0,0);
    maxSpeed.x=fSettings["maxSpeed.x"];
    maxSpeed.y=fSettings["maxSpeed.x"];
    maxSpeed.z=fSettings["maxSpeed.x"];
    zVector3f angleAcc(0,0,0.1);
    angleAcc.x=fSettings["angleAcc.x"];
    angleAcc.y=fSettings["angleAcc.y"];
    angleAcc.z=fSettings["angleAcc.z"];
    zVector3f maxAngleSpeed(0,0,2.0);
    maxAngleSpeed.x=fSettings["maxAngleSpeed.x"];
    maxAngleSpeed.y=fSettings["maxAngleSpeed.y"];
    maxAngleSpeed.z=fSettings["maxAngleSpeed.z"];
    int omnidirectional=fSettings["omnidirectional"];
    while(ros::ok()){
        if(keyBoard.isFinished()){
            
            break;
        }
        if(keyBoard.readKeyState(keyCodeW)){
            twistVal.linear.x+=acc.x;
            twistVal.linear.x=min(maxSpeed.x,float(twistVal.linear.x));
        }
        else if(keyBoard.readKeyState(keyCodeS)){
            twistVal.linear.x-=acc.x;
            twistVal.linear.x=max(-maxSpeed.x,float(twistVal.linear.x));
        }
        else{
            twistVal.linear.x=0;
        }
        if(keyBoard.readKeyState(keyCodeA)){
            if(omnidirectional){
                twistVal.linear.y+=acc.x;
                twistVal.linear.y=min(maxSpeed.y,float(twistVal.linear.y));
            }
            else{
                twistVal.angular.z+=angleAcc.z;
                twistVal.angular.z=min(maxAngleSpeed.z,float(twistVal.angular.z));
            }
        }
        else if(keyBoard.readKeyState(keyCodeD)){
            if(omnidirectional){
                twistVal.linear.y-=acc.y;
                twistVal.linear.y=max(-maxSpeed.y,float(twistVal.linear.y));
            }
            else{
                twistVal.angular.z-=angleAcc.z;
                twistVal.angular.z=max(-maxAngleSpeed.z,float(twistVal.angular.z));
            }
            
        }
        else{
            if(omnidirectional){
                twistVal.linear.y=0;
            }
            else{
                twistVal.angular.z=0;
            }
        }
        if(omnidirectional&&keyBoard.readKeyState(keyCodeQ)){
            twistVal.angular.z+=angleAcc.z;
            twistVal.angular.z=min(maxAngleSpeed.z,float(twistVal.angular.z));
        }
        else if(omnidirectional&&keyBoard.readKeyState(keyCodeE)){
            twistVal.angular.z-=angleAcc.z;
            twistVal.angular.z=max(-maxAngleSpeed.z,float(twistVal.angular.z));
        }
        else if(omnidirectional){
            twistVal.angular.z=0;
        }
        twistCmd.publish(twistVal);
        ros::spinOnce();
        loopRate.sleep();
    }
    
    return 0;
}
