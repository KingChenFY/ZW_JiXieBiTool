#include "actionboiltest.h"
#include "quihelper.h"
#include "global.h"

#define WAIT_SLEEP_COUNT_MAX    4   //4*500=2000ms
ActionBOilTest::ActionBOilTest(ActionMotorXYZ* &objActionMotorXYZ, ActionDripOil* &objActionDripOil,
                               ActionCleanOil* &objActionCleanOil, QObject *parent)
        : m_objActionMotorXYZ(objActionMotorXYZ),m_objActionDripOil(objActionDripOil),
          m_objActionCleanOil(objActionCleanOil),QThread(parent)
{
    emActionStep = emOilTest_MoveToSafePosY;
    emCurStage = emOilStage_DripOil;
    m_u64WaitSleepCount = 0;
}

void ActionBOilTest::GetOilConifgPos(ST_OILTEST_TAGETPOS &m_stOilUiConfigPos)
{
    m_stOilConfigPos = m_stOilUiConfigPos;
}

void ActionBOilTest::run()
{
    while(!m_bNeedStop)
    {
        msleep(500);
        if(emOilTest_MoveToSafePosY == emActionStep)
        {
            if((emSeluteDMoveXYZ_NoErr != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)
                &&(emSeluteDMoveXYZ_Outtime != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD))
            {
                break;//XYZm模块任务结论异常
            }
            if(emWorkStatus_finish == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)
            {
                m_objActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_maxPosYSet;
                m_objActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                m_objActionMotorXYZ->setTaskSend();
                _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_MoveToSafePosY]");
                emActionStep = emOilTest_WaitMoveToSafePosY;
            }
            continue;
        }
        else if(emOilTest_WaitMoveToSafePosY == emActionStep)
        {
            if((emSeluteDMoveXYZ_NoErr != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)
                &&(emSeluteDMoveXYZ_Outtime != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD))
            {
                break;//XYZm模块任务结论异常
            }
            if(emWorkStatus_finish == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)
            {
                _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_WaitMoveToSafePosY]");
                if(emOilStage_DripOil == emCurStage)
                    emActionStep = emOilTest_MoveToDripPosX;
                else if(emOilStage_CleanOil == emCurStage)
                    emActionStep = emOilTest_MoveToCleanPosX;
                else
                    break;
            }
            continue;
        }
        else if(emOilTest_MoveToDripPosX == emActionStep)
        {
            m_objActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_move;
            m_objActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fX = m_stOilConfigPos.m_stDripOilPos.m_fX;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fY = WK_PosNotLimit;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fZ = WK_PosNotLimit;
            m_objActionMotorXYZ->convertLogicPosToPhyPos(m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos,
                                                       m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos);
            m_objActionMotorXYZ->setTaskSend();
            emActionStep = emOilTest_WaitMoveToDripPosX;
            _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_MoveToDripPosX]");
        }
        else if(emOilTest_WaitMoveToDripPosX == emActionStep)
        {
            if((emSeluteDMoveXYZ_NoErr != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)
                &&(emSeluteDMoveXYZ_Outtime != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD))
            {
                break;//XYZm模块任务结论异常
            }
            if(emWorkStatus_finish == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)
            {
                emActionStep = emOilTest_MoveToDripPosZ;
                _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_WaitMoveToDripPosX]");
            }
            continue;
        }
        else if(emOilTest_MoveToDripPosZ == emActionStep)
        {
            m_objActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_move;
            m_objActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fX = WK_PosNotLimit;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fY = WK_PosNotLimit;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fZ = m_stOilConfigPos.m_stDripOilPos.m_fZ;
            m_objActionMotorXYZ->convertLogicPosToPhyPos(m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos,
                                                       m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos);
            m_objActionMotorXYZ->setTaskSend();
            emActionStep = emOilTest_WaitMoveToDripPosZ;
            _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_MoveToDripPosZ]");
        }
        else if(emOilTest_WaitMoveToDripPosZ == emActionStep)
        {
            if((emSeluteDMoveXYZ_NoErr != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)
                &&(emSeluteDMoveXYZ_Outtime != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD))
            {
                break;//XYZm模块任务结论异常
            }
            if(emWorkStatus_finish == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)
            {
                emActionStep = emOilTest_MoveToDripPosY;
                _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_WaitMoveToDripPosZ]");
            }
            continue;
        }
        else if(emOilTest_MoveToDripPosY == emActionStep)
        {
            m_objActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_move;
            m_objActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fX = WK_PosNotLimit;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fY = m_stOilConfigPos.m_stDripOilPos.m_fY;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fZ = WK_PosNotLimit;
            m_objActionMotorXYZ->convertLogicPosToPhyPos(m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos,
                                                       m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos);
            m_objActionMotorXYZ->setTaskSend();
            emActionStep = emOilTest_WaitMoveToDripPosY;
            _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_MoveToDripPosY]");
        }
        else if(emOilTest_WaitMoveToDripPosY == emActionStep)
        {
            if((emSeluteDMoveXYZ_NoErr != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)
                &&(emSeluteDMoveXYZ_Outtime != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD))
            {
                break;//XYZm模块任务结论异常
            }
            if(emWorkStatus_finish == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)
            {
                emActionStep = emOilTest_StartDripOil;
                _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_WaitMoveToDripPosY]");
            }
            continue;
        }
        else if(emOilTest_StartDripOil == emActionStep)
        {
            m_objActionDripOil->m_stTaskToSend.m_eTaskType = emTaskDDripOil_Drop;
            m_objActionDripOil->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionDripOil->setTaskSend();
            emActionStep = emOilTest_WaitDripOilEnd;
            _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_StartDripOil]");
        }
        else if(emOilTest_WaitDripOilEnd == emActionStep)
        {
            if((emSeluteDDropOil_NoErr != m_objActionDripOil->m_stDTaskInfo.m_eTaskSeluteD)
                &&(emSeluteDDropOil_Outtime != m_objActionDripOil->m_stDTaskInfo.m_eTaskSeluteD))
            {
                break;//滴油模块任务结论异常
            }
            if(emWorkStatus_finish == m_objActionDripOil->m_stDTaskInfo.m_eTaskStatusD)
            {
                emActionStep = emOilTest_MoveToSafePosY;
                _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_WaitDripOilEnd]");
            }
            continue;
        }
        else if(emOilTest_MoveToCleanPosX == emActionStep)
        {
            m_objActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_move;
            m_objActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fX = m_stOilConfigPos.m_stCleanOilPos.m_fX;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fY = WK_PosNotLimit;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fZ = WK_PosNotLimit;
            m_objActionMotorXYZ->convertLogicPosToPhyPos(m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos,
                                                       m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos);
            m_objActionMotorXYZ->setTaskSend();
            emActionStep = emOilTest_WaitMoveToCleanPosX;
            _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_MoveToCleanPosX]");
        }
        else if(emOilTest_WaitMoveToCleanPosX == emActionStep)
        {
            if((emSeluteDMoveXYZ_NoErr != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)
                &&(emSeluteDMoveXYZ_Outtime != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD))
            {
                break;//XYZm模块任务结论异常
            }
            if(emWorkStatus_finish == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)
            {
                emActionStep = emOilTest_MoveToCleanPosZ;
                _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_WaitMoveToCleanPosX]");
            }
            continue;
        }
        else if(emOilTest_MoveToCleanPosZ == emActionStep)
        {
            m_objActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_move;
            m_objActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fX = WK_PosNotLimit;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fY = WK_PosNotLimit;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fZ = m_stOilConfigPos.m_stCleanOilPos.m_fZ;
            m_objActionMotorXYZ->convertLogicPosToPhyPos(m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos,
                                                       m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos);
            m_objActionMotorXYZ->setTaskSend();
            emActionStep = emOilTest_WaitMoveToCleanPosZ;
            _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_MoveToCleanPosZ]");
        }
        else if(emOilTest_WaitMoveToCleanPosZ == emActionStep)
        {
            if((emSeluteDMoveXYZ_NoErr != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)
                &&(emSeluteDMoveXYZ_Outtime != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD))
            {
                break;//XYZm模块任务结论异常
            }
            if(emWorkStatus_finish == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)
            {
                emActionStep = emOilTest_MoveToCleanPosY;
                _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_WaitMoveToCleanPosZ]");
            }
            continue;
        }
        else if(emOilTest_MoveToCleanPosY == emActionStep)
        {
            m_objActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_move;
            m_objActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fX = WK_PosNotLimit;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fY = m_stOilConfigPos.m_stCleanOilPos.m_fY;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fZ = WK_PosNotLimit;
            m_objActionMotorXYZ->convertLogicPosToPhyPos(m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos,
                                                       m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos);
            m_objActionMotorXYZ->setTaskSend();
            emActionStep = emOilTest_WaitMoveToCleanPosY;
            _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_MoveToCleanPosY]");
        }
        else if(emOilTest_WaitMoveToCleanPosY == emActionStep)
        {
            if((emSeluteDMoveXYZ_NoErr != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)
                &&(emSeluteDMoveXYZ_Outtime != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD))
            {
                break;//XYZm模块任务结论异常
            }
            if(emWorkStatus_finish == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)
            {
                emActionStep = emOilTest_StartCleanPutPaper;
                _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_WaitMoveToCleanPosY]");
            }
            continue;
        }
        else if(emOilTest_StartCleanPutPaper == emActionStep)
        {
            m_objActionCleanOil->m_stTaskToSend.m_eTaskType = emTaskDCleanOilType_Clean;
            m_objActionCleanOil->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionCleanOil->setTaskSend();
            emActionStep = emOilTest_WaitCleanPutPaper;
            _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_StartCleanPutPaper]");
        }
        else if(emOilTest_WaitCleanPutPaper == emActionStep)
        {
            if((emSeluteDCleanOil_NoErr != m_objActionCleanOil->m_stTaskD.m_eTaskSeluteD)
                &&(emSeluteDCleanOil_Outtime != m_objActionCleanOil->m_stTaskD.m_eTaskSeluteD))
            {
                break;//除油模块任务结论异常
            }
            if(emWorkStatus_finish == m_objActionCleanOil->m_stTaskD.m_eTaskStatusD)
            {
                m_u64WaitSleepCount = 0;
                emActionStep = emOilTest_WaitCleanOilSomeTime;
                _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_WaitCleanPutPaper]");
            }
            continue;
        }
        else if(emOilTest_WaitCleanOilSomeTime == emActionStep)
        {
            m_u64WaitSleepCount++;
            if(m_u64WaitSleepCount >= WAIT_SLEEP_COUNT_MAX)
            {
                emActionStep = emOilTest_StartCleanFrapPaper;
            }
            continue;
        }
        else if(emOilTest_StartCleanFrapPaper == emActionStep)
        {
            m_objActionCleanOil->m_stTaskToSend.m_eTaskType = emTaskDCleanOilType_Frap;
            m_objActionCleanOil->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionCleanOil->setTaskSend();
            emActionStep = emOilTest_WaitCleanFrapPaper;
            _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_StartCleanFrapPaper]");
        }
        else if(emOilTest_WaitCleanFrapPaper == emActionStep)
        {
            if((emSeluteDCleanOil_NoErr != m_objActionCleanOil->m_stTaskD.m_eTaskSeluteD)
                &&(emSeluteDCleanOil_Outtime != m_objActionCleanOil->m_stTaskD.m_eTaskSeluteD))
            {
                break;//除油模块任务结论异常
            }
            if(emWorkStatus_finish == m_objActionCleanOil->m_stTaskD.m_eTaskStatusD)
            {
                emCurStage = emOilStage_DripOil;
                emActionStep = emOilTest_MoveToSafePosY;
                _LOG("{Oil_Auto_Test}: emActionStep [emOilTest_WaitCleanFrapPaper]");
            }
            continue;
        }
        else
        {
            break;
        }

    }
}
