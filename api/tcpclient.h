#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QtNetwork>

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QTcpSocket *socket, QObject *parent = 0);

private:
    QTcpSocket *socket;
    QString ip;
    int port;

public:
    QString getIP() const;
    int getPort()   const;

private slots:
    void slot_disconnected();
    void slot_error();
    void slot_readData();

signals:
    void disconnected(const QString &ip, int port);
    void error(const QString &ip, int port, const QString &error);

    void sendData(const QString &ip, int port, const QString &data);
    void receiveData(const QString &ip, int port, const QString &data);

public slots:
    void sendData(const QString &data);
    void disconnectFromHost();
    void abort();
};

#endif // TCPCLIENT_H
