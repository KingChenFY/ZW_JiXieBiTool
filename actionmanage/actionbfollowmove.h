#ifndef ACTIONBFOLLOWMOVE_H
#define ACTIONBFOLLOWMOVE_H

#include <QObject>
#include <QThread>
#include "hardcmdparseagent.h"
#include "harddef.h"

#include "actiontriggerset.h"
#include "actionmotorxyz.h"
#include "cejutcpclient.h"

#define TRIG_SAMPLE_TEMP (1.5) //1.5ms

typedef enum
{
    emFollow_TrigParameterSet = 0,
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
    emFollow_FollowMoveExit,
}emFollowMoveStep;

class ActionBFollowMove : public QThread
{
    Q_OBJECT
public:
    explicit ActionBFollowMove(ActionTriggerSet* &objActionTriggerSet, ActionMotorXYZ* &objActionMotorXYZ,
                               CeJuTcpClient* &objCeJuTcpClient, QObject *parent = nullptr);

    bool m_bNeedStop;
    ST_XYZ_DPOS m_stAimStPhyPos;
    ST_XYZ_DPOS m_stAimEdPhyPos;
protected:
    void run();
private:
    uint16_t trigPosNeedNum;
    void saveCejuRecordToExcel();
    emFollowMoveStep emActionStep;
    ActionTriggerSet* &m_objActionTriggerSet;
    ActionMotorXYZ* &m_objActionMotorXYZ;
    CeJuTcpClient* &m_objCeJuTcpClient;

signals:
    void signal_Ceju_RecordStart(); //控制测距线程执行测距开始任务
    void signal_Ceju_RecordEnd(); //控制测距线程执行测距结束任务
};

#endif // ACTIONBFOLLOWMOVE_H
