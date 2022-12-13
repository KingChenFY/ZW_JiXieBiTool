#include "actionbeltin.h"
#include "global.h"
#include "head.h"

extern SetTaskRingFifo stTaskFifo;
//extern GetTaskLinkList gtTaskLinkList;
int ActionBeltIn::timeOutValue_setTaskReSend = 10000;
uint32_t ActionBeltIn::u32MaxReadNum_to_reSend = 20;

ActionBeltIn::ActionBeltIn(QObject *parent)
    : HardCmdParser(), QObject(parent)
{
    m_eTaskStatusD = emWorkStatus_start;
    m_eTaskSeluteD = emSeluteDBelt_start;
    m_stTaskToSend.m_eTaskType = emTskDBeltInType_Start;
    m_stTaskToSend.m_bNeedQR = false;

    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_setBeltInTask, this);
    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_getBeltInTaskInfo, this);

    //信号关联
    connect(&timer_setTaskReSend, SIGNAL(timeout()), this, SLOT(slot_timer_setTaskReSend()));
    connect(this, SIGNAL(signal_SetTask_ReSend()), this, SLOT(setTaskSend()), Qt::QueuedConnection);
}

emWKCmdType ActionBeltIn::parseCmd(uint8_t* puData)
{
    uint8_t uCmdId = puData[CMD_ID_INDEX];
    uint32_t uReturnCmdFlag = common_read_u32(&puData[CMD_FLAG_INDEX]);

    if (EnumBoardId_setBeltInTask == uCmdId)
    {
        // 收到SetTask回复,且id和当前的任务id相同，停止重发定时器
        //??QObject::killTimer: Timers cannot be stopped from another thread???
//        if(m_stTaskD.m_u32CmdFlag == m_stTaskToSend.m_u32CmdFlag)
//            timer_setTaskReSend.stop();
        // 向gettask链表中压入查询指令
        getTaskSend();
        qDebug() << QString("EnumBoardId_setBeltInTask::parseCmd, Send getstatus");
        return emCMD_SET;
    }

    if (EnumBoardId_getBeltInTaskInfo == uCmdId)
    {
        uint8_t uLen = CMD_CONTENT_INDEX;

        /* 返回：eTaskType(U8 emTaskDBeltInType)  + uTaskId(U8) + uIsScan(U8) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteBeltD)
        + m_bIsMoving(U8 WK_TRUE/WK_FALSE) 皮带是否在运动
        + m_bIsIn(U8 WK_TRUE/WK_FALSE) 皮带方向是否向里
        + QRScanContent(U8[12]) 二维码扫描内容 */
        //......协议解析补全
        m_stTaskD.m_eTaskType = (emTaskDBeltInType)puData[uLen];
        uLen += 1;

        m_stTaskD.m_uTaskId = (uint8_t)puData[uLen];
        uLen += 1;

        m_stTaskD.m_bNeedQR = (uint8_t)puData[uLen];
        uLen += 1;

        m_eTaskStatusD = (emWorkStatus)puData[uLen];
        uLen += 1;

        m_eTaskSeluteD = (emSeluteDBelt)puData[uLen];
        uLen += 1;

        m_IsMoving = (uint8_t)puData[uLen];
        uLen += 1;

        m_IsIn = (uint8_t)puData[uLen];
        uLen += 1;

        memcpy(m_uDQRScan, &puData[uLen], WK_LEN_QRCODE_CONTENT);
        uLen += WK_LEN_QRCODE_CONTENT;

        // 更新 入舱皮带 界面信息
        emit signal_UiUpdate();
        // 如果任务是finish的，在链表中查找对应CmdFlag的结点删除，没找到，丢弃该数据
        if(emWorkStatus_finish == m_eTaskStatusD)
        {
//            gtTaskLinkList.app_deleteNodeWithTaskId(EnumBoardObject_BeltIn, m_stTaskD.m_uTaskId);
            GetTaskLinkList::GetInstance().app_deleteNodeWithTaskId(EnumBoardObject_BeltIn, m_stTaskD.m_uTaskId);
            qDebug() << QString("EnumBoardId_getBeltInTaskInfo::parseCmd, ID:[%1], task finish").arg(m_stTaskD.m_uTaskId) << TIMEMS;
        }
        return emCMD_GET;
    }
    qDebug() << "ActionBeltIn::parseCmd no such uCmdId = " << uCmdId;
}

void ActionBeltIn::setTaskSend()
{
    uint16_t i = 0;
    STRUCT_SETTASK_MESSAGE_INFO *pWriteAddr;

    //把 settask 指令封装到发送队列
    pWriteAddr = stTaskFifo.getWriteMessageAddr(EnumBoardId_setBeltInTask);
    if(pWriteAddr)
    {
        pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTaskToSend.m_eTaskType;
        pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTaskToSend.m_uTaskId;
        pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTaskToSend.m_bNeedQR;
        pWriteAddr->u16CmdContentLen = i;

        //压入发送队列前，开启重发定时器，如果本来就开着会重启定时器
        timer_setTaskReSend.start(timeOutValue_setTaskReSend);
        stTaskFifo.pushWriteMessageToFifo();
        qDebug() << QString("ActionBeltIn::setTaskSend, TYPE:[%1],ID:[%2], set task OK!").arg(m_stTaskToSend.m_eTaskType).arg(m_stTaskToSend.m_uTaskId);
    }
    else
    {
        qDebug() << QString("ActionBeltIn::setTaskSend, settask fifo full, data lost");
    }
}

/*
 * 收到set指令的回复触发，往链表中push查询指令
 * 搜索该链表中有没有指令ID一样的结点，有的话直接删除，表示该模块的任务切换了，用户不关心之前指令ID对应的任务。
 * 然后添加新的结点，该结点带有指令ID和CmdFlag数据
 */
void ActionBeltIn::getTaskSend()
{
    uint8_t getCmdData[WK_NET_ONE_MESSAGE_MAX_LEN] = {0};
    uint16_t cmdDataLen = 0;

    //封装指令
    cmdDataLen = formatBoardCmd(EnumBoardId_getBeltInTaskInfo, getCmdData, cmdDataLen);
    //删除重复ID结点
    GetTaskLinkList::GetInstance().app_deleteNodeWithCmdId(EnumBoardId_getBeltInTaskInfo);
//    gtTaskLinkList.app_deleteNodeWithCmdId(EnumBoardId_getBeltInTaskInfo);
    //插入数据
    GetTaskLinkList::GetInstance().app_insertTail(m_stTaskToSend.m_uTaskId, EnumBoardObject_BeltIn, getCmdData, cmdDataLen);
//    gtTaskLinkList.app_insertTail(m_stTaskToSend.m_uTaskId, EnumBoardObject_BeltIn, getCmdData, cmdDataLen);
}

/*
 * settask 任务发送后，未收到settask回复的重发处理函数（用定时器实现）
 */
void ActionBeltIn::slot_timer_setTaskReSend()
{
    //如果下层同步到的taskId和当前准备重发的taskId相等，说明设置的指令已经收到回复，停止重发定时器
    if(m_stTaskD.m_uTaskId == m_stTaskToSend.m_uTaskId)
    {
        qDebug() << QString("ActionBeltIn, ID:[%1], close timer_setTaskReSend").arg(m_stTaskD.m_uTaskId);
        timer_setTaskReSend.stop();
    }
    else
    {
        setTaskSend();
    }
}
/*
 * 在查询链表中的查询数据发送超次数，任务仍然未finish的重发处理函数
 */
bool ActionBeltIn::setTaskCmdReSend(uint32_t sdNum)
{
    if(sdNum > u32MaxReadNum_to_reSend)
    {
        emit signal_SetTask_ReSend();
        return true;
    }
    return false;
}


