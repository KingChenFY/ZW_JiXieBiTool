#ifndef ACTIONFOURPOSTEST_H
#define ACTIONFOURPOSTEST_H

#include <QObject>
#include <QThread>
#include "hardcmdparseagent.h"
#include "harddef.h"

#include "cejutcpclient.h"
#include "actionmotorxyz.h"

#define XLSX_ROW_LINE_TITLE   1
#define XLSX_ROW_LINE_DATA    2

#define XLSX_COL_LINE_TASK1   1
#define XLSX_COL_LINE_TASK2   2
#define XLSX_COL_LINE_TASK3   3

typedef enum
{
    emFPTest_CejuConfigAdjust = 0,
    emFPTest_WaitCejuConfigAdjust,
    emFPTest_MoveToSafePosY,
    emFPTest_WaitMoveToSafePosY,
    emFPTest_MoveToTestPosZ,
    emFPTest_WaitMoveToTestPosZ,
    emFPTest_MoveToTestPointX,
    emFPTest_WaitMoveToTestPointX,
    emFPTest_MoveToTestPointY,
    emFPTest_WaitMoveToTestPointY,
    emFPTest_CejuRecordStart,
    emFPTest_WaitCejuRecordEnd,
    emFPTest_Exit,
}emFourPointTestStep;

typedef enum
{
    emPointPos_One = 0,
    emPointPos_Two,
    emPointPos_Three,
    emPointPos_Four,
    emPointPos_End
}emPoint;

typedef struct
{
    int32_t m_i32StPosX;
    int32_t m_i32StPosY;
    int32_t m_i32ToPosX;
    int32_t m_i32ToPosY;
    int32_t m_i32Height;
    uint16_t m_u16PointT;
    uint16_t m_u16ReadFreq;
}ST_FOURPOINT_TEST_INFO;

class ActionFourPosTest : public QThread
{
    Q_OBJECT
public:
    explicit ActionFourPosTest(ActionMotorXYZ* &objActionMotorXYZ,
                               CeJuTcpClient* &objCeJuTcpClient, QObject *parent = nullptr);
    bool m_bNeedStop;
    void getPointSetInfo(ST_FOURPOINT_TEST_INFO &point);
    void InitFourPointTest();
protected:
    void run();
private:
    //Excel文件路径
    QString path;
    //Excel文件名称
    QString name;
    //Excel文件完整名称
    QString fileName;
    //当前写的行数
    uint16_t xlsx_row;

    emPoint m_eCurPoint;
    ST_XYZ_DPOS m_stAimPoint[emPointPos_End];
    uint16_t m_u16PointT;
    uint16_t m_u16ReadFreq;
    uint16_t m_u16ReadNumPerPoint;
    int32_t m_i32CejuData[emCeJuDataTtype_End][WK_CeJuRecordNumMax];//移动阶段测距数据
    uint16_t m_u16ActualNum;

    emFourPointTestStep emActionStep;
    ActionMotorXYZ* &m_objActionMotorXYZ;
    CeJuTcpClient* &m_objCeJuTcpClient;
signals:
    void signal_FourPoint_CejuInit();//控制测距线程执行四点移动测距环境配置
    void signal_Ceju_RecordStart(uint16_t num, uint16_t freq); //控制测距线程执行测距开始任务
};

#endif // ACTIONFOURPOSTEST_H
