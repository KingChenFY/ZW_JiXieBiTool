#ifndef ACTIONDATAMANAGE_H
#define ACTIONDATAMANAGE_H

#include <QObject>
#include "hardcmdparseagent.h"

class ActionDataManage : public QObject
{
    Q_OBJECT
public:
    explicit ActionDataManage(QObject *parent = nullptr);
    emWKCmdType parseCmd(uint8_t* puData);//回复解析

    void slot_initDataMange();
    void slot_netRecvData();


signals:


};

#endif // ACTIONDATAMANAGE_H
