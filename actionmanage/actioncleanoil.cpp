#include "actioncleanoil.h"
#include <QRandomGenerator>
#include "global.h"

#include "head.h"


extern SetTaskRingFifo stTaskFifo;
//extern GetTaskLinkList gtTaskLinkList;
int ActionCleanOil::timeOutValue_setTaskReSend = 10000;
uint32_t ActionCleanOil::u32MaxReadNum_to_reSend = 20;

ActionCleanOil::ActionCleanOil(QObject *parent)
    : HardCmdParser(), QObject(parent)
{
    m_stTaskD.m_eTaskStatusD = emWorkStatus_start;
    m_stTaskD.m_eTaskSeluteD = emSeluteDCleanOil_start;
    m_stTaskToSend.m_eTaskType = emTaskDCleanOilType_Start;

    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_setCleanOilTask, this);
    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_getCleanOilTaskInfo, this);

    //信号关联
    connect(&timer_setTaskReSend, SIGNAL(timeout()), this, SLOT(slot_timer_setTaskReSend()));
    connect(this, SIGNAL(signal_SetTask_ReSend()), this, SLOT(setTaskSend()), Qt::QueuedConnection);
}

emWKCmdType ActionCleanOil::parseCmd(uint8_t* puData)
{
    uint8_t uCmdId = puData[CMD_ID_INDEX];
    uint32_t uReturnFlag = common_read_u32(&puData[CMD_FLAG_INDEX]);

    if (EnumBoardId_setCleanOilTask == uCmdId)
    {
        // 向gettask链表中压入查询指令
        getTaskSend();
        return emCMD_SET;
    }

    if (EnumBoardId_getCleanOilTaskInfo == uCmdId)
    {
        uint8_t uLen = CMD_CONTENT_INDEX;

        /*内容：eTskType(U8 emTskDCleanOilType) + uTaskId(U8) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDCleanOil) */
        m_stTaskD.m_eTaskTypeD = (emTaskDCleanOilType)puData[uLen];
        uLen += 1;

        m_stTaskD.m_uTaskId = puData[uLen];
        uLen += 1;

        // 任务状态
        m_stTaskD.m_eTaskStatusD = (emWorkStatus)puData[uLen];
        uLen += 1;

        //任务结果
        m_stTaskD.m_eTaskSeluteD = (emSeluteDCleanOil)puData[uLen];
        uLen += 1;

        // 更新 除油 界面信息
        emit signal_UiUpdate();
        // 如果任务是finish的，在链表中查找对应CmdFlag的结点删除，没找到，丢弃该数据
        if(emWorkStatus_finish == m_stTaskD.m_eTaskStatusD)
        {
            GetTaskLinkList::GetInstance().app_deleteNodeWithTaskId(EnumBoardObject_CleanOil, m_stTaskD.m_uTaskId);
//            gtTaskLinkList.app_deleteNodeWithTaskId(EnumBoardObject_BeltOut, m_stTaskD.m_uTaskId);
//            qDebug() << QString("ActionBeltOut::parseCmd, ID:[%1], task finish").arg(m_stTaskD.m_uTaskId) << TIMEMS;
        }
        return emCMD_GET;
    }
//    qDebug() << "[ActionBeltOut]parseCmd Not found Cmd:" << uCmdId << ",not do parse\r\n";
}

void ActionCleanOil::setTaskSend()
{
    uint16_t i = 0;
    STRUCT_SETTASK_MESSAGE_INFO *pWriteAddr;

    //构造settask指令内容 到 发送队列
    pWriteAddr = stTaskFifo.getWriteMessageAddr(EnumBoardId_setCleanOilTask);
    /* 请求： eTskType(U8 emTskDBeltOutType) + uTskId(U8)*/
    pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTaskToSend.m_eTaskType;
    pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTaskToSend.m_uTaskId;
    pWriteAddr->u16CmdContentLen = i;

    //压入发送队列前，开启重发定时器，如果本来就开着会重启定时器
    timer_setTaskReSend.start(timeOutValue_setTaskReSend);
    stTaskFifo.pushWriteMessageToFifo();
}

/*
 * 收到set指令的回复触发，往链表中push查询指令
 * 搜索该链表中有没有指令ID一样的结点，有的话直接删除，表示该模块的任务切换了，用户不关心之前指令ID对应的任务。
 * 然后添加新的结点，该结点带有指令ID和CmdFlag数据
 */
void ActionCleanOil::getTaskSend()
{
    uint8_t getCmdData[WK_NET_ONE_MESSAGE_MAX_LEN] = {0};
    uint16_t cmdDataLen = 0;

    //封装指令
    cmdDataLen = formatBoardCmd(EnumBoardId_getCleanOilTaskInfo, getCmdData, cmdDataLen);
    //删除重复ID结点
    GetTaskLinkList::GetInstance().app_deleteNodeWithCmdId(EnumBoardId_getCleanOilTaskInfo);
//    gtTaskLinkList.app_deleteNodeWithCmdId(EnumBoardId_getBeltOutTaskInfo);
    //插入数据
    GetTaskLinkList::GetInstance().app_insertTail(m_stTaskToSend.m_uTaskId, EnumBoardObject_CleanOil, getCmdData, cmdDataLen);
//    gtTaskLinkList.app_insertTail(m_stTaskToSend.m_uTaskId, EnumBoardObject_BeltOut, getCmdData, cmdDataLen);
}

/*
 * settask 任务发送后，未收到settask回复的重发处理函数（用定时器实现）
 */
void ActionCleanOil::slot_timer_setTaskReSend()
{
    //如果下层返回的CmdFlag和当前准备发送的CmdFlag相等，说明设置的指令已经收到回复
    if(m_stTaskD.m_uTaskId == m_stTaskToSend.m_uTaskId)
    {
        timer_setTaskReSend.stop();
    }
    else
    {
        setTaskSend();
    }
}

bool ActionCleanOil::setTaskCmdReSend(uint32_t sdNum)
{
    if(sdNum > u32MaxReadNum_to_reSend)
    {
        emit signal_SetTask_ReSend();
        return true;
    }
    return false;
}

