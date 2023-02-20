#include "actionfourpostest.h"
#include "quihelper.h"
#include "global.h"
#include "xlsxdocument.h"

#define QDATETIMS qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))

ActionFourPosTest::ActionFourPosTest(ActionMotorXYZ* &objActionMotorXYZ,
                                     CeJuTcpClient* &objCeJuTcpClient, QObject *parent)
    : m_objActionMotorXYZ(objActionMotorXYZ),m_objCeJuTcpClient(objCeJuTcpClient),QThread(parent)
{
    m_eCurPoint = emPointPos_One;
    emActionStep = emFPTest_CejuConfigAdjust;
    m_bNeedStop = true;

    //默认取应用程序根目录
    path = qApp->applicationDirPath() + "/FourPointTest";
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

void ActionFourPosTest::InitFourPointTest()
{
    m_eCurPoint = emPointPos_One;
    emActionStep = emFPTest_CejuConfigAdjust;
}

void ActionFourPosTest::run()
{
    while(!m_bNeedStop)
    {
        msleep(500);
        if(emFPTest_CejuConfigAdjust == emActionStep)
        {
            m_objCeJuTcpClient->m_bIsFourPointCejuInitSucceed = false;
            emit signal_FourPoint_CejuInit();
            emActionStep = emFPTest_WaitCejuConfigAdjust;
            _LOG("{Four_Point_Test}: emActionStep [emFPTest_CejuConfigAdjust]");
            continue;
        }
        else if(emFPTest_WaitCejuConfigAdjust == emActionStep)
        {
            if(m_objCeJuTcpClient->m_bIsFourPointCejuInitSucceed)
            {
                emActionStep = emFPTest_MoveToSafePosY;
            }
            _LOG("{Follow_Auto_Test}: emActionStep [emFPTest_WaitCejuConfigAdjust]");
            continue;
        }
        else if(emFPTest_MoveToSafePosY == emActionStep)
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
                _LOG("{Four_Point_Test}: emActionStep [emFPTest_MoveToSafePosY]");
                emActionStep = emFPTest_WaitMoveToSafePosY;
            }
            continue;
        }
        else if(emFPTest_WaitMoveToSafePosY == emActionStep)
        {
            if((emSeluteDMoveXYZ_NoErr != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)
                &&(emSeluteDMoveXYZ_Outtime != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD))
            {
                break;//XYZm模块任务结论异常
            }
            if(emWorkStatus_finish == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)
            {
                emActionStep = emFPTest_MoveToTestPosZ;
                _LOG("{Four_Point_Test}: emActionStep [emFPTest_WaitMoveToSafePosY]");
            }
            continue;
        }
        else if(emFPTest_MoveToTestPosZ == emActionStep)
        {
            m_objActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_move;
            m_objActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32X = WK_PhyPosNotLimit;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Y = WK_PhyPosNotLimit;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Z = m_stAimPoint[emPointPos_One].m_i32Z;
            m_objActionMotorXYZ->convertPhyPosToLogicPos(m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos,
                                                       m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos);
            m_objActionMotorXYZ->setTaskSend();
            _LOG("{Four_Point_Test}: emActionStep [emFPTest_WaitMoveToTestPosZ]");
            emActionStep = emFPTest_WaitMoveToTestPosZ;
            continue;
        }
        else if(emFPTest_WaitMoveToTestPosZ == emActionStep)
        {
            if((emSeluteDMoveXYZ_NoErr != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)
                &&(emSeluteDMoveXYZ_Outtime != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD))
            {
                break;//XYZm模块任务结论异常
            }
            if(emWorkStatus_finish == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)
            {
                emActionStep = emFPTest_MoveToTestPointX;
                _LOG("{Four_Point_Test}: emActionStep [emFPTest_WaitMoveToTestPosZ]");
            }
            continue;
        }
        else if(emFPTest_MoveToTestPointX == emActionStep)
        {
            m_objActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_move;
            m_objActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32X = m_stAimPoint[m_eCurPoint].m_i32X;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Y = WK_PhyPosNotLimit;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Z = WK_PhyPosNotLimit;
            m_objActionMotorXYZ->convertPhyPosToLogicPos(m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos,
                                                       m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos);
            m_objActionMotorXYZ->setTaskSend();
            _LOG("{Four_Point_Test}: emActionStep [emFPTest_WaitMoveToTestPointX]");
            emActionStep = emFPTest_WaitMoveToTestPointX;
            continue;
        }
        else if(emFPTest_WaitMoveToTestPointX == emActionStep)
        {
            if((emSeluteDMoveXYZ_NoErr != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)
                &&(emSeluteDMoveXYZ_Outtime != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD))
            {
                break;//XYZm模块任务结论异常
            }
            if(emWorkStatus_finish == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)
            {
                emActionStep = emFPTest_MoveToTestPointY;
                _LOG("{Four_Point_Test}: emActionStep [emFPTest_WaitMoveToTestPointX]");
            }
            continue;
        }
        else if(emFPTest_MoveToTestPointY == emActionStep)
        {
            m_objActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_move;
            m_objActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32X = WK_PhyPosNotLimit;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Y = m_stAimPoint[m_eCurPoint].m_i32Y;
            m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Z = WK_PhyPosNotLimit;
            m_objActionMotorXYZ->convertPhyPosToLogicPos(m_objActionMotorXYZ->m_stTaskToSend.m_stAimDPos,
                                                       m_objActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos);
            m_objActionMotorXYZ->setTaskSend();
            _LOG("{Four_Point_Test}: emActionStep [emFPTest_MoveToTestPointY]");
            emActionStep = emFPTest_WaitMoveToTestPointY;
            continue;
        }
        else if(emFPTest_WaitMoveToTestPointY == emActionStep)
        {
            if((emSeluteDMoveXYZ_NoErr != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)
                &&(emSeluteDMoveXYZ_Outtime != m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD))
            {
                break;//XYZm模块任务结论异常
            }
            if(emWorkStatus_finish == m_objActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)
            {
                emActionStep = emFPTest_CejuRecordStart;
                _LOG("{Four_Point_Test}: emActionStep [emFPTest_WaitMoveToTestPointY]");
            }
            continue;
        }
        else if(emFPTest_CejuRecordStart == emActionStep)
        {
            m_objCeJuTcpClient->m_bIsOnePointRecordComplete = false;
            emit signal_Ceju_RecordStart(m_u16ReadNumPerPoint, m_u16ReadFreq);
            emActionStep = emFPTest_WaitCejuRecordEnd;
            _LOG("{Four_Point_Test}: emActionStep [emFPTest_CejuRecordStart]");
            continue;
        }
        else if(emFPTest_WaitCejuRecordEnd == emActionStep)
        {
            if(m_objCeJuTcpClient->m_bIsOnePointRecordComplete)
            {
                //在这里取测距数据记录excel
                fileName = QString("%1/%2_Pont%3_%4.xlsx").arg(path).arg(name).arg(m_eCurPoint).arg(QDATETIMS);
                //在内存中新建xlsx文件
                QXlsx::Document xlsx;
                xlsx_row = XLSX_ROW_LINE_TITLE;
                xlsx.write(xlsx_row, XLSX_COL_LINE_TASK1, QString("Task_1"));
                xlsx.write(xlsx_row, XLSX_COL_LINE_TASK2, QString("Task_2"));
                xlsx.write(xlsx_row, XLSX_COL_LINE_TASK3, QString("Task_3"));
                xlsx_row = XLSX_ROW_LINE_DATA;
                m_objCeJuTcpClient->Ceju_FourPoint_GetRecordData(m_i32CejuData, m_u16ActualNum);
                for(uint16_t i=0; i<m_u16ActualNum; i++)
                {
                    xlsx.write(xlsx_row, XLSX_COL_LINE_TASK1, m_i32CejuData[emCeJuDataTtype_task1][i]);
                    xlsx.write(xlsx_row, XLSX_COL_LINE_TASK2, m_i32CejuData[emCeJuDataTtype_task2][i]);
                    xlsx.write(xlsx_row, XLSX_COL_LINE_TASK3, m_i32CejuData[emCeJuDataTtype_task3][i]);
                    xlsx_row++;
                }
                xlsx.saveAs(fileName);

                //切换点位置
                if(emPointPos_One == m_eCurPoint)
                    m_eCurPoint = emPointPos_Two;
                else if(emPointPos_Two == m_eCurPoint)
                    m_eCurPoint = emPointPos_Three;
                else if(emPointPos_Three == m_eCurPoint)
                    m_eCurPoint = emPointPos_Four;
                else if(emPointPos_Four == m_eCurPoint)
                    m_eCurPoint = emPointPos_One;
                else
                    break;
                emActionStep = emFPTest_MoveToTestPointX;
            }
            _LOG("{Four_Point_Test}: emActionStep [emFPTest_WaitCejuRecordEnd]");
            continue;
        }
    }
}

void ActionFourPosTest::getPointSetInfo(ST_FOURPOINT_TEST_INFO &point)
{
    for(uint8_t i=emPointPos_One; i<emPointPos_End; i++)
    {
        m_stAimPoint[i].m_i32Z = point.m_i32Height;
    }
    m_stAimPoint[emPointPos_One].m_i32X = point.m_i32StPosX;
    m_stAimPoint[emPointPos_One].m_i32Y = point.m_i32StPosY;

    m_stAimPoint[emPointPos_Two].m_i32X = point.m_i32StPosX;
    m_stAimPoint[emPointPos_Two].m_i32Y = point.m_i32ToPosY;

    m_stAimPoint[emPointPos_Three].m_i32X = point.m_i32ToPosX;
    m_stAimPoint[emPointPos_Three].m_i32Y = point.m_i32ToPosY;

    m_stAimPoint[emPointPos_Four].m_i32X = point.m_i32ToPosX;
    m_stAimPoint[emPointPos_Four].m_i32Y = point.m_i32StPosY;

    m_u16PointT = point.m_u16PointT;
    m_u16ReadFreq = point.m_u16ReadFreq;
    m_u16ReadNumPerPoint = m_u16PointT / m_u16ReadFreq;
}
