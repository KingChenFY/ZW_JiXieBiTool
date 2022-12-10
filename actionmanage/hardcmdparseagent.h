#ifndef HARDCMDPARSEAGENT_H
#define HARDCMDPARSEAGENT_H

#include "hardcmd.h"

/*| 1 | 4 | N |*/
/*| 指令ID | 指令FLAG | 指令内容 |*/

#define CMD_ID_INDEX            0
#define CMD_FLAG_INDEX          1
#define CMD_CONTENT_INDEX       5

typedef enum
{
    emCMD_SET,
    emCMD_GET,
}emWKCmdType;

class HardCmdParser
{
public:
    HardCmdParser(){};
    ~HardCmdParser(){}
    virtual emWKCmdType parseCmd(uint8_t* puData) = 0;
    virtual bool setTaskCmdReSend(uint32_t sdNum) = 0;
};

class HardCmdParseAgent
{
public:
    static HardCmdParseAgent &GetInstance()
    {
        static HardCmdParseAgent m_instance;
        return m_instance;
    }
    ~HardCmdParseAgent();

    bool registerParser(emBoardCmdId eCmdId, HardCmdParser* pParser);
    bool setTaskCmdReSend(emBoardCmdId eCmdId, uint32_t sdNum, bool &isReSend);
    bool parseCmd(emBoardCmdId eCmdId, uint8_t* puCmdData, emWKCmdType &cmdType);

private:
    HardCmdParseAgent();
    HardCmdParser* m_listCmdParsers[EnumBoardId_END];
};

#endif // HARDCMDPARSEAGENT_H
