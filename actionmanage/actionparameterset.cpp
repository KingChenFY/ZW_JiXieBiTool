#include "actionparameterset.h"
#include "global.h"
#include "head.h"

extern SetTaskRingFifo stTaskFifo;

ActionParameterSet::ActionParameterSet(QObject *parent)
    : QObject{parent}, HardCmdParser()
{
    m_bISTrailInfoSet = false;
    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_setEquationParam, this);
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

    if (EnumBoardId_setEncodeCh == uCmdId)
    {
        // 向gettask链表中压入查询指令
//        getTaskSend();
        _LOG(QString("parse setTask cmd return"));
        return emCMD_SET;
    }

    if (EnumBoardId_getEncodeCh == uCmdId)
    {
        uint8_t uLen = CMD_CONTENT_INDEX;

        //......协议解析补全
        m_stTrigInfoD.emAimAxis = (emMotorObj)puData[uLen];
        uLen += 1;

        m_stTrigInfoD.u16TrigDistance = common_read_u16(&puData[uLen]);
        uLen += 2;

        m_stTrigInfoD.u8TrigObj = puData[uLen];;
        uLen += 1;

        // 更新 垂直扫描 界面信息
//        emit signal_UiUpdate();
        return emCMD_GET;
    }
    _LOG(QString("No such cmd = [%1]").arg(uCmdId));
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

void ActionParameterSet::setTrigParameterSend()
{
    uint16_t i = 0;
    STRUCT_SETTASK_MESSAGE_INFO *pWriteAddr;

    //把 settask 指令封装到发送队列
    pWriteAddr = stTaskFifo.getWriteMessageAddr(EnumBoardId_setEncodeCh);
    if(pWriteAddr)
    {
        pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTrigInfoToSend.emAimAxis;
        common_write_u16(&pWriteAddr->u8CmdContent[i], (uint16_t)m_stTrigInfoToSend.u16TrigDistance);
        i += 2;
        pWriteAddr->u8CmdContent[i++] = (uint8_t)m_stTrigInfoToSend.u8TrigObj;
        pWriteAddr->u16CmdContentLen = i;

        stTaskFifo.pushWriteMessageToFifo();
    }
    else
    {
        qDebug() << QString("[%1][%2][%3]: settask fifo full, data lost").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__);
    }
}

void ActionParameterSet::getTrigParameterSend()
{
    STRUCT_SETTASK_MESSAGE_INFO *pWriteAddr;

    //把 settask 指令封装到发送队列
    pWriteAddr = stTaskFifo.getWriteMessageAddr(EnumBoardId_getEncodeCh);
    if(pWriteAddr)
    {
        pWriteAddr->u16CmdContentLen = 0;
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
