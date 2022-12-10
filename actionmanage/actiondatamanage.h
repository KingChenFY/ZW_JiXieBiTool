#ifndef ACTIONDATAMANAGE_H
#define ACTIONDATAMANAGE_H

#include <QObject>

class ActionDataManage : public QObject
{
    Q_OBJECT
public:
    explicit ActionDataManage(QObject *parent = nullptr);
    void slot_initDataMange();
    void slot_netRecvData();

signals:


};

#endif // ACTIONDATAMANAGE_H
