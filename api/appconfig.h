#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "head.h"

class AppConfig
{
public:
    static QString ConfigFile;          //配置文件路径

    static QString coXPos;  //除油位置X坐标
    static QString coYPos;  //除油位置Y坐标
    static QString coZPos;  //除油位置Z坐标

    static QString doXPos;  //滴油位置X坐标
    static QString doYPos;  //滴油位置Y坐标
    static QString doZPos;  //滴油位置Z坐标

    //读写配置参数
    static void readConfig();           //读取配置参数
    static void writeConfig();          //写入配置参数
};

#endif // APPCONFIG_H
