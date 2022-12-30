#include "actiondatamanage.h"
#include "global.h"
#include <QDebug>
#include <QThread>
#include <QMutex>

#include "hardcmdparseagent.h"
#include "head.h"

extern QMutex mutex_LNode;
extern NetRecvRingFifo ntRecvFifo;

ActionDataManage::ActionDataManage(QObject *parent)
    : QObject{parent}
{

}

void ActionDataManage::slot_initDataMange()
{
    qDebug() << "datamanage thread:" << QThread::currentThread();
}

void ActionDataManage::slot_netRecvData()
{
    HardCmdParseAgent &cmdParseAgent = HardCmdParseAgent::GetInstance();

    uint32_t packLength = ntRecvFifo.findOnePackFromFifo();
    while(packLength != 0)
    {
//        qDebug() << "ActionDataManage::slot_netRecvData deal"<< packLength << TIMEMS;
        uint8_t packData[WK_NET_ONE_MESSAGE_MAX_LEN] = {0};
        ntRecvFifo.popOnePackFromFifo(packData, packLength);

        emBoardCmdId eCmdId = (emBoardCmdId)packData[WK_BOARD_COMMAND_ID_INDEX];
        emWKCmdType eCmdType;
        //对应模块解析指令数据
        cmdParseAgent.parseCmd(eCmdId, &packData[WK_BOARD_COMMAND_ID_INDEX], eCmdType);
        packLength = ntRecvFifo.findOnePackFromFifo();
    }
}
