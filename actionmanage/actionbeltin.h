#ifndef ACTIONBELTIN_H
#define ACTIONBELTIN_H

#include <QObject>
#include <QDebug>
#include <QTimer>

#include "hardcmdparseagent.h"
#include "harddef.h"
#include "hardselutedef.h"

typedef struct
{
    emTaskDBeltInType m_eTaskType;  //任务类型
    uint8_t m_uTaskId;              //任务Id
    bool m_bNeedQR;                 //需要打开扫码
}ST_BELTIN_SETTASK_INFO;

class ActionBeltIn :  public QObject, public HardCmdParser
{
    Q_OBJECT

public:
    explicit ActionBeltIn(QObject *parent = 0);
    ~ActionBeltIn(){};

    // 入(出)舱皮带任务执行结果定义
    enum E_SELUTE_BELTIN {
        emSeluteDBelt_start,

        //动作结论
        emSeluteDBelt_NoErr,//正常
        emSeluteDBelt_Outtime,//超时(动作次数)
        emSeluteDBelt_DCfgErr,//D层配置信息无效
        emSeluteDBelt_C2DDataErr,//C->D传入参数错误
        //任务结论
        emSeluteDBelt_EnvErr,//环境异常
        emSeluteDBelt_BeltHardwareErr,//传送带硬件错误(有盒子但未移动)
        emSeluteDBelt_ScanHardwareErr,//扫码器硬件错误
        emSeluteDBelt_QRScanNolink,//二维码扫描器无连接
        emSeluteDBelt_SlideErr,//玻片异常(激光被遮挡)
        emSeluteDBelt_SwitchB1Err,//行程开关1异常(远翻转盒侧)
        emSeluteDBelt_SwitchB2Err,//行程开关2异常(近翻转盒侧)

        emSeluteDBelt_end
    };
    Q_ENUM(E_SELUTE_BELTIN)

    emWKCmdType parseCmd(uint8_t* puData);//回复解析
    bool setTaskCmdReSend(uint32_t sdNum);//重发当前模块的设置任务指令

    static uint32_t u32MaxReadNum_to_reSend;
    ST_BELTIN_SETTASK_INFO m_stTaskToSend;//准备设置的任务信息/之前设置的任务信息
    //下层获取信息
    emWorkStatus m_eTaskStatusD;// 任务状态
//    emSeluteBeltD m_eTaskSeluteD;//任务结果
    E_SELUTE_BELTIN m_eTaskSeluteD;

private:
    void getTaskSend();//封装查询指令到链表
    //自动重发定时器，用于settask指令未回复的情况，重发settask
    static int timeOutValue_setTaskReSend;
    QTimer timer_setTaskReSend;
    //下层获取信息
    ST_BELTIN_SETTASK_INFO m_stTaskD;//任务信息
    uint8_t m_uDQRScan[WK_LEN_QRCODE_CONTENT];//玻片盒二维码信息
    bool m_IsMoving;//皮带运动状态
    bool m_IsIn;//皮带运动方向

public slots:
    void setTaskSend();//封装页面的设置指令入链表
private slots:
    void slot_timer_setTaskReSend();

signals:
    void signal_UiUpdate();
    void signal_SetTask_ReSend();// 关联到setTaskSend，用于getstatus和获取不到任务finish重发数据

};

#endif // ACTIONBELTIN_H
