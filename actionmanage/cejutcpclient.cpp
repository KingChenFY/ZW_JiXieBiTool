#include "cejutcpclient.h"
#include "global.h"
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
    m_bIsCejuInitSucceed = false;
    m_bIsFourPointCejuInitSucceed = false;
    m_bIsCejuRecordStartSucceed = false;
    m_bIsCejuRecordEndSucceed = false;
}
/***********************************四点测距测试任务***********************************/
void CeJuTcpClient::Ceju_FourPoint_Init()
{
    bool isSucceed;
    do{
        isSucceed = setCejuTriggerMode(emCeJuTrigMode_internal);
    }while(!isSucceed);

    m_bIsFourPointCejuInitSucceed = true;
}
void CeJuTcpClient::slot_Ceju_FourPoint_RecordStart(uint16_t num, uint16_t freq)
{
    m_stFPParam.m_u16OnePointReadNum = num;
    m_stFPParam.m_u16SampleFreq = freq;
    m_stFPParam.m_u16CurIndex = 0;
    memset(m_uTask1, 0, sizeof(int32_t)*WK_CeJuRecordNumMax);
    memset(m_uTask2, 0, sizeof(int32_t)*WK_CeJuRecordNumMax);
    memset(m_uTask3, 0, sizeof(int32_t)*WK_CeJuRecordNumMax);
    m_bIsOnePointRecordComplete = false;
//    m_autoMeasureTimer->start(m_stFPParam.m_u16SampleFreq*1000);
    m_autoMeasureTimer->start(m_stFPParam.m_u16SampleFreq);
}

void CeJuTcpClient::slot_AutoMeasureTimerOut()
{
    if(getCejuCurValue(stCurTaskValue))
    {
        emit signal_cejuValueUpdate();
        //如果采集的数量达到了要求，停止记录
        if(m_stFPParam.m_u16CurIndex < m_stFPParam.m_u16OnePointReadNum)
        {
            m_uTask1[m_stFPParam.m_u16CurIndex] = stCurTaskValue.task1Value;
            m_uTask2[m_stFPParam.m_u16CurIndex] = stCurTaskValue.task2Value;
            m_uTask3[m_stFPParam.m_u16CurIndex] = stCurTaskValue.task3Value;
            m_stFPParam.m_u16CurIndex++;
        }
        else
        {
            m_bIsOnePointRecordComplete = true;
        }
    }
    else
    {
        qDebug()<< QString("{CeJu}: AutoMeasure Fail");
    }
}
void CeJuTcpClient::Ceju_FourPoint_GetRecordData(int32_t destArray[emCeJuDataTtype_End][WK_CeJuRecordNumMax], uint16_t& num)
{
    num = m_stFPParam.m_u16CurIndex;
    memcpy(&destArray[emCeJuDataTtype_task1][0], &m_uTask1[0], num*sizeof(int32_t));
    memcpy(&destArray[emCeJuDataTtype_task2][0], &m_uTask2[0], num*sizeof(int32_t));
    memcpy(&destArray[emCeJuDataTtype_task3][0], &m_uTask3[0], num*sizeof(int32_t));
}
/***********************************测距任务***********************************/
void CeJuTcpClient::Ceju_Init()
{
    bool isSucceed;
    do{
        isSucceed = setCejuTriggerMode(emCeJuTrigMode_external);
    }while(!isSucceed);

    for(uint8_t i=0; i<4; i++)
    {
        do{
            isSucceed = setCejuTaskParameter((emCeJuTaskId)i);
        }while(!isSucceed);
    }

    do{
        isSucceed = setCejuOutputFormat(emCeJuOutputFormat_binary);
    }while(!isSucceed);

    m_bIsCejuInitSucceed = true;
}

void CeJuTcpClient::Ceju_RecordStart()
{
    bool isSucceed;
    qDebug() << "BFollow thread22:" << QThread::currentThread();

    while(true)
    {
        do{
            isSucceed = getCejuLogInfo(m_bIsRecordOn, m_uRecordNum);
        }while(!isSucceed);

        if(m_bIsRecordOn)
        {
            do{
                isSucceed = stopCejuRecord();
            }while(!isSucceed);
        }
        else if(m_uRecordNum != 0)
        {
            do{
                isSucceed = clearCejuRecord();
            }while(!isSucceed);
        }
        else
        {
            do{
                isSucceed = startCejuRecord(1, WK_CeJuRecordNumMax);
            }while(!isSucceed);
            break;
        }
    }
    m_bIsCejuRecordStartSucceed = true;
}

void CeJuTcpClient::Ceju_RecordEnd()
{
    bool isSucceed;
    m_uReadNum1 = 0;
    m_uReadNum2 = 0;
    m_uReadNum3 = 0;

    while(true)
    {
        do{
            isSucceed = getCejuLogInfo(m_bIsRecordOn, m_uRecordNum);
        }while(!isSucceed);

        if(m_bIsRecordOn)
        {
            do{
                isSucceed = stopCejuRecord();
            }while(!isSucceed);
        }
        else
        {
            break;
        }
    }

    while(true)
    {
        if(m_uReadNum1 >= m_uRecordNum)
            break;
        m_uAskNum = m_uRecordNum - m_uReadNum1;
        if(m_uAskNum > WK_CeJuAskNumMax)
            m_uAskNum = WK_CeJuAskNumMax;
        do{
            isSucceed = getCejuLogData(m_uReadNum1, m_uAskNum, emCeJuDataTtype_task1, m_uTask1);
        }while(!isSucceed);
        do{
            isSucceed = getCejuLogData(m_uReadNum2, m_uAskNum, emCeJuDataTtype_task2, m_uTask2);
        }while(!isSucceed);
        do{
            isSucceed = getCejuLogData(m_uReadNum3, m_uAskNum, emCeJuDataTtype_task3, m_uTask3);
        }while(!isSucceed);
    }

    m_bIsCejuRecordEndSucceed = true;
}

void CeJuTcpClient::Ceju_GetRecordData(int32_t destArray[emCeJuDataTtype_End][WK_CeJuRecordNumMax], uint32_t& num)
{
    num = m_uRecordNum;
    memcpy(&destArray[emCeJuDataTtype_task1][0], &m_uTask1[0], m_uRecordNum*sizeof(int32_t));
    memcpy(&destArray[emCeJuDataTtype_task2][0], &m_uTask2[0], m_uRecordNum*sizeof(int32_t));
    memcpy(&destArray[emCeJuDataTtype_task3][0], &m_uTask3[0], m_uRecordNum*sizeof(int32_t));
}
void CeJuTcpClient::Ceju_GetOneData(ST_CeJuCurValue &stData)
{
    stData = stCurTaskValue;
}

/***********************************测距指令发送读取解析***********************************/
void CeJuTcpClient::slot_readData()
{
    qDebug() << "CeJuTcpClient slot_readData thread:" << QThread::currentThread();
}

bool CeJuTcpClient::setCejuOutputFormat(emCeJuOutputFormat outFormat)
{
    QString cmdStr;
    if(emCeJuOutputFormat_binary == outFormat)
        cmdStr = "YS 756 1\r";
    else
        cmdStr = "YS 756 0\r";

    m_socket->write(cmdStr.toUtf8());

    if(m_socket->waitForReadyRead())
    {
        QByteArray qdata = m_socket->readAll();
        if (qdata.length() < 3) {
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

bool CeJuTcpClient::setCejuTaskParameter(emCeJuTaskId cejuTaskId)
{
    uint8_t u8UnitNumber = 40 + 20*cejuTaskId;


    for(uint8_t u8DataNumber = 1; u8DataNumber<3; u8DataNumber++)
    {
        QString cmdStr("DS");
        cmdStr += " ";
        cmdStr += QString::number(u8UnitNumber);
        cmdStr += " ";
        cmdStr += QString::number(u8DataNumber);
        cmdStr += " ";
        cmdStr += QString::number(cejuTaskId);
        cmdStr += "\r";
        m_socket->write(cmdStr.toUtf8());

        if(m_socket->waitForReadyRead())
        {
            QByteArray qdata = m_socket->readAll();
            if (qdata.length() < 3) {
                return false;
            }
            uint8_t *puData = (uint8_t *)qdata.data();

            if ('E' == puData[0] && 'R' == puData[1])
                return false;
            if ('O' == puData[0] && 'K' == puData[1])
                continue;
        }
        return false;
    }
    return true;
}
//no use
bool CeJuTcpClient::setCejuTriggerModeMain(emCeJuTrigMode trigMode)
{
    QString cmdStr;
    if(emCeJuTrigMode_external == trigMode)
        cmdStr = "DS 46 3 0\r";
    else
        cmdStr = "DS 46 3 1\r";

    m_socket->write(cmdStr.toUtf8());

    if(m_socket->waitForReadyRead())
    {
        QByteArray qdata = m_socket->readAll();
        if (qdata.length() < 3) {
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

bool CeJuTcpClient::setCejuTriggerMode(emCeJuTrigMode trigMode)
{
    QString cmdStr;
    if(emCeJuTrigMode_external == trigMode)
        cmdStr = "YS 1120 1\r";
    else
        cmdStr = "YS 1120 0\r";

    m_socket->write(cmdStr.toUtf8());

    if(m_socket->waitForReadyRead())
    {
        QByteArray qdata = m_socket->readAll();
        if (qdata.length() < 3) {
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
            records[u32StartId] = common_read_u32(&puData[j]);
            u32StartId++;
            j+=4;
        }
        return true;
    }
    return false;
}

/***********************************响应测距ui线程的槽***********************************/
void CeJuTcpClient::slot_getCejuLogInfo()
{
    bool isSucceed;
    isSucceed = getCejuLogInfo(m_bIsRecordOn, m_uRecordNum);
    emit signal_getCejuLog_UiUpdate(isSucceed);
}

void CeJuTcpClient::slot_startCejuRecord(uint16_t u16RecordGap, uint32_t u32SaveNum)
{
    bool isSucceed;
    isSucceed = startCejuRecord(u16RecordGap, u32SaveNum);
    emit signal_startCejuRecord_UiUpdate(isSucceed);
}

void CeJuTcpClient::slot_stopCejuRecord()
{
    bool isSucceed;
    isSucceed = stopCejuRecord();
    emit signal_stopCejuRecord_UiUpdate(isSucceed);
}

void CeJuTcpClient::slot_clearCejuRecord()
{
    bool isSucceed;
    isSucceed = clearCejuRecord();
    emit signal_clearCejuRecord_UiUpdate(isSucceed);
}

void CeJuTcpClient::slot_setCejuTriggerModeExternal()
{
    bool isSucceed;
    isSucceed = setCejuTriggerMode(emCeJuTrigMode_external);
    if(isSucceed)
        m_autoMeasureTimer->stop();
    emit signal_setCejuTriggerModeExternal_UiUpdate(isSucceed);
}

void CeJuTcpClient::slot_setCejuTriggerModeInternal()
{
    bool isSucceed;
    isSucceed = setCejuTriggerMode(emCeJuTrigMode_internal);
    if(isSucceed)
        m_autoMeasureTimer->start(CEJU_READ_FREQ);
    emit signal_setCejuTriggerModeInternal_UiUpdate(isSucceed);
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

    slot_connectToCeju();
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
    m_autoMeasureTimer->stop();
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
//    m_autoMeasureTimer->start(60);

    //连接上测距后触发测距初始化
    Ceju_Init();

    emit signal_cejuConnected();
}

void CeJuTcpClient::slot_noLink()
{
    qDebug()<< (QString("{CeJu}:%1:%2, NoLink!").arg(m_strIp).arg(m_u16Port));
    isConnect = false;
    m_bIsCejuInitSucceed = false;

    emit signal_cejuNoLink();
}

void CeJuTcpClient::slot_netErrDeal()
{}
