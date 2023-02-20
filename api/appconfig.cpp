#include "appconfig.h"
#include "quihelper.h"

QString AppConfig::ConfigFile = "config.ini";

QString AppConfig::coXPos = "2";
QString AppConfig::coYPos = "100";
QString AppConfig::coZPos = "500";

QString AppConfig::doXPos = "300";
QString AppConfig::doYPos = "200";
QString AppConfig::doZPos = "100";


QString AppConfig::fourStPosX = "1111";  //起点
QString AppConfig::fourStPosY = "1111";  //起点
QString AppConfig::fourToPosX = "1111";  //对角点
QString AppConfig::fourToPosY = "1111";  //对角点
QString AppConfig::fourPointH = "1111";  //测试高度
QString AppConfig::fourPointT = "1111";  //单点时长
QString AppConfig::fourPointTurnT = "1111";  //换点间隔

void AppConfig::readConfig()
{
    QSettings set(AppConfig::ConfigFile, QSettings::IniFormat);

    set.beginGroup("OilConfig");
    AppConfig::coXPos = set.value("coXPos", AppConfig::coXPos).toString();
    AppConfig::coYPos = set.value("coYPos", AppConfig::coYPos).toString();
    AppConfig::coZPos = set.value("coZPos", AppConfig::coZPos).toString();

    AppConfig::doXPos = set.value("doXPos", AppConfig::doXPos).toString();
    AppConfig::doYPos = set.value("doYPos", AppConfig::doYPos).toString();
    AppConfig::doZPos = set.value("doZPos", AppConfig::doZPos).toString();
    set.endGroup();

    set.beginGroup("FourPointConfig");
    AppConfig::fourStPosX = set.value("fourStPosX", AppConfig::fourStPosX).toString();
    AppConfig::fourStPosY = set.value("fourStPosY", AppConfig::fourStPosY).toString();
    AppConfig::fourToPosX = set.value("fourToPosX", AppConfig::fourToPosX).toString();
    AppConfig::fourToPosY = set.value("fourToPosY", AppConfig::fourToPosY).toString();
    AppConfig::fourPointH = set.value("fourPointH", AppConfig::fourPointH).toString();
    AppConfig::fourPointT = set.value("fourPointT", AppConfig::fourPointT).toString();
    AppConfig::fourPointTurnT = set.value("fourPointTurnT", AppConfig::fourPointTurnT).toString();
    set.endGroup();

    //配置文件不存在或者不全则重新生成
    if (!QUIHelper::checkIniFile(AppConfig::ConfigFile)) {
        writeConfig();
        return;
    }
}

void AppConfig::writeConfig()
{    
    QSettings set(AppConfig::ConfigFile, QSettings::IniFormat);

    set.beginGroup("OilConfig");
    set.setValue("coXPos", AppConfig::coXPos);
    set.setValue("coYPos", AppConfig::coYPos);
    set.setValue("coZPos", AppConfig::coZPos);

    set.setValue("doXPos", AppConfig::doXPos);
    set.setValue("doYPos", AppConfig::doYPos);
    set.setValue("doZPos", AppConfig::doZPos);
    set.endGroup();

    set.beginGroup("FourPointConfig");
    set.setValue("fourStPosX", AppConfig::fourStPosX);
    set.setValue("fourStPosY", AppConfig::fourStPosY);
    set.setValue("fourToPosX", AppConfig::fourToPosX);
    set.setValue("fourToPosY", AppConfig::fourToPosY);
    set.setValue("fourPointH", AppConfig::fourPointH);
    set.setValue("fourPointT", AppConfig::fourPointT);
    set.setValue("fourPointTurnT", AppConfig::fourPointTurnT);
    set.endGroup();
}
