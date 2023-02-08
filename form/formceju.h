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

    //测距线程
    CeJuTcpClient *cejuClient;
    QThread cejuThread;

private:
    Ui::FormCeJu *ui;

private slots:
    void initForm();
    void initConfig();

    void slot_getCejuLog_UiUpdate(bool isSucceed);
    void slot_startCejuRecord_UiUpdate(bool isSucceed);
    void slot_stopCejuRecord_UiUpdate(bool isSucceed);
    void slot_clearCejuRecord_UiUpdate(bool isSucceed);
    void slot_setCejuTriggerModeExternal_UiUpdate(bool isSucceed);
    void slot_setCejuTriggerModeInternal_UiUpdate(bool isSucceed);


    void slot_cejuConnected();
    void slot_cejuNoLink();
    void slot_cejuValueUpdate();

    void on_pbtn_linkceju_clicked();
    void on_pbtn_getLog_clicked();
    void on_pbtn_stLog_clicked();
    void on_pbtn_clearLog_clicked();
    void on_rbtn_ExTrigger_clicked();
    void on_rbtn_InTrigger_clicked();

signals:
    void signal_cejuNeedConnect();
    void signal_cejuAbortConnect();
    void signal_getCejuLogInfo();
    void signal_startCejuRecord(uint16_t u16RecordGap, uint32_t u32SaveNum);
    void signal_stopCejuRecord();
    void signal_clearCejuRecord();
    void signal_setCejuTriggerModeExternal();
    void signal_setCejuTriggerModeInternal();
};

#endif // FORMCEJU_H
