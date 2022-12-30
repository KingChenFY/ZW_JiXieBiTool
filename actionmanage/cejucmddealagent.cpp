#include "cejucmddealagent.h"
#include "global.h"
#include <string>

bool GetLogInfo_LI(uint8_t* puData, uint16_t &uLen)
{
    uint16_t uRecordNum = 0;
    bool bRecordOn = false;
    if('E' == puData[0] && 'R' == puData[1])
        return false;

    uint8_t a[7] = { '0', '0', '0', '0', '0', '0', '0' };
    if (!memcmp(a, &puData[10], 7))
        uRecordNum = 0;
    else
        uRecordNum = atoi((const char*)&puData[10]);
    if ('1' == puData[0])
        bRecordOn = true;
    else if ('0' == puData[0])
        bRecordOn = false;
    return true;
}

CeJuCmdDealAgent::CeJuCmdDealAgent()
{
    registerParser(EnumCeJuId_GetLogInfo_LI, GetLogInfo_LI);
}

bool CeJuCmdDealAgent::registerParser(emCeJuCmdId eCmdId, CeJuCmdParser pParser)
{
    if(EnumCeJuId_START < eCmdId && eCmdId < EnumCeJuId_END)
    {
        m_listCmdParsers[eCmdId] = pParser;
        return true;
    }

    _LOG(QString("Registe CeJuCmdParser Fail, not found id[%1]").arg(eCmdId));
    return false;
}

bool CeJuCmdDealAgent::parseCmd(emCeJuCmdId eCmdId, uint8_t* puCmdData, uint16_t &uLen)
{
     qDebug() << "CeJuCmdDealAgent parseCmd thread:" << QThread::currentThread();
    if( EnumCeJuId_START < eCmdId && eCmdId < EnumCeJuId_END )
    {
        CeJuCmdParser pParser = m_listCmdParsers[eCmdId];
        if(NULL == pParser)
        {
            _LOG(QString("registerParser : Cmd[%1], Not regist parser").arg(eCmdId));
        }
        else
        {
            pParser(puCmdData, uLen);
        }
        return true;
    }

    _LOG(QString("registerParser : not find Cmd[%1] in cmd list").arg(eCmdId));
    return false;
}
