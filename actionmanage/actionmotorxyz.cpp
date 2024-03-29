#include "actionmotorxyz.h"
#include "global.h"
#include "head.h"

extern SetTaskRingFifo stTaskFifo;
//extern GetTaskLinkList gtTaskLinkList;
int ActionMotorXYZ::timeOutValue_setTaskReSend = 1000;
uint32_t ActionMotorXYZ::u32MaxReadNum_to_reSend = 2000;

ActionMotorXYZ::ActionMotorXYZ(QObject *parent)
    : QObject{parent}, HardCmdParser()
{
    /***********下发任务信息***********/
    m_stTaskToSend.m_eTaskType = emTskDXYZType_start;
    m_stTaskToSend.m_uTaskId = 0;
    m_stTaskToSend.m_stAimDPos = {WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit};
    m_stTaskToSend.m_stDPosBgn = {WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit};
    m_stTaskToSend.m_u32MoveTime = 0;
    m_stTaskToSend.m_u16PathId = 0;
    m_stTaskToSend.m_eXZeroDir = emMorotX_UnKnow;
    /***********下层同步任务信息***********/
    m_stDTaskInfo.m_eTaskTypeD = emTskDXYZType_start;
    m_stDTaskInfo.m_eTaskStatusD = emWorkStatus_start;
    m_stDTaskInfo.m_eTaskSeluteD = emSeluteDMoveXYZ_start;
    m_stDTaskInfo.m_stAimDPos = {WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit};
    m_stDTaskInfo.m_u32MoveTime = 0;
    m_stDTaskInfo.m_u16XZeroSpeed = WK_SpeedNotLimit;
    m_stDTaskInfo.m_stCurDPos = {WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit};
    m_stDTaskInfo.m_stCurLogicDPos = {LOGIC_LINE, LOGIC_LINE, LOGIC_LINE};
    m_stDTaskInfo.m_eXZeroDir = emMorotX_UnKnow;
    m_stDTaskInfo.m_stLinePos = {WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit,
                                WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit};
    m_stDTaskInfo.m_bXIsMoving = false;
    m_stDTaskInfo.m_bYIsMoving = false;
    m_stDTaskInfo.m_bZIsMoving = false;
    m_stDTaskInfo.m_u16PathId = 0;
    m_stDTaskInfo.emYInOut = emMorotY_UnKnow;
    m_stDTaskInfo.emZUpDown = emMorotZ_UnKnow;
    m_stDTaskInfo.m_stDPosBgn = {WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit};
    m_stDTaskInfo.m_stXZeroEPos = {WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit};
    m_stDTaskInfo.m_uTaskId = 0;
    m_stDTaskInfo.m_u32XErrCode = 0;
    m_stDTaskInfo.m_u32YErrCode = 0;
    m_stDTaskInfo.m_u32ZErrCode = 0;

    emMetaTaskType = QMetaEnum::fromType<E_TASKTPYE_MOTORXYZ>();

    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_setXYZTask, this);
    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_getXYZTaskInfo, this);

    //信号关联
    connect(&timer_setTaskReSend, SIGNAL(timeout()), this, SLOT(slot_timer_setTaskReSend()));
    connect(this, SIGNAL(signal_SetTask_ReSend()), this, SLOT(setTaskSend()), Qt::QueuedConnection);
}

emWKCmdType ActionMotorXYZ::parseCmd(uint8_t* puData)
{
    uint8_t uCmdId = puData[CMD_ID_INDEX];
    uint32_t uReturnCmdFlag = common_read_u32(&puData[CMD_FLAG_INDEX]);

    if (EnumBoardId_setXYZTask == uCmdId)
    {
        // 向gettask链表中压入查询指令
        getTaskSend();
        _LOG(QString("parse setTask cmd return"));
        return emCMD_SET;
    }

    if (EnumBoardId_getXYZTaskInfo == uCmdId)
    {
        uint8_t uLen = CMD_CONTENT_INDEX;

        /* 内容：eTskType(U8 emTskDXYZType) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDMoveXYZ)
        + PosX(I32) + PosY(I32) + PosZ(I32) + MoveTime(U32 ms)/XZeroSpeed(U16)
        + curPosX(I32)/(XZeroSetDir(U8)) + curPosY(I32) + curPosZ(I32)
        + rdTmStamp(U64)(硬件时间片)
        + xMin(I32) + xMax(I32) + yMin(I32) + yMax(I32) + zMin(I32) + zMax(I32)
        + ismoveX(u8) + ismoveY(u8) + ismoveZ(u8)
        + Point_id(U16)(零点移动测试唯一id(u8)弃用) +YInOut(U8) + ZUpDown(U8)
        + PosXStart(I32) + PosYStart(I32) + PosZStart(I32)
        + (emWorkStatus_finish-->m_i32MoveDirMinEMinPos+m_i32MoveDirMinEMaxPos+m_i32MoveDirMaxEMinPos+m_i32MoveDirMaxEMaxPos
        + newAimId(u8) + XErrCode[U32]+ YErrCode[U32]+ ZErrCode[U32]*/

        m_stDTaskInfo.m_eTaskTypeD = (emTaskDXYZType)puData[uLen];
        uLen += 1;

        m_stDTaskInfo.m_eTaskStatusD = (emWorkStatus)puData[uLen];
        uLen += 1;

        m_stDTaskInfo.m_eTaskSeluteD = (emSeluteDMoveXYZ)puData[uLen];
        uLen += 1;

        m_stDTaskInfo.m_stAimDPos.m_i32X = common_read_u32(&puData[uLen]);
        uLen += 4;
        m_stDTaskInfo.m_stAimDPos.m_i32Y = common_read_u32(&puData[uLen]);
        uLen += 4;
        m_stDTaskInfo.m_stAimDPos.m_i32Z = common_read_u32(&puData[uLen]);
        uLen += 4;

        if(emTskDXYZType_zeroXMoveCheck == m_stDTaskInfo.m_eTaskTypeD)
            m_stDTaskInfo.m_u16XZeroSpeed = common_read_u32(&puData[uLen]);
        else
            m_stDTaskInfo.m_u32MoveTime = common_read_u32(&puData[uLen]);
        uLen += 4;

        if(emTskDXYZType_setXPosZero == m_stDTaskInfo.m_eTaskTypeD)
            m_stDTaskInfo.m_eXZeroDir = (emMotorXDir)common_read_u32(&puData[uLen]);
        else
            m_stDTaskInfo.m_stCurDPos.m_i32X = common_read_u32(&puData[uLen]);
        uLen += 4;
        m_stDTaskInfo.m_stCurDPos.m_i32Y = common_read_u32(&puData[uLen]);
        uLen += 4;
        m_stDTaskInfo.m_stCurDPos.m_i32Z = common_read_u32(&puData[uLen]);
        uLen += 4;

        m_stDTaskInfo.m_u64CurPosDTime = common_read_u64(&puData[uLen]);
        uLen += 8;

        mutexXYZLine.lock();
        m_stDTaskInfo.m_stLinePos.m_i32XLineLeft = common_read_u32(&puData[uLen]);
        uLen += 4;
        m_stDTaskInfo.m_stLinePos.m_i32XLineRight = common_read_u32(&puData[uLen]);
        uLen += 4;
        m_stDTaskInfo.m_stLinePos.m_i32YLineIn = common_read_u32(&puData[uLen]);
        uLen += 4;
        m_stDTaskInfo.m_stLinePos.m_i32YLineOut = common_read_u32(&puData[uLen]);
        uLen += 4;
        m_stDTaskInfo.m_stLinePos.m_i32ZLineDown = common_read_u32(&puData[uLen]);
        uLen += 4;
        m_stDTaskInfo.m_stLinePos.m_i32ZLineUp = common_read_u32(&puData[uLen]);
        uLen += 4;
        mutexXYZLine.unlock();

        m_stDTaskInfo.m_bXIsMoving = (bool)puData[uLen++];
        m_stDTaskInfo.m_bYIsMoving = (bool)puData[uLen++];
        m_stDTaskInfo.m_bZIsMoving = (bool)puData[uLen++];

        m_stDTaskInfo.m_u16PathId = common_read_u16(&puData[uLen]);
        uLen += 2;

        m_stDTaskInfo.emYInOut = (emMotorYDir)puData[uLen++];
        m_stDTaskInfo.emZUpDown = (emMotorZDir)puData[uLen++];

        m_stDTaskInfo.m_stDPosBgn.m_i32X = common_read_u32(&puData[uLen]);
        uLen += 4;
        m_stDTaskInfo.m_stDPosBgn.m_i32Y = common_read_u32(&puData[uLen]);
        uLen += 4;
        m_stDTaskInfo.m_stDPosBgn.m_i32Z = common_read_u32(&puData[uLen]);
        uLen += 4;

        if(emWorkStatus_finish == m_stDTaskInfo.m_eTaskStatusD)
        {
            m_stDTaskInfo.m_stXZeroEPos.m_i32MoveDirMinEMinPos = common_read_u32(&puData[uLen]);
            uLen += 4;
            m_stDTaskInfo.m_stXZeroEPos.m_i32MoveDirMinEMaxPos = common_read_u32(&puData[uLen]);
            uLen += 4;
            m_stDTaskInfo.m_stXZeroEPos.m_i32MoveDirMaxEMinPos = common_read_u32(&puData[uLen]);
            uLen += 4;
            m_stDTaskInfo.m_stXZeroEPos.m_i32MoveDirMaxEMaxPos = common_read_u32(&puData[uLen]);
            uLen += 4;
        }

        m_stDTaskInfo.m_uTaskId = puData[uLen++];

        m_stDTaskInfo.m_u32XErrCode = common_read_u32(&puData[uLen]);
        uLen += 4;
        m_stDTaskInfo.m_u32YErrCode = common_read_u32(&puData[uLen]);
        uLen += 4;
        m_stDTaskInfo.m_u32ZErrCode = common_read_u32(&puData[uLen]);
        uLen += 4;

        //将物理坐标转换成逻辑值，获得一份逻辑坐标
        convertPhyPosToLogicPos(m_stDTaskInfo.m_stCurDPos, m_stDTaskInfo.m_stCurLogicDPos);
        // 更新 XYZ 界面信息
        emit signal_UiUpdate();
        // 如果任务是finish的，在链表中查找对应CmdFlag的结点删除，没找到，丢弃该数据
        if(emWorkStatus_finish == m_stDTaskInfo.m_eTaskStatusD)
        {
//            gtTaskLinkList.app_deleteNodeWithTaskId(EnumBoardObject_BeltIn, m_stTaskD.m_uTaskId);
            GetTaskLinkList::GetInstance().app_deleteNodeWithTaskId(EnumBoardObject_MotorXYZ, m_stDTaskInfo.m_uTaskId);
            QMetaEnum emMotorXYZTaskType = QMetaEnum::fromType<ActionMotorXYZ::E_TASKTPYE_MOTORXYZ>();
            _LOG(QString("parse getTask cmd return, task finish,TYPE[%1][%2],ID[%3]")
                 .arg(QString(emMotorXYZTaskType.valueToKey(m_stDTaskInfo.m_eTaskTypeD))).arg(m_stDTaskInfo.m_eTaskTypeD).arg(m_stDTaskInfo.m_uTaskId));
        }
        return emCMD_GET;
    }
    _LOG(QString("No such cmd = [%1]").arg(uCmdId));
}

void ActionMotorXYZ::setTaskSend()
{
    uint16_t i = 0;
    STRUCT_SETTASK_MESSAGE_INFO *pWriteAddr;

    //把 settask 指令封装到发送队列
    pWriteAddr = stTaskFifo.getWriteMessageAddr(EnumBoardId_setXYZTask);
    if(pWriteAddr)
    {
        /*emTskDXYZType eTaskType,int32_t Posx,int32_t Posy,int32_t Posz ,
        uint32_t Time,uint16_t Point_id,
        int32_t StartPosx,int32_t StartPosy,int32_t StartPosz,uint8_t newId*/
        pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTaskToSend.m_eTaskType;
        common_write_u32(&pWriteAddr->u8CmdContent[i], (uint32_t)m_stTaskToSend.m_stAimDPos.m_i32X);
        i += 4;
        common_write_u32(&pWriteAddr->u8CmdContent[i], (uint32_t)m_stTaskToSend.m_stAimDPos.m_i32Y);
        i += 4;
        common_write_u32(&pWriteAddr->u8CmdContent[i], (uint32_t)m_stTaskToSend.m_stAimDPos.m_i32Z);
        i += 4;
        common_write_u32(&pWriteAddr->u8CmdContent[i], (uint32_t)m_stTaskToSend.m_u32MoveTime);
        i += 4;
        common_write_u16(&pWriteAddr->u8CmdContent[i], (uint16_t)m_stTaskToSend.m_u16PathId);
        i += 2;
        common_write_u32(&pWriteAddr->u8CmdContent[i], (uint32_t)m_stTaskToSend.m_stDPosBgn.m_i32X);
        i += 4;
        common_write_u32(&pWriteAddr->u8CmdContent[i], (uint32_t)m_stTaskToSend.m_stDPosBgn.m_i32Y);
        i += 4;
        common_write_u32(&pWriteAddr->u8CmdContent[i], (uint32_t)m_stTaskToSend.m_stDPosBgn.m_i32Z);
        i += 4;
        pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTaskToSend.m_uTaskId;
        pWriteAddr->u16CmdContentLen = i;

        //getWriteMessageAddr已经对 pWriteAddr->eCmdId 进行了赋值，下面不需要了
        pWriteAddr->u8TaskId = (uint8_t)m_stTaskToSend.m_uTaskId;
        pWriteAddr->u8TaskType = (uint8_t)m_stTaskToSend.m_eTaskType;

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
void ActionMotorXYZ::getTaskSend()
{
    uint8_t getCmdData[WK_NET_ONE_MESSAGE_MAX_LEN] = {0};
    uint16_t cmdDataLen = 0;

    //封装指令
    cmdDataLen = formatBoardCmd(EnumBoardId_getXYZTaskInfo, getCmdData, cmdDataLen);
    //删除重复ID结点
    GetTaskLinkList::GetInstance().app_deleteNodeWithCmdId(EnumBoardId_getXYZTaskInfo);
    //插入数据
    GetTaskLinkList::GetInstance().app_insertTail(m_stTaskToSend.m_uTaskId, EnumBoardObject_MotorXYZ, getCmdData, cmdDataLen);
}

/*
 * settask 任务发送后，未收到settask回复的重发处理函数（用定时器实现）
 */
void ActionMotorXYZ::slot_timer_setTaskReSend()
{
    //如果下层同步到的taskId和当前准备重发的taskId相等，说明设置的指令已经收到回复，停止重发定时器
    if(m_stDTaskInfo.m_uTaskId == m_stTaskToSend.m_uTaskId)
    {
        _LOG(QString("set task is get recv,TYPE[%1][%2],ID[%3]").
             arg(QString(emMetaTaskType.valueToKey(m_stTaskToSend.m_eTaskType))).arg(m_stTaskToSend.m_eTaskType).arg(m_stTaskToSend.m_uTaskId));
        timer_setTaskReSend.stop();
    }
    else
    {
        _LOG(QString("set task lost,ReSend[settask],TYPE[%1][%2],ID[%3]").
             arg(QString(emMetaTaskType.valueToKey(m_stTaskToSend.m_eTaskType))).arg(m_stTaskToSend.m_eTaskType).arg(m_stTaskToSend.m_uTaskId));
        setTaskSend();
    }
}
/*
 * 在查询链表中的查询数据发送超次数，任务仍然未finish的重发处理函数
 */
bool ActionMotorXYZ::setTaskCmdReSend(uint32_t sdNum)
{
    if(sdNum > u32MaxReadNum_to_reSend)
    {
        _LOG(QString("get task not finish timeout,ReSend[settask],TYPE[%1][%2],ID[%3]").
             arg(QString(emMetaTaskType.valueToKey(m_stTaskToSend.m_eTaskType))).arg(m_stTaskToSend.m_eTaskType).arg(m_stTaskToSend.m_uTaskId));
        emit signal_SetTask_ReSend();
        return true;
    }
    return false;
}

bool ActionMotorXYZ::isLineHasLocated(emMotorObj motorObj)
{
    if( (emMorot_X == motorObj) &&
            (WK_PhyPosNotLimit != m_stDTaskInfo.m_stLinePos.m_i32XLineLeft) && ((WK_PhyPosNotLimit - 1) != m_stDTaskInfo.m_stLinePos.m_i32XLineLeft)
            && (WK_PhyPosNotLimit != m_stDTaskInfo.m_stLinePos.m_i32XLineRight) && ((WK_PhyPosNotLimit - 1) != m_stDTaskInfo.m_stLinePos.m_i32XLineRight) )
    {
        return true;
    }
    else if( (emMorot_Y == motorObj) &&
             (WK_PhyPosNotLimit != m_stDTaskInfo.m_stLinePos.m_i32YLineIn) && ((WK_PhyPosNotLimit - 1) != m_stDTaskInfo.m_stLinePos.m_i32YLineIn)
             && (WK_PhyPosNotLimit != m_stDTaskInfo.m_stLinePos.m_i32YLineOut) && ((WK_PhyPosNotLimit - 1) != m_stDTaskInfo.m_stLinePos.m_i32YLineOut) )
    {
        return true;
    }
    else if( (emMorot_Z == motorObj) &&
            (WK_PhyPosNotLimit != m_stDTaskInfo.m_stLinePos.m_i32ZLineUp) && ((WK_PhyPosNotLimit - 1) != m_stDTaskInfo.m_stLinePos.m_i32ZLineUp)
            && (WK_PhyPosNotLimit != m_stDTaskInfo.m_stLinePos.m_i32ZLineDown) && ((WK_PhyPosNotLimit - 1) != m_stDTaskInfo.m_stLinePos.m_i32ZLineDown) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ActionMotorXYZ::isAllLineHasLocated()
{
    if( isLineHasLocated(emMorot_X) && isLineHasLocated(emMorot_Y) && isLineHasLocated(emMorot_Z) )
        return true;
    else
        return false;
}

void ActionMotorXYZ::convertPhyPosToLogicPos(ST_XYZ_DPOS &phyPos, ST_XYZ_CPOS &logicPos)
{
    if((isLineHasLocated(emMorot_X)) && (WK_PhyPosNotLimit != phyPos.m_i32X))
    {
        float xLineLength = abs(m_stDTaskInfo.m_stLinePos.m_i32XLineLeft - m_stDTaskInfo.m_stLinePos.m_i32XLineRight);
            logicPos.m_fX = ( std::max(m_stDTaskInfo.m_stLinePos.m_i32XLineLeft, m_stDTaskInfo.m_stLinePos.m_i32XLineRight) -
                              phyPos.m_i32X ) / xLineLength * LOGIC_LINE;
    }

    if((isLineHasLocated(emMorot_Y)) && (WK_PhyPosNotLimit != phyPos.m_i32Y))
    {
        float yLineLength = abs(m_stDTaskInfo.m_stLinePos.m_i32YLineIn - m_stDTaskInfo.m_stLinePos.m_i32YLineOut);
        logicPos.m_fY = ( std::max(m_stDTaskInfo.m_stLinePos.m_i32YLineIn, m_stDTaskInfo.m_stLinePos.m_i32YLineOut) -
                          phyPos.m_i32Y ) / yLineLength * LOGIC_LINE;
    }

    if((isLineHasLocated(emMorot_Z)) && (WK_PhyPosNotLimit != phyPos.m_i32Y))
    {
        float zLineLength = abs(m_stDTaskInfo.m_stLinePos.m_i32ZLineUp - m_stDTaskInfo.m_stLinePos.m_i32ZLineDown);
        logicPos.m_fZ = ( std::max(m_stDTaskInfo.m_stLinePos.m_i32ZLineUp, m_stDTaskInfo.m_stLinePos.m_i32ZLineDown) -
                          phyPos.m_i32Z ) / zLineLength * LOGIC_LINE;
    }
}

void ActionMotorXYZ::convertLogicPosToPhyPos(ST_XYZ_CPOS &logicPos, ST_XYZ_DPOS &phyPos)
{
    if(WK_PosNotLimit != logicPos.m_fX)
    {
        float xLineLength = abs(m_stDTaskInfo.m_stLinePos.m_i32XLineLeft - m_stDTaskInfo.m_stLinePos.m_i32XLineRight);
        phyPos.m_i32X = std::max(m_stDTaskInfo.m_stLinePos.m_i32XLineLeft, m_stDTaskInfo.m_stLinePos.m_i32XLineRight) -
                logicPos.m_fX / LOGIC_LINE * xLineLength;
    }
    else
    {
        phyPos.m_i32X = WK_PhyPosNotLimit;
    }

    if(WK_PosNotLimit != logicPos.m_fY)
    {
        float yLineLength = abs(m_stDTaskInfo.m_stLinePos.m_i32YLineIn - m_stDTaskInfo.m_stLinePos.m_i32YLineOut);
        phyPos.m_i32Y = std::max(m_stDTaskInfo.m_stLinePos.m_i32YLineIn, m_stDTaskInfo.m_stLinePos.m_i32YLineOut) -
                logicPos.m_fY / LOGIC_LINE * yLineLength;
    }
    else
    {
        phyPos.m_i32Y = WK_PhyPosNotLimit;
    }

    if(WK_PosNotLimit != logicPos.m_fZ)
    {
        float zLineLength = abs(m_stDTaskInfo.m_stLinePos.m_i32ZLineUp - m_stDTaskInfo.m_stLinePos.m_i32ZLineDown);
        phyPos.m_i32Z = std::max(m_stDTaskInfo.m_stLinePos.m_i32ZLineUp, m_stDTaskInfo.m_stLinePos.m_i32ZLineDown) -
                logicPos.m_fZ / LOGIC_LINE * zLineLength;
    }
    else
    {
        phyPos.m_i32Z = WK_PhyPosNotLimit;
    }
}

bool ActionMotorXYZ::isAimLogicPosOverLimit(ST_XYZ_CPOS &logicPos)
{
    if( ((WK_PosNotLimit == logicPos.m_fX) ||
         ((logicPos.m_fX >= LOGIC_ZERO) && (logicPos.m_fX <= LOGIC_LINE)) ) &&
            ((WK_PosNotLimit == logicPos.m_fY) ||
            ((logicPos.m_fY >= LOGIC_ZERO) && (logicPos.m_fY <= LOGIC_LINE)) ) &&
            ((WK_PosNotLimit == logicPos.m_fZ) ||
            ((logicPos.m_fZ >= LOGIC_ZERO) && (logicPos.m_fZ <= LOGIC_LINE)) ) )
        return true;
    return false;
}

bool ActionMotorXYZ::isAimPhyPosOverLimit(ST_XYZ_DPOS &phyPos)
{
    if(!isAllLineHasLocated())
        return false;
    if( ((WK_PhyPosNotLimit == phyPos.m_i32X) ||
            ((phyPos.m_i32X >= m_stDTaskInfo.m_stLinePos.m_i32XLineLeft) && (phyPos.m_i32X <= m_stDTaskInfo.m_stLinePos.m_i32XLineRight)) )&&
            ( (WK_PhyPosNotLimit == phyPos.m_i32Y) ||
              ((phyPos.m_i32Y >= m_stDTaskInfo.m_stLinePos.m_i32YLineOut) && (phyPos.m_i32Y <= m_stDTaskInfo.m_stLinePos.m_i32YLineIn)) ) &&
            ( (WK_PhyPosNotLimit == phyPos.m_i32Z) ||
              ((phyPos.m_i32Z >= m_stDTaskInfo.m_stLinePos.m_i32ZLineUp) && (phyPos.m_i32Z <= m_stDTaskInfo.m_stLinePos.m_i32ZLineDown)) ) )
        return true;
    return false;
}

void ActionMotorXYZ::getPhyPos(ST_XYZ_DPOS &phyPos)
{
    phyPos = m_stDTaskInfo.m_stCurDPos;
}
