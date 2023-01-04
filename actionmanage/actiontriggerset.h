#ifndef ACTIONTRIGGERSET_H
#define ACTIONTRIGGERSET_H

#include <QObject>
#include <QDebug>
#include <QTimer>

#include "hardcmdparseagent.h"
#include "harddef.h"
#include "hardselutedef.h"

typedef struct
{
    emAxisType m_eAxis;//目标轴
    uint16_t m_u16Interval;//触发间隔
    uint8_t m_u8TrigObj;//触发对象
}ST_TRIG_SETTASK_INFO;

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

private:
    //自动重发定时器，用于settask指令未回复的情况，重发settask
    static int timeOutValue_setTaskReSend;
    QTimer timer_setTaskReSend;

public slots:
    void setTaskSend();//封装页面的设置指令入链表
    void getTaskSend();//封装查询指令到链表
    void slot_stopSetTaskReSendTimer();
private slots:
    void slot_timer_setTaskReSend();

signals:
    void signal_UiUpdate();
    void signal_getTaskSend();
    void signal_stopSetTaskReSendTimer();

};

#endif // ACTIONTRIGGERSET_H
