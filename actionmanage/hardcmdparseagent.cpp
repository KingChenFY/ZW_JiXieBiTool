#include "hardcmdparseagent.h"

#include "global.h"

HardCmdParseAgent::HardCmdParseAgent()
{
    for(int idx = EnumBoardId_START; idx < EnumBoardId_END; idx++)
    {
        m_listCmdParsers[idx] = NULL;
    }
}

HardCmdParseAgent::~HardCmdParseAgent()
{}

bool HardCmdParseAgent::registerParser(emBoardCmdId eCmdId, HardCmdParser* pParser)
{
    if(EnumBoardId_START < eCmdId && eCmdId < EnumBoardId_END)
    {
        m_listCmdParsers[eCmdId] = pParser;
        return true;
    }

    _LOG(QString("registerParser : 指令类型[%1]无法注册解析对象，不应出现的打印，请检查代码").arg(eCmdId));
    return false;
}

bool HardCmdParseAgent::setTaskCmdReSend(emBoardCmdId eCmdId, uint32_t sdNum, bool &isReSend)
{
    if( EnumBoardId_START < eCmdId && eCmdId < EnumBoardId_END )
    {
        HardCmdParser* pParser = m_listCmdParsers[eCmdId];
        if(NULL == pParser)
        {
            _LOG(QString("setTaskCmdReSend : 指令类型[%1]未注册解析对象，丢弃").arg(eCmdId));
        }
        else
        {
            isReSend = pParser->setTaskCmdReSend(sdNum);
        }
        return true;
    }

    _LOG(QString("setTaskCmdReSend : 指令类型[%1]不在定义类型内，不应出现的打印，请检查代码").arg(eCmdId));
    return false;
}

bool HardCmdParseAgent::parseCmd(emBoardCmdId eCmdId, uint8_t* puCmdData, emWKCmdType &cmdType)
{
    if( EnumBoardId_START < eCmdId && eCmdId < EnumBoardId_END )
    {
        HardCmdParser* pParser = m_listCmdParsers[eCmdId];
        if(NULL == pParser)
        {
            _LOG(QString("registerParser : Cmd[%1], Not regist parser").arg(eCmdId));
        }
        else
        {
            cmdType = pParser->parseCmd(puCmdData);
        }
        return true;
    }

    _LOG(QString("registerParser : not find Cmd[%1] in cmd list").arg(eCmdId));
    return false;
}
