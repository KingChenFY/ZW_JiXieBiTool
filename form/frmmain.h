#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QWidget>
#include <QThread>
#include <QTcpSocket>

#include "formctrl.h"
#include "ctrltcpclient.h"
#include "actiondatamanage.h"

namespace Ui {
class FrmMain;
}

class FrmMain : public QWidget
{
    Q_OBJECT

public:
    explicit FrmMain(QWidget *parent = nullptr);
    ~FrmMain();

    FormCtrl *formCtrl;

private:
    Ui::FrmMain *ui;
    //机械臂客户端
    CtrlTcpClient *ctrlClient;
    QThread ctrlNetThread;
    //机械臂解析响应线程
    ActionDataManage *actnDataManage;
    QThread dataManageThread;

private slots:
    void initForm();
    void initConfig();
};

#endif // FRMMAIN_H
