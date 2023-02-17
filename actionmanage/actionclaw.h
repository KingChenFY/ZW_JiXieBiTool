#ifndef ACTIONCLAW_H
#define ACTIONCLAW_H

#include <QObject>
#include <QTimer>
#include <QDebug>

#include "hardcmdparseagent.h"
#include "harddef.h"
#include "hardselutedef.h"

typedef enum
{
    emSlide_No = 0,
    emSlide_In = 1,
}emSlideState;

typedef struct
{
    emTaskDGripperType m_eTaskType;//任务类型
    uint8_t m_uTaskId;//任务Id
    uint16_t m_AimPos;//目标坐标位置
    uint8_t m_u8IsSlideIn;//上电有无玻片
    uint8_t m_u8IsBrake;//是否抱闸
}ST_CLAW_SETTASK_INFO;

typedef struct
{
    emTaskDGripperType m_eTaskTypeD;    //任务类型
    uint8_t m_uTaskId;                  //任务Id
    emWorkStatus m_eTaskStatusD;        //任务状态
    emSeluteDGripper m_eTaskSeluteD;   //任务结论
    uint8_t m_u8Dir;
    uint16_t m_u16AimPosD;
    int32_t m_i32CurPosD;
    uint16_t m_u16MaxOpenPos;
    uint8_t m_u8IsSynSlide;
    uint8_t m_u8IsSlideIn;
}ST_CLAW_GETTASK_INFO;

class ActionClaw : public QObject, public HardCmdParser
{
    Q_OBJECT
public:
    explicit ActionClaw(QObject *parent = nullptr);
    ~ActionClaw(){};

    enum E_SELUTE_CLAW
    {
        start,
        NoErr,//正常
        Outtime,//超时(动作次数)

        DCfgErr,//配置参数错误
        C2DDataErr,//C->D传入参数错误
        GripperErr,//夹爪异常
        SwitchErr,//行程开关异常
        EdgeErr,//边界异常
        //emSeluteDGripper_EnvErr,//环境异常
        NoLink,//无法确认设备
        end
    };
    Q_ENUM(E_SELUTE_CLAW)

    emWKCmdType parseCmd(uint8_t* puData);//回复解析
    bool setTaskCmdReSend(uint32_t sdNum);//重发当前模块的设置任务指令

    static uint32_t u32MaxReadNum_to_reSend;
    ST_CLAW_SETTASK_INFO m_stTaskToSend;//准备设置的任务信息/之前设置的任务信息
    //下层获取信息
    ST_CLAW_GETTASK_INFO m_stDTaskInfo;

    bool isSynStatus;
    bool moveEnvironmentCheck();
    bool movePosIsValid(uint16_t &m_u16Pos);//移动位置有效性检查

private:
    void getTaskSend();//封装查询指令到链表
    //自动重发定时器，用于settask指令未回复的情况，重发settask
    static int timeOutValue_setTaskReSend;
    QTimer timer_setTaskReSend;

public slots:
    void setTaskSend();//封装页面的设置指令入链表
private slots:
    void slot_timer_setTaskReSend();

signals:
    void signal_UiUpdate();
    void signal_SetTask_ReSend();// 关联到setTaskSend，用于getstatus和获取不到任务finish重发数据
};

#endif // ACTIONCLAW_H
