#ifndef ACTIONUPENDER_H
#define ACTIONUPENDER_H

#include <QObject>
#include <QDebug>
#include <QTimer>

#include "hardcmdparseagent.h"
#include "harddef.h"
#include "hardselutedef.h"

typedef struct
{
    emTaskDUpenderType m_eTaskType;     //任务类型
    uint8_t m_uTaskId;                  //任务Id
}ST_UPENDER_SETTASK_INFO;

class ActionUpender :  public QObject, public HardCmdParser
{
    Q_OBJECT
public:
    ActionUpender(QObject *parent = 0);
    ~ActionUpender(){};

    // 入(出)舱皮带任务执行结果定义
    enum E_SELUTE_UPENDER {
        emSeluteDUpender_start,
        emSeluteDUpender_NoErr, 			//正常
        emSeluteDUpender_OutTime,			//超时(动作次数)

        emSeluteDUpender_DCfgErr,			//配置参数错误
        emSeluteDUpender_C2DDataErr,		//C->D传入参数错误
        emSeluteDUpender_HardwareErr,		//设备异常，移动了很久也没移动到位置
        emSeluteDUpender_LimitSwitchErr,	//限位开关异常
        emSeluteDUpender_LimitSwitch1Err,	//限位开关S异常
        emSeluteDUpender_LimitSwitch2Err,	//限位开关E异常(S:Start起点 E:End终点)
        emSeluteDUpender_EnvInErr,			//旋转舱旋入环境位置错误（环境不符合）
        emSeluteDUpender_EnvOutErr,			//旋转舱旋出环境位置错误（环境不符合）
        emSeluteDUpender_Disconnect,		//板2失去连接（未知的位置信息）

        emSeluteDUpender_end
    };
    Q_ENUM(E_SELUTE_UPENDER)

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
    ST_UPENDER_SETTASK_INFO m_stTaskToSend;//准备设置的任务信息/之前设置的任务信息
    //下层获取信息
    emWorkStatus m_eTaskStatusD;//任务状态
    E_SELUTE_UPENDER m_eTaskSeluteD;//任务结论

private:
    void getTaskSend();//封装查询指令到链表
    //自动重发定时器，用于settask指令未回复的情况，重发settask
    static int timeOutValue_setTaskReSend;
    QTimer timer_setTaskReSend;
    //下层获取信息
    ST_UPENDER_SETTASK_INFO m_stTaskD;//任务信息
    emPositionType m_eCurPos;//当前位置
    bool m_bIsMoving;//轴运动状态

public slots:
    void setTaskSend();//封装页面的设置指令入链表
private slots:
    void slot_timer_setTaskReSend();

signals:
    void signal_UiUpdate();
    void signal_SetTask_ReSend();// 关联到setTaskSend，用于getstatus和获取不到任务finish重发数据
};

#endif // ACTIONUPENDER_H
