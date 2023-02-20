#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "head.h"

class AppConfig
{
public:
    static QString ConfigFile;          //配置文件路径

    //Oil Test
    static QString coXPos;  //除油位置X坐标
    static QString coYPos;  //除油位置Y坐标
    static QString coZPos;  //除油位置Z坐标
    static QString doXPos;  //滴油位置X坐标
    static QString doYPos;  //滴油位置Y坐标
    static QString doZPos;  //滴油位置Z坐标

    //FourPoint Test
    static QString fourStPosX;  //起点
    static QString fourStPosY;  //起点
    static QString fourToPosX;  //对角点
    static QString fourToPosY;  //对角点
    static QString fourPointH;  //测试高度
    static QString fourPointT;  //单点时长
    static QString fourPointTurnT;  //换点间隔

    //读写配置参数
    static void readConfig();           //读取配置参数
    static void writeConfig();          //写入配置参数
};

#endif // APPCONFIG_H
