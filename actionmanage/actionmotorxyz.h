#ifndef ACTIONMOTORXYZ_H
#define ACTIONMOTORXYZ_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QMetaEnum>
#include <QMutex>

#include "hardcmdparseagent.h"
#include "harddef.h"
#include "hardselutedef.h"

#define WK_SpeedNotLimit (0)//此为未指定的速度值，不可移动
#define WK_PhyPosNotLimit  (0x7FFFFFFF)//以此为坐标表示无需移动(物理)
#define WK_PosNotLimit ((float)(0x7FFFFFFF))//以此为坐标表示无需移动(逻辑)
#define LOGIC_ZERO  ((float)(0.00))//逻辑起点
#define LOGIC_LINE  ((float)(1000.00))//逻辑行程

typedef enum
{
    emMorot_X,
    emMorot_Y,
    emMorot_Z,
}emMotorObj;
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
    ST_XYZ_CPOS m_stAimLogicDPos;   //XYZ当前坐标逻辑值
    ST_XYZ_DPOS m_stDPosBgn;       //XYZ的startPos
    uint32_t m_u32MoveTime;        //定时移动的时间
    uint16_t m_u16PathId;          //轨迹Id
     /*****较零下发信息*****/
    emMotorXDir m_eXZeroDir;        //X上电首次设置零点方向__Posx作为方向使用
    uint16_t m_u16XZeroSpeed;       //X零点检验速度大小__Time作为速度使用，Point_id做任务ID用
}ST_MOTORXYZ_SETTASK_INFO;

typedef struct
{
    emTaskDXYZType m_eTaskTypeD;    //任务类型
    emWorkStatus m_eTaskStatusD;    //任务状态
    emSeluteDMoveXYZ m_eTaskSeluteD; //任务结论
    ST_XYZ_DPOS m_stAimDPos;        //XYZ目标坐标物理值
    uint32_t m_u32MoveTime;         //定时移动的时间
    uint16_t m_u16XZeroSpeed;       //X较零速度大小
    ST_XYZ_DPOS m_stCurDPos;        //XYZ当前坐标物理值
    ST_XYZ_CPOS m_stCurLogicDPos;   //XYZ当前坐标逻辑值
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

    enum E_TASKTPYE_MOTORXYZ
    {
       E_TASKTPYE_MOTORXYZ_start = 0,
       zeroXCheck,       //X轴原点校验1
       setXPosZero,      //设置X轴原点2
       minPosYSet,       //里边界定位3
       maxPosYSet,       //外边界定位4
       minPosZSet,       //底边界定位5
       maxPosZSet,       //顶边界定位6
       stop,             //停止7
       move,             //移动到指定点8
       moveInTime,       //一定时间内移动到指定点9
       moveTogether,     //连轴移动10
       moveTwoPos,       //两点移动(移动到起点，定时移动到终点)11
       enableAxis,       //使能轴12
       disableAxis,      //失能轴13
       adaptive,         //自适应轴参数14
       zeroXMoveCheck,   //设置不同的移动速度进行移动测试15
       E_TASKTPYE_MOTORXYZ_end
    };

    // XYZ平台移动功能D层任务执行结果定义
    enum E_SELUTE_MOTORXYZ
    {
        E_SELUTE_MOTORXYZ_start,
        NoErr, // 正常
        Outtime,//动作超时（动作次数）
        DCfgErr,//配置参数错误
        C2DTypeErr,//C->D传入类型错误
        C2DDataErr,//C->D传入参数错误
        xLimitSwitch1Err,//x限位开关1异常
        yLimitSwitch1Err,//y限位开关1异常
        zLimitSwitch1Err,//z限位开关1异常
        xLimitSwitch2Err,//x限位开关2异常
        yLimitSwitch2Err,//y限位开关2异常
        zLimitSwitch2Err,//z限位开关2异常
        xLimitSwitchErr,//x限位开关异常(两头都遮挡)
        yLimitSwitchErr,//y限位开关异常(两头都遮挡)
        zLimitSwitchErr,//z限位开关异常(两头都遮挡)
        xLimitInvalid,//x边界异常(为默认值)
        yLimitInvalid,//y边界异常(为默认值)
        zLimitInvalid,//z边界异常(为默认值)
        xLimitBigErr,//x边界定位超出预期
        yLimitBigErr,//y边界定位超出预期
        zLimitBigErr,//z边界定位超出预期
        xLengthLargeErr,//x参考行程过大
        yLengthLargeErr,//y参考行程过大
        zLengthLargeErr,//z参考行程过大
    //	xOutofLineErr,//x轴目标位置越过边界
    //	yOutofLineErr,//y轴目标位置越过边界
    //	zOutofLineErr,//z轴目标位置越过边界
        xDriverErr,//x驱动异常
        yDriverErr,//y驱动异常
        zDriverErr,//z驱动异常
        xNoLink,//x驱动无连接
        xZeroCheckFaild,//x轴原点校验失败
        TrailPointInvalid,//跟随移动轨迹点异常
        TrailInvalid,//跟随移动轨迹缺失
        yNoLink,//y驱动无连接
        zAdptEnvErr,//Z轴自适应环境错误(判断Y轴是否完成定位，若已完成则在有效范围内，若未完成则需要Y在最外侧)
        zMoveAdptFailed,//Z移动自适应失败
        zFollowAdptFailed,//Z跟随自适应失败
        xLimitZeroErr,//零点限位开关异常
        xOverLimit,//位置超出边界限位
        xEncodeErr,//X轴编码器异常
        xSetZeroFail,//设置零点失败
        xZeroOutAllowErr,//零点动态校验失败
        ySetMoveCtrlErr,//y轴设置移动控制错误
        E_SELUTE_MOTORXYZ_end
    };

    Q_ENUM(E_TASKTPYE_MOTORXYZ)
    Q_ENUM(E_SELUTE_MOTORXYZ)

    emWKCmdType parseCmd(uint8_t* puData);//回复解析
    bool setTaskCmdReSend(uint32_t sdNum);//重发当前模块的设置任务指令
    bool isLineHasLocated(emMotorObj motorObj);
    bool isAllLineHasLocated();//XYZ三轴坐标是否都已经完成定位
    void convertLogicPosToPhyPos(ST_XYZ_CPOS &logicPos, ST_XYZ_DPOS &phyPos);//把0.00-1000.00的逻辑坐标转换到物理坐标_______m_stTaskToSend
    void convertPhyPosToLogicPos(ST_XYZ_DPOS &phyPos, ST_XYZ_CPOS &logicPos);//把物理坐标转换到0.00-1000.00的逻辑坐标_______m_stDTaskInfo
    bool isAimLogicPosOverLimit(ST_XYZ_CPOS &logicPos);
    bool isAimPhyPosOverLimit(ST_XYZ_DPOS &phyPos);

    static uint32_t u32MaxReadNum_to_reSend;
    ST_MOTORXYZ_SETTASK_INFO m_stTaskToSend;//准备设置的任务信息/之前设置的任务信息
    //下层获取信息
    ST_MOTORXYZ_GETTASK_INFO m_stDTaskInfo;

private:
    void getTaskSend();//封装查询指令到链表 
    //自动重发定时器，用于settask指令未回复的情况，重发settask
    static int timeOutValue_setTaskReSend;
    QTimer timer_setTaskReSend;
    QMetaEnum emMetaTaskType;
    QMutex mutexXYZLine;

public slots:
    void setTaskSend();//封装页面的设置指令入链表
private slots:
    void slot_timer_setTaskReSend();

signals:
    void signal_UiUpdate();
    void signal_SetTask_ReSend();// 关联到setTaskSend，用于getstatus和获取不到任务finish重发数据
};

#endif // ACTIONMOTORXYZ_H
