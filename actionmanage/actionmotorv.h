#ifndef ACTIONMOTORV_H
#define ACTIONMOTORV_H

#include <QObject>
#include <QTimer>
#include <QDebug>

#include "hardcmdparseagent.h"
#include "harddef.h"
#include "hardselutedef.h"

#define SmearCoverFlagTurnOn 0x71 // 激光扫玻片的遮挡信号，未遮挡转遮挡
#define SmearCoverFlagTurnOff 0x70 // 激光扫玻片的遮挡信号，遮挡转未遮挡
#define SmearCoverRecordMaxNum 200 // 激光扫玻片的遮挡信号记录数量值获取
#define SmearBoxColIdWait 1 // 等待列号
#define SmearBoxColIdFinish 2 // 完成列号

#define WK_PhyPosNotLimit  (0x7FFFFFFF)//以此为坐标表示无需移动(物理)
#define WK_PosNotLimit ((float)(0x7FFFFFFF))//以此为坐标表示无需移动(逻辑)
#define LOGIC_ZERO  ((float)(0.00))//逻辑起点
#define LOGIC_LINE  ((float)(1000.00))//逻辑行程
#define MOTOR_V_BOXOUT_LOGICPOS ((float)999.0)

typedef struct
{
    emTaskDBoxVType m_eTaskType;   //任务类型
    uint8_t m_uTaskId;             //任务Id
    int32_t m_fTargetPos;          //目标坐标
    int32_t m_fScanBgn;            //扫描起点
    int32_t m_fScanEnd;            //扫描终点
}ST_MOTORV_SETTASK_INFO;

class ActionMotorV :  public QObject, public HardCmdParser
{
    Q_OBJECT
public:
    ActionMotorV(QObject *parent = 0);
    ~ActionMotorV(){};

    // 垂直扫描D层任务执行结果定义
    enum E_SELUTE_MOTORV {
        start,
        NoErr, // 正常
        Outtime,//动作超时（动作次数）

        DCfgErr,//D层配置信息无效
        C2DTypeErr,//C->D传入参数错误
        C2DDataErr,//C->D传入参数错误
        Nolink,  //设备连接异常
        HardWareErr,  //设备异常
        LineErr,  //边界异常(为默认值)
        LineLimit12Err,  //限位开关异常（1,2遮挡）
        LimitSwitch1Err,// 开关信号异常
        LimitSwitch2Err,// 开关信号异常
        EnvirErr,//环境异常
        MotorErr,//电机报错
        IRErr,//激光异常
        NotConnectB2,//未检测到板2（翻转盒位置信息未知）
        sfZeroCheckFaild,//sf原点校验失败
        sfDriverErr,//x驱动异常

        end
    };
    Q_ENUM(E_SELUTE_MOTORV)

    // 垂直扫描玻片盒位置枚举
    enum emBoxVPosType
    {
        emBoxVPos_Unknow,//位置异常
        emBoxVPos_Top,//顶部接玻片盒位置
        emBoxVPos_Mid,//中间位置
        emBoxVPos_Bottom,//底部出玻片盒位置
        emBoxVPos_End
    };

    emWKCmdType parseCmd(uint8_t* puData);//回复解析
    bool setTaskCmdReSend(uint32_t sdNum);//重发当前模块的设置任务指令
    bool getBoxLieDownPos(int32_t &pos);//获取盒子翻转位置

    static uint32_t u32MaxReadNum_to_reSend;
    ST_MOTORV_SETTASK_INFO m_stTaskToSend;//准备设置的任务信息/之前设置的任务信息
    //下层获取信息
    emWorkStatus m_eTaskStatusD;//任务状态
    emSeluteDMoveBoxV m_eTaskSeluteD;//任务结论
    int32_t m_i32CurPos;//当前物理坐标

private:
    void getTaskSend();//封装查询指令到链表
    //自动重发定时器，用于settask指令未回复的情况，重发settask
    static int timeOutValue_setTaskReSend;
    QTimer timer_setTaskReSend;
    //下层获取信息
    ST_MOTORV_SETTASK_INFO m_stTaskD;//任务信息
    uint16_t errFlag;//弃用    
    emBoxVPosType m_eCurPosStatus;
    //边界信息
    int32_t m_iLineDown;//小边界
    int32_t m_iLineUp;//大边界
    bool m_bIsMoving;
    uint32_t m_u32AlarmCode;//垂直驱动器当前报警

public slots:
    void setTaskSend();//封装页面的设置指令入链表
private slots:
    void slot_timer_setTaskReSend();

signals:
    void signal_UiUpdate();
    void signal_SetTask_ReSend();// 关联到setTaskSend，用于getstatus和获取不到任务finish重发数据
};

#endif // ACTIONMOTORV_H
