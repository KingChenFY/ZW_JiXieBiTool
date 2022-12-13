#include <QThread>
#include <QMutex>

#include "ctrltcpclient.h"
#include "global.h"
#include "hardcmdparseagent.h"
#include "head.h"
extern QMutex mutex_LNode;

extern SetTaskRingFifo stTaskFifo;
extern NetRecvRingFifo ntRecvFifo;
//extern GetTaskLinkList gtTaskLinkList;

CtrlTcpClient::CtrlTcpClient(QObject *parent)
    : QObject{parent}
{
    m_strIp = "10.10.10.150";
//    m_strIp = "171.16.100.225";
//    m_strIp = "192.168.182.1";
    m_u16Port = 8888;
    m_defaultListId = 0;
    m_defaultListReadCnt = 0;
    m_eGetTaskSender = emSENDER_GETTASK_LINKLIST;
}

void CtrlTcpClient::slot_initCtrlClient()
{
    qDebug() << "net thread:" << QThread::currentThread();
    m_socket = new QTcpSocket;
    connect(m_socket, &QTcpSocket::disconnected, this, &CtrlTcpClient::slot_noLink);
    connect(m_socket, &QTcpSocket::readyRead, this, &CtrlTcpClient::slot_readData);
    connect(m_socket, &QTcpSocket::connected, this, &CtrlTcpClient::slot_connected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &CtrlTcpClient::slot_netErrDeal);

    m_linkTimer = new QTimer;
    m_sendTimer = new QTimer;
    connect(m_linkTimer, SIGNAL(timeout()), this, SLOT(slot_LinkTimerOut()));
    connect(m_sendTimer, SIGNAL(timeout()), this, SLOT(slot_SendTimerOut()));

    m_socket->connectToHost(m_strIp, m_u16Port);
    _LOG(QString("connect in process__%1:%2").arg(m_strIp).arg(m_u16Port));

    if(!m_socket->waitForConnected(2000))
    {
        _LOG(QString("connect to %1:%2 fail, wait connect...").arg(m_strIp).arg(m_u16Port));
        isConnect = false;
        m_linkTimer->start(3000);
    }
}

void CtrlTcpClient::slot_connected()
{
    _LOG(QString("JXB__%1:%2, Connect OK!\n").arg(m_strIp).arg(m_u16Port));
    isConnect = true;
    m_linkTimer->stop();
    m_sendTimer->start(10);

    emit signal_netConnected();
}

void CtrlTcpClient::slot_noLink()
{
    _LOG(QString("JXB__%1:%2, NoLink!\n").arg(m_strIp).arg(m_u16Port));
    isConnect = false;
    m_linkTimer->start(3000);

    emit signal_netNoLink();
}

void CtrlTcpClient::slot_netErrDeal()
{}

void CtrlTcpClient::slot_readData()
{
    QByteArray qdata = m_socket->readAll();
    if (qdata.length() <= 0) {
        return;
    }

    uint8_t *data = (uint8_t *)qdata.data();
    uint32_t dataLen = qdata.size();
    if( ntRecvFifo.pushOnePackToFifo(data, dataLen) )
    {
        _LOG(QString("<==========TcpClient recv[%1]bytes").arg(dataLen));
        emit signal_netRecvData();
    }
    else
    {
        _LOG(QString("<==========TcpClient recv fifo FULL, data lost"));
    }
}

void CtrlTcpClient::slot_LinkTimerOut()
{
    if(!isConnect)
    {
        m_socket->abort();
        m_socket->connectToHost(m_strIp, m_u16Port);
        _LOG(QString("connect in process__%1:%2").arg(m_strIp).arg(m_u16Port));
        if(!m_socket->waitForConnected(2000))
             _LOG(QString("connect to %1:%2 fail, wait connect...").arg(m_strIp).arg(m_u16Port));
    }
}

void CtrlTcpClient::slot_SendTimerOut()
{
    if(isConnect)
    {
        STRUCT_SETTASK_MESSAGE_INFO *pSendAddr;
        uint8_t netSendData[WK_BOARD_COMMAND_MAX_LEN] = {0};
        uint16_t netSendLen = 0;

        //首先查询settask的队列有无待发送的数据
        pSendAddr = stTaskFifo.getReadMessageAddr();
        if( NULL != pSendAddr )
        {
            //先拷贝指令内容部分
            memcpy(&netSendData[WK_BOARD_CMD_HEAD_LENGTH], pSendAddr->u8CmdContent, pSendAddr->u16CmdContentLen);
            //封装指令
            netSendLen = formatBoardCmd(pSendAddr->eCmdId, netSendData, pSendAddr->u16CmdContentLen);
            //发送
            m_socket->write((const char*)netSendData, netSendLen);
            m_socket->flush();
            //把数据从队列移除
            stTaskFifo.popReadMessageOutFifo();

            _LOG(QString("==========>[settask]:TcpClient send CMD[%1],TYPE[%2],ID[%3]").
                 arg(netSendData[WK_BOARD_COMMAND_ID_INDEX]).arg(pSendAddr->u8TaskType).arg(pSendAddr->u8TaskId));
            return;
        }

        //发送权是否属于gettask链表
        if(emSENDER_GETTASK_LINKLIST == m_eGetTaskSender)
        {
            emBoardObject curSendObj;
            uint32_t curObjSdNum = 0;
            //查询gettask链表是否所有的数据都依次发送过一遍了
            if( GetTaskLinkList::GetInstance()/*gtTaskLinkList*/.app_getOneNotSendNodeData(netSendData, netSendLen) )
            {
                m_socket->write((const char*)netSendData, netSendLen);
                m_socket->flush();
    //                HardCmdParseAgent &cmdParseAgent = HardCmdParseAgent::GetInstance();
    //                cmdParseAgent.setTaskCmdReSend(st_wk_taskcmd_list[curSendObj].getCmd, curObjSdNum);

                _LOG(QString("==========>[gettask_linklist]:TcpClient send cmd[%1]").arg(netSendData[WK_BOARD_COMMAND_ID_INDEX]));
                return;
            }
            else
            {
                //查询链表中的数据已全部查过一遍或者查询链表为空，默认查询列表获得发送权
                m_eGetTaskSender = emSENDER_DEFAULT_LIST;
            }
        }

        //发送权属于默认查询链表
        netSendLen = formatBoardCmd((uint8_t)st_wk_taskcmd_list[m_defaultListId].getCmd, netSendData, 0);
        qint64 atcual_sendNum = m_socket->write((const char*)netSendData, netSendLen);
        m_socket->flush();
        m_defaultListId++;
        if (m_defaultListId >= EnumBoardObject_End )
            m_defaultListId = 0;
        m_defaultListReadCnt++;
        //默认列表连续读取DEFALUT_LIST_CONTINUOUS_READ_MAX条查询指令后，恢复status优先查询列表的执行
        if(m_defaultListReadCnt >= DEFALUT_LIST_CONTINUOUS_READ_MAX)
        {
            m_defaultListReadCnt = 0;
            m_eGetTaskSender = emSENDER_GETTASK_LINKLIST;       //gettask 链表获得发送权
        }
        _LOG(QString("==========>[gettask_defaultlist(T=%1)]:TcpClient send cmd[%2]").arg(m_defaultListReadCnt).arg(netSendData[WK_BOARD_COMMAND_ID_INDEX]));
        return;
    }
    else
    {
        m_sendTimer->stop();
    }
}
