#include "actiontriggerset.h"
#include "global.h"
#include "head.h"

extern SetTaskRingFifo stTaskFifo;
int ActionTriggerSet::timeOutValue_setTaskReSend = 10000;

ActionTriggerSet::ActionTriggerSet(QObject *parent)
    : QObject{parent}
{
    m_bStatusSyn = false;

    m_stTaskInfoD.m_eAxis = emAxis_End;
    m_stTaskInfoD.m_u16Interval = 0;
    m_stTaskInfoD.m_u8TrigObj = 0;

    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_setEncodeCh, this);
    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_getEncodeCh, this);
    //信号关联
    connect(&timer_setTaskReSend, SIGNAL(timeout()), this, SLOT(slot_timer_setTaskReSend()));
    connect(this, SIGNAL(signal_getTaskSend()), this, SLOT(getTaskSend()), Qt::QueuedConnection);
    connect(this, SIGNAL(signal_stopSetTaskReSendTimer()), this, SLOT(slot_stopSetTaskReSendTimer()), Qt::QueuedConnection);
}

emWKCmdType ActionTriggerSet::parseCmd(uint8_t* puData)
{
    uint8_t uCmdId = puData[CMD_ID_INDEX];
    uint32_t uReturnCmdFlag = common_read_u32(&puData[CMD_FLAG_INDEX]);

    if (EnumBoardId_setEncodeCh == uCmdId)
    {
        emit signal_getTaskSend();
        _LOG(QString("setEncodeCh OK, send signal_getTaskSend"));
        return emCMD_SET;
    }

    if (EnumBoardId_getEncodeCh == uCmdId)
    {
        uint8_t uLen = CMD_CONTENT_INDEX;

        //解析其他信息及后续操作
        /* 返回：eTskType(U8 emTskDUpenderType) + uTskId(U8) + eStatus(U8 emWorkStatus)
         * + eSelute(U8 emSeluteDUpender) + eCurPos(U8 emPositionType)+ m_iIsMove(U8) */
        //任务信息
        m_stTaskInfoD.m_eAxis = (emAxisType)puData[uLen];
        uLen += 1;

        m_stTaskInfoD.m_u16Interval = common_read_u16(&puData[uLen]);
        uLen += 2;

        m_stTaskInfoD.m_u8TrigObj = (uint8_t)puData[uLen];
        uLen += 1;

        m_bStatusSyn = true;//指令已同步
        if( (m_stTaskToSend.m_eAxis == m_stTaskInfoD.m_eAxis) &&
                (m_stTaskToSend.m_u16Interval == m_stTaskInfoD.m_u16Interval) &&
                (m_stTaskToSend.m_u8TrigObj == m_stTaskInfoD.m_u8TrigObj) )
        {
            emit signal_stopSetTaskReSendTimer();
        }
        return emCMD_GET;
    }
    _LOG(QString("No such cmd = [%1]").arg(uCmdId));
}

void ActionTriggerSet::setTaskSend()
{
    uint16_t i = 0;
    STRUCT_SETTASK_MESSAGE_INFO *pWriteAddr;

    //把 settask 指令封装到发送队列
    pWriteAddr = stTaskFifo.getWriteMessageAddr(EnumBoardId_setEncodeCh);
    if(pWriteAddr)
    {
        m_bStatusSyn = false;//指令未同步
        pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTaskToSend.m_eAxis;
        common_write_u16(&pWriteAddr->u8CmdContent[i], (uint32_t)m_stTaskToSend.m_u16Interval); i += 2;
        pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTaskToSend.m_u8TrigObj;
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
void ActionTriggerSet::getTaskSend()
{
    STRUCT_SETTASK_MESSAGE_INFO *pWriteAddr;

    //把 gettask 指令封装到发送队列
    pWriteAddr = stTaskFifo.getWriteMessageAddr(EnumBoardId_getEncodeCh);
    if(pWriteAddr)
    {
        pWriteAddr->u16CmdContentLen = 0;//指令内容为空
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
 * settask 任务发送后，未收到settask回复的重发处理函数（用定时器实现）
 */
void ActionTriggerSet::slot_timer_setTaskReSend()
{
    //如果指令已同步，且下层信息和上层相同，停止重发定时器
    if( (m_bStatusSyn) && (m_stTaskToSend.m_eAxis == m_stTaskInfoD.m_eAxis) &&
            (m_stTaskToSend.m_u16Interval == m_stTaskInfoD.m_u16Interval) &&
            (m_stTaskToSend.m_u8TrigObj == m_stTaskInfoD.m_u8TrigObj) )
    {
        timer_setTaskReSend.stop();
        emit signal_UiUpdate();
    }
    else
    {
        _LOG(QString("ReSend"));
        setTaskSend();
    }
}

void ActionTriggerSet::slot_stopSetTaskReSendTimer()
{
    timer_setTaskReSend.stop();
    emit signal_UiUpdate();
}

bool ActionTriggerSet::setTaskCmdReSend(uint32_t sdNum)
{
    return true;
}
