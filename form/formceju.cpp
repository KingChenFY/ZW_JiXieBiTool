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

    connect(this, &FormCeJu::signal_getCejuLogInfo, cejuClient, &CeJuTcpClient::slot_getCejuLogInfo);
    connect(cejuClient, &CeJuTcpClient::signal_getCejuLog_UiUpdate, this, &FormCeJu::slot_getCejuLog_UiUpdate);
    connect(this, &FormCeJu::signal_startCejuRecord, cejuClient, &CeJuTcpClient::slot_startCejuRecord);
    connect(cejuClient, &CeJuTcpClient::signal_startCejuRecord_UiUpdate, this, &FormCeJu::slot_startCejuRecord_UiUpdate);
    connect(this, &FormCeJu::signal_stopCejuRecord, cejuClient, &CeJuTcpClient::slot_stopCejuRecord);
    connect(cejuClient, &CeJuTcpClient::signal_stopCejuRecord_UiUpdate, this, &FormCeJu::slot_stopCejuRecord_UiUpdate);
    connect(this, &FormCeJu::signal_clearCejuRecord, cejuClient, &CeJuTcpClient::slot_clearCejuRecord);
    connect(cejuClient, &CeJuTcpClient::signal_clearCejuRecord_UiUpdate, this, &FormCeJu::slot_clearCejuRecord_UiUpdate);

    connect(this, &FormCeJu::signal_setCejuTriggerModeExternal, cejuClient, &CeJuTcpClient::slot_setCejuTriggerModeExternal);
    connect(cejuClient, &CeJuTcpClient::signal_setCejuTriggerModeExternal_UiUpdate, this, &FormCeJu::slot_setCejuTriggerModeExternal_UiUpdate);
    connect(this, &FormCeJu::signal_setCejuTriggerModeInternal, cejuClient, &CeJuTcpClient::slot_setCejuTriggerModeInternal);
    connect(cejuClient, &CeJuTcpClient::signal_setCejuTriggerModeInternal_UiUpdate, this, &FormCeJu::slot_setCejuTriggerModeInternal_UiUpdate);

    //启动线程
    cejuThread.start();
}

/****************************************************测距 UI 界面更新****************************************************/
void FormCeJu::slot_cejuValueUpdate()
{
    ui->lcdN_CejuTask1->display(cejuClient->stCurTaskValue.task1Value);
    ui->lcdN_CejuTask2->display(cejuClient->stCurTaskValue.task2Value);
    ui->lcdN_CejuTask3->display(cejuClient->stCurTaskValue.task3Value);
}

void FormCeJu::slot_getCejuLog_UiUpdate(bool isSucceed)
{
    if(isSucceed)
    {
        if(cejuClient->m_bIsRecordOn)
            ui->lab_logState->setText("正在记录");
        else
            ui->lab_logState->setText("未在记录");

        ui->lab_logNum->setNum((double)cejuClient->m_uRecordNum);
    }
    else
    {
        QMessageBox::warning(this, tr("获取记录信息"), tr("获取记录信息 失败"));
    }
}

void FormCeJu::slot_startCejuRecord_UiUpdate(bool isSucceed)
{
    if(isSucceed)
    {
        ui->pbtn_stLog->setText("停止记录");
    }
    else
    {
        QMessageBox::warning(this, tr("开始内部数据记录"), tr("开始内部数据记录 失败"));
    }
    ui->pbtn_stLog->setEnabled(true);
}

void FormCeJu::slot_stopCejuRecord_UiUpdate(bool isSucceed)
{
    if(isSucceed)
    {
        ui->pbtn_stLog->setText("开始记录");
    }
    else
    {
        QMessageBox::warning(this, tr("停止内部数据记录"), tr("停止内部数据记录 失败"));
    }
    ui->pbtn_stLog->setEnabled(true);
}

void FormCeJu::slot_setCejuTriggerModeExternal_UiUpdate(bool isSucceed)
{
    ui->gb_trigMode->setEnabled(true);
    if(!isSucceed)
        QMessageBox::warning(this, tr("切换外部触发"), tr("切换外部触发 失败"));
}

void FormCeJu::slot_setCejuTriggerModeInternal_UiUpdate(bool isSucceed)
{
    ui->gb_trigMode->setEnabled(true);
    if(!isSucceed)
        QMessageBox::warning(this, tr("切换内部触发"), tr("切换内部触发 失败"));
}

void FormCeJu::slot_clearCejuRecord_UiUpdate(bool isSucceed)
{
    ui->pbtn_clearLog->setEnabled(true);
    if(isSucceed)
        ui->lab_logNum->setNum(0);
    else
        QMessageBox::warning(this, tr("清除内部数据记录"), tr("清除内部数据记录 失败"));
}

/****************************************************测距 用户操作槽****************************************************/
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

void FormCeJu::on_pbtn_getLog_clicked()
{
    emit signal_getCejuLogInfo();
}

void FormCeJu::on_pbtn_stLog_clicked()
{
    ui->pbtn_stLog->setDisabled(true);
    if("开始记录" == ui->pbtn_stLog->text())
    {
        uint16_t u16RecordGap = ui->sbox_logIntervalSet->value();
        uint32_t u32SaveNum = ui->sbox_logNumSet->value();
        emit signal_startCejuRecord(u16RecordGap, u32SaveNum);
    }
    else
    {
        emit signal_stopCejuRecord();
    }
}

void FormCeJu::on_pbtn_clearLog_clicked()
{
    ui->pbtn_clearLog->setDisabled(true);
    emit signal_clearCejuRecord();
}

void FormCeJu::on_rbtn_ExTrigger_clicked()
{
    ui->gb_trigMode->setDisabled(true);
    emit signal_setCejuTriggerModeExternal();
}

void FormCeJu::on_rbtn_InTrigger_clicked()
{
    ui->gb_trigMode->setDisabled(true);
    emit signal_setCejuTriggerModeInternal();
}

