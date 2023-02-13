#include "actionbfollowmove.h"
#include "quihelper.h"
#include "global.h"
#include "xlsxdocument.h"

#define QDATETIMS qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))

ActionBFollowMove::ActionBFollowMove(ActionTriggerSet* &objActionTriggerSet, ActionMotorXYZ* &objActionMotorXYZ,
                           CeJuTcpClient* &objCeJuTcpClient, ActionParameterSet* &objActionParameterSet, QObject *parent)
    : m_objActionTriggerSet(objActionTriggerSet),m_objActionMotorXYZ(objActionMotorXYZ),
      m_objCeJuTcpClient(objCeJuTcpClient),m_objActionParameterSet(objActionParameterSet),QThread(parent)
{
//    emActionStep = emFollow_FollowPrepare;
    emCurStage = emFStage_Move;
    //状态机测试
    emActionStep = emFollow_FollowPrepare;

    m_dTrailSyn[TRAIL_SYN_MAX_NUM] = {0.0};

    //默认取应用程序根目录
    path = qApp->applicationDirPath() + "/ExcelCejuData";
    //默认取应用程序可执行文件名称
    QString str = qApp->applicationFilePath();
    QStringList list = str.split("/");
    name = list.at(list.count() - 1).split(".").at(0);
    fileName = "";

    //目录不存在则先新建目录
   QDir dir(path);
   if (!dir.exists()) {
       dir.mkdir(path);
   }
}

void ActionBFollowMove::run()
{
    while(!m_bNeedStop)
    {
        msleep(500);
        if(emFollow_FollowPrepare == emActionStep)
        {
            fileName = QString("%1/%2_ExcelCejuData_%3.xlsx").arg(path).arg(name).arg(QDATETIMS);
            //在内存中新建xlsx文件
            QXlsx::Document xlsx;
            xlsx_row = XLSX_ROW_LINE_TITLE;
            xlsx.write(xlsx_row, XLSX_COL_LINE_TASK1, QString("Task_1"));
            xlsx.write(xlsx_row, XLSX_COL_LINE_TASK2, QString("Task_2"));
            xlsx.write(xlsx_row, XLSX_COL_LINE_TASK3, QString("Task_3"));
            xlsx.write(xlsx_row, XLSX_COL_LINE_CEJU1, QString("TrigPos_1"));
            xlsx.write(xlsx_row, XLSX_COL_LINE_CEJU2, QString("TrigPos_2"));
            xlsx.write(xlsx_row, XLSX_FOLLOW_COL_LINE_TASK1, QString("F_Task_1"));
            xlsx.write(xlsx_row, XLSX_FOLLOW_COL_LINE_TASK2, QString("F_Task_2"));
            xlsx.write(xlsx_row, XLSX_FOLLOW_COL_LINE_TASK3, QString("F_Task_3"));
            xlsx.write(xlsx_row, XLSX_FOLLOW_COL_LINE_CEJU1, QString("F_TrigPos_1"));
            xlsx.write(xlsx_row, XLSX_FOLLOW_COL_LINE_CEJU2, QString("F_TrigPos_2"));
            xlsx.write(xlsx_row, XLSX_FOLLOW_COL_LINE_TRAILSYNS, QString("TrailSyns"));
            xlsx.saveAs(fileName);
            emCurStage = emFStage_Move;
            emActionStep = emFollow_TrigParameterSet;
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_FollowPrepare]");
            continue;
        }
        else if(emFollow_TrigParameterSet == emActionStep)
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
                if(emFStage_Move == emCurStage)
                    emActionStep = emFollow_YMoveToEnd;
                else
                    emActionStep = emFollow_FollowMoveStart;
            }
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_WaitCejuRecordStart]");
            continue;
        }
        else if(emFollow_YMoveToEnd == emActionStep)
        {//定时移动设下去的时间以毫秒为单位,所以上位机在计算时也以毫秒为单位
            m_stMoveTrigData.m_u32AxisMoveLength = 0;
            m_stMoveTrigData.m_fMoveInTimeVMax = 0;
            m_stMoveTrigData.m_u32MoveInTimeTMin = 0;
            if(emAxis_Y == m_objActionTriggerSet->m_stTaskInfoD.m_eAxis)
            {
                m_stMoveTrigData.m_u32AxisMoveLength = abs(m_stAimStPhyPos.m_i32Y - m_stAimEdPhyPos.m_i32Y);  //单位是光栅
                m_stMoveTrigData.m_fMoveInTimeVMax = m_objActionTriggerSet->m_stTaskInfoD.m_u16Interval * 1.0 / (float)TRIG_SAMPLE_TEMP; //光栅/ms
                m_stMoveTrigData.m_u32MoveInTimeTMin = (uint32_t)(m_stMoveTrigData.m_u32AxisMoveLength * 1.0 / m_stMoveTrigData.m_fMoveInTimeVMax); //单位是ms
                trigPosNeedNum = m_stMoveTrigData.m_u32AxisMoveLength/m_objActionTriggerSet->m_stTaskInfoD.m_u16Interval/2;
            }
            m_objActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_moveInTime;
            m_objActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32X = m_stAimEdPhyPos.m_i32X;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Y = m_stAimEdPhyPos.m_i32Y;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Z = m_stAimEdPhyPos.m_i32Z;
            m_objActionMotorXYZ->m_stTaskToSend.m_u32MoveTime = m_stMoveTrigData.m_u32MoveInTimeTMin << 1;
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
                QXlsx::Document xlsx(fileName);
                xlsx_row = XLSX_ROW_LINE_DATA;
                if(emFStage_Move == emCurStage)
                {
                    m_objCeJuTcpClient->Ceju_GetRecordData(m_stAcionFollowData.m_i32CejuMoveStageData, m_stAcionFollowData.CejuTaskDataNum[emCurStage]);
                    for(uint16_t i=0; i<m_stAcionFollowData.CejuTaskDataNum[emCurStage]; i++)
                    {
                        xlsx.write(xlsx_row, XLSX_COL_LINE_TASK1, m_stAcionFollowData.m_i32CejuMoveStageData[emCeJuDataTtype_task1][i]);
                        xlsx.write(xlsx_row, XLSX_COL_LINE_TASK2, m_stAcionFollowData.m_i32CejuMoveStageData[emCeJuDataTtype_task2][i]);
                        xlsx.write(xlsx_row, XLSX_COL_LINE_TASK3, m_stAcionFollowData.m_i32CejuMoveStageData[emCeJuDataTtype_task3][i]);
                        xlsx_row++;
                    }
                }
                else
                {
                    m_objCeJuTcpClient->Ceju_GetRecordData(m_stAcionFollowData.m_i32CejuFollowStageData, m_stAcionFollowData.CejuTaskDataNum[emCurStage]);
                    for(uint16_t i=0; i<m_stAcionFollowData.CejuTaskDataNum[emCurStage]; i++)
                    {
                        xlsx.write(xlsx_row, XLSX_FOLLOW_COL_LINE_TASK1, m_stAcionFollowData.m_i32CejuFollowStageData[emCeJuDataTtype_task1][i]);
                        xlsx.write(xlsx_row, XLSX_FOLLOW_COL_LINE_TASK2, m_stAcionFollowData.m_i32CejuFollowStageData[emCeJuDataTtype_task2][i]);
                        xlsx.write(xlsx_row, XLSX_FOLLOW_COL_LINE_TASK3, m_stAcionFollowData.m_i32CejuFollowStageData[emCeJuDataTtype_task3][i]);
                        xlsx_row++;
                    }
                }
                xlsx.saveAs(fileName);
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
                QXlsx::Document xlsx(fileName);
                xlsx_row = XLSX_ROW_LINE_DATA;
                if(emFStage_Move == emCurStage)
                {
                    m_objActionTriggerSet->Trig_GetTrigPosData(m_stAcionFollowData.m_i32MoveStageTrigPos, m_stAcionFollowData.m_u32TrigPosNum[emCurStage]);
                    for(uint16_t i=0; i<m_stAcionFollowData.m_u32TrigPosNum[emCurStage]; i++)
                    {
                        xlsx.write(xlsx_row, XLSX_COL_LINE_CEJU1, m_stAcionFollowData.m_i32MoveStageTrigPos[0][i]);
                        xlsx.write(xlsx_row, XLSX_COL_LINE_CEJU2, m_stAcionFollowData.m_i32MoveStageTrigPos[1][i]);
                        xlsx_row++;
                    }
                }
                else
                {
                    m_objActionTriggerSet->Trig_GetTrigPosData(m_stAcionFollowData.m_i32FollowStageTrigPos, m_stAcionFollowData.m_u32TrigPosNum[emCurStage]);
                    for(uint16_t i=0; i<m_stAcionFollowData.m_u32TrigPosNum[emCurStage]; i++)
                    {
                        xlsx.write(xlsx_row, XLSX_FOLLOW_COL_LINE_CEJU1, m_stAcionFollowData.m_i32FollowStageTrigPos[0][i]);
                        xlsx.write(xlsx_row, XLSX_FOLLOW_COL_LINE_CEJU2, m_stAcionFollowData.m_i32FollowStageTrigPos[1][i]);
                        xlsx_row++;
                    }
                }
                xlsx.saveAs(fileName);
                emActionStep = emFollow_TrigInfoManage;
            }
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_WaitGetTrigInfo]");
            continue;
        }
        else if(emFollow_TrigInfoManage == emActionStep)
        {
            if(emFStage_Move == emCurStage)
            {
//                m_stDealData_Original.m_uTotalNum
//                        = (m_stAcionFollowData.CejuTaskDataNum[emFStage_Move] >= m_stAcionFollowData.m_u32TrigPosNum[emFStage_Move]) ? m_stAcionFollowData.m_u32TrigPosNum[emFStage_Move]:m_stAcionFollowData.CejuTaskDataNum[emFStage_Move];
//                memcpy(m_stDealData_Original.m_iPos, &m_stAcionFollowData.m_i32MoveStageTrigPos[0][0], m_stDealData_Original.m_uTotalNum*sizeof(int32_t));
//                memcpy(m_stDealData_Original.m_iPosZ, &m_stAcionFollowData.m_i32MoveStageTrigPos[1][0], m_stDealData_Original.m_uTotalNum*sizeof(int32_t));
//                memcpy(m_stDealData_Original.m_iH1, &m_stAcionFollowData.m_i32CejuMoveStageData[emCeJuDataTtype_task1][0], m_stDealData_Original.m_uTotalNum*sizeof(int32_t));
//                memcpy(m_stDealData_Original.m_iH2, &m_stAcionFollowData.m_i32CejuMoveStageData[emCeJuDataTtype_task2][0], m_stDealData_Original.m_uTotalNum*sizeof(int32_t));
//                memcpy(m_stDealData_Original.m_iH3, &m_stAcionFollowData.m_i32CejuMoveStageData[emCeJuDataTtype_task3][0], m_stDealData_Original.m_uTotalNum*sizeof(int32_t));
//                selectCases(m_stDealData_Original, m_stDealData_Filter);
                m_stDealData_Original = new stCejuTempInfo;
                m_stDealData_Original->m_uTotalNum
                        = (m_stAcionFollowData.CejuTaskDataNum[emFStage_Move] >= m_stAcionFollowData.m_u32TrigPosNum[emFStage_Move]) ? m_stAcionFollowData.m_u32TrigPosNum[emFStage_Move]:m_stAcionFollowData.CejuTaskDataNum[emFStage_Move];
                memcpy(m_stDealData_Original->m_iPos, &m_stAcionFollowData.m_i32MoveStageTrigPos[0][0], m_stDealData_Original->m_uTotalNum*sizeof(int32_t));
                memcpy(m_stDealData_Original->m_iPosZ, &m_stAcionFollowData.m_i32MoveStageTrigPos[1][0], m_stDealData_Original->m_uTotalNum*sizeof(int32_t));
                memcpy(m_stDealData_Original->m_iH1, &m_stAcionFollowData.m_i32CejuMoveStageData[emCeJuDataTtype_task1][0], m_stDealData_Original->m_uTotalNum*sizeof(int32_t));
                memcpy(m_stDealData_Original->m_iH2, &m_stAcionFollowData.m_i32CejuMoveStageData[emCeJuDataTtype_task2][0], m_stDealData_Original->m_uTotalNum*sizeof(int32_t));
                memcpy(m_stDealData_Original->m_iH3, &m_stAcionFollowData.m_i32CejuMoveStageData[emCeJuDataTtype_task3][0], m_stDealData_Original->m_uTotalNum*sizeof(int32_t));
                selectCases(*m_stDealData_Original, m_stDealData_Filter);
                free(m_stDealData_Original);
                m_stDealData_Original = NULL;
                polyfit(m_stDealData_Filter.m_uTotalNum, m_stDealData_Filter.m_iPos, m_stDealData_Filter.m_iPosZ, WK_SycNum_FactUse, m_dTrailSyn);
                QXlsx::Document xlsx(fileName);
                xlsx_row = XLSX_ROW_LINE_DATA;
                for(uint8_t i=0; i<WK_SycNum_FactUse; i++)
                {
                    xlsx.write(xlsx_row, XLSX_FOLLOW_COL_LINE_TRAILSYNS, m_dTrailSyn[i]);
                    xlsx_row++;
                }
                xlsx.saveAs(fileName);

    //            polyfit(trigPosNeedNum, &m_objActionTriggerSet->m_stTrigInfoD.m_i32TrigPosArray[0][0],
    //                    &m_objActionTriggerSet->m_stTrigInfoD.m_i32TrigPosArray[1][0], 6, m_fTrailCoefficient);
                emActionStep = emFollow_FollowParameterSet;
//                emActionStep = emFollow_FollowMoveExit;
            }
            else
            {
                emActionStep = emFollow_FollowMoveExit;
            }
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_TrigInfoManage]");
            continue;
        }
        else if(emFollow_FollowParameterSet == emActionStep)
        {
            m_objActionParameterSet->m_stTrailInfoToSend.u16TrailId = 6;
            m_objActionParameterSet->m_stTrailInfoToSend.u16TrailSynNum = WK_SycNum_FactUse;
            for(uint8_t i=0; i < WK_SycNum_FactUse; i++)
            {
                m_objActionParameterSet->m_stTrailInfoToSend.udSyn[i] = m_dTrailSyn[i];
            }
            m_objActionParameterSet->setFollowParameterSend();
            emActionStep = emFollow_WaitFollowParameterSet;
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_FollowParameterSet]");
            continue;
        }
        else if(emFollow_WaitFollowParameterSet == emActionStep)
        {
            if(m_objActionParameterSet->m_bISTrailInfoSet)
            {
                emCurStage = emFStage_Follow;
                emActionStep = emFollow_YMoveToStart;
            }
            else
                emActionStep = emFollow_FollowParameterSet;
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_WaitFollowParameterSet]");
            continue;
        }
        else if(emFollow_FollowMoveStart == emActionStep)
        {//跟随移动
            m_objActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_moveTogether;
            m_objActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32X = m_stAimEdPhyPos.m_i32X;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Y = m_stAimEdPhyPos.m_i32Y;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Z = m_stAimEdPhyPos.m_i32Z;
            m_objActionMotorXYZ->m_stTaskToSend.m_u32MoveTime = m_stMoveTrigData.m_u32MoveInTimeTMin << 1;
            m_objActionMotorXYZ->convertPhyPosToLogicPos(m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos,
                                                       m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos);
            m_objActionMotorXYZ->setTaskSend();
            emActionStep = emFollow_WaitFollowMoveStartEnd;
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_FollowMoveStart]");
            continue;
        }
        else if(emFollow_WaitFollowMoveStartEnd == emActionStep)
        {
            if((emTskDXYZType_moveTogether == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskTypeD) &&
                    ((emSeluteDMoveXYZ_NoErr == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)||(emSeluteDMoveXYZ_Outtime == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)) &&
                    (emWorkStatus_finish == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD))
            {
                emActionStep = emFollow_CejuRecordEnd;
                _LOG("{Follow_Auto_Test}: emActionStep [emFollow_WaitFollowMoveStartEnd]");
            }
            continue;
        }
        else
        {
            emActionStep = emFollow_FollowPrepare;//退出线程前，复位状态机的初始值
            _LOG("{Follow_Auto_Test}: emActionStep [emFollow_FollowMoveExit]");
            break;
        }
    }
}

void ActionBFollowMove::polyfit(int n, int x[], int y[], int poly_n, double p[])
{
    int i, j;
    double *tempx, *tempy, *sumxx, *sumxy, *ata;

    tempx = (double *)calloc(n, sizeof(double));
    sumxx = (double *)calloc((poly_n * 2 + 1), sizeof(double));
    tempy = (double *)calloc(n, sizeof(double));
    sumxy = (double *)calloc((poly_n + 1), sizeof(double));
    ata = (double *)calloc((poly_n + 1)*(poly_n + 1), sizeof(double));
    for (i = 0; i<n; i++)
    {
        tempx[i] = 1;
        tempy[i] = y[i];
    }
    for (i = 0; i<2 * poly_n + 1; i++)
    {
        for (sumxx[i] = 0, j = 0; j<n; j++)
        {
            sumxx[i] += tempx[j];
            tempx[j] *= x[j];
        }
    }
    for (i = 0; i<poly_n + 1; i++)
    {
        for (sumxy[i] = 0, j = 0; j<n; j++)
        {
            sumxy[i] += tempy[j];
            tempy[j] *= x[j];
        }
    }
    for (i = 0; i<poly_n + 1; i++)
    {
        for (j = 0; j<poly_n + 1; j++)
        {
            ata[i*(poly_n + 1) + j] = sumxx[i + j];
        }
    }
    gauss_solve(poly_n + 1, ata, p, sumxy);

    free(tempx);
    free(sumxx);
    free(tempy);
    free(sumxy);
    free(ata);
}

void ActionBFollowMove::gauss_solve(int n, double A[], double x[], double b[])
{
    int i, j, k, r;
    double max;
    for (k = 0; k<n - 1; k++)
    {
        max = fabs(A[k*n + k]);					// find maxmum
        r = k;
        for (i = k + 1; i<n - 1; i++)
        {
            if (max<fabs(A[i*n + i]))
            {
                max = fabs(A[i*n + i]);
                r = i;
            }
        }
        if (r != k)
        {
            for (i = 0; i<n; i++)		//change array:A[k]&A[r]
            {
                max = A[k*n + i];
                A[k*n + i] = A[r*n + i];
                A[r*n + i] = max;
            }
            max = b[k];                    //change array:b[k]&b[r]
            b[k] = b[r];
            b[r] = max;
        }

        for (i = k + 1; i<n; i++)
        {
            for (j = k + 1; j<n; j++)
                A[i*n + j] -= A[i*n + k] * A[k*n + j] / A[k*n + k];
            b[i] -= A[i*n + k] * b[k] / A[k*n + k];
        }
    }

    for (i = n - 1; i >= 0; x[i] /= A[i*n + i], i--)
    {
        for (j = i + 1, x[i] = b[i]; j<n; j++)
            x[i] -= A[i*n + j] * x[j];
    }
}

//产生厚度0 和 厚度2 x与h1的关系拟合线
//数据筛选函数步骤：1.筛去无效值；2.筛去超过厚度均值范围20微米；3.筛选拟合值上下5微米；4.循环删除超出拟合线范围1微米左右的值
//输入参数：m_stCejuEdge->测距的漆面信息 m_stCeju->筛选后数据 uIndex->漆面序号
void ActionBFollowMove::selectCases(stCejuTempInfo m_stCejuEdge, stCejuTempInfo &m_stCeju)
{
    //**************************************************************************************
    ///************************************数据筛选部分*************************************/
    //**************************************************************************************

    /*********************第一步：丢弃第一表面或第三表面无效值*********************/
    uint32_t validNum = 0;
    double thick_avg = 0.0;//厚度均值
    for (uint32_t i = 0; i < m_stCejuEdge.m_uTotalNum; i++)
    {
        if (WK_DEFAULT_TrailData == m_stCejuEdge.m_iH1[i] || WK_DEFAULT_TrailData == m_stCejuEdge.m_iH3[i])
        {
            continue;
        }
        m_stCeju.m_iH1[validNum] = m_stCejuEdge.m_iH1[i];
        m_stCeju.m_iH2[validNum] = m_stCejuEdge.m_iH2[i];
        m_stCeju.m_iH3[validNum] = m_stCejuEdge.m_iH3[i];
        m_stCeju.m_iPos[validNum] = m_stCejuEdge.m_iPos[i];
        m_stCeju.m_iPosZ[validNum] = m_stCejuEdge.m_iPosZ[i];
        //计算厚度
        double thick = m_stCeju.m_iH1[validNum] - m_stCeju.m_iH3[validNum];
        thick_avg += thick;
        validNum++;
    }
    thick_avg /= validNum;/*计算厚度均值*/
    m_stCeju.m_uTotalNum = validNum;
    //输出第一步数据
    validNum = 0;


    /********************第二步：筛选均值上下20000筛选数据******************/
    for (uint32_t i = 0; i < m_stCeju.m_uTotalNum; i++)
    {
        if (thick_avg + WK_THICK_AVG_RANGE < m_stCeju.m_iH1[i] - m_stCeju.m_iH3[i]
            || thick_avg - WK_THICK_AVG_RANGE > m_stCeju.m_iH1[i] - m_stCeju.m_iH3[i])
        {
            continue;
        }
        m_stCeju.m_iH1[validNum] = m_stCeju.m_iH1[i];
        m_stCeju.m_iH2[validNum] = m_stCeju.m_iH2[i];
        m_stCeju.m_iH3[validNum] = m_stCeju.m_iH3[i];
        m_stCeju.m_iPos[validNum] = m_stCeju.m_iPos[i];
        m_stCeju.m_iPosZ[validNum] = m_stCeju.m_iPosZ[i];
        validNum++;
    }
    m_stCeju.m_uTotalNum = validNum;
    validNum = 0;

    /****************第三步：拟合厚度三次项，筛去超过拟合线上下5000**************/
    //1.对厚度进行拟合
    int RealNum = 0;
    int XOut[WK_CeJuRecordNumMax] = { 0 };
    int YOut[WK_CeJuRecordNumMax] = { 0 };
    double m_poly[TRAIL_SYN_MAX_NUM] = { 0.0 };
    for (uint32_t i = 0; i < m_stCeju.m_uTotalNum; i++)
    {
        XOut[RealNum] = m_stCeju.m_iPos[i];
        YOut[RealNum] = m_stCeju.m_iH1[i] - m_stCeju.m_iH3[i];
        RealNum++;
    }
    polyfit(RealNum, XOut, YOut, 3, m_poly);//拟合厚度曲线
    for (uint32_t i = 0; i < m_stCeju.m_uTotalNum; i++)
    {
        double thick_calcu = polyCalculate(m_stCeju.m_iPos[i], 3, m_poly);//厚度回算
        if ((m_stCeju.m_iH1[i] - m_stCeju.m_iH3[i]) > thick_calcu + WK_THICK_FIT_RANGE
            || (m_stCeju.m_iH1[i] - m_stCeju.m_iH3[i]) < thick_calcu - WK_THICK_FIT_RANGE)
        {
            continue;
        }
        m_stCeju.m_iH1[validNum] = m_stCeju.m_iH1[i];
        m_stCeju.m_iH2[validNum] = m_stCeju.m_iH2[i];
        m_stCeju.m_iH3[validNum] = m_stCeju.m_iH3[i];
        m_stCeju.m_iPos[validNum] = m_stCeju.m_iPos[i];
        m_stCeju.m_iPosZ[validNum] = m_stCeju.m_iPosZ[i];
        validNum++;
    }
    m_stCeju.m_uTotalNum = validNum;

    /*******************第四步：拟合三次项，筛去超过拟合线上下1000******************/
    int InvalidData = 0;//筛去无效值数量
    do
    {
        validNum = 0;
        InvalidData = 0;
        //1.对厚度进行拟合
        RealNum = 0;
        int X[WK_CeJuRecordNumMax] = { 0 };
        int Y[WK_CeJuRecordNumMax] = { 0 };
        double m_poly[TRAIL_SYN_MAX_NUM] = { 0.0 };
        for (uint32_t j = 0; j < m_stCeju.m_uTotalNum; j++)
        {
            X[RealNum] = m_stCeju.m_iPos[j];
            Y[RealNum] = m_stCeju.m_iH1[j] - m_stCeju.m_iH3[j];
            RealNum++;
        }
        polyfit(RealNum, X, Y, 3, m_poly);//拟合厚度曲线
        //2.计算厚度回算,并筛选超过拟合线上下1000
        for (uint32_t m = 0; m < m_stCeju.m_uTotalNum; m++)
        {
            double thick_calcu = polyCalculate(m_stCeju.m_iPos[m], 3, m_poly);
            if ((m_stCeju.m_iH1[m] - m_stCeju.m_iH3[m]) > thick_calcu + WK_THICK_FIT_AGAIN_RANGE
                || (m_stCeju.m_iH1[m] - m_stCeju.m_iH3[m]) < thick_calcu - WK_THICK_FIT_AGAIN_RANGE)
            {
                InvalidData++;
                continue;
            }
            m_stCeju.m_iH1[validNum] = m_stCeju.m_iH1[m];
            m_stCeju.m_iH2[validNum] = m_stCeju.m_iH2[m];
            m_stCeju.m_iH3[validNum] = m_stCeju.m_iH3[m];
            m_stCeju.m_iPos[validNum] = m_stCeju.m_iPos[m];
            m_stCeju.m_iPosZ[validNum] = m_stCeju.m_iPosZ[m];
            validNum++;
        }
        m_stCeju.m_uTotalNum = validNum;
    } while (0 != InvalidData);
    //输出第四步数据

    ////**************************************************************************************
    /////**********************************拟合第一表面轨迹***********************************/
    ////**************************************************************************************
    //RealNum = 0;
    //int X[WK_CeJuRecordNumMax] = { 0 };
    //int Y[WK_CeJuRecordNumMax] = { 0 };
    //for (WK_UINT32 j = 0; j < m_stEdge[0].m_uTotalNum; j++)
    //{
    //	X[RealNum] = m_stEdge[0].m_iPos[j];
    //	Y[RealNum] = m_stEdge[0].m_iH1[j];

    //	RealNum++;
    //}
    ////拟合数据
    //fncCellDstrbtPcnt::polyfit(RealNum, X, Y, WK_POLY_N, poly);
}

float ActionBFollowMove::polyCalculate(float x, int poly_n, double p[])
{
    float ret = p[0];//最终结果
    float retTemp;//n次项结果
    for (int i = 1; i <= poly_n; i++)
    {
        retTemp = p[i];
        for (int j = 0; j < i; j++)
        {
            retTemp *= x;
        }
        ret += retTemp;
    }
    return ret;
}
