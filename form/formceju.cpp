#include "formceju.h"
#include "ui_formceju.h"
#include "global.h"

FormCeJu::FormCeJu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCeJu)
{
    ui->setupUi(this);
    initForm();
    initConfig();
}

FormCeJu::~FormCeJu()
{
    delete ui;
}

void FormCeJu::initForm()
{
}

void FormCeJu::initConfig()
{
    //实例化测距类
    cejuClient = new CeJuTcpClient;
    //测距线程
    cejuClient->moveToThread(&cejuThread);
    //关联数据管理线程的启动信号
    connect(&cejuThread, &QThread::started, cejuClient, &CeJuTcpClient::slot_initCeJuClient);
    _LOG(QString("create CeJu thread"));

    connect(this, &FormCeJu::signal_cejuNeedConnect, cejuClient, &CeJuTcpClient::slot_connectToCeju);
    connect(this, &FormCeJu::signal_cejuAbortConnect, cejuClient, &CeJuTcpClient::slot_abortConnectCeju);
    connect(cejuClient, &CeJuTcpClient::signal_cejuConnected, this, &FormCeJu::slot_cejuConnected);
    connect(cejuClient, &CeJuTcpClient::signal_cejuNoLink, this, &FormCeJu::slot_cejuNoLink);

    connect(cejuClient, &CeJuTcpClient::signal_cejuValueUpdate, this, &FormCeJu::slot_cejuValueUpdate);
//    connect(this, &FormCeJu::signal_getCejuLogInfo, cejuClient, &CeJuTcpClient::slot_getCejuLogInfo);

    //启动线程
    cejuThread.start();
}

void FormCeJu::slot_cejuValueUpdate()
{
    ui->lcdN_CejuTask1->display(cejuClient->stCurTaskValue.task1Value);
    ui->lcdN_CejuTask2->display(cejuClient->stCurTaskValue.task2Value);
    ui->lcdN_CejuTask3->display(cejuClient->stCurTaskValue.task3Value);
}
void FormCeJu::on_pbtn_linkceju_clicked()
{
    if("连接测距" == ui->pbtn_linkceju->text())
    {
        ui->pbtn_linkceju->setDisabled(true);
        emit signal_cejuNeedConnect();
    }
    else if("断开测距" == ui->pbtn_linkceju->text())
    {
        ui->pbtn_linkceju->setDisabled(true);
        emit signal_cejuAbortConnect();
    }
}

void FormCeJu::slot_cejuConnected()
{
    if("连接测距" == ui->pbtn_linkceju->text())
    {
        ui->pbtn_linkceju->setEnabled(true);
        ui->pbtn_linkceju->setText("断开测距");
    }
}

void FormCeJu::slot_cejuNoLink()
{
    if("断开测距" == ui->pbtn_linkceju->text())
    {
        ui->pbtn_linkceju->setEnabled(true);
        ui->pbtn_linkceju->setText("连接测距");
    }
}


void FormCeJu::on_pbtn_getLogInfo_clicked()
{
    emit signal_getCejuLogInfo();
}

