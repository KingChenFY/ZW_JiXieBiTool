#ifndef ACTIONLIGHT_H
#define ACTIONLIGHT_H

#include <QObject>
#include <QTimer>
#include <QDebug>

#include "hardcmdparseagent.h"
#include "harddef.h"
#include "hardselutedef.h"

#define COOL_LED1_SEL 	0
#define COOL_LED2_SEL 	1

#define ENABLE_COOL_LED_TRI		1
#define DISABLE_COOL_LED_TRI 	0

#define LIGHT_TRI_TIME_DEFAULT  100

typedef struct
{
    emTaskDSetLEDType m_eTaskType;//任务类型
//    uint8_t m_uTaskId;//任务Id
    uint8_t m_uIsTri1;
    uint16_t m_u16Tri1Time;
    uint16_t m_u16Level1;
    uint8_t m_uIsTri2;
    uint16_t m_u16Tri2Time;
    uint16_t m_u16Level2;
}ST_LIGHT_SETTASK_INFO;

typedef struct
{
    emTaskDSetLEDType m_eTaskTypeD;     //任务类型
    emWorkStatus m_eTaskStatusD;        //任务状态
    emSeluteDBright m_eTaskSeluteD;     //任务结论
//    uint8_t m_uTaskId;                  //任务Id
    uint8_t m_uIsTri1;
    uint16_t m_u16Tri1Time;
    uint16_t m_u16Level1;
    uint8_t m_uIsTri2;
    uint16_t m_u16Tri2Time;
    uint16_t m_u16Level2;
}ST_LIGHT_GETTASK_INFO;

class ActionLight : public QObject, public HardCmdParser
{
    Q_OBJECT
public:
    explicit ActionLight(QObject *parent = nullptr);
    ~ActionLight(){};

    // 除油任务执行结果定义
    enum E_SELUTE_LIGHT {
        start,
        NoErr,		//正常
        Outtime,	//超时(动作次数)
        DCfgErr,	//D层配置信息无效
        C2DDataErr,	//C->D传入参数错误
        NoLink,		//无法确认光源设备
        HardwareErr,//光源芯片异常
        end
    };
    Q_ENUM(E_SELUTE_LIGHT)

    emWKCmdType parseCmd(uint8_t* puData);//回复解析
    bool setTaskCmdReSend(uint32_t sdNum);//重发当前模块的设置任务指令

    static uint32_t u32MaxReadNum_to_reSend;
    ST_LIGHT_SETTASK_INFO m_stTaskToSend;//准备设置的任务信息/之前设置的任务信息
    //下层获取信息
    ST_LIGHT_GETTASK_INFO m_stTaskD;//任务信息

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

#endif // ACTIONLIGHT_H
