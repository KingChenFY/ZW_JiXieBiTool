#ifndef ACTIONBFOLLOWMOVE_H
#define ACTIONBFOLLOWMOVE_H

#include <QObject>
#include <QThread>
#include "hardcmdparseagent.h"
#include "harddef.h"

#include "actiontriggerset.h"
#include "actionmotorxyz.h"
#include "cejutcpclient.h"
#include "actionparameterset.h"

#define TRIG_SAMPLE_TEMP (1.5) //1.5ms

#define XLSX_ROW_LINE_TITLE   1
#define XLSX_ROW_LINE_DATA    2

#define XLSX_COL_LINE_TASK1   1
#define XLSX_COL_LINE_TASK2   2
#define XLSX_COL_LINE_TASK3   3
#define XLSX_COL_LINE_CEJU1   4
#define XLSX_COL_LINE_CEJU2   5

#define XLSX_FOLLOW_COL_LINE_TASK1   6
#define XLSX_FOLLOW_COL_LINE_TASK2   7
#define XLSX_FOLLOW_COL_LINE_TASK3   8
#define XLSX_FOLLOW_COL_LINE_CEJU1   9
#define XLSX_FOLLOW_COL_LINE_CEJU2   10

#define XLSX_FOLLOW_COL_LINE_TRAILSYNS      11

///////////////////////////////////////////////////////////////////////数据处理/////////////////////////////////////////////////
#define WK_SycNum_FactUse           6//系数
#define WK_DEFAULT_TrailData        -2147483648
#define WK_THICK_AVG_RANGE          20000//厚度均值上下范围
#define WK_THICK_FIT_RANGE          5000//厚度拟合线上下范围
#define WK_THICK_FIT_AGAIN_RANGE    1000//厚度拟合线上下范围

typedef struct
{
    uint32_t m_uTotalNum;//数据个数
    int32_t m_iPos[WK_CeJuRecordNumMax];//触发轴坐标
    int32_t m_iPosZ[WK_CeJuRecordNumMax];//触发Z坐标
    int32_t m_iH1[WK_CeJuRecordNumMax];//第一表面
    int32_t m_iH2[WK_CeJuRecordNumMax];//第二表面
    int32_t m_iH3[WK_CeJuRecordNumMax];//第三表面
}stCejuTempInfo;//测距数据结构

typedef enum
{
    emFStage_Move,
    emFStage_Follow,
    emFStage_END
}emFollowMoveStage;

typedef enum
{
    emFollow_FollowPrepare = 0,
    emFollow_TrigParameterSet,
    emFollow_WaitTrigParameterSet,
    emFollow_YMoveToStart,
    emFollow_WaitYMoveToStart,
    emFollow_CejuRecordStart,
    emFollow_WaitCejuRecordStart,
    emFollow_YMoveToEnd,
    emFollow_WaitYMoveToEnd,
    emFollow_CejuRecordEnd,
    emFollow_WaitCejuRecordEnd,
    emFollow_GetTrigInfo,
    emFollow_WaitGetTrigInfo,
    emFollow_TrigInfoManage,
    emFollow_FollowParameterSet,
    emFollow_WaitFollowParameterSet,
//    emFollow_FollowCejuRecordStart,
//    emFollow_FollowWaitCejuRecordStart,
    emFollow_FollowMoveStart,
    emFollow_WaitFollowMoveStartEnd,
//    emFollow_FollowCejuRecordEnd,
//    emFollow_FollowWaitCejuRecordEnd,
    emFollow_FollowMoveExit,
}emFollowMoveStep;

typedef struct
{
    uint32_t CejuTaskDataNum[emFStage_END];//每个阶段测距的获取总量
    int32_t m_i32CejuMoveStageData[emCeJuDataTtype_End][WK_CeJuRecordNumMax];//移动阶段测距数据
    int32_t m_i32CejuFollowStageData[emCeJuDataTtype_End][WK_CeJuRecordNumMax];//跟随阶段距数据

    uint32_t m_u32TrigPosNum[emFStage_END];//每个阶段测距触发坐标的获取总量
    int32_t m_i32MoveStageTrigPos[FOLLOW_AXIS_NUM][WK_CeJuRecordNumMax];//移动阶段测距触发坐标
    int32_t m_i32FollowStageTrigPos[FOLLOW_AXIS_NUM][WK_CeJuRecordNumMax];//跟随阶段测距触发坐标
}ST_ACTIONFOLLOW_DATA;

typedef struct
{
    int32_t m_u32AxisMoveLength;
    float m_fMoveInTimeVMax;
    uint32_t m_u32MoveInTimeTMin;
}ST_MOVETRIG_DATA;

class ActionBFollowMove : public QThread
{
    Q_OBJECT
public:
    explicit ActionBFollowMove(ActionTriggerSet* &objActionTriggerSet, ActionMotorXYZ* &objActionMotorXYZ,
                               CeJuTcpClient* &objCeJuTcpClient, ActionParameterSet* &objActionParameterSet, QObject *parent = nullptr);

    bool m_bNeedStop;
    ST_XYZ_DPOS m_stAimStPhyPos;
    ST_XYZ_DPOS m_stAimEdPhyPos;
protected:
    void run();
private:
    uint16_t trigPosNeedNum;
    double m_dTrailSyn[TRAIL_SYN_MAX_NUM];
    void saveCejuRecordToExcel();
    emFollowMoveStep emActionStep;
    emFollowMoveStage emCurStage;
    ST_MOVETRIG_DATA m_stMoveTrigData;
    ST_ACTIONFOLLOW_DATA m_stAcionFollowData;
//    stCejuTempInfo m_stDealData_Original;
    stCejuTempInfo* m_stDealData_Original;
    stCejuTempInfo m_stDealData_Filter;
    ActionTriggerSet* &m_objActionTriggerSet;
    ActionMotorXYZ* &m_objActionMotorXYZ;
    CeJuTcpClient* &m_objCeJuTcpClient;
    ActionParameterSet* &m_objActionParameterSet;

    //Excel文件路径
    QString path;
    //Excel文件名称
    QString name;
    //Excel文件完整名称
    QString fileName;

    //当前写的行数
    uint16_t xlsx_row;

    void polyfit(int n, int x[], int y[], int poly_n, double p[]);
    void gauss_solve(int n, double A[], double x[], double b[]);
    float polyCalculate(float x, int poly_n, double p[]);
    void selectCases(stCejuTempInfo m_stCejuEdge, stCejuTempInfo &m_stCeju);

signals:
    void signal_Ceju_RecordStart(); //控制测距线程执行测距开始任务
    void signal_Ceju_RecordEnd(); //控制测距线程执行测距结束任务
};

#endif // ACTIONBFOLLOWMOVE_H
