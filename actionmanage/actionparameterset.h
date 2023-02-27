#ifndef ACTIONPARAMETERSET_H
#define ACTIONPARAMETERSET_H

#include "actionmotorxyz.h"

#define TRAIL_SYN_MAX_NUM    10
#define DEFAULT_SLIDE_MAX_SUM   120
#define SmearCoverFlagTurnOn 0x71 // 激光扫玻片的遮挡信号，未遮挡转遮挡
#define SmearCoverFlagTurnOff 0x70 // 激光扫玻片的遮挡信号，遮挡转未遮挡
#define SmearCoverRecordMaxNum 200 // 激光扫玻片的遮挡信号记录数量值获取
#define SmearBoxColIdWait 1 // 等待列号
#define SmearSlidePosNull 0x7FFFFFFF
#define SmearBoxColIdFinish 2 // 完成列号

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

typedef struct
{
    uint8_t u8AskNum;
    uint8_t u8BgnIdx;
}ST_MOTORV_SLIDEINFO_SET;

typedef struct{
    int32_t m_i32pos;//位置坐标
    uint8_t m_u8ztoo;//从有到无位2，从无到有为1
    uint8_t m_u8insex;//从左向右编号1-2
}ST_MOTORV_SLIDE_POS;

typedef struct
{
    int32_t m_i32pos;//位置坐标
    uint8_t m_u8ztoo;//从有到无位2，从无到有为1
}ST_MOTORV_SLIDE_CAC;

typedef struct
{
    uint8_t u8AskNum;
    uint8_t u8BgnIdx;
    uint8_t u8TotalNum; //硬件实际扫描得到的总上下沿数量
    uint8_t u8GetNum;//查询得到的总上下沿数量
    ST_MOTORV_SLIDE_POS stSlidePos[DEFAULT_SLIDE_MAX_SUM];
}ST_MOTORV_SLIDEINFO_D;

class ActionParameterSet : public QObject, public HardCmdParser
{
    Q_OBJECT
public:
    explicit ActionParameterSet(QObject *parent = nullptr);

    ST_TRAIL_PARAMETER_INFO m_stTrailInfoToSend;
    ST_TRAIL_PARAMETER_INFO_D m_stTrailInfoGetD;
    bool m_bISTrailInfoSet;

    emWKCmdType parseCmd(uint8_t* puData);//回复解析
    bool setTaskCmdReSend(uint32_t sdNum);//重发当前模块的设置任务指令
    void setFollowParameterSend();//设置跟随轨迹的系数


    ST_MOTORV_SLIDEINFO_SET m_stMotorVGetSlideInfoSet;
    void getMotorVScanInfo();//获取垂直扫描的玻片信息
    void getMotorVSlideData(ST_MOTORV_SLIDEINFO_D &stData);

    void setMotorVLaser();
    uint8_t m_u8IsOpenLaser;
    uint8_t m_u8IsOpenLaserD;

private:

    ST_MOTORV_SLIDEINFO_D m_stMotorVGetSlideInfoD;
signals:

    void signal_SlideInfoUiUpdate();
    void signal_MVLaserUiUpdate(uint8_t isSucceed);

};

#endif // ACTIONPARAMETERSET_H
