#ifndef CEJUCMDDEALAGENT_H
#define CEJUCMDDEALAGENT_H

#include <QObject>

typedef bool (*CeJuCmdParser) (uint8_t* puData, uint16_t &uLen);

typedef enum
{
    EnumCeJuId_START        = 0,

    EnumCeJuId_GetLogInfo_LI   = 1,
    EnumCeJuId_END,
}emCeJuCmdId;

class CeJuCmdDealAgent
{
public:
    static CeJuCmdDealAgent &GetInstance()
    {
        static CeJuCmdDealAgent m_instance;
        return m_instance;
    }
    ~CeJuCmdDealAgent(){};

    bool registerParser(emCeJuCmdId eCmdId, CeJuCmdParser pParser);
    bool parseCmd(emCeJuCmdId eCmdId, uint8_t* puCmdData, uint16_t &uLen);

private:
    CeJuCmdDealAgent();
    CeJuCmdParser m_listCmdParsers[EnumCeJuId_END];
};
#endif // CEJUCMDDEALAGENT_H
