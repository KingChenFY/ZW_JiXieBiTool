#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "head.h"

class AppConfig
{
public:
    static QString ConfigFile;          //配置文件路径
    static QString SendFileName;        //发送配置文件名
    static QString DeviceFileName;      //模拟设备数据文件名

    static float coXPos;  //除油位置X坐标
    static float coYPos;  //除油位置Y坐标
    static float coZPos;  //除油位置Z坐标

    static float doXPos;  //滴油位置X坐标
    static float doYPos;  //滴油位置Y坐标
    static float doZPos;  //滴油位置Z坐标

    //读写配置参数
    static void readConfig();           //读取配置参数
    static void writeConfig();          //写入配置参数
};

#endif // APPCONFIG_H
