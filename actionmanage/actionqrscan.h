#ifndef ACTIONQRSCAN_H
#define ACTIONQRSCAN_H

#include <QObject>
#include <QDebug>
#include <QTimer>

#include "hardcmdparseagent.h"
#include "harddef.h"
#include "hardselutedef.h"

#define COM_FIFO_DATA_LENGHT_QRSCAN 		128 		//二维码缓存长度

typedef struct
{
    emTaskDQRScannerType m_eTaskType;   //任务类型
    uint8_t m_uTaskId;                  //任务Id
}ST_QRSCAN_SETTASK_INFO;

typedef struct
{
    emTaskDQRScannerType m_eTaskTypeD;      //任务类型
    uint8_t m_uTaskId;                      //任务Id
    emWorkStatus m_eTaskStatusD;            //任务状态
    emSeluteDQRScanner m_eTaskSeluteD;      //任务结论
    uint8_t m_u8QRContent[COM_FIFO_DATA_LENGHT_QRSCAN];//二维码扫描结果
}ST_QRSCAN_GETTASK_INFO;

class ActionQRScan : public QObject, public HardCmdParser
{
    Q_OBJECT
public:
    explicit ActionQRScan(QObject *parent = nullptr);
    ~ActionQRScan(){};

    // XYZ平台移动功能D层任务执行结果定义
    enum E_SELUTE_QRSCAN
    {
        start,
        NoErr, // 正常
        Outtime,//超时(动作次数)
        DCfgErr,//D层配置信息无效
        C2DDataErr,//C->D传入参数错误
        NoLink,//无法确认二维码设备
        HardwareErr,//硬件错误
        end
    };
    Q_ENUM(E_SELUTE_QRSCAN)

    emWKCmdType parseCmd(uint8_t* puData);//回复解析
    bool setTaskCmdReSend(uint32_t sdNum);//重发当前模块的设置任务指令
    void getQRScanContent(uint8_t *data);

    static uint32_t u32MaxReadNum_to_reSend;
    ST_QRSCAN_SETTASK_INFO m_stTaskToSend;//准备设置的任务信息/之前设置的任务信息
    //下层获取信息
    ST_QRSCAN_GETTASK_INFO m_stDTaskInfo;

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

#endif // ACTIONQRSCAN_H
