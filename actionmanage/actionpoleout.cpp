#include "actionpoleout.h"
#include "global.h"
#include "head.h"

extern SetTaskRingFifo stTaskFifo;
//extern GetTaskLinkList gtTaskLinkList;
int ActionPoleOut::timeOutValue_setTaskReSend = 10000;
uint32_t ActionPoleOut::u32MaxReadNum_to_reSend = 20;

ActionPoleOut::ActionPoleOut(QObject *parent)
    : HardCmdParser(), QObject(parent)
{
    m_eTaskStatusD = emWorkStatus_start;
    m_eTaskSeluteD = emSeluteDPole_start;
    m_stTaskToSend.m_eTaskType = emTskDPoleOutType_Start;

    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_setPoleOutTask, this);
    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_getPoleOutTaskInfo, this);

    //信号关联
    connect(&timer_setTaskReSend, SIGNAL(timeout()), this, SLOT(slot_timer_setTaskReSend()));
    connect(this, SIGNAL(signal_SetTask_ReSend()), this, SLOT(setTaskSend()), Qt::QueuedConnection);
}

emWKCmdType ActionPoleOut::parseCmd(uint8_t* puData)
{
    uint8_t uCmdId = puData[CMD_ID_INDEX];
    uint32_t uReturnCmdFlag = common_read_u32(&puData[CMD_FLAG_INDEX]);

    if (EnumBoardId_setPoleOutTask == uCmdId)
    {
        // 向gettask链表中压入查询指令
        getTaskSend();
//        qDebug() << QString("[%1][%2][%3]: setTask").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__);
        return emCMD_SET;
    }

    if (EnumBoardId_getPoleOutTaskInfo == uCmdId)
    {
        uint8_t uLen = CMD_CONTENT_INDEX;

        //解析其他信息及后续操作
        //任务信息
        /* 返回：eTaskType(U8 emTaskDPoleOutType) + uTaskId(U8) +
         * eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDPole) +
         * eCurPos(U8 emPositionType) + eBoxNum(U8)*/
        m_stTaskD.m_eTaskType = (emTaskDPoleOutType)puData[uLen];
        uLen += 1;

        m_stTaskD.m_uTaskId = puData[uLen];
        uLen += 1;

        // 任务状态
        m_eTaskStatusD = (emWorkStatus)puData[uLen];
        uLen += 1;

        //任务结果
        m_eTaskSeluteD = (emSeluteDPole)puData[uLen];
        uLen += 1;

        //当前位置
        m_eCurPos = (emPositionType)puData[uLen];
        uLen += 1;

        m_uEmptyLeft = puData[uLen];
        uLen += 1;

        // 更新 出舱推杆 界面信息
        emit signal_UiUpdate();
        // 如果任务是finish的，在链表中查找对应CmdFlag的结点删除，没找到，丢弃该数据
        if(emWorkStatus_finish == m_eTaskStatusD)
        {
//            gtTaskLinkList.app_deleteNodeWithTaskId(EnumBoardObject_BeltIn, m_stTaskD.m_uTaskId);
            GetTaskLinkList::GetInstance().app_deleteNodeWithTaskId(EnumBoardObject_PoleOut, m_stTaskD.m_uTaskId);
//            qDebug() << QString("[%1][%2][%3]: gettask").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__);
        }
        return emCMD_GET;
    }
    qDebug() << QString("[%1][%2][%3]: No such cmd=4%").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).arg(uCmdId);
}

void ActionPoleOut::setTaskSend()
{
    uint16_t i = 0;
    STRUCT_SETTASK_MESSAGE_INFO *pWriteAddr;

    //把 settask 指令封装到发送队列
    pWriteAddr = stTaskFifo.getWriteMessageAddr(EnumBoardId_setPoleOutTask);
    if(pWriteAddr)
    {
        pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTaskToSend.m_eTaskType;
        pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTaskToSend.m_uTaskId;
        pWriteAddr->u16CmdContentLen = i;

        //压入发送队列前，开启重发定时器，如果本来就开着会重启定时器
        timer_setTaskReSend.start(timeOutValue_setTaskReSend);
        stTaskFifo.pushWriteMessageToFifo();
    }
    else
    {
        qDebug() << QString("[%1][%2][%3]: settask fifo full, data lost").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__);
    }
}

/*
 * 收到set指令的回复触发，往链表中push查询指令
 * 搜索该链表中有没有指令ID一样的结点，有的话直接删除，表示该模块的任务切换了，用户不关心之前指令ID对应的任务。
 * 然后添加新的结点，该结点带有指令ID和CmdFlag数据
 */
void ActionPoleOut::getTaskSend()
{
    uint8_t getCmdData[WK_NET_ONE_MESSAGE_MAX_LEN] = {0};
    uint16_t cmdDataLen = 0;

    //封装指令
    cmdDataLen = formatBoardCmd(EnumBoardId_getPoleOutTaskInfo, getCmdData, cmdDataLen);
    //删除重复ID结点
    GetTaskLinkList::GetInstance().app_deleteNodeWithCmdId(EnumBoardId_getPoleOutTaskInfo);
    //插入数据
    GetTaskLinkList::GetInstance().app_insertTail(m_stTaskToSend.m_uTaskId, EnumBoardObject_PoleOut, getCmdData, cmdDataLen);
}

/*
 * settask 任务发送后，未收到settask回复的重发处理函数（用定时器实现）
 */
void ActionPoleOut::slot_timer_setTaskReSend()
{
    //如果下层同步到的taskId和当前准备重发的taskId相等，说明设置的指令已经收到回复，停止重发定时器
    if(m_stTaskD.m_uTaskId == m_stTaskToSend.m_uTaskId)
    {
        timer_setTaskReSend.stop();
    }
    else
    {
//        qDebug() << QString("[%1][%2][%3]:  ReSend").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__);
        setTaskSend();
    }
}
/*
 * 在查询链表中的查询数据发送超次数，任务仍然未finish的重发处理函数
 */
bool ActionPoleOut::setTaskCmdReSend(uint32_t sdNum)
{
    if(sdNum > u32MaxReadNum_to_reSend)
    {
        emit signal_SetTask_ReSend();
        return true;
    }
    return false;
}
