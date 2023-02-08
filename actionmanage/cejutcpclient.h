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

typedef enum
{
    emCeJuTrigMode_internal,
    emCeJuTrigMode_external,

    emCeJuTrigMode_End
}emCeJuTrigMode;

typedef enum
{
    emCeJuOutputFormat_ascii,
    emCeJuOutputFormat_binary,  //二进制

    emCeJuOutputFormat_End
}emCeJuOutputFormat;

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

    bool setCejuTriggerMode(emCeJuTrigMode trigMode);
    bool setCejuTriggerModeMain(emCeJuTrigMode trigMode);
    bool setCejuTaskParameter(emCeJuTaskId cejuTaskId);
    bool setCejuOutputFormat(emCeJuOutputFormat outFormat);
    bool getCejuCurValue(ST_CeJuCurValue &stCurCejuValue);
    bool getCejuLogInfo(bool &bRecordOn, uint32_t &uRecordNum);
    bool startCejuRecord(uint16_t u16RecordGap, uint32_t u32SaveNum);
    bool stopCejuRecord();
    bool clearCejuRecord();
    bool getCejuLogData(uint32_t &u32StartId, uint32_t &u32AskNum, emCeJuTaskId cejuTaskId, int32_t *records);

    void slot_getCejuLogInfo();
    void slot_startCejuRecord(uint16_t u16RecordGap, uint32_t u32SaveNum);
    void slot_stopCejuRecord();
    void slot_clearCejuRecord();
    void slot_setCejuTriggerModeExternal();
    void slot_setCejuTriggerModeInternal();

    void Ceju_Init();
    void Ceju_RecordStart();
    void Ceju_RecordEnd();

    bool isConnect;
    bool m_bIsCejuInitSucceed;
    bool m_bIsCejuRecordStartSucceed;
    bool m_bIsCejuRecordEndSucceed;

    ST_CeJuCurValue stCurTaskValue;
    bool m_bIsRecordOn;
    uint32_t m_uRecordNum;// 当前记录总量

private:
    QTcpSocket *m_socket;
    QString m_strIp;
    uint16_t m_u16Port;

    //Excel文件路径
    QString path;
    //Excel文件名称
    QString name;
    //Excel文件完整名称
    QString fileName;
    void saveCejuRecordToExcel();

    QTimer *m_linkTimer;
    QTimer *m_autoMeasureTimer;

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

    void signal_getCejuLog_UiUpdate(bool isSucceed);
    void signal_startCejuRecord_UiUpdate(bool isSucceed);
    void signal_stopCejuRecord_UiUpdate(bool isSucceed);
    void signal_clearCejuRecord_UiUpdate(bool isSucceed);
    void signal_setCejuTriggerModeExternal_UiUpdate(bool isSucceed);
    void signal_setCejuTriggerModeInternal_UiUpdate(bool isSucceed);

private slots:
    void slot_LinkTimerOut();
    void slot_AutoMeasureTimerOut();
};

#endif // CEJUTCPCLIENT_H
