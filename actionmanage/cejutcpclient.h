#ifndef CEJUTCPCLIENT_H
#define CEJUTCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

#define WK_CeJuRecordNumMax 4096*14 //测距最大记录个数
#define WK_CeJuAskNumMax	100//单次取最大个数
typedef struct
{
    int32_t task1Value;
    int32_t task2Value;
    int32_t task3Value;
}ST_CeJuCurValue;

typedef enum
{
    emCeJuDataTtype_task1 = 0,
    emCeJuDataTtype_task2,
    emCeJuDataTtype_task3,
    emCeJuDataTtype_task4,

    emCeJuDataTtype_End
}emCeJuTaskId;

class CeJuTcpClient : public QObject
{
    Q_OBJECT
public:
    explicit CeJuTcpClient(QObject *parent = nullptr);

    void slot_initCeJuClient();
    void slot_connectToCeju();
    void slot_abortConnectCeju();
    void slot_noLink();
    void slot_readData();
    void slot_connected();
    void slot_netErrDeal();

    bool getCejuCurValue(ST_CeJuCurValue &stCurCejuValue);
    bool getCejuLogInfo(bool &bRecordOn, uint32_t &uRecordNum);
    bool startCejuRecord(uint16_t u16RecordGap, uint32_t u32SaveNum);
    bool stopCejuRecord();
    bool clearCejuRecord();
    bool getCejuLogData(uint32_t &u32StartId, uint32_t &u32AskNum, emCeJuTaskId cejuTaskId, int32_t *records);

    ST_CeJuCurValue stCurTaskValue;

private:
    QTcpSocket *m_socket;
    QString m_strIp;
    uint16_t m_u16Port;
    bool isConnect;

    QTimer *m_linkTimer;
    QTimer *m_autoMeasureTimer;

    uint32_t m_uRecordNum;// 当前记录总量
    uint32_t m_uAskNum;// 一次读取数量

    uint32_t m_uReadNum1;// 当前已读取数量
    int32_t m_uTask1[WK_CeJuRecordNumMax];// 数据内容
    uint32_t m_uReadNum2;// 已读取数量--厚度
    int32_t m_uTask2[WK_CeJuRecordNumMax];// 数据内容
    uint32_t m_uReadNum3;// 已读取数量--厚度
    int32_t m_uTask3[WK_CeJuRecordNumMax];// 数据内容

signals:
    void signal_cejuConnected();
    void signal_cejuNoLink();
    void signal_cejuValueUpdate();

private slots:
    void slot_LinkTimerOut();
    void slot_AutoMeasureTimerOut();
};

#endif // CEJUTCPCLIENT_H
