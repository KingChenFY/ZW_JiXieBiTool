#ifndef ACTIONBTRANSPORT_H
#define ACTIONBTRANSPORT_H

#include <QObject>
#include <QThread>
#include "actionpoleout.h"
#include "hardcmdparseagent.h"
#include "harddef.h"
#include "actionbeltin.h"
#include "actionbeltout.h"
#include "actionpolein.h"
#include "actionbeltout.h"
#include "actionupender.h"
#include "actionmotorv.h"

class ActionBTransport : public QThread, public HardCmdParser
{
    Q_OBJECT
public:
    explicit ActionBTransport(ActionBeltIn* &objActionBeltIn, ActionBeltOut* &objActionBeltOut,
                              ActionPoleIn* &objActionPoleIn, ActionPoleOut* &m_objActionPoleOut,
                              ActionUpender* &objActionUpender, ActionMotorV* &objActionMotorV, QObject *parent = nullptr);

    emWKCmdType parseCmd(uint8_t* puData);//回复解析
    bool setTaskCmdReSend(uint32_t sdNum);

    bool m_bNeedStop;
    //*********************************信号信息*******************************************
    bool m_bIsBoard2Connect;
    emSensorStatus m_eListAllSensorStatus[emSensorId_End];

protected:
    void run();

private:
    ActionBeltIn* &m_objActionBeltIn;
    ActionBeltOut* &m_objActionBeltOut;
    ActionPoleIn* &m_objActionPoleIn;
    ActionPoleOut* &m_objActionPoleOut;
    ActionUpender* &m_objActionUpender;
    ActionMotorV* &m_objActionMotorV;

signals:

};

#endif // ACTIONBTRANSPORT_H
