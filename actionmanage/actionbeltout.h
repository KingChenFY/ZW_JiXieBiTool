#ifndef ACTIONBELTOUT_H
#define ACTIONBELTOUT_H

#include <QObject>
#include <QTimer>
#include <QDebug>

#include "hardcmdparseagent.h"
#include "harddef.h"
#include "hardselutedef.h"

typedef struct
{
    emTaskDBeltOutType m_eTaskType;//任务类型
    uint8_t m_uTaskId;//任务Id
}ST_BELTOUT_SETTASK_INFO;

class ActionBeltOut : public QObject, public HardCmdParser
{
    Q_OBJECT
public:
    explicit ActionBeltOut(QObject *parent = nullptr);
    ~ActionBeltOut(){};

    // 入(出)舱皮带任务执行结果定义
    enum E_SELUTE_BELTOUT {
        start,

        //动作结论
        NoErr,//正常
        Outtime,//超时(动作次数)
        DCfgErr,//D层配置信息无效
        C2DDataErr,//C->D传入参数错误
        //任务结论
        EnvErr,//环境异常
        BeltHardwareErr,//传送带硬件错误(有盒子但未移动)
        ScanHardwareErr,//扫码器硬件错误
        QRScanNolink,//二维码扫描器无连接
        SlideErr,//玻片异常(激光被遮挡)
        SwitchB1Err,//行程开关1异常(远翻转盒侧)
        SwitchB2Err,//行程开关2异常(近翻转盒侧)

        end
    };
    Q_ENUM(E_SELUTE_BELTOUT)

    emWKCmdType parseCmd(uint8_t* puData);//回复解析
    bool setTaskCmdReSend(uint32_t sdNum);//重发当前模块的设置任务指令

    static uint32_t u32MaxReadNum_to_reSend;
    ST_BELTOUT_SETTASK_INFO m_stTaskToSend;//准备设置的任务信息/之前设置的任务信息
    //下层获取信息
    emWorkStatus m_eTaskStatusD;// 任务状态
//    emSeluteBeltD m_eTaskSeluteD;//任务结果
    emSeluteDBelt m_eTaskSeluteD;

private:
    void getTaskSend();//封装查询指令到链表
    //自动重发定时器
    static int timeOutValue_setTaskReSend;
    QTimer timer_setTaskReSend;
    //下层获取信息
    ST_BELTOUT_SETTASK_INFO m_stTaskD;//任务信息
    bool m_IsMoving;//皮带运动状态
    bool m_IsIn;//皮带运动方向

public slots:
    void setTaskSend();//封装页面的设置指令入链表
private slots:
    void slot_timer_setTaskReSend();

signals:
    void signal_UiUpdate();
    void signal_SetTask_ReSend();

};

#endif // ACTIONBELTOUT_H
