#ifndef ACTIONPARAMETERSET_H
#define ACTIONPARAMETERSET_H

#include "actionmotorxyz.h"

typedef struct
{
    emMotorObj emAimAxis;
    uint16_t u16TrigDistance;
    uint8_t u8TrigObj;
}ST_TRIG_PARAMETER_INFO;

class ActionParameterSet : public QObject, public HardCmdParser
{
    Q_OBJECT
public:
    explicit ActionParameterSet(QObject *parent = nullptr);

    ST_TRIG_PARAMETER_INFO m_stTrigInfoToSend;
    ST_TRIG_PARAMETER_INFO m_stTrigInfoD;
    emWKCmdType parseCmd(uint8_t* puData);//回复解析
    void setTrigParameterSend();//封装触发参数设置指令到发送队列里
    void getTrigParameterSend();

signals:

};

#endif // ACTIONPARAMETERSET_H
