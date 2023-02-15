#include "appconfig.h"
#include "quihelper.h"

QString AppConfig::ConfigFile = "config.ini";
QString AppConfig::SendFileName = "send.txt";
QString AppConfig::DeviceFileName = "device.txt";

float AppConfig::coXPos = 0;
float AppConfig::coYPos = 0;
float AppConfig::coZPos = 0;

float AppConfig::doXPos = 0;
float AppConfig::doYPos = 0;
float AppConfig::doZPos = 0;


void AppConfig::readConfig()
{
    QSettings set(AppConfig::ConfigFile, QSettings::IniFormat);

    set.beginGroup("OilConfig");
    AppConfig::coXPos = set.value("coXPos", AppConfig::coXPos).toFloat();
    AppConfig::coYPos = set.value("coYPos", AppConfig::coYPos).toFloat();
    AppConfig::coZPos = set.value("coZPos", AppConfig::coZPos).toFloat();

    AppConfig::doXPos = set.value("doXPos", AppConfig::doXPos).toFloat();
    AppConfig::doYPos = set.value("doYPos", AppConfig::doYPos).toFloat();
    AppConfig::doZPos = set.value("doZPos", AppConfig::doZPos).toFloat();
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

}
