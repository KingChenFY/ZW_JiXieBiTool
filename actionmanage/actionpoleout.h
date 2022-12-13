#ifndef ACTIONPOLEOUT_H
#define ACTIONPOLEOUT_H

#include <QObject>
#include <QDebug>
#include <QTimer>

#include "hardcmdparseagent.h"
#include "harddef.h"
#include "hardselutedef.h"

typedef struct
{
    emTaskDPoleOutType m_eTaskType;
    uint8_t m_uTaskId;
}ST_POLEOUT_SETTASK_INFO;

class ActionPoleOut :  public QObject, public HardCmdParser
{
    Q_OBJECT
public:
    explicit ActionPoleOut(QObject *parent = 0);
    ~ActionPoleOut(){};
    // 出仓推杆任务执行结果定义
    enum E_SELUTE_POLEOUT {
        start,

        //动作结论
        NoErr,//正常
        Outtime,//超时(动作次数)
        DCfgErr,//D层配置信息无效
        C2DDataErr,//C->D传入参数错误
        //任务结论
        EnvErr,//环境异常
        HardwareErr,//推杆硬件错误(长时间未移动)
        SwitchP1Err,//起点限位开关P1异常
        SwitchP2Err,//终点限位开关P2异常
        SwitchP1_P2Err,//开关信号异常（P1、P2同时遮挡）
        NoBox,//无盒子存在
        FullBox,//盒子已满
        PoleInPosErr,//入推杆位置异常
        SwitchL1Err,//起点限位开关L1异常
        SwitchL2Err,//终点限位开关L2异常
        SwitchL1_L2Err,//开关信号异常（L1、L2同时遮挡）
        PoleOutPosErr,//出推杆位置异常
        Disconnect,//板2失去连接（未知的位置信息）

        end
    };
    Q_ENUM(E_SELUTE_POLEOUT)

    // 翻转装置，推杆动作位置枚举
    enum emPositionType
    {
        emPosition_start,
        emPosition_One,
        emPosition_Two,
        emPosition_Three,
        emPosition_Four,
        emPosition_Five,
        emPosition_end
    };

    emWKCmdType parseCmd(uint8_t* puData);//回复解析
    bool setTaskCmdReSend(uint32_t sdNum);//重发当前模块的设置任务指令

    static uint32_t u32MaxReadNum_to_reSend;
    ST_POLEOUT_SETTASK_INFO m_stTaskToSend;//准备设置的任务信息/之前设置的任务信息
    //下层获取信息
    emWorkStatus m_eTaskStatusD;// 任务状态
    emSeluteDPole m_eTaskSeluteD;// 任务结论

private:
    void getTaskSend();//封装查询指令到链表
    //自动重发定时器，用于settask指令未回复的情况，重发settask
    static int timeOutValue_setTaskReSend;
    QTimer timer_setTaskReSend;
    //下层获取信息
    ST_POLEOUT_SETTASK_INFO m_stTaskD;//任务信息
    emPositionType m_eCurPos;//当前位置
    uint8_t m_uEmptyLeft;//剩余空位

public slots:
    void setTaskSend();//封装页面的设置指令入链表
private slots:
    void slot_timer_setTaskReSend();

signals:
    void signal_UiUpdate();
    void signal_SetTask_ReSend();// 关联到setTaskSend，用于getstatus和获取不到任务finish重发数据

};

#endif // ACTIONPOLEOUT_H
