#ifndef CTRLTCPCLIENT_H
#define CTRLTCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

#define DEFALUT_LIST_CONTINUOUS_READ_MAX        3

typedef enum
{
    emSENDER_GETTASK_LINKLIST,
    emSENDER_DEFAULT_LIST,
}emGetTaskSender;
class CtrlTcpClient : public QObject
{
    Q_OBJECT
public:
    explicit CtrlTcpClient(QObject *parent = nullptr);
    void slot_initCtrlClient();
    void slot_noLink();
    void slot_readData();
    void slot_connected();
    void slot_netErrDeal();

private:
    QTcpSocket *m_socket;
    QString m_strIp;
    uint16_t m_u16Port;
    bool isConnect;
    QTimer *m_linkTimer;
    QTimer *m_sendTimer;
    uint8_t m_defaultListId;
    uint8_t m_defaultListReadCnt;
    emGetTaskSender m_eGetTaskSender;

signals:
    void signal_netRecvData();
    void signal_netConnected();
    void signal_netNoLink();

private slots:
    void slot_LinkTimerOut();
    void slot_SendTimerOut();
};

#endif // CTRLTCPCLIENT_H
