#include "appconfig.h"
#include "quihelper.h"

QString AppConfig::ConfigFile = "config.ini";

QString AppConfig::coXPos = "2";
QString AppConfig::coYPos = "100";
QString AppConfig::coZPos = "500";

QString AppConfig::doXPos = "300";
QString AppConfig::doYPos = "200";
QString AppConfig::doZPos = "100";


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
