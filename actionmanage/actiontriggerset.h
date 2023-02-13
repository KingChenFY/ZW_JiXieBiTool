#ifndef ACTIONTRIGGERSET_H
#define ACTIONTRIGGERSET_H

#include <QObject>
#include <QDebug>
#include <QTimer>

#include "cejutcpclient.h"
#include "hardcmdparseagent.h"
#include "harddef.h"
#include "hardselutedef.h"

#define GET_TRIGPOS_PER_MAX 10
#define FOLLOW_AXIS_NUM     2

typedef struct
{
    emAxisType m_eAxis;//目标轴
    uint16_t m_u16Interval;//触发间隔
    uint8_t m_u8TrigObj;//触发对象
}ST_TRIG_SETTASK_INFO;

typedef struct
{
    uint16_t m_u16StartIndex;     //当次获取开始位置
    uint8_t m_u8NeedNum;      //需要触发位置的数量
}ST_TRIGINFO_SETTASK_INFO;

typedef struct
{
    uint16_t m_u16StartIndex;     //当次获取开始位置
    uint8_t m_u8NeedNum;      //需要触发位置的数量
    uint8_t m_u8AimAxis;
    int32_t m_i32AimPos;
    int32_t m_i32AimMoveTime;
    uint16_t m_u16TotalNum;     //当前总的触发数量
    uint16_t m_u16RealStartIndex;
    uint16_t m_u16RealGetNum;
    int32_t m_i32TrigPosArray[FOLLOW_AXIS_NUM][WK_CeJuRecordNumMax]; //单条数据包保存的数量
}ST_TRIGINFO_GETTASK_INFO;

typedef struct
{
    uint16_t m_u16NeedTrigPosNum;   //B层需要的触发位置的总量，实际不一定有那么多
    uint16_t m_u16factGetPosNum;    //B层实际获得的触发总量
    bool m_bIsGetAllNeedNum;        //是否已获得要求的触发位置量
}ST_BFollowSetParameter;

class ActionTriggerSet : public QObject, public HardCmdParser
{
    Q_OBJECT
public:
    explicit ActionTriggerSet(QObject *parent = nullptr);

    emWKCmdType parseCmd(uint8_t* puData);//回复解析
    bool setTaskCmdReSend(uint32_t sdNum);//重发当前模块的设置任务指令
    ST_TRIG_SETTASK_INFO m_stTaskToSend;//准备设置的任务信息/之前设置的任务信息
    ST_TRIG_SETTASK_INFO m_stTaskInfoD;//获取到的D层任务信息
    bool m_bStatusSyn;//D层信息是否已同步

    ST_TRIGINFO_SETTASK_INFO m_stTrigInfoGet_TaskToSend;
    ST_TRIGINFO_GETTASK_INFO m_stTrigInfoD;
    ST_BFollowSetParameter m_stBFollowTrigSetParameter;

    void Trig_GetTrigPosData(int32_t destArray[FOLLOW_AXIS_NUM][WK_CeJuRecordNumMax], uint32_t& num);

private:
    //自动重发定时器，用于settask指令未回复的情况，重发settask
    static int timeOutValue_setTaskReSend;
    QTimer timer_setTaskReSend;
    QTimer timer_getTrigInfoTaskReSend;

public slots:
    void setTaskSend();//封装页面的设置指令入链表
    void getTaskSend();//封装查询指令到链表
    void getTrigInfoTaskSend();//获得硬件保存的触发测距的位置
    void slot_stopSetTaskReSendTimer();
    void slot_stopGetTrigInfoTaskReSendTimer();
private slots:
    void slot_timer_setTaskReSend();
    void slot_timer_getTrigInfoTaskReSend();

signals:
    void signal_UiUpdate();
    void signal_getTaskSend();
    void signal_stopSetTaskReSendTimer();
    void signal_stopGetTrigInfoTaskReSendTimer();
    void signal_getTrigInfoSend();
};

#endif // ACTIONTRIGGERSET_H
