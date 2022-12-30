#ifndef FORMCEJU_H
#define FORMCEJU_H

#include <QWidget>
#include <QThread>
#include "cejutcpclient.h"

namespace Ui {
class FormCeJu;
}

class FormCeJu : public QWidget
{
    Q_OBJECT

public:
    explicit FormCeJu(QWidget *parent = nullptr);
    ~FormCeJu();

private:
    Ui::FormCeJu *ui;

    //测距线程
    CeJuTcpClient *cejuClient;
    QThread cejuThread;

private slots:
    void initForm();
    void initConfig();

    void slot_cejuConnected();
    void slot_cejuNoLink();
    void slot_cejuValueUpdate();

    void on_pbtn_linkceju_clicked();

    void on_pbtn_getLogInfo_clicked();

signals:
    void signal_cejuNeedConnect();
    void signal_cejuAbortConnect();
    void signal_getCejuLogInfo();
};

#endif // FORMCEJU_H
