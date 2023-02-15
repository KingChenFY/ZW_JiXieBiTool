#ifndef ACTIONBOILTEST_H
#define ACTIONBOILTEST_H

#include <QObject>
#include <QThread>
#include "hardcmdparseagent.h"
#include "harddef.h"

#include "actionmotorxyz.h"
#include "actiondripoil.h"
#include "actioncleanoil.h"

typedef enum
{
    emOilTest_MoveToSafePosY = 0,
    emOilTest_WaitMoveToSafePosY,
    emOilTest_MoveToDripPosX,
    emOilTest_WaitMoveToDripPosX,
    emOilTest_MoveToDripPosZ,
    emOilTest_WaitMoveToDripPosZ,
    emOilTest_MoveToDripPosY,
    emOilTest_WaitMoveToDripPosY,
    emOilTest_StartDripOil,
    emOilTest_WaitDripOilEnd,
    emOilTest_MoveToCleanPosX,
    emOilTest_WaitMoveToCleanPosX,
    emOilTest_MoveToCleanPosZ,
    emOilTest_WaitMoveToCleanPosZ,
    emOilTest_MoveToCleanPosY,
    emOilTest_WaitMoveToCleanPosY,
    emOilTest_StartCleanPutPaper,
    emOilTest_WaitCleanPutPaper,
    emOilTest_WaitCleanOilSomeTime,
    emOilTest_StartCleanFrapPaper,
    emOilTest_WaitCleanFrapPaper,
    emOilTest_Exit,
}emOilTestStep;

typedef enum
{
    emOilStage_DripOil = 0,
    emOilStage_CleanOil,
}emOilTestStage;

typedef struct
{
    ST_XYZ_CPOS m_stCleanOilPos;
    ST_XYZ_CPOS m_stDripOilPos;
}ST_OILTEST_TAGETPOS;

class ActionBOilTest : public QThread
{
    Q_OBJECT
public:
    explicit ActionBOilTest(ActionMotorXYZ* &objActionMotorXYZ, ActionDripOil* &objActionDripOil,
                            ActionCleanOil* &objActionCleanOil, QObject *parent = nullptr);
    bool m_bNeedStop;

    void GetOilConifgPos(ST_OILTEST_TAGETPOS &m_stOilUiConfigPos);
protected:
    void run();

private:
    ActionMotorXYZ* &m_objActionMotorXYZ;
    ActionDripOil* &m_objActionDripOil;
    ActionCleanOil* &m_objActionCleanOil;
    ST_OILTEST_TAGETPOS m_stOilConfigPos;
    emOilTestStep emActionStep;
    emOilTestStage emCurStage;
    uint64_t m_u64WaitSleepCount;
};

#endif // ACTIONBOILTEST_H
