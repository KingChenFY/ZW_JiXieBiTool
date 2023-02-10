#include "actionbfollowmove.h"
#include "quihelper.h"
#include "global.h"
#include "xlsxdocument.h"

ActionBFollowMove::ActionBFollowMove(ActionTriggerSet* &objActionTriggerSet, ActionMotorXYZ* &objActionMotorXYZ,
                           CeJuTcpClient* &objCeJuTcpClient, QObject *parent)
    : m_objActionTriggerSet(objActionTriggerSet),m_objActionMotorXYZ(objActionMotorXYZ),
      m_objCeJuTcpClient(objCeJuTcpClient),QThread(parent)
{
    emActionStep = emFollow_TrigParameterSet;
}

void ActionBFollowMove::run()
{
    while(!m_bNeedStop)
    {
        msleep(500);
        if(emFollow_TrigParameterSet == emActionStep)
        {
            m_objActionTriggerSet->setTaskSend();
            emActionStep = emFollow_WaitTrigParameterSet;
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_TrigParameterSet]");
            continue;
        }
        else if(emFollow_WaitTrigParameterSet == emActionStep)
        {
            if( (m_objActionTriggerSet->m_stTaskToSend.m_eAxis == m_objActionTriggerSet->m_stTaskInfoD.m_eAxis) &&
                    (m_objActionTriggerSet->m_stTaskToSend.m_u16Interval == m_objActionTriggerSet->m_stTaskInfoD.m_u16Interval) &&
                    (m_objActionTriggerSet->m_stTaskToSend.m_u8TrigObj == m_objActionTriggerSet->m_stTaskInfoD.m_u8TrigObj) )
            {//等待硬件测距触发参数设置完成
                emActionStep = emFollow_YMoveToStart;
                _LOG("{Follow_Auto_Test}: emActionStep [emFollow_WaitTrigParameterSet]");
            }
            continue;
        }
        else if(emFollow_YMoveToStart == emActionStep)
        {
            m_objActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_move;
            m_objActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32X = m_stAimStPhyPos.m_i32X;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Y = m_stAimStPhyPos.m_i32Y;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Z = m_stAimStPhyPos.m_i32Z;
            m_objActionMotorXYZ->convertPhyPosToLogicPos(m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos,
                                                       m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos);
            m_objActionMotorXYZ->setTaskSend();
            emActionStep = emFollow_WaitYMoveToStart;
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_YMoveToStart]");
            continue;
        }
        else if(emFollow_WaitYMoveToStart == emActionStep)
        {
            if((emTskDXYZType_move == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskTypeD) &&
                    ((emSeluteDMoveXYZ_NoErr == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)||(emSeluteDMoveXYZ_Outtime == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)) &&
                    (emWorkStatus_finish == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD))
            {
                emActionStep = emFollow_CejuRecordStart;
                _LOG("{Follow_Auto_Test}: emActionStep [emFollow_WaitYMoveToStart]");
            }
            continue;
        }
        else if(emFollow_CejuRecordStart == emActionStep)
        {
            qDebug() << "BFollow thread11:" << QThread::currentThread();
//            m_objCeJuTcpClient->Ceju_RecordStart();
            m_objCeJuTcpClient->m_bIsCejuRecordStartSucceed = false;
            emit signal_Ceju_RecordStart();
            emActionStep = emFollow_WaitCejuRecordStart;
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_CejuRecordStart]");
            continue;
        }
        else if(emFollow_WaitCejuRecordStart == emActionStep)
        {
            if(m_objCeJuTcpClient->m_bIsCejuRecordStartSucceed)
            {
                emActionStep = emFollow_YMoveToEnd;
            }
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_WaitCejuRecordStart]");
            continue;
        }
        else if(emFollow_YMoveToEnd == emActionStep)
        {//定时移动设下去的时间以毫秒为单位,所以上位机在计算时也以毫秒为单位
            int32_t AxisMoveLength = 0;
            float MoveInTimeVMax = 0;
            uint32_t MoveInTimeTMin = 0;
            if(emAxis_Y == m_objActionTriggerSet->m_stTaskInfoD.m_eAxis)
            {
                AxisMoveLength = abs(m_stAimStPhyPos.m_i32Y - m_stAimEdPhyPos.m_i32Y);  //单位是光栅
                MoveInTimeVMax = m_objActionTriggerSet->m_stTaskInfoD.m_u16Interval * 1.0 / (float)TRIG_SAMPLE_TEMP; //光栅/ms
                MoveInTimeTMin = (uint32_t)(AxisMoveLength * 1.0 / MoveInTimeVMax); //单位是ms
                trigPosNeedNum = AxisMoveLength/m_objActionTriggerSet->m_stTaskInfoD.m_u16Interval/2;
            }
            m_objActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_moveInTime;
            m_objActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32X = m_stAimEdPhyPos.m_i32X;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Y = m_stAimEdPhyPos.m_i32Y;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Z = m_stAimEdPhyPos.m_i32Z;
            m_objActionMotorXYZ->m_stTaskToSend.m_u32MoveTime = MoveInTimeTMin << 1;
            m_objActionMotorXYZ->convertPhyPosToLogicPos(m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos,
                                                       m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos);
            m_objActionMotorXYZ->setTaskSend();
            emActionStep = emFollow_WaitYMoveToEnd;
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_YMoveToEnd]");
            continue;
        }
        else if(emFollow_WaitYMoveToEnd == emActionStep)
        {
            if((emTskDXYZType_moveInTime == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskTypeD) &&
                    ((emSeluteDMoveXYZ_NoErr == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)||(emSeluteDMoveXYZ_Outtime == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)) &&
                    (emWorkStatus_finish == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD))
            {
                emActionStep = emFollow_CejuRecordEnd;
                _LOG("{Follow_Auto_Test}: emActionStep [emFollow_WaitYMoveToEnd]");
            }
            continue;
        }
        else if(emFollow_CejuRecordEnd == emActionStep)
        {
            m_objCeJuTcpClient->m_bIsCejuRecordEndSucceed = false;
            emit signal_Ceju_RecordEnd();
//            m_objCeJuTcpClient->Ceju_RecordEnd();
            emActionStep = emFollow_WaitCejuRecordEnd;
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_CejuRecordEnd]");
            continue;
        }
        else if(emFollow_WaitCejuRecordEnd == emActionStep)
        {
            if(m_objCeJuTcpClient->m_bIsCejuRecordEndSucceed)
            {
                emActionStep = emFollow_GetTrigInfo;
            }
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_WaitCejuRecordEnd]");
            continue;
        }
        else if(emFollow_GetTrigInfo == emActionStep)
        {
            m_objActionTriggerSet->m_stBFollowTrigSetParameter.m_u16NeedTrigPosNum = trigPosNeedNum;
            m_objActionTriggerSet->m_stBFollowTrigSetParameter.m_bIsGetAllNeedNum = false;
            m_objActionTriggerSet->m_stTrigInfoGet_TaskToSend.m_u8NeedNum = GET_TRIGPOS_PER_MAX;
            m_objActionTriggerSet->m_stTrigInfoGet_TaskToSend.m_u16StartIndex = 0;
            m_objActionTriggerSet->getTrigInfoTaskSend();
            emActionStep = emFollow_WaitGetTrigInfo;
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_GetTrigInfo]");
            continue;
        }
        else if(emFollow_WaitGetTrigInfo == emActionStep)
        {
            if(m_objActionTriggerSet->m_stBFollowTrigSetParameter.m_bIsGetAllNeedNum)
            {
                emActionStep = emFollow_TrigInfoManage;
            }
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_WaitGetTrigInfo]");
            continue;
        }
        else if(emFollow_TrigInfoManage == emActionStep)
        {
            QXlsx::Document xlsx(m_objCeJuTcpClient->fileName);
            xlsx.write(1, XLSX_COL_LINE_CEJU1, QString("ceju_1"));
            xlsx.write(1, XLSX_COL_LINE_CEJU2, QString("ceju_2"));
            uint16_t xlsx_row = 2;
            for(uint16_t i=0; i<trigPosNeedNum; i++)
            {
                xlsx.write(xlsx_row, XLSX_COL_LINE_CEJU1, m_objActionTriggerSet->m_stTrigInfoD.m_i32TrigPosArray[0][i]);
                xlsx.write(xlsx_row, XLSX_COL_LINE_CEJU2, m_objActionTriggerSet->m_stTrigInfoD.m_i32TrigPosArray[1][i]);
                xlsx_row++;
            }
            xlsx.saveAs(m_objCeJuTcpClient->fileName);
            emActionStep = emFollow_FollowMoveExit;
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_TrigInfoManage]");
            continue;
        }
        else
        {
            emActionStep = emFollow_TrigParameterSet;//退出线程前，复位状态机的初始值
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_FollowMoveExit]");
            break;
        }
    }
}
