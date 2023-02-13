#ifndef ACTIONPARAMETERSET_H
#define ACTIONPARAMETERSET_H

#include "actionmotorxyz.h"

#define TRAIL_SYN_MAX_NUM    10
typedef struct
{
    emMotorObj emAimAxis;
    uint16_t u16TrigDistance;
    uint8_t u8TrigObj;
}ST_TRIG_PARAMETER_INFO;

typedef struct
{
    uint16_t u16TrailId;
    uint16_t u16TrailSynNum;
    double udSyn[TRAIL_SYN_MAX_NUM];
}ST_TRAIL_PARAMETER_INFO;

typedef struct
{
    bool ubIsSuccess;
    uint16_t u16TrailId;
    uint16_t u16TrailSynNum;
    double udSyn[TRAIL_SYN_MAX_NUM];
}ST_TRAIL_PARAMETER_INFO_D;

class ActionParameterSet : public QObject, public HardCmdParser
{
    Q_OBJECT
public:
    explicit ActionParameterSet(QObject *parent = nullptr);

    ST_TRIG_PARAMETER_INFO m_stTrigInfoToSend;
    ST_TRIG_PARAMETER_INFO m_stTrigInfoD;
    ST_TRAIL_PARAMETER_INFO m_stTrailInfoToSend;
    ST_TRAIL_PARAMETER_INFO_D m_stTrailInfoGetD;
    bool m_bISTrailInfoSet;

    emWKCmdType parseCmd(uint8_t* puData);//回复解析
    bool setTaskCmdReSend(uint32_t sdNum);//重发当前模块的设置任务指令
    void setTrigParameterSend();//封装触发参数设置指令到发送队列里
    void getTrigParameterSend();
    void setFollowParameterSend();//设置跟随轨迹的系数

signals:

};

#endif // ACTIONPARAMETERSET_H
