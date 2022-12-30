#include "cejutcpclient.h"
#include "global.h"
#include "cejucmddealagent.h"
#include "quihelperdata.h"
#include <QThread>

CeJuTcpClient::CeJuTcpClient(QObject *parent)
    : QObject{parent}
{
    m_strIp = "192.168.250.50";
    m_u16Port = 9600;
//      m_strIp = "171.16.100.225";
//      m_u16Port = 8888;
    isConnect = false;
}

/***********************************测距指令发送读取解析***********************************/
void CeJuTcpClient::slot_readData()
{
    qDebug() << "CeJuTcpClient slot_readData thread:" << QThread::currentThread();
}

bool CeJuTcpClient::getCejuCurValue(ST_CeJuCurValue &stCurCejuValue)
{
    QString cmdStr("MS 4\r");
    m_socket->write(cmdStr.toUtf8());

    if(m_socket->waitForReadyRead())
    {
        QByteArray qdata = m_socket->readAll();
        if (qdata.length() < 48) {
            return false;
        }

        QByteArray task1 = qdata.mid(0, 11);
        QByteArray task2 = qdata.mid(12, 11);
        QByteArray task3 = qdata.mid(24, 11);

        QString invalidStr("-----------");
        QString value1 = QUIHelperData::byteArrayToAsciiStr(task1);
        if(invalidStr == value1)
            stCurCejuValue.task1Value = 0;
        else
            stCurCejuValue.task1Value = value1.toInt();
        QString value2 = QUIHelperData::byteArrayToAsciiStr(task2);
        if(invalidStr == value2)
            stCurCejuValue.task2Value = 0;
        else
            stCurCejuValue.task2Value = value2.toInt();
        QString value3 = QUIHelperData::byteArrayToAsciiStr(task3);
        if(invalidStr == value3)
            stCurCejuValue.task3Value = 0;
        else
            stCurCejuValue.task3Value = value3.toInt();
        return true;

    }
    return false;
}

bool CeJuTcpClient::getCejuLogInfo(bool &bRecordOn, uint32_t &uRecordNum)
{
    QString cmdStr("LI\r");
    m_socket->write(cmdStr.toUtf8());

    if(m_socket->waitForReadyRead())
    {
        QByteArray qdata = m_socket->readAll();
        if (qdata.length() <= 0) {
            return false;
        }
        uint8_t *puData = (uint8_t *)qdata.data();

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
    return false;
}

bool CeJuTcpClient::startCejuRecord(uint16_t u16RecordGap, uint32_t u32SaveNum)
{
    QString cmdStr("LS");

    cmdStr += " ";
    cmdStr += QString::number(u16RecordGap);
    cmdStr += " ";
    cmdStr += QString::number(u32SaveNum);
    cmdStr += "\r";

    m_socket->write(cmdStr.toUtf8());

    if(m_socket->waitForReadyRead())
    {
        QByteArray qdata = m_socket->readAll();
        if (qdata.length() <= 0) {
            return false;
        }
        uint8_t *puData = (uint8_t *)qdata.data();

        if ('E' == puData[0] && 'R' == puData[1])
            return false;
        if ('O' == puData[0] && 'K' == puData[1])
            return true;
    }
    return false;
}

bool CeJuTcpClient::stopCejuRecord()
{
    QString cmdStr("LE\r");
    m_socket->write(cmdStr.toUtf8());

    if(m_socket->waitForReadyRead())
    {
        QByteArray qdata = m_socket->readAll();
        if (qdata.length() <= 0) {
            return false;
        }
        uint8_t *puData = (uint8_t *)qdata.data();

        if ('E' == puData[0] && 'R' == puData[1])
            return false;
        if ('O' == puData[0] && 'K' == puData[1])
            return true;
    }
    return false;
}

bool CeJuTcpClient::clearCejuRecord()
{
    QString cmdStr("LC\r");
    m_socket->write(cmdStr.toUtf8());

    if(m_socket->waitForReadyRead())
    {
        QByteArray qdata = m_socket->readAll();
        if (qdata.length() <= 0) {
            return false;
        }
        uint8_t *puData = (uint8_t *)qdata.data();

        if ('E' == puData[0] && 'R' == puData[1])
            return false;
        if ('O' == puData[0] && 'K' == puData[1])
            return true;
    }
    return false;
}

bool CeJuTcpClient::getCejuLogData(uint32_t &u32StartId, uint32_t &u32AskNum, emCeJuTaskId cejuTaskId, int32_t *records)
{
    QString cmdStr("LO");
    cmdStr += " ";
    cmdStr += QString::number(cejuTaskId);
    cmdStr += " ";
    cmdStr += QString::number(u32StartId);
    cmdStr += " ";
    cmdStr += QString::number(u32AskNum);
    cmdStr += "\r";
    m_socket->write(cmdStr.toUtf8());

    if(m_socket->waitForReadyRead())
    {
        QByteArray qdata = m_socket->readAll();
        if (qdata.length() <= 0) {
            return false;
        }
        uint8_t *puData = (uint8_t *)qdata.data();
        uint16_t puDataLen = qdata.length();

        if ('E' == puData[0] && 'R' == puData[1])
            return false;

        if(puDataLen < 4*u32AskNum)
            return false;

        for(uint32_t i=0, j=0; i<u32AskNum; i++)
        {
            records[u32StartId+i] = common_read_u32(&puData[j]);
            j+=4;
        }
        return true;
    }
    return false;
}


void CeJuTcpClient::slot_AutoMeasureTimerOut()
{
    if(getCejuCurValue(stCurTaskValue))
        emit signal_cejuValueUpdate();
}
/***********************************测距网络底层驱动***********************************/
void CeJuTcpClient::slot_initCeJuClient()
{
    qDebug() << "net thread:" << QThread::currentThread();
    m_socket = new QTcpSocket;
    connect(m_socket, &QTcpSocket::disconnected, this, &CeJuTcpClient::slot_noLink);
//    connect(m_socket, &QTcpSocket::readyRead, this, &CeJuTcpClient::slot_readData);
    connect(m_socket, &QTcpSocket::connected, this, &CeJuTcpClient::slot_connected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &CeJuTcpClient::slot_netErrDeal);

    m_linkTimer = new QTimer;
    connect(m_linkTimer, SIGNAL(timeout()), this, SLOT(slot_LinkTimerOut()));
    m_autoMeasureTimer = new QTimer;
    connect(m_autoMeasureTimer, SIGNAL(timeout()), this, SLOT(slot_AutoMeasureTimerOut()));
}

void CeJuTcpClient::slot_connectToCeju()
{
    m_socket->connectToHost(m_strIp, m_u16Port);
    qDebug()<< (QString("{CeJu}:connect in process__%1:%2").arg(m_strIp).arg(m_u16Port));

    if(!m_socket->waitForConnected(2000))
    {
        qDebug()<< (QString("{CeJu}:connect to %1:%2 fail, wait connect...").arg(m_strIp).arg(m_u16Port));
        isConnect = false;
        m_linkTimer->start(3000);
    }
}

void CeJuTcpClient::slot_abortConnectCeju()
{
    m_socket->abort();
}

void CeJuTcpClient::slot_LinkTimerOut()
{
    if(!isConnect)
    {
        m_socket->abort();
        m_socket->connectToHost(m_strIp, m_u16Port);
        qDebug()<< (QString("{CeJu}:connect in process__%1:%2").arg(m_strIp).arg(m_u16Port));
        if(!m_socket->waitForConnected(2000))
             qDebug()<< (QString("{CeJu}:connect to %1:%2 fail, wait connect...").arg(m_strIp).arg(m_u16Port));
    }
}

void CeJuTcpClient::slot_connected()
{
    qDebug()<< (QString("{CeJu}:%1:%2, Connect OK!").arg(m_strIp).arg(m_u16Port));
    isConnect = true;
    m_linkTimer->stop();

    m_autoMeasureTimer->start(5);

//    bool bRecordOn = false;
//    uint32_t uRecordNum = 3;
//    getCejuLogInfo(bRecordOn, uRecordNum);
//    uint32_t u32StartId = 0;
//    uint32_t u32AskNum = 100;
//    getCejuLogData(u32StartId, u32AskNum, emCeJuDataTtype_task1, m_uTask1);
    emit signal_cejuConnected();
}

void CeJuTcpClient::slot_noLink()
{
    qDebug()<< (QString("{CeJu}:%1:%2, NoLink!").arg(m_strIp).arg(m_u16Port));
    isConnect = false;

    emit signal_cejuNoLink();
}

void CeJuTcpClient::slot_netErrDeal()
{}
