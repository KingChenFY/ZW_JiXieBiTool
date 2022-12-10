#ifndef ACTIONMOTORXYZ_H
#define ACTIONMOTORXYZ_H

#include <QObject>
#include <QTimer>
#include <QDebug>

#include "hardcmdparseagent.h"
#include "harddef.h"
#include "hardselutedef.h"

#define WK_SpeedNotLimit (0)//此为未指定的速度值，不可移动
#define WK_PhyPosNotLimit  (0x7FFFFFFF)//以此为坐标表示无需移动(物理)
#define WK_PosNotLimit ((float)(0x7FFFFFFF))//以此为坐标表示无需移动(逻辑)
#define LOGIC_ZERO  (0.0)//逻辑起点
#define LOGIC_LINE  (1000.0)//逻辑行程

typedef enum
{
    emMorotX_UnKnow,
    emMorotX_Left,
    emMorotX_Right,
}emMotorXDir;
typedef enum
{
    emMorotY_UnKnow,
    emMorotY_In,
    emMorotY_Out,
}emMotorYDir;
typedef enum
{
    emMorotZ_UnKnow,
    emMorotZ_Down,
    emMorotZ_Up,
}emMotorZDir;
typedef struct
{
    int32_t m_i32X;
    int32_t m_i32Y;
    int32_t m_i32Z;
}ST_XYZ_DPOS;
typedef struct
{
    float m_fX;
    float m_fY;
    float m_fZ;
}ST_XYZ_CPOS;
typedef struct
{
    int32_t m_i32XLineLeft;     //左边界
    int32_t m_i32XLineRight;    //右边界
    int32_t m_i32YLineIn;       //里边界
    int32_t m_i32YLineOut;      //里边界
    int32_t m_i32ZLineDown;     //底边界
    int32_t m_i32ZLineUp;       //顶边界
}ST_XYZ_DLinePOS;
typedef struct
{
    int32_t m_i32MoveDirMinEMinPos;
    int32_t m_i32MoveDirMinEMaxPos;
    int32_t m_i32MoveDirMaxEMinPos;
    int32_t m_i32MoveDirMaxEMaxPos;
}ST_XZERO_ENCODE_POS;

typedef struct
{
    /*****基础下发信息*****/
    emTaskDXYZType m_eTaskType;   //任务类型
    uint8_t m_uTaskId;             //任务Id
    ST_XYZ_DPOS m_stAimDPos;       //XYZ目标坐标物理值
    ST_XYZ_DPOS m_stDPosBgn;       //XYZ的startPos
    uint32_t m_u32MoveTime;        //定时移动的时间
    uint16_t m_u16PathId;          //轨迹Id
     /*****较零下发信息*****/
    emMotorXDir m_eXZeroDir;        //X上电首次设置零点方向__Posx作为方向使用
    uint16_t m_u16XZeroSpeed;       //X零点检验速度大小__Time作为速度使用，Point_id做任务ID用
}ST_MOTORXYZ_SETTASK_INFO;

typedef struct
{
    emTaskDXYZType m_eTaskType;    //任务类型
    emWorkStatus m_eTaskStatusD;    //任务状态
    emSeluteDMoveXYZ m_eTaskSeluteD; //任务结论
    ST_XYZ_DPOS m_stAimDPos;        //XYZ目标坐标物理值
    uint32_t m_u32MoveTime;         //定时移动的时间
    uint16_t m_u16XZeroSpeed;       //X较零速度大小
    ST_XYZ_DPOS m_stCurDPos;        //XYZ当前坐标物理值
    emMotorXDir m_eXZeroDir;        //X上电首次设置零点方向
    uint64_t m_u64CurPosDTime;
    ST_XYZ_DLinePOS m_stLinePos;    //XYZ边界
    bool m_bXIsMoving;
    bool m_bYIsMoving;
    bool m_bZIsMoving;
    uint16_t m_u16PathId;           //轨迹Id
    emMotorYDir emYInOut;
    emMotorZDir emZUpDown;
    ST_XYZ_DPOS m_stDPosBgn;        //XYZ的startPos
    ST_XZERO_ENCODE_POS m_stXZeroEPos;
    uint8_t m_uTaskId;              //任务Id
    uint32_t m_u32XErrCode;         //X电机错误码
    uint32_t m_u32YErrCode;         //Y电机错误码
    uint32_t m_u32ZErrCode;         //Z电机错误码
}ST_MOTORXYZ_GETTASK_INFO;

class ActionMotorXYZ : public QObject, public HardCmdParser
{
    Q_OBJECT
public:
    explicit ActionMotorXYZ(QObject *parent = nullptr);
    ~ActionMotorXYZ(){};

    Q_ENUM(emSeluteDMoveXYZ)

    emWKCmdType parseCmd(uint8_t* puData);//回复解析
    bool setTaskCmdReSend(uint32_t sdNum);//重发当前模块的设置任务指令

    static uint32_t u32MaxReadNum_to_reSend;
    ST_MOTORXYZ_SETTASK_INFO m_stTaskToSend;//准备设置的任务信息/之前设置的任务信息
    //下层获取信息
    ST_MOTORXYZ_GETTASK_INFO m_stDTaskInfo;

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

#endif // ACTIONMOTORXYZ_H
