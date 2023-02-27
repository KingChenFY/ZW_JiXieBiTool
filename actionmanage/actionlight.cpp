#include "actionlight.h"
#include <QRandomGenerator>
#include "global.h"

#include "head.h"

extern SetTaskRingFifo stTaskFifo;
//extern GetTaskLinkList gtTaskLinkList;
int ActionLight::timeOutValue_setTaskReSend = 10000;
uint32_t ActionLight::u32MaxReadNum_to_reSend = 20;

ActionLight::ActionLight(QObject *parent)
    : QObject{parent}
{
    m_stTaskD.m_eTaskStatusD = emWorkStatus_start;
    m_stTaskD.m_eTaskSeluteD = emSeluteDSetLED_start;
    m_stTaskToSend.m_eTaskType = emTaskDSetLED_Start;

    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_setBrightTask, this);
    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_getBrightTaskInfo, this);

    //信号关联
    connect(&timer_setTaskReSend, SIGNAL(timeout()), this, SLOT(slot_timer_setTaskReSend()));
    connect(this, SIGNAL(signal_SetTask_ReSend()), this, SLOT(setTaskSend()), Qt::QueuedConnection);
}

emWKCmdType ActionLight::parseCmd(uint8_t* puData)
{
    uint8_t uCmdId = puData[CMD_ID_INDEX];
    uint32_t uReturnFlag = common_read_u32(&puData[CMD_FLAG_INDEX]);

    if (EnumBoardId_setBrightTask == uCmdId)
    {
        // 向gettask链表中压入查询指令
//        getTaskSend();
        return emCMD_SET;
    }

    if (EnumBoardId_getBrightTaskInfo == uCmdId)
    {
        uint8_t uLen = CMD_CONTENT_INDEX;

        /* 返回：eTskType(U8 emTskDBrightType) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDBright)
                + IsTri(U8)+ TriTime(U16) + BrightLevel(U16) + IsTri(U8)+ TriTime(U16) + BrightLevel(U16)  */
        m_stTaskD.m_eTaskTypeD = (emTaskDSetLEDType)puData[uLen];
        uLen += 1;

//        m_stTaskD.m_uTaskId = puData[uLen];
//        uLen += 1;

        // 任务状态
        m_stTaskD.m_eTaskStatusD = (emWorkStatus)puData[uLen];
        uLen += 1;

        //任务结果
        m_stTaskD.m_eTaskSeluteD = (emSeluteDBright)puData[uLen];
        uLen += 1;

        m_stTaskD.m_uIsTri1 = puData[uLen];
        uLen += 1;
        m_stTaskD.m_u16Tri1Time = common_read_u16(&puData[uLen]);
        uLen += 2;
        m_stTaskD.m_u16Level1 = common_read_u16(&puData[uLen]);
        uLen += 2;
        m_stTaskD.m_uIsTri2 = puData[uLen];
        uLen += 1;
        m_stTaskD.m_u16Tri2Time = common_read_u16(&puData[uLen]);
        uLen += 2;
        m_stTaskD.m_u16Level2 = common_read_u16(&puData[uLen]);
        uLen += 2;

        // 更新 光源 界面信息
        emit signal_UiUpdate();
        // 如果任务是finish的，在链表中查找对应CmdFlag的结点删除，没找到，丢弃该数据
//        if(emWorkStatus_finish == m_stTaskD.m_eTaskStatusD)
//        {
//            GetTaskLinkList::GetInstance().app_deleteNodeWithTaskId(EnumBoardObject_Light, m_stTaskD.m_uTaskId);
////            gtTaskLinkList.app_deleteNodeWithTaskId(EnumBoardObject_BeltOut, m_stTaskD.m_uTaskId);
////            qDebug() << QString("ActionBeltOut::parseCmd, ID:[%1], task finish").arg(m_stTaskD.m_uTaskId) << TIMEMS;
//        }
        return emCMD_GET;
    }
//    qDebug() << "[ActionBeltOut]parseCmd Not found Cmd:" << uCmdId << ",not do parse\r\n";
}

void ActionLight::setTaskSend()
{
    uint16_t i = 0;
    STRUCT_SETTASK_MESSAGE_INFO *pWriteAddr;

    //构造settask指令内容 到 发送队列
    pWriteAddr = stTaskFifo.getWriteMessageAddr(EnumBoardId_setBrightTask);
    /* 请求： eTskType(U8 emTskDBeltOutType) + uTskId(U8)*/
    pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTaskToSend.m_eTaskType;
//    pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTaskToSend.m_uTaskId;
    pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTaskToSend.m_uIsTri1;
    common_write_u16 (&pWriteAddr->u8CmdContent[i], m_stTaskToSend.m_u16Tri1Time);
    i += 2;
    common_write_u16 (&pWriteAddr->u8CmdContent[i], m_stTaskToSend.m_u16Level1);
    i += 2;
    pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTaskToSend.m_uIsTri2;
    common_write_u16 (&pWriteAddr->u8CmdContent[i], m_stTaskToSend.m_u16Tri2Time);
    i += 2;
    common_write_u16 (&pWriteAddr->u8CmdContent[i], m_stTaskToSend.m_u16Level2);
    i += 2;
    pWriteAddr->u16CmdContentLen = i;

    //压入发送队列前，开启重发定时器，如果本来就开着会重启定时器
//    timer_setTaskReSend.start(timeOutValue_setTaskReSend);
    stTaskFifo.pushWriteMessageToFifo();
}

/*
 * 收到set指令的回复触发，往链表中push查询指令
 * 搜索该链表中有没有指令ID一样的结点，有的话直接删除，表示该模块的任务切换了，用户不关心之前指令ID对应的任务。
 * 然后添加新的结点，该结点带有指令ID和CmdFlag数据
 */
void ActionLight::getTaskSend()
{
    uint8_t getCmdData[WK_NET_ONE_MESSAGE_MAX_LEN] = {0};
    uint16_t cmdDataLen = 0;

//    //封装指令
//    cmdDataLen = formatBoardCmd(EnumBoardId_getBrightTaskInfo, getCmdData, cmdDataLen);
//    //删除重复ID结点
//    GetTaskLinkList::GetInstance().app_deleteNodeWithCmdId(EnumBoardId_getBrightTaskInfo);
////    gtTaskLinkList.app_deleteNodeWithCmdId(EnumBoardId_getBeltOutTaskInfo);
//    //插入数据
//    GetTaskLinkList::GetInstance().app_insertTail(m_stTaskToSend.m_uTaskId, EnumBoardObject_Light, getCmdData, cmdDataLen);
////    gtTaskLinkList.app_insertTail(m_stTaskToSend.m_uTaskId, EnumBoardObject_BeltOut, getCmdData, cmdDataLen);
}

/*
 * settask 任务发送后，未收到settask回复的重发处理函数（用定时器实现）
 */
void ActionLight::slot_timer_setTaskReSend()
{
//    //如果下层返回的CmdFlag和当前准备发送的CmdFlag相等，说明设置的指令已经收到回复
//    if(m_stTaskD.m_uTaskId == m_stTaskToSend.m_uTaskId)
//    {
//        timer_setTaskReSend.stop();
//    }
//    else
//    {
//        setTaskSend();
//    }
}

bool ActionLight::setTaskCmdReSend(uint32_t sdNum)
{
//    if(sdNum > u32MaxReadNum_to_reSend)
//    {
//        emit signal_SetTask_ReSend();
        return true;
//    }
//    return false;
}

