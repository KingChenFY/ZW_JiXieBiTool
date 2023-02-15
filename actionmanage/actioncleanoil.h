#ifndef ACTIONCLEANOIL_H
#define ACTIONCLEANOIL_H

#include <QObject>
#include <QTimer>
#include <QDebug>

#include "hardcmdparseagent.h"
#include "harddef.h"
#include "hardselutedef.h"

typedef struct
{
    emTaskDCleanOilType m_eTaskType;//任务类型
    uint8_t m_uTaskId;//任务Id
}ST_CLEANOIL_SETTASK_INFO;

typedef struct
{
    emTaskDCleanOilType m_eTaskTypeD;   //任务类型
    emWorkStatus m_eTaskStatusD;        //任务状态
    emSeluteDCleanOil m_eTaskSeluteD;   //任务结论
    uint8_t m_uTaskId;                  //任务Id
}ST_CLEANOIL_GETTASK_INFO;

class ActionCleanOil : public QObject, public HardCmdParser
{
    Q_OBJECT
public:
    explicit ActionCleanOil(QObject *parent = nullptr);
    ~ActionCleanOil(){};

    // 除油任务执行结果定义
    enum E_SELUTE_CLEANOIL {
        start,

        //动作结论
        NoErr,//正常
        Outtime,//超时(动作次数)
        DCfgErr,//D层配置信息无效
        C2DDataErr,//C->D传入参数错误
        //任务结论
        NoLink,// 无法确认除油设备
        HardwareErr,//除油硬件错误
        EnvErr,
        SwitchL1Err,
        SwitchL2Err,
        NoPaper,

        end
    };
    Q_ENUM(E_SELUTE_CLEANOIL)

    emWKCmdType parseCmd(uint8_t* puData);//回复解析
    bool setTaskCmdReSend(uint32_t sdNum);//重发当前模块的设置任务指令

    static uint32_t u32MaxReadNum_to_reSend;
    ST_CLEANOIL_SETTASK_INFO m_stTaskToSend;//准备设置的任务信息/之前设置的任务信息
    //下层获取信息
    ST_CLEANOIL_GETTASK_INFO m_stTaskD;//任务信息

private:
    void getTaskSend();//封装查询指令到链表
    //自动重发定时器
    static int timeOutValue_setTaskReSend;
    QTimer timer_setTaskReSend;

public slots:
    void setTaskSend();//封装页面的设置指令入链表
private slots:
    void slot_timer_setTaskReSend();

signals:
    void signal_UiUpdate();
    void signal_SetTask_ReSend();
};

#endif // ACTIONCLEANOIL_H
