#include "actionparameterset.h"
#include "global.h"
#include "head.h"

extern SetTaskRingFifo stTaskFifo;

ActionParameterSet::ActionParameterSet(QObject *parent)
    : QObject{parent}, HardCmdParser()
{
    m_bISTrailInfoSet = false;
    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_setEquationParam, this);
    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_getBoxVRecords, this);
    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_setBoxVLedOn, this);
}

emWKCmdType ActionParameterSet::parseCmd(uint8_t* puData)
{
    uint8_t uCmdId = puData[CMD_ID_INDEX];
    uint32_t uReturnCmdFlag = common_read_u32(&puData[CMD_FLAG_INDEX]);

    if (EnumBoardId_setEquationParam == uCmdId)
    {
        uint8_t uLen = CMD_CONTENT_INDEX;

        //......协议解析补全
        m_stTrailInfoGetD.ubIsSuccess = (bool)puData[uLen];
        uLen += 1;

        m_stTrailInfoGetD.u16TrailId = common_read_u16(&puData[uLen]);
        uLen += 2;

        m_stTrailInfoGetD.u16TrailSynNum = common_read_u16(&puData[uLen]);
        uLen += 2;

        double tempSyn[TRAIL_SYN_MAX_NUM] = {0};
        for (uint16_t i=0; i<m_stTrailInfoGetD.u16TrailSynNum; i++)
        {
            memcpy((uint8_t*)tempSyn, &puData[uLen], 8);
            uLen += 8;
        }
        m_bISTrailInfoSet = true;

        // 更新 垂直扫描 界面信息
//        emit signal_UiUpdate();
        return emCMD_GET;
    }
    else if (EnumBoardId_getBoxVRecords == uCmdId)
    {
        uint16_t uLen = CMD_CONTENT_INDEX;

        //......协议解析补全
        m_stMotorVGetSlideInfoD.u8AskNum = puData[uLen];
        uLen += 1;

        m_stMotorVGetSlideInfoD.u8BgnIdx = puData[uLen];
        uLen += 1;

        m_stMotorVGetSlideInfoD.u8TotalNum = puData[uLen];
        uLen += 1;

        m_stMotorVGetSlideInfoD.u8GetNum = puData[uLen];
        uLen += 1;

        for(uint8_t i=m_stMotorVGetSlideInfoD.u8BgnIdx; i<m_stMotorVGetSlideInfoD.u8GetNum; i++)
        {
            m_stMotorVGetSlideInfoD.stSlidePos[i].m_u8insex = puData[uLen];
            uLen += 1;
            m_stMotorVGetSlideInfoD.stSlidePos[i].m_u8ztoo = puData[uLen];
            uLen += 1;
            m_stMotorVGetSlideInfoD.stSlidePos[i].m_i32pos = (int32_t)common_read_u32(&puData[uLen]);
            uLen += 4;
        }
        // 更新 垂直扫描 界面信息
        if(m_stMotorVGetSlideInfoD.u8GetNum > 0)
            emit signal_SlideInfoUiUpdate();
        return emCMD_GET;
    }
    if (EnumBoardId_setBoxVLedOn == uCmdId)
    {
        uint8_t uLen = CMD_CONTENT_INDEX;

        //......协议解析补全
        m_u8IsOpenLaserD = (bool)puData[uLen];
        uLen += 1;
        // 更新 垂直扫描激光按钮 界面信息
        emit signal_MVLaserUiUpdate(m_u8IsOpenLaserD);
        return emCMD_GET;
    }
    _LOG(QString("No such cmd = [%1]").arg(uCmdId));
}

void ActionParameterSet::getMotorVSlideData(ST_MOTORV_SLIDEINFO_D &stData)
{
    stData = m_stMotorVGetSlideInfoD;
}

void ActionParameterSet::setFollowParameterSend()
{
    uint16_t i = 0;
    STRUCT_SETTASK_MESSAGE_INFO *pWriteAddr;

    //把 settask 指令封装到发送队列
    pWriteAddr = stTaskFifo.getWriteMessageAddr(EnumBoardId_setEquationParam);
    if(pWriteAddr)
    {
        common_write_u16(&pWriteAddr->u8CmdContent[i], m_stTrailInfoToSend.u16TrailId);
        i += 2;
        common_write_u16(&pWriteAddr->u8CmdContent[i], m_stTrailInfoToSend.u16TrailSynNum);
        i += 2;
        for (uint16_t j=0; j<m_stTrailInfoToSend.u16TrailSynNum; j++)
        {
            memcpy(&pWriteAddr->u8CmdContent[i], (uint8_t*)&m_stTrailInfoToSend.udSyn[j], 8);
            i += 8;
        }
        pWriteAddr->u16CmdContentLen = i;

        stTaskFifo.pushWriteMessageToFifo();
    }
    else
    {
        qDebug() << QString("[%1][%2][%3]: settask fifo full, data lost").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__);
    }
}

//获取垂直扫描的玻片信息
void ActionParameterSet::getMotorVScanInfo()
{
    uint16_t i = 0;
    STRUCT_SETTASK_MESSAGE_INFO *pWriteAddr;

    //把 settask 指令封装到发送队列
    pWriteAddr = stTaskFifo.getWriteMessageAddr(EnumBoardId_getBoxVRecords);
    if(pWriteAddr)
    {/* 请求：AskNum(U8) + BgnIdx(U8) */
        pWriteAddr->u8CmdContent[i++] = m_stMotorVGetSlideInfoSet.u8AskNum;
        pWriteAddr->u8CmdContent[i++] = m_stMotorVGetSlideInfoSet.u8BgnIdx;
        pWriteAddr->u16CmdContentLen = i;

        stTaskFifo.pushWriteMessageToFifo();
    }
    else
    {
        qDebug() << QString("[%1][%2][%3]: settask fifo full, data lost").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__);
    }
}

//获取垂直扫描的玻片信息
void ActionParameterSet::setMotorVLaser()
{
    uint16_t i = 0;
    STRUCT_SETTASK_MESSAGE_INFO *pWriteAddr;

    //把 settask 指令封装到发送队列
    pWriteAddr = stTaskFifo.getWriteMessageAddr(EnumBoardId_setBoxVLedOn);
    if(pWriteAddr)
    {
        pWriteAddr->u8CmdContent[i++] = m_u8IsOpenLaser;
        pWriteAddr->u16CmdContentLen = i;

        stTaskFifo.pushWriteMessageToFifo();
    }
    else
    {
        qDebug() << QString("[%1][%2][%3]: settask fifo full, data lost").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__);
    }
}

bool ActionParameterSet::setTaskCmdReSend(uint32_t sdNum)
{
    return true;
}
