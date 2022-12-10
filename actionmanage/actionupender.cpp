#include "actionupender.h"
#include "global.h"
#include "head.h"

extern SetTaskRingFifo stTaskFifo;
//extern GetTaskLinkList gtTaskLinkList;
int ActionUpender::timeOutValue_setTaskReSend = 10000;
uint32_t ActionUpender::u32MaxReadNum_to_reSend = 20;

ActionUpender::ActionUpender(QObject *parent)
    : HardCmdParser(), QObject(parent)
{
    m_eTaskStatusD = emWorkStatus_start;
    m_eTaskSeluteD = emSeluteDUpender_start;
    m_stTaskToSend.m_eTaskType = emTskDUpenderType_Start;

    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_setUpenderTask, this);
    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_getUpenderTaskInfo, this);

    //信号关联
    connect(&timer_setTaskReSend, SIGNAL(timeout()), this, SLOT(slot_timer_setTaskReSend()));
    connect(this, SIGNAL(signal_SetTask_ReSend()), this, SLOT(setTaskSend()), Qt::QueuedConnection);
}

emWKCmdType ActionUpender::parseCmd(uint8_t* puData)
{
    uint8_t uCmdId = puData[CMD_ID_INDEX];
    uint32_t uReturnCmdFlag = common_read_u32(&puData[CMD_FLAG_INDEX]);

    if (EnumBoardId_setUpenderTask == uCmdId)
    {
        // 向gettask链表中压入查询指令
        getTaskSend();
        _LOG(QString("parse setTask cmd return"));
        return emCMD_SET;
    }

    if (EnumBoardId_getUpenderTaskInfo == uCmdId)
    {
        uint8_t uLen = CMD_CONTENT_INDEX;

        //解析其他信息及后续操作
        /* 返回：eTskType(U8 emTskDUpenderType) + uTskId(U8) + eStatus(U8 emWorkStatus)
         * + eSelute(U8 emSeluteDUpender) + eCurPos(U8 emPositionType)+ m_iIsMove(U8) */
        //任务信息
        m_stTaskD.m_eTaskType = (emTaskDUpenderType)puData[uLen];
        uLen += 1;

        m_stTaskD.m_uTaskId = (uint8_t)puData[uLen];
        uLen += 1;

        m_eTaskStatusD = (emWorkStatus)puData[uLen];
        uLen += 1;

        m_eTaskSeluteD = (E_SELUTE_UPENDER)puData[uLen];
        uLen += 1;

        m_eCurPos = (emPositionType)puData[uLen];
        uLen += 1;

        m_bIsMoving = (bool)puData[uLen];
        uLen += 1;

        // 更新 翻转装置 界面信息
        emit signal_UiUpdate();
        // 如果任务是finish的，在链表中查找对应CmdFlag的结点删除，没找到，丢弃该数据
        if(emWorkStatus_finish == m_eTaskStatusD)
        {
//            gtTaskLinkList.app_deleteNodeWithTaskId(EnumBoardObject_BeltIn, m_stTaskD.m_uTaskId);
            GetTaskLinkList::GetInstance().app_deleteNodeWithTaskId(EnumBoardObject_Upender, m_stTaskD.m_uTaskId);
            _LOG(QString("parse getTask cmd return, task finish"));
        }
        return emCMD_GET;
    }
    _LOG(QString("No such cmd = [%1]").arg(uCmdId));
}

void ActionUpender::setTaskSend()
{
    uint16_t i = 0;
    STRUCT_SETTASK_MESSAGE_INFO *pWriteAddr;

    //把 settask 指令封装到发送队列
    pWriteAddr = stTaskFifo.getWriteMessageAddr(EnumBoardId_setUpenderTask);
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
        _LOG(QString("settask fifo full, data lost"));
    }
}

/*
 * 收到set指令的回复触发，往链表中push查询指令
 * 搜索该链表中有没有指令ID一样的结点，有的话直接删除，表示该模块的任务切换了，用户不关心之前指令ID对应的任务。
 * 然后添加新的结点，该结点带有指令ID和CmdFlag数据
 */
void ActionUpender::getTaskSend()
{
    uint8_t getCmdData[WK_NET_ONE_MESSAGE_MAX_LEN] = {0};
    uint16_t cmdDataLen = 0;

    //封装指令
    cmdDataLen = formatBoardCmd(EnumBoardId_getUpenderTaskInfo, getCmdData, cmdDataLen);
    //删除重复ID结点
    GetTaskLinkList::GetInstance().app_deleteNodeWithCmdId(EnumBoardId_getUpenderTaskInfo);
    //插入数据
    GetTaskLinkList::GetInstance().app_insertTail(m_stTaskToSend.m_uTaskId, EnumBoardObject_Upender, getCmdData, cmdDataLen);
}

/*
 * settask 任务发送后，未收到settask回复的重发处理函数（用定时器实现）
 */
void ActionUpender::slot_timer_setTaskReSend()
{
    //如果下层同步到的taskId和当前准备重发的taskId相等，说明设置的指令已经收到回复，停止重发定时器
    if(m_stTaskD.m_uTaskId == m_stTaskToSend.m_uTaskId)
    {
        timer_setTaskReSend.stop();
    }
    else
    {
        _LOG(QString("ReSend"));
        setTaskSend();
    }
}
/*
 * 在查询链表中的查询数据发送超次数，任务仍然未finish的重发处理函数
 */
bool ActionUpender::setTaskCmdReSend(uint32_t sdNum)
{
    if(sdNum > u32MaxReadNum_to_reSend)
    {
        emit signal_SetTask_ReSend();
        return true;
    }
    return false;
}
