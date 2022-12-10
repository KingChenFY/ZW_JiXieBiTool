#include "tcpclient.h"
#include "quihelper.h"
#include "quihelperdata.h"

TcpClient::TcpClient(QTcpSocket *socket, QObject *parent) : QObject(parent)
{
    this->socket = socket;
    ip = socket->peerAddress().toString();
    ip = ip.replace("::ffff:", "");
    port = socket->peerPort();

    connect(socket, SIGNAL(disconnected()), this, SLOT(slot_disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(slot_readData()));
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(slot_error()));
#else
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slot_error()));
#endif
}

QString TcpClient::getIP() const
{
    return this->ip;
}

int TcpClient::getPort() const
{
    return this->port;
}

void TcpClient::slot_disconnected()
{
    emit disconnected(ip, port);
    socket->deleteLater();
    this->deleteLater();
}

void TcpClient::slot_error()
{
    emit error(ip, port, socket->errorString());
}

void TcpClient::slot_readData()
{
    QByteArray data = socket->readAll();
    if (data.length() <= 0) {
        return;
    }

//    emit receiveData(ip, port, buffer);
}

void TcpClient::sendData(const QString &data)
{
    QByteArray buffer;

    socket->write(buffer);
}

void TcpClient::disconnectFromHost()
{
    socket->disconnectFromHost();
}

void TcpClient::abort()
{
    socket->abort();
}
